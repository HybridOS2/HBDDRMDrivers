# Copyright © 2017-2018 Intel Corporation

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

project(
  'libhidrmdrivers',
  ['c'],
  version : '0.1.0',
  license : 'MIT',
  meson_version : '>= 0.43',
  default_options : ['buildtype=debugoptimized', 'c_std=gnu99'],
)

pkg = import('pkgconfig')

prog_pkgconfig = find_program('pkg-config')

with_tests = get_option('tests')

dep_minigui = dependency('minigui', version : '>= 4.1.0', required: true)
dep_libdrm = dependency('libdrm', version : '>= 2.5.0', required: true)

with_intel = false
_intel = get_option('intel')
if _intel != 'false'
  with_intel = _intel == 'true' or host_machine.cpu_family().startswith('x86')
endif

dep_libdrm = declare_dependency(
  compile_args : run_command(prog_pkgconfig, ['libdrm', '--cflags']).stdout().split()
)

if with_intel
  dep_drm_intel = dependency('libdrm_intel', version : '>= 2.5.0', required: false)
  if not dep_drm_intel.found()
    with_intel = false
  endif
endif

cc = meson.get_compiler('c')

pkg.generate(
  name : 'libhidrmdrivers',
  subdirs : ['.', 'src'],
  version : meson.project_version(),
  description : 'DRM Drivers for MiniGUI DRM engine',
)

subdir('src')

message('')
message('@0@ will be compiled with:'.format(meson.project_name()))
message('')
message('  Intel (i915)   @0@'.format(with_intel))
message('  Test programs  @0@'.format(with_tests))
message('')
