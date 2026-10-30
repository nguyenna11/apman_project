
#
# Compile tests and programs from default project:
#   make
#
# Compile tests from default project:
#   make tests
#
# Compile programs from default project:
#   make progs
#
# Execute tests from default project:
#   make check
#
# Clean default project:
#   make clean
#
#
# Runtime memory checks may be performed by Valgrind (if it is available) or
# AddressSanitizer (ASan), which is the default.
#
# Available options - may be overriden by command-line invocations of `make [target]`:
#
# codegen=debug|benchmark|release
#
#   => Generate code suitable for these various situations. If the codegen=
#   option is missing, use the following inline options:
#
# EDIT: C code: optimization level (default: -O0)
# C_OPT    = -O2
# EDIT: C code: do not include debug info (default: incl. debug info)
# C_DBG    = no
#
#   Presets           C_OPT  C_DBG  use_asan
#   - debug, default    -O0    yes       yes
#   - benchmark         -O3    yes        no
#   - release           -O3     no        no
#
#
# project=foo
#
#   => Load project/foo.conf config if present, or project/default.conf.
#
#
# use_asan=no
#
#   => Drop ASan and try to use Valgrind if present.
#   Historical note: In case of multiple DEADLYSIGNAL messages at runtime when
#   using ASan (typically on a Debian 12 or an Ubuntu 22.04), you may first
#   need to: `sudo sysctl vm.mmap_rnd_bits=28`
#
#
# Example: make test/foo@check project=packrat codegen=benchmark use_asan=yes
#
#
# EDIT: We could add our own additional compile/link options here:
# CFLAGS  +=
# LDFLAGS +=
# LDLIBS  += -pthread

# See project config, likely in project/default.conf!

# Load project config
include .Makefile.inc    # Common targets: all, deep-clean, check, etc.
