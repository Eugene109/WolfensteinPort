# ----------------------------
# Makefile Options
# ----------------------------

NAME = WOLF
ICON = icon.png
DESCRIPTION = "CE C Toolchain Demo"
COMPRESSED = NO
ARCHIVED = NO

# OPTIMIZE_FLAG = -ggdb3 -O3

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)