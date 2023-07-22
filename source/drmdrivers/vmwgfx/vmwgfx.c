/**************************************************************************
 *
 * Copyright © 2023 FMSoft.CN
 * Copyright © 2009 VMware, Inc., Palo Alto, CA., USA
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#undef NDEBUG

#include "config.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <xf86drm.h>
#include <vmwgfx_drm.h>

#define _DEBUG

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/exstubs.h>

#include "libdrm-macros.h"
#include "helpers.h"

struct vmwgfx_buffer
{
    DrmSurfaceBuffer base;
    off_t map_offset;
    uint64_t map_handle;
    unsigned map_count;
};

/* the driver data struct */
struct _DrmDriver {
    int         fd;
    unsigned    nr_bufs;
};

static DrmDriver* vmwgfx_create_driver(int device_fd)
{
    DrmDriver *driver;

    driver = calloc(1, sizeof(DrmDriver));
    driver->fd = device_fd;
    driver->nr_bufs = 0;
    return driver;
}

static void
vmwgfx_destroy_driver(DrmDriver *driver)
{
    if (driver->nr_bufs) {
        _WRN_PRINTF ("There is still %d buffers left\n", driver->nr_bufs);
    }

    free(driver);
}

static DrmSurfaceBuffer* vmwgfx_create_buffer (DrmDriver *driver,
        uint32_t drm_format, uint32_t hdr_size,
        uint32_t width, uint32_t height, uint32_t flags)
{
    int bpp, cpp;
    uint32_t pitch, nr_hdr_lines = 0;

    _DBG_PRINTF("called\n");

    if (drm_format_to_bpp(drm_format, &bpp, &cpp) == 0) {
        _ERR_PRINTF ("DRM>vmwgfx: not supported format: %d\n", drm_format);
        return NULL;
    }

    pitch = ROUND_TO_MULTIPLE (width * cpp, 4);
    if (hdr_size) {
        nr_hdr_lines = hdr_size / pitch;
        if (hdr_size % pitch)
            nr_hdr_lines++;
    }

    struct vmwgfx_buffer *bo;
    bo = calloc(1, sizeof(*bo));
    if (!bo)
        return NULL;

    {
        size_t size = (height + nr_hdr_lines) * pitch;
        int ret;
        union drm_vmw_alloc_dmabuf_arg arg;
        struct drm_vmw_alloc_dmabuf_req *req = &arg.req;
        struct drm_vmw_dmabuf_rep *rep = &arg.rep;

        memset(&arg, 0, sizeof(arg));
        req->size = size;

        do {
            ret = drmCommandWriteRead(driver->fd,
                          DRM_VMW_ALLOC_DMABUF,
                          &arg, sizeof(arg));
        } while (ret == -ERESTART);

        if (ret)
            goto error;

        bo->base.size = size;
        bo->base.handle = rep->handle;
        bo->map_handle = rep->map_handle;
        bo->map_offset = rep->cur_gmr_offset;
        bo->base.offset = nr_hdr_lines * pitch;
    }

    bo->base.prime_fd = -1;
    bo->base.name = 0;
    bo->base.fb_id = 0;
    bo->base.drm_format = drm_format;
    bo->base.bpp = bpp;
    bo->base.cpp = cpp;
    bo->base.scanout = (flags & DRM_SURBUF_TYPE_SCANOUT) ? 1 : 0;
    bo->base.width = width;
    bo->base.height = height;
    bo->base.pitch = pitch;
    bo->base.buff = NULL;

    _DBG_PRINTF ("Allocate GEM object for surface bo: "
            "width (%d), height (%d), (pitch: %d), size (%u), handle (0x%llx)\n",
            bo->base.width, bo->base.height, bo->base.pitch,
            (unsigned)bo->base.size, (long long unsigned)bo->map_handle);

    driver->nr_bufs++;
    return &bo->base;

error:
    free(bo);
    return NULL;
}

extern void *mmap64(void *addr, size_t len, int prot, int flags,
        int fildes, uint64_t off);

static uint8_t* vmwgfx_map_buffer(DrmDriver *driver,
        DrmSurfaceBuffer* buffer)
{
    struct vmwgfx_buffer *bo = (struct vmwgfx_buffer *)buffer;

    assert(bo->map_handle % sysconf(_SC_PAGE_SIZE) == 0);

    void *map = mmap64(NULL, bo->base.size,
            PROT_READ | PROT_WRITE, MAP_SHARED, driver->fd,
            bo->map_handle);
    if (map == NULL || map == MAP_FAILED) {
        _ERR_PRINTF ("Failed mmap(): %m (size: %u, fd: %d, handle: 0x%llx)\n",
                (unsigned)bo->base.size, driver->fd,
                (long long unsigned)bo->map_handle);
        return NULL;
    }

    _DBG_PRINTF ("Mapped GEM object: %p\n", map);
    bo->map_count++;
    bo->base.buff = map;
    return map;
}

static void vmwgfx_unmap_buffer(DrmDriver *driver,
        DrmSurfaceBuffer* buffer)
{
    (void)driver;
    struct vmwgfx_buffer *bo = (struct vmwgfx_buffer *)buffer;

    bo->map_count--;
}

static void vmwgfx_destroy_buffer(DrmDriver *driver, DrmSurfaceBuffer* buffer)
{
    struct vmwgfx_buffer *bo = (struct vmwgfx_buffer *)buffer;
    struct drm_vmw_unref_dmabuf_arg arg;

    if (bo->base.buff) {
        /* XXX Sanity check map_count */
        drm_munmap(bo->base.buff, bo->base.size);
        bo->base.buff = NULL;
    }

    memset(&arg, 0, sizeof(arg));
    arg.handle = bo->base.handle;
    drmCommandWrite(driver->fd, DRM_VMW_UNREF_DMABUF, &arg, sizeof(arg));

    driver->nr_bufs--;
    free(bo);
}

DrmDriverOps *_drm_device_get_vmwgfx_driver(int device_fd)
{
    (void)device_fd;

    static DrmDriverOps vmwgfx_driver = {
        .create_driver = vmwgfx_create_driver,
        .destroy_driver = vmwgfx_destroy_driver,
        .flush_driver = NULL,
        .create_buffer = vmwgfx_create_buffer,
        .create_buffer_from_handle = NULL,
        .create_buffer_from_name = NULL,
        .create_buffer_from_prime_fd = NULL,
        .map_buffer = vmwgfx_map_buffer,
        .unmap_buffer = vmwgfx_unmap_buffer,
        .destroy_buffer = vmwgfx_destroy_buffer,
    };

    return &vmwgfx_driver;
}

