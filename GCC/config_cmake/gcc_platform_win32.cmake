SET(target i686-pc-win32)
SET(cpu_type i386)
SET(tm_defines )
SET(tm_file i386/i386.h i386/unix.h i386/bsd.h i386/gas.h dbxcoff.h i386/cygming.h i386/mingw32.h)
SET(xm_defines POSIX)
SET(xm_file i386/xm-mingw32.h)
SET(host_xm_defines POSIX)
SET(host_xm_file i386/xm-mingw32.h)
SET(build_xm_defines POSIX)
SET(build_xm_file i386/xm-mingw32.h)
SET(tm_p_file i386/i386-protos.h)
SET(extra_modes i386/i386-modes.def)
SET(extra_objs winnt.o winnt-stubs.o)
SET(extra_options i386/i386.opt i386/cygming.opt)
SET(c_target_objs)
SET(cxx_target_objs winnt-cxx.o)
SET(target_cpu_default)
SET(out_host_hook_obj host-mingw32.o)
