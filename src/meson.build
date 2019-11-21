# Copyright © 2019 FMSoft Technologies

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

lib_files = files('drivers.c', 'drivers.h')

priv_reqs = ['minigui', 'libdrm']
if with_intel
  lib_files += files(
    'intel/intel-batchbuffer.h',
    'intel/intel-context.h',
    'intel/intel-i915-driver.c',
    'intel/intel-reg.h',
  )
  priv_reqs += 'libdrm-intel'
endif

libhidrmdrivers = shared_library(
  'hidrmdrivers',
  [
    lib_files
  ],
  dependencies : [dep_minigui, dep_libdrm, dep_drm_intel],
  install : true,
  version : meson.project_version(),
)

install_headers(
)

pkg.generate(
  name : 'libhidrmdrivers',
  libraries : libhidrmdrivers,
  version : meson.project_version(),
  requires : priv_reqs,
  description : 'DRM Drivers for MiniGUI DRM engine',
)

