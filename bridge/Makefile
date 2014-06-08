# This makefile is for bridge between
# a library stuff and stuff that the library exports to world
# and, particularly, exports into our project.
# This makefile manages header and library files we use.
# Every file in these folders updates with owner Makefile.

INCLUDE_DIR = include
LIBRARY_DIR = lib
EXTERNALS_BASEDIR = ../externals

TARGET = easybmp
.DEFAULT_GOAL := $(TARGET)

# Avoid useless remaking of this Makefile.
Makefile : ;

# Path to EasyBMP library
EASYBMP_LOCATION = $(EXTERNALS_BASEDIR)/EasyBMP
# Makefile name of EasyBMP
EASYBMP_MAKEFILE = Makefile

# Path to files in the EasyBMP directory to import in the main project
# Headers
EASYBMP_HEADERS = include/EasyBMP_BMP.h \
		  include/EasyBMP_DataStructures.h \
		  include/EasyBMP.h \
		  include/EasyBMP_VariousBMPutilities.h
# Statically compiled library (.a means archive)
EASYBMP_LIBRARY = lib/libeasybmp.a

.PHONY: easybmp
# Copy prepared for import files from EasyBMP directory
easybmp: $(EASYBMP_HEADERS) $(EASYBMP_LIBRARY)
	cp $(addprefix $(EASYBMP_LOCATION)/, $(EASYBMP_HEADERS)) $(INCLUDE_DIR)/
	cp $(addprefix $(EASYBMP_LOCATION)/, $(EASYBMP_LIBRARY)) $(LIBRARY_DIR)/

.PHONY: $(EASYBMP_HEADERS)
# Prepare library headers by calling to EasyBMP makefile
$(EASYBMP_HEADERS):
	make -C $(EASYBMP_LOCATION) -f $(EASYBMP_MAKEFILE) $@

.PHONY: $(EASYBMP_LIBRARY)
# Prepare library by calling to EasyBMP makefile
$(EASYBMP_LIBRARY):
	make -C $(EASYBMP_LOCATION) -f $(EASYBMP_MAKEFILE) $@

.PHONY: clean
clean:
	rm -f $(wildcard $(INCLUDE_DIR)/*) $(wildcard $(LIBRARY_DIR)/*)
	make -C $(EASYBMP_LOCATION) -f $(EASYBMP_MAKEFILE) $@
