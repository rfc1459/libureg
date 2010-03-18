############################################################
# libureg - Bison CMake module                             #
############################################################
# Copyright 2010 Matteo Panella. All rights reserved.      #
# Distributed under a BSD-style license (see LICENSE file) #
############################################################

#### Locate and cache GNU Bison executable path ####
MACRO(FIND_BISON)
	IF(NOT BISON_EXECUTABLE)
		MESSAGE("-- Looking for bison executable")
		FIND_PROGRAM(BISON_EXECUTABLE bison)
		IF(NOT BISON_EXECUTABLE)
			MESSAGE(FATAL_ERROR "bison not found - aborting")
		ELSE(NOT BISON_EXECUTABLE)
			MESSAGE("-- Looking for bison executable - found")
		ENDIF(NOT BISON_EXECUTABLE)
	ENDIF(NOT BISON_EXECUTABLE)
ENDMACRO(FIND_BISON)

#### Wrapper for Bison sources ####
MACRO(ADD_BISON_SOURCE f_src f_dest)
	ADD_CUSTOM_COMMAND(SOURCE ${f_src}
		COMMAND ${BISON_EXECUTABLE}
		ARGS ${f_src} -o ${f_dest}
		OUTPUT ${f_dest}
		DEPENDS ${f_src})
	SET_SOURCE_FILES_PROPERTIES(${f_dest} GENERATED)
ENDMACRO(ADD_BISON_SOURCE f_src f_dest)
