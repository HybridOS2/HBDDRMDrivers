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

project(
  'libhidrmdrivers',
  ['c', 'cpp'],
  version : '0.1.0',
  license : 'MIT',
  meson_version : '>= 0.43',
  default_options : ['buildtype=debugoptimized', 'c_std=c99'],
)

cc = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')

null_dep = dependency('', required : false)

pkg = import('pkgconfig')

# check required dependencies
dep_minigui = dependency('minigui', version : '>= 4.0.4', required: true)
dep_libdrm = dependency('libdrm', version : '>= 2.5.0', required: true)

# Arguments for the preprocessor, put these in a separate array from the C and
# C++ (cpp in meson terminology) arguments since they need to be added to the
# default arguments for both C and C++.
pre_args = [
  '-D__STDC_CONSTANT_MACROS',
  '-D__STDC_FORMAT_MACROS',
  '-D__STDC_LIMIT_MACROS',
  '-DPACKAGE_VERSION="@0@"'.format(meson.project_version()),
  '-DPACKAGE_BUGREPORT="https://gitlab.fmsoft.cn/hybridos/hidrmdrivers/issues"',
]

# Check for generic C arguments
c_args = []
foreach a : ['-Werror=implicit-function-declaration',
             '-Werror=missing-prototypes',
             '-Werror=return-type',
             '-Werror=incompatible-pointer-types',
             '-Werror=format',
             '-Wformat-security',
             '-Wno-missing-field-initializers',
             '-Wno-format-truncation',
             '-fno-math-errno',
             '-fno-trapping-math',
             '-Qunused-arguments']
  if cc.has_argument(a)
    c_args += a
  endif
endforeach

c_vis_args = []
if cc.has_argument('-fvisibility=hidden')
  c_vis_args += '-fvisibility=hidden'
endif

# Check for generic C++ arguments
cpp_args = []
foreach a : ['-Werror=return-type',
             '-Werror=format',
             '-Wformat-security',
             '-Wno-non-virtual-dtor',
             '-Wno-missing-field-initializers',
             '-Wno-format-truncation',
             '-fno-math-errno',
             '-fno-trapping-math',
             '-Qunused-arguments']
  if cpp.has_argument(a)
    cpp_args += a
  endif
endforeach

no_override_init_args = []
foreach a : ['-Wno-override-init',
             '-Wno-initializer-overrides']
  if cc.has_argument(a)
    no_override_init_args += a
  endif
endforeach

cpp_vis_args = []
if cpp.has_argument('-fvisibility=hidden')
  cpp_vis_args += '-fvisibility=hidden'
endif

prog_pkgconfig = find_program('pkg-config')

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
  else
    pre_args += '-DHAVE_DRM_INTEL'
  endif
endif

with_tests = get_option('tests')

# add pre_args, c_args, and cpp_args to project arguments
foreach a : pre_args
  add_project_arguments(a, language : ['c', 'cpp'])
endforeach
foreach a : c_args
  add_project_arguments(a, language : ['c'])
endforeach
foreach a : cpp_args
  add_project_arguments(a, language : ['cpp'])
endforeach

subdir('src')

lines = [
  '',
  'Global settings',
  '    prefix:          ' + get_option('prefix'),
  '    libdir:          ' + get_option('libdir'),
  '    includedir:      ' + get_option('includedir'),
  '',
  'Compilers and arguments',
  '    cc:              ' + cc.get_id() + ' ' + cc.version(),
  '    cpp:             ' + cpp.get_id() + ' ' + cpp.version(),
  '    pre_args:        ' + (' ').join(pre_args),
  '    c_args:          ' + (' ').join(c_args),
  '    cpp_args:        ' + (' ').join(cpp_args),
  '',
  'Components',
  '    Intel (i915)   @0@'.format(with_intel),
  '    Test programs  @0@'.format(with_tests),
]

summary = ('\n').join(lines)
message('Configuration summary:\n@0@\n'.format(summary))
