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
    uint32_t            rk_flags;
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

static void wrap_rga_buffer(DrmDriver *drv, my_surface_buffer *buf)
{
    if (drmPrimeHandleToFD(drv->devfd, buf->base.handle,
            DRM_RDWR | DRM_CLOEXEC, &buf->base.prime_fd)) {
        _WRN_PRINTF("DRM>ROCKCHIP: failed to get prime FD of buffer: %m.\n");
    }
    else {
        uint32_t height = buf->nr_hdr_lines + buf->base.height;

        im_handle_param_t param = { (uint32_t)buf->base.width, height,
            (uint32_t)buf->rk_format };
        buf->rga_handle = importbuffer_fd(buf->base.prime_fd, &param);
        if (buf->rga_handle) {
            buf->rga_buffer = wrapbuffer_handle(buf->rga_handle,
                    buf->base.width, height, buf->rk_format,
                    buf->base.pitch, buf->base.height);
        }
        else {
            _WRN_PRINTF("DRM>ROCKCHIP: failed importbuffer_fd(): %m.\n");
        }
    }
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

    size_t size = (height + nr_hdr_lines) * pitch;
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
    buffer->base.size = size;
    buffer->base.offset = nr_hdr_lines * pitch;
    buffer->base.buff = NULL;

    buffer->nr_hdr_lines = nr_hdr_lines;
    buffer->rk_format = rk_format;
    buffer->rk_flags = rk_flags;

    drv->nr_bufs++;

    _DBG_PRINTF("Allocate GEM object for surface buffer: "
            "width (%d), height (%d), (pitch: %d), size (%lu), offset (%ld)\n",
            buffer->base.width, buffer->base.height, buffer->base.pitch,
            buffer->base.size, buffer->base.offset);

    wrap_rga_buffer(drv, buffer);
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

static int check_format_size(size_t size,
        uint32_t drm_format, int *bpp, int *cpp,
        uint32_t hdr_size, uint32_t width, uint32_t height, uint32_t pitch)
{
    int rk_format;
    uint32_t nr_hdr_lines = 0;

    rk_format = rockchip_format_from_drm_format(drm_format, bpp, cpp);
    if (rk_format == -1) {
        _ERR_PRINTF("DRM>i915: not supported format: %d\n", drm_format);
        return -1;
    }

    if (pitch != ROUND_TO_MULTIPLE(width * *cpp, 4)) {
        _ERR_PRINTF("DRM>i915: bad pitch value: %u\n", pitch);
        return -1;
    }

    if (hdr_size) {
        nr_hdr_lines = hdr_size / pitch;
        if (hdr_size % pitch)
            nr_hdr_lines++;
    }

    if (size && size != (height + nr_hdr_lines) * pitch) {
        _ERR_PRINTF("DRM>i915: bad size: %lu\n", (unsigned long)size);
        return -1;
    }

    return rk_format;
}

static DrmSurfaceBuffer* rockchip_create_buffer_from_handle(DrmDriver *drv,
        uint32_t handle, size_t size,
        uint32_t drm_format, uint32_t hdr_size,
        uint32_t width, uint32_t height, uint32_t pitch)
{
    my_surface_buffer *buffer = NULL;
    int rk_format;
    int bpp, cpp;

    rk_format = check_format_size(size, drm_format, &bpp, &cpp,
            hdr_size, width, height, pitch);
    if (rk_format == -1) {
        _ERR_PRINTF("DRM>ROCKCHIP: bad surface parameters for handle %u: "
                "whole size: <unknown>, header size: %u, %u x %u, pitch: %u\n",
                handle, hdr_size, width, height, pitch);
        return NULL;
    }

    uint32_t nr_hdr_lines = hdr_size / pitch;

    buffer = calloc(1, sizeof(*buffer));
    if (buffer == NULL) {
        _ERR_PRINTF ("DRM>ROCKCHIP: could not allocate surface buffer: %m\n");
        goto failed;
    }

    buffer->base.handle = handle;
    buffer->base.prime_fd = -1;
    buffer->base.name = 0;
    buffer->base.fb_id = 0;
    buffer->base.drm_format = drm_format;
    buffer->base.bpp = bpp;
    buffer->base.cpp = cpp;
    buffer->base.scanout = 0;
    buffer->base.width = width;
    buffer->base.height = height;
    buffer->base.pitch = pitch;
    buffer->base.size = size;
    buffer->base.offset = nr_hdr_lines * pitch;
    buffer->base.buff = NULL;

    buffer->nr_hdr_lines = nr_hdr_lines;
    buffer->rk_format = rk_format;
    buffer->rk_flags = 0;

    drv->nr_bufs++;

    _DBG_PRINTF("Create surface buffer from handle: %u; "
            "width (%d), height (%d), (pitch: %d), size (%lu), offset (%ld)\n",
            buffer->base.handle, buffer->base.width, buffer->base.height,
            buffer->base.pitch, buffer->base.size, buffer->base.offset);

    wrap_rga_buffer(drv, buffer);
    return &buffer->base;

failed:
    if (buffer)
        free(buffer);
    return NULL;
}

static DrmSurfaceBuffer* rockchip_create_buffer_from_name(DrmDriver *drv,
        uint32_t name, uint32_t drm_format, uint32_t hdr_size,
        uint32_t width, uint32_t height, uint32_t pitch)
{
    my_surface_buffer *buffer = NULL;
    int rk_format;
    int bpp, cpp;

    rk_format = check_format_size(0, drm_format, &bpp, &cpp, hdr_size,
            width, height, pitch);
    if (rk_format == -1) {
        _ERR_PRINTF("DRM>ROCKCHIP: bad surface parameters for name %u: "
                "whole size: <unknown>, header size: %u, %u x %u, pitch: %u\n",
                name, hdr_size, width, height, pitch);
        return NULL;
    }

    uint32_t nr_hdr_lines = hdr_size / pitch;
    uint32_t size = (height + nr_hdr_lines) * pitch;

    buffer = calloc(1, sizeof(*buffer));
    if (buffer == NULL) {
        _ERR_PRINTF ("DRM>ROCKCHIP: could not allocate surface buffer: %m\n");
        goto failed;
    }

    struct drm_gem_open req = {
        .name = name,
    };

    if (drmIoctl(drv->devfd, DRM_IOCTL_GEM_OPEN, &req)) {
        fprintf(stderr, "failed to open gem object: %m.\n");
        goto failed;
    }

    buffer->base.handle = req.handle;
    buffer->base.prime_fd = -1;
    buffer->base.name = name;
    buffer->base.fb_id = 0;
    buffer->base.drm_format = drm_format;
    buffer->base.bpp = bpp;
    buffer->base.cpp = cpp;
    buffer->base.scanout = 0;
    buffer->base.width = width;
    buffer->base.height = height;
    buffer->base.pitch = pitch;
    buffer->base.size = size;
    buffer->base.offset = nr_hdr_lines * pitch;
    buffer->base.buff = NULL;

    buffer->nr_hdr_lines = nr_hdr_lines;
    buffer->rk_format = rk_format;
    buffer->rk_flags = 0;

    drv->nr_bufs++;

    _DBG_PRINTF("Create surface buffer from name: %u; "
            "width (%d), height (%d), (pitch: %d), size (%lu), offset (%ld)\n",
            buffer->base.name, buffer->base.width, buffer->base.height,
            buffer->base.pitch, buffer->base.size, buffer->base.offset);

    wrap_rga_buffer(drv, buffer);
    return &buffer->base;

failed:
    if (buffer)
        free(buffer);
    return NULL;
}

static DrmSurfaceBuffer *rockchip_create_buffer_from_prime_fd(DrmDriver *drv,
        int prime_fd, size_t size,
        uint32_t drm_format, uint32_t hdr_size,
        uint32_t width, uint32_t height, uint32_t pitch)
{
    my_surface_buffer *buffer = NULL;
    int rk_format;
    int bpp, cpp;

    size_t file_size;
    off_t seek = lseek (prime_fd, 0, SEEK_END);
    if (seek != -1)
        file_size = seek;
    else {
        _ERR_PRINTF("DRM>ROCKCHIP: Failed to get size of buffer from fd (%d): "
                "%m\n", prime_fd);
        goto failed;
    }

    _DBG_PRINTF("File size got from lseek(): %lu\n", (unsigned long)file_size);

    if (size == 0) {
        size = file_size;
    }
    else if (size > file_size) {
        _ERR_PRINTF("DRM>ROCKCHIP: size (%lu) doesn't match file size (%lu)\n",
                (unsigned long)size, (unsigned long)file_size);
        goto failed;
    }

    rk_format = check_format_size(size, drm_format, &bpp, &cpp, hdr_size,
            width, height, pitch);
    if (rk_format == -1) {
        _ERR_PRINTF("DRM>ROCKCHIP: bad surface parameters for prime fd %d: "
                "whole size: %lu, header size: %u, %u x %u, pitch: %u\n",
                prime_fd, (unsigned long)size, hdr_size, width, height, pitch);
        goto failed;
    }

    uint32_t nr_hdr_lines = hdr_size / pitch;

    buffer = calloc(1, sizeof(*buffer));
    if (buffer == NULL) {
        _ERR_PRINTF ("DRM>ROCKCHIP: could not allocate surface buffer: %m\n");
        goto failed;
    }

    buffer->base.handle = 0;
    buffer->base.prime_fd = prime_fd;
    buffer->base.name = 0;
    buffer->base.fb_id = 0;
    buffer->base.drm_format = drm_format;
    buffer->base.bpp = bpp;
    buffer->base.cpp = cpp;
    buffer->base.scanout = 0;
    buffer->base.width = width;
    buffer->base.height = height;
    buffer->base.pitch = pitch;
    buffer->base.size = size;
    buffer->base.offset = nr_hdr_lines * pitch;
    buffer->base.buff = NULL;

    buffer->nr_hdr_lines = nr_hdr_lines;
    buffer->rk_format = rk_format;
    buffer->rk_flags = 0;

    drv->nr_bufs++;

    _DBG_PRINTF("Create surface buffer from prime fd: %d; "
            "width (%d), height (%d), (pitch: %d), size (%lu), offset (%ld)\n",
            buffer->base.prime_fd, buffer->base.width, buffer->base.height,
            buffer->base.pitch, buffer->base.size, buffer->base.offset);

    wrap_rga_buffer(drv, buffer);
    return &buffer->base;

failed:
    if (buffer)
        free(buffer);
    return NULL;
}

extern void *mmap64(void *addr, size_t len, int prot, int flags,
        int fildes, uint64_t off);

static uint8_t *rockchip_map_buffer(DrmDriver *drv,
        DrmSurfaceBuffer *buffer)
{
    assert(buffer->buff == NULL);

    if (buffer->prime_fd) {
        buffer->buff = mmap(0, buffer->size,
                PROT_READ | PROT_WRITE, MAP_SHARED, buffer->prime_fd, 0);
    }
    else {
        struct drm_rockchip_gem_map_off req = {
            .handle = buffer->handle,
        };

        if (drmIoctl(drv->devfd, DRM_IOCTL_ROCKCHIP_GEM_MAP_OFFSET, &req)) {
            _ERR_PRINTF("failed to ioctl gem map offset: %m.\n");
            return NULL;
        }

        buffer->buff = mmap64(0, buffer->size, PROT_READ | PROT_WRITE,
               MAP_SHARED, drv->devfd, req.offset);
    }

    if (buffer->buff == MAP_FAILED) {
        buffer->buff = NULL;
        _ERR_PRINTF("failed to mmap buffer: %m.\n");
        return NULL;
    }

    return buffer->buff;
}

static void rockchip_unmap_buffer(DrmDriver *drv,
        DrmSurfaceBuffer* buffer)
{
    (void)drv;
    assert(buffer->buff);

    if (munmap(buffer->buff, buffer->size)) {
        _WRN_PRINTF("Failed munmap: %m\n");
    }

    buffer->buff = NULL;
}

DrmDriverOps* _drm_device_get_rockchip_driver(int device_fd)
{
    (void)device_fd;

    static DrmDriverOps rockchip_driver = {
        .create_driver = rockchip_create_driver,
        .destroy_driver = rockchip_destroy_driver,
        .flush_driver = NULL,
        .create_buffer = rockchip_create_buffer,
        .create_buffer_from_handle = rockchip_create_buffer_from_handle,
        .create_buffer_from_name = rockchip_create_buffer_from_name,
        .create_buffer_from_prime_fd = rockchip_create_buffer_from_prime_fd,
        .map_buffer = rockchip_map_buffer,
        .unmap_buffer = rockchip_unmap_buffer,
        .destroy_buffer = rockchip_destroy_buffer,
    };

    return &rockchip_driver;
}

