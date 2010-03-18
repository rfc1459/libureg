############################################################
# libureg - Test target CMake module                       #
############################################################
# Copyright 2010 Matteo Panella. All rights reserved.      #
# Distributed under a BSD-style license (see LICENSE file) #
############################################################

# Create a test executable and add it to "check" target
# "make check" should be used in place of "make test"

MACRO(CREATE_TEST_TARGET)
	IF(NOT HAVE_CHECK_TARGET)
		SET(HAVE_CHECK_TARGET "YES")
		ADD_CUSTOM_TARGET(check COMMAND ${CMAKE_CTEST_COMMAND})
	ENDIF(NOT HAVE_CHECK_TARGET)
ENDMACRO(CREATE_TEST_TARGET)

MACRO(ADD_TEST_TARGET _exe_name)
	# Fuck CMake 2.6
	CREATE_TEST_TARGET()
	ADD_EXECUTABLE(${_exe_name} EXCLUDE_FROM_ALL ${ARGN})
	TARGET_LINK_LIBRARIES(${_exe_name} ureg)
	ADD_DEPENDENCIES(check ${_exe_name})
ENDMACRO(ADD_TEST_TARGET _exe_name)
