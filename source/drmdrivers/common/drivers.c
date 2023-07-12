/*
** The DRM driver for HybridOS/MiniGUI.
**
** Copyright (C) 2019 ~ 2023 FMSoft Technologies (http://www.fmsoft.cn).
**
** Some drivers are derived from Mesa.
** Copyright 2003 VMware, Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sub license, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice (including the
** next paragraph) shall be included in all copies or substantial portions
** of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
** OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
** IN NO EVENT SHALL VMWARE AND/OR ITS SUPPLIERS BE LIABLE FOR
** ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/exstubs.h>

#include "drivers.h"

extern DrmDriverOps* __drm_ex_driver_get(const char* driver_name, int dev_fd,
        int* version)
{
    _MG_PRINTF("%s called with driver name: %s\n", __func__, driver_name);

    *version = DRM_DRIVER_VERSION;
    if (strcmp(driver_name, "vmwgfx") == 0) {
#if HAVE(VMWGFX_DRM_H)
        return _drm_device_get_vmwgfx_driver(dev_fd);
#endif
    }
    else if (strcmp(driver_name, "i915") == 0) {
#if HAVE(DRM_INTEL)
        return _drm_device_get_i915_driver(dev_fd);
#endif
    }

    _MG_PRINTF("%s unknown DRM driver: %s (%d)\n", __func__,
            driver_name, dev_fd);
    return NULL;
}

