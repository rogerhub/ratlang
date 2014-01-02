BUILT_SOURCES = ratlang_bison.h
AM_YFLAGS = -d --report all
AM_LFLAGS =
bin_PROGRAMS = ratlang
ratlang_SOURCES = ratlang_bison.y ratlang_flex.l env.c env.h value.c value.h node.c node.h runtime.h
ratlang_LDADD = -lm -lreadline
