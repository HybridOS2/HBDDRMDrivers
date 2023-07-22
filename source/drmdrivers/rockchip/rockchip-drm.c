/*
 * Copyright (C) FMSoft.CN <https://www.fmsoft.cn>
 * Copyright (C) ROCKCHIP, Inc.
 * Author:yzq<yzq@rock-chips.com>
 *
 * based on exynos_drm.c
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/mman.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <drm.h>
#include <drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/exstubs.h>

#include <rga.h>
#include <drmrga.h>
#include <im2d.h>

#include "libdrm-macros.h"
#include "helpers.h"

#include "rockchip-drm.h"

struct _DrmDriver {
    int devfd;
    unsigned nr_bufs;
};

typedef struct my_surface_buffer {
    DrmSurfaceBuffer    base;
    int                 nr_hdr_lines;
    int                 rk_format;
    uint32_t            flags;
    rga_buffer_handle_t rga_handle;
    rga_buffer_t        rga_buffer;
} my_surface_buffer;

/* Create rockchip DRM userland driver. */
static DrmDriver *rockchip_create_driver(int devfd)
{
    DrmDriver *drv;

    drv = calloc(1, sizeof(*drv));
    if (!drv) {
        _ERR_PRINTF("Failed to create device: %m.\n");
        return NULL;
    }

    drv->devfd = devfd;
    return drv;
}

/* Destroy rockchip DRM userland driver. */
static void rockchip_destroy_driver(DrmDriver *drv)
{
    if (drv->nr_bufs) {
        _WRN_PRINTF ("There is still %d buffers left\n", drv->nr_bufs);
    }

    free(drv);
}

static int
rockchip_format_from_drm_format(uint32_t drm_format, int* bpp, int* cpp)
{
    int rk_format = -1;

    switch (drm_format) {
        case DRM_FORMAT_ARGB4444:
            rk_format = RK_FORMAT_ARGB_4444;
            *bpp = 16;
            *cpp = 2;
            break;

        case DRM_FORMAT_ABGR4444:
            rk_format = RK_FORMAT_ABGR_4444;
            *bpp = 16;
            *cpp = 2;
            break;

        case DRM_FORMAT_RGBA4444:
            rk_format = RK_FORMAT_RGBA_4444;
            *bpp = 16;
            *cpp = 2;
            break;

        case DRM_FORMAT_BGRA4444:
            rk_format = RK_FORMAT_BGRA_4444;
            *bpp = 16;
            *cpp = 2;
            break;

        case DRM_FORMAT_RGBA5551:
            rk_format = RK_FORMAT_RGBA_5551;
            *bpp = 16;
            *cpp = 2;
            break;

        case DRM_FORMAT_BGRA5551:
            rk_format = RK_FORMAT_BGRA_5551;
            *bpp = 16;
            *cpp = 2;
            break;

        case DRM_FORMAT_RGB565:
            rk_format = RK_FORMAT_RGB_565;
            *bpp = 16;
            *cpp = 2;
            break;

        case DRM_FORMAT_BGR565:
            rk_format = RK_FORMAT_BGR_565;
            *bpp = 16;
            *cpp = 2;
            break;

        case DRM_FORMAT_RGB888:
            rk_format = RK_FORMAT_RGB_888;
            *bpp = 24;
            *cpp = 3;
            break;

        case DRM_FORMAT_BGR888:
            rk_format = RK_FORMAT_BGR_888;
            *bpp = 24;
            *cpp = 3;
            break;

        case DRM_FORMAT_XRGB8888:
            rk_format = RK_FORMAT_XRGB_8888;
            *bpp = 32;
            *cpp = 4;
            break;

        case DRM_FORMAT_XBGR8888:
            rk_format = RK_FORMAT_XBGR_8888;
            *bpp = 32;
            *cpp = 4;
            break;

        case DRM_FORMAT_RGBX8888:
            rk_format = RK_FORMAT_RGBX_8888;
            *bpp = 32;
            *cpp = 4;
            break;

        case DRM_FORMAT_BGRX8888:
            rk_format = RK_FORMAT_BGRX_8888;
            *bpp = 32;
            *cpp = 4;
            break;

        case DRM_FORMAT_ARGB8888:
            rk_format = RK_FORMAT_ARGB_8888;
            *bpp = 32;
            *cpp = 4;
            break;

        case DRM_FORMAT_ABGR8888:
            rk_format = RK_FORMAT_ABGR_8888;
            *bpp = 32;
            *cpp = 4;
            break;

        case DRM_FORMAT_RGBA8888:
            rk_format = RK_FORMAT_RGBA_8888;
            *bpp = 32;
            *cpp = 4;
            break;

        case DRM_FORMAT_BGRA8888:
            rk_format = RK_FORMAT_BGRA_8888;
            *bpp = 32;
            *cpp = 4;
            break;

        default:
            break;
    }

    return rk_format;
}

static DrmSurfaceBuffer* rockchip_create_buffer(DrmDriver *drv,
        uint32_t drm_format, uint32_t hdr_size,
        uint32_t width, uint32_t height, uint32_t flags)
{
    my_surface_buffer *buffer = NULL;
    int rk_format;
    int bpp, cpp;
    uint32_t pitch, nr_hdr_lines = 0;
    uint32_t rk_flags;

    if ((flags & DRM_SURBUF_TYPE_MASK) == DRM_SURBUF_TYPE_SCANOUT) {
        rk_flags = ROCKCHIP_BO_CONTIG;
    }
    else if ((flags & DRM_SURBUF_TYPE_MASK) == DRM_SURBUF_TYPE_SHADOW) {
        rk_flags = ROCKCHIP_BO_CONTIG | ROCKCHIP_BO_CACHABLE;
    }
    else {
        rk_flags = ROCKCHIP_BO_CACHABLE;
    }

    rk_format = rockchip_format_from_drm_format(drm_format, &bpp, &cpp);
    if (rk_format == -1) {
        _ERR_PRINTF("DRM>ROCKCHIP: not supported format: %d\n", drm_format);
        goto failed;
    }

    pitch = ROUND_TO_MULTIPLE(width * cpp, 4);
    if (hdr_size) {
        nr_hdr_lines = hdr_size / pitch;
        if (hdr_size % pitch)
            nr_hdr_lines++;
    }

    uint32_t size = (height + nr_hdr_lines) * pitch;
    if (size == 0) {
        _ERR_PRINTF("DRM>ROCKCHIP: zero size requested (%u x %u)\n",
                width, height);
        goto failed;
    }

    buffer = calloc(1, sizeof(*buffer));
    if (buffer == NULL) {
        _ERR_PRINTF ("DRM>ROCKCHIP: could not allocate surface buffer: %m\n");
        goto failed;
    }

    struct drm_rockchip_gem_create req = {
        .size = size,
        .flags = rk_flags,
    };

    if (drmIoctl(drv->devfd, DRM_IOCTL_ROCKCHIP_GEM_CREATE, &req)){
        _ERR_PRINTF("DRM>ROCKCHIP: failed to create gem object: %m.\n");
        goto failed;
    }

    buffer->base.handle = req.handle;
    buffer->base.prime_fd = -1;
    buffer->base.name = 0;
    buffer->base.fb_id = 0;
    buffer->base.drm_format = drm_format;
    buffer->base.bpp = bpp;
    buffer->base.cpp = cpp;
    buffer->base.scanout = IS_SURFACE_FOR_SCANOUT(flags) ? 1 : 0;
    buffer->base.width = width;
    buffer->base.height = height;
    buffer->base.pitch = pitch;
    buffer->base.offset = nr_hdr_lines * pitch;
    buffer->base.buff = NULL;

    buffer->nr_hdr_lines = nr_hdr_lines;
    buffer->rk_format = rk_format;
    buffer->flags = flags;

    drv->nr_bufs++;

    _DBG_PRINTF("Allocate GEM object for surface buffer: "
            "width (%d), height (%d), (pitch: %d), size (%lu), offset (%ld)\n",
            buffer->base.width, buffer->base.height, buffer->base.pitch,
            buffer->base.size, buffer->base.offset);

    if (drmPrimeHandleToFD(drv->devfd, req.handle,
            DRM_RDWR | DRM_CLOEXEC, &buffer->base.prime_fd)) {
        _WRN_PRINTF("DRM>ROCKCHIP: failed to get prime FD of buffer: %m.\n");
    }
    else {
        im_handle_param_t param = {(uint32_t)width,
            (uint32_t)(nr_hdr_lines + height), (uint32_t)rk_format};
        buffer->rga_handle = importbuffer_fd(buffer->base.prime_fd, &param);
        if (buffer->rga_handle) {
            buffer->rga_buffer = wrapbuffer_handle(buffer->rga_handle,
                    width, height, rk_format, pitch, height);
        }
        else {
            _WRN_PRINTF("DRM>ROCKCHIP: failed importbuffer_fd(): %m.\n");
        }
    }

    return &buffer->base;

failed:
    if (buffer)
        free(buffer);
    return NULL;
}

static void rockchip_destroy_buffer(DrmDriver *drv, DrmSurfaceBuffer *buffer)
{
    my_surface_buffer *mybuf = (my_surface_buffer *)buffer;
    assert(mybuf != NULL);

    if (mybuf->base.buff) {
        munmap(mybuf->base.buff, mybuf->base.size);
    }

    if (mybuf->rga_handle) {
        releasebuffer_handle(mybuf->rga_handle);
    }

    if (mybuf->base.prime_fd) {
        close(mybuf->base.prime_fd);
    }

    struct drm_gem_close req = {
        .handle = mybuf->base.handle,
    };

    if (drmIoctl(drv->devfd, DRM_IOCTL_GEM_CLOSE, &req)) {
        _WRN_PRINTF("Failed drmIoctl(DRM_IOCTL_GEM_CLOSE): %m\n");
    }
    else {
        drv->nr_bufs--;
        _DBG_PRINTF("Buffer object (%u) destroied\n", mybuf->base.handle);
    }

    free(mybuf);
}

#if 0
struct rockchip_bo *rockchip_bo_from_handle(DrmDriver *drv,
            uint32_t handle, uint32_t flags, uint32_t size)
{
    struct rockchip_bo *bo;

    if (size == 0) {
        fprintf(stderr, "invalid size.\n");
        return NULL;
    }

    bo = calloc(1, sizeof(*bo));
    if (!bo) {
        fprintf(stderr, "failed to create bo[%s].\n",
                strerror(errno));
        return NULL;
    }

    bo->drv = drv;
    bo->handle = handle;
    bo->size = size;
    bo->flags = flags;

    return bo;
}

struct rockchip_bo *rockchip_bo_from_name(DrmDriver *drv,
                        uint32_t name)
{
    struct rockchip_bo *bo;
    struct drm_gem_open req = {
        .name = name,
    };

    bo = calloc(1, sizeof(*bo));
    if (!bo) {
        fprintf(stderr, "failed to allocate bo[%s].\n",
                strerror(errno));
        return NULL;
    }

    if (drmIoctl(drv->fd, DRM_IOCTL_GEM_OPEN, &req)) {
        fprintf(stderr, "failed to open gem object[%s].\n",
                strerror(errno));
        goto err_free_bo;
    }

    bo->drv = drv;
    bo->name = name;
    bo->handle = req.handle;

    return bo;

err_free_bo:
    free(bo);
    return NULL;
}

/*
 * Get a gem global object name from a gem object handle.
 *
 * @bo: a rockchip buffer object including gem handle.
 * @name: a gem global object name to be got by kernel driver.
 *
 * this interface is used to get a gem global object name from a gem object
 * handle to a buffer that wants to share it with another process.
 *
 * if true, return 0 else negative.
 */
int rockchip_bo_get_name(struct rockchip_bo *bo, uint32_t *name)
{
    if (!bo->name) {
        struct drm_gem_flink req = {
            .handle = bo->handle,
        };
        int ret;

        ret = drmIoctl(bo->drv->fd, DRM_IOCTL_GEM_FLINK, &req);
        if (ret) {
            fprintf(stderr, "failed to get gem global name[%s].\n",
                    strerror(errno));
            return ret;
        }

        bo->name = req.name;
    }

    *name = bo->name;

    return 0;
}

uint32_t rockchip_bo_handle(struct rockchip_bo *bo)
{
    return bo->handle;
}

extern void *mmap64(void *addr, size_t len, int prot, int flags,
        int fildes, uint64_t off);

/*
 * Mmap a buffer to user space.
 *
 * @bo: a rockchip buffer object including a gem object handle to be mmapped
 *    to user space.
 *
 * if true, user pointer mmaped else NULL.
 */
void *rockchip_bo_map(struct rockchip_bo *bo)
{
    if (!bo->vaddr) {
        DrmDriver *drv = bo->drv;
        struct drm_rockchip_gem_map_off req = {
            .handle = bo->handle,
        };
        int ret;

        ret = drmIoctl(drv->fd, DRM_IOCTL_ROCKCHIP_GEM_MAP_OFFSET, &req);
        if (ret) {
            fprintf(stderr, "failed to ioctl gem map offset[%s].\n",
                strerror(errno));
            return NULL;
        }

        bo->vaddr = mmap64(0, bo->size, PROT_READ | PROT_WRITE,
               MAP_SHARED, drv->fd, req.offset);
        if (bo->vaddr == MAP_FAILED) {
            fprintf(stderr, "failed to mmap buffer[%s].\n",
                strerror(errno));
            return NULL;
        }
    }

    return bo->vaddr;
}
#endif

DrmDriverOps* _drm_device_get_rockchip_driver(int device_fd)
{
    (void)device_fd;

    static DrmDriverOps rockchip_driver = {
        .create_driver = rockchip_create_driver,
        .destroy_driver = rockchip_destroy_driver,
        .flush_driver = NULL,
        .create_buffer = rockchip_create_buffer,
        // .create_buffer_from_handle = rockchip_create_buffer_from_handle,
        // .create_buffer_from_name = rockchip_create_buffer_from_name,
        // .create_buffer_from_prime_fd = rockchip_create_buffer_from_prime_fd,
        // .map_buffer = rockchip_map_buffer,
        // .unmap_buffer = rockchip_unmap_buffer,
        .destroy_buffer = rockchip_destroy_buffer,
    };

    return &rockchip_driver;
}

