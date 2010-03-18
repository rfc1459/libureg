############################################################
# libureg - Test target CMake module                       #
############################################################
# Copyright 2010 Matteo Panella. All rights reserved.      #
# Distributed under a BSD-style license (see LICENSE file) #
############################################################

# Create a test executable and add it to "check" target
# "make check" should be used in place of "make test"
MACRO(ADD_TEST_TARGET _exe_name)
	IF(NOT TARGET check)
		ADD_CUSTOM_TARGET(check COMMAND ${CMAKE_CTEST_COMMAND})
	ENDIF(NOT TARGET check)
	ADD_EXECUTABLE(${_exe_name} EXCLUDE_FROM_ALL ${ARGN})
	TARGET_LINK_LIBRARIES(${_exe_name} ureg)
	ADD_DEPENDENCIES(check ${_exe_name})
ENDMACRO(ADD_TEST_TARGET _exe_name)
