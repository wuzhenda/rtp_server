
CROSS  = /usr/local/codesourcery/mips-4.3/bin/mips-linux-gnu-

TOOL_FLAG = -muclibc
TOOL_FLAG =

export CC  = $(CROSS)gcc -EL $(TOOL_FLAG)
export CXX = $(CROSS)g++ -EL $(TOOL_FLAG)
export AR := $(CROSS)ar
export AS := $(CROSS)as
export STRIP := $(CROSS)strip
export CPP = $(CC)
export OBJCOPY  = $(CROSS)objcopy
export OBJDUMP  = $(CROSS)objdump
export NM       = $(CROSS)nm
export LD       = $(CROSS)ld -EL
export RANLIB    = $(CROSS)ranlib
export LIB_DIR  = $(PUBLIC_DIR)/lib/glibc

OPTIMIZE_DEFINES = -O2

CFLAGS =  -Wall -O2 -MD -g -mips32r2 -march=mips32r2 \
          -fno-builtin-cos -fno-builtin-sin  -mhard-float -fshort-wchar\
           $(INC) $(MACRODEF) $(OPTIMIZE_DEFINES)

AFLAGS ?= $(INC) -EL -mips1 -c

#######################################################################
#          option for linker
#######################################################################
LIBS =  -lpthread -lstdc++ -lrt -ldl  \
                 -lm -lmt_common  -lcrypto  -lssl \
         -lmt_mpi -lmt_ecs -lz -losal_mtos -lavcpu

LIBDIR = -L$(TOP_DIR)/lib/concerto \
         -L$(TOP_DIR)/driver/module_drv/concerto/sdk/lib/glibc \
         -L$(FLAX_DIR)/platform/public/lib/glibc

#####################################################################3
#              Application Name
#####################################################################33


release:
        $(CC)  rtp_server.c  -Wall -o rtp_server   -Wl,--start-group  -Wl,--end-group $(LIBDIR) -static
