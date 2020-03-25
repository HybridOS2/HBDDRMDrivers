dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.60)
AC_INIT(hidrmdrivers,0.1.0)
AC_CONFIG_SRCDIR(src/drivers.c)

dnl Set various version strings - taken gratefully from the SDL sources
#
# Making releases:
# Change the version, then:
#   DRMDRIVER_MICRO_VERSION += 1;
#   DRMDRIVER_INTERFACE_AGE += 1;
#   DRMDRIVER_BINARY_AGE += 1;
# if any functions have been added, set DRMDRIVER_INTERFACE_AGE to 0.
# if backwards compatibility has been broken,
# set DRMDRIVER_BINARY_AGE and DRMDRIVER_INTERFACE_AGE to 0.
#
DRMDRIVER_MAJOR_VERSION=0
DRMDRIVER_MINOR_VERSION=1
DRMDRIVER_MICRO_VERSION=0
DRMDRIVER_INTERFACE_AGE=0
DRMDRIVER_BINARY_AGE=0
DRMDRIVER_VERSION=$DRMDRIVER_MAJOR_VERSION.$DRMDRIVER_MINOR_VERSION.$DRMDRIVER_MICRO_VERSION

dnl ========================================================================
dnl libtool versioning
LT_RELEASE=$DRMDRIVER_MAJOR_VERSION.$DRMDRIVER_MINOR_VERSION
LT_CURRENT=`expr $DRMDRIVER_MICRO_VERSION - $DRMDRIVER_INTERFACE_AGE`
LT_REVISION=$DRMDRIVER_INTERFACE_AGE
LT_AGE=`expr $DRMDRIVER_BINARY_AGE - $DRMDRIVER_INTERFACE_AGE`

AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

dnl ========================================================================
dnl Need for cross-compiling
AC_CANONICAL_SYSTEM

dnl ========================================================================
dnl Init automake
AM_INIT_AUTOMAKE

dnl ========================================================================
dnl Checks for programs.
AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_CXX
dnl AC_PROG_LD
dnl AC_PROG_RANLIB
dnl AC_PROG_LIBTOOL

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT([disable-static])

dnl ========================================================================
dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

dnl ========================================================================
dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_CHECK_HEADERS(sys/time.h unistd.h)

PKG_CHECK_MODULES([MINIGUI], [minigui >= 4.0.7])
PKG_CHECK_MODULES(DRM, [libudev >= 200, libdrm >= 2.4], video_drm="yes",
    video_drm="no; this package requires libdrm 2.4+ and libudev 200+")
if test "x$video_drm" = "xyes"; then
    DRM_INC_DIR="`$PKG_CONFIG --variable includedir libdrm`/libdrm"
    AC_SUBST(DRM_INC_DIR)
    DEP_LIBS="$DEP_LIBS -ldrm"
else
    AC_MSG_WARN([$video_drm])
fi

PKG_CHECK_MODULES(DRM_INTEL, [libdrm_intel >= 2.4], video_drm_intel="yes",
    video_drm_intel="no; the package requires libdrm_intel 2.4+")
if test "x$video_drm_intel" = "xyes"; then
    CPPFLAGS="$CPPFLAGS -DHAVE_DRM_INTEL"
    DEP_LIBS="$DEP_LIBS -ldrm_intel"
else
    AC_MSG_WARN([$video_drm_intel])
fi

if test "$ac_cv_prog_gcc" = "yes"; then
    CPPFLAGS="$CPPFLAGS -D_DEFAULT_SOURCE -D_GNU_SOURCE"
    CFLAGS="$CFLAGS -std=c99 -Wall -Werror -Wstrict-prototypes -pipe"
fi

dnl ========================================================================
dnl Write Output

DEP_LIBS="$DEP_LIBS $MINIGUI_LIBS"
AC_SUBST(DEP_LIBS)

AC_CHECK_DECLS(_MGRM_PROCESSES, minigui_runmode="procs", foo=bar, [#include <minigui/common.h>])
AM_CONDITIONAL(MGRM_PROCESSES, test "x$minigui_runmode" = "xprocs")

AM_CONDITIONAL(DRM_INTEL, test "x$video_drm_intel" = "xyes")

AC_OUTPUT(
    libhidrmdrivers.pc
    Makefile
    src/Makefile
)
