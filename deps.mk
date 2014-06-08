# This content is separated from main Makefile for strict
# order of inclusions:
# we need to make bridge files *BEFORE* all dependency generation
# proceed, so following inclusions will be made only after touch
# target.

# Include dependency information (force to build if missing).
ifneq ($(MAKECMDGOALS), clean)
-include $(call src_to_dep, $(CXXFILES))
endif

# Modeline for vim
# vim: set syntax=make:
