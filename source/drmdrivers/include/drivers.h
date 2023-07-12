///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/**************************************************************************
 *
 * Copyright 2019 FMSoft Technologies (http://www.fmsoft.cn).
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL VMWARE AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#ifndef HBDDRMDRIVERS_DRIVERS_H
#define HBDDRMDRIVERS_DRIVERS_H

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#if HAVE(VMWGFX_DRM_H)
DrmDriverOps* _drm_device_get_vmwgfx_driver(int devfd) WTF_INTERNAL;
#endif

#if HAVE(DRM_INTEL)
DrmDriverOps* _drm_device_get_i915_driver(int devfd) WTF_INTERNAL;
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* HBDDRMDRIVERS_DRIVERS_H */

