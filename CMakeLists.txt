############################################################
# libureg CMake specifications                             #
############################################################
# Copyright 2010 Matteo Panella. All rights reserved.      #
# Distributed under a BSD-style license (see LICENSE file) #
############################################################

PROJECT(libureg)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

#### External CMake modules ####
INCLUDE(HandleBison)
INCLUDE(TestMacros)

#### Platform tests ####
FIND_BISON()

#### Enable CTest framework ####
ENABLE_TESTING()

#### Include paths et. al. ####
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR})

#### libureg target ####
SET(ureg_LIB_SRCS
compile.c
${PROJECT_BINARY_DIR}/parse.c
thompsonvm.c
ureg.c
)
ADD_BISON_SOURCE(${PROJECT_SOURCE_DIR}/parse.y ${PROJECT_BINARY_DIR}/parse.c)
ADD_LIBRARY(ureg STATIC ${ureg_LIB_SRCS})

#### Test targets ####
#ADD_TEST_TARGET(parser-suite tests/parser-suite.c)
#ADD_TEST_TARGET(compiler-suite tests/compiler-suite.c)
ADD_TEST_TARGET(api-test tests/api.c)

#### Test cases - Work in progress ####

# Parser tests
#ADD_TEST(NAME parser-basic COMMAND parser-suite basic)
#ADD_TEST(NAME parser-bracket COMMAND parser-suite bracket)
#ADD_TEST(NAME parser-corner COMMAND parser-suite corner)
#ADD_TEST(NAME parser-negbracket COMMAND parser-suite negbracket)

# Compiler tests - TBD

# Full API tests
ADD_TEST(NAME basic-match
	COMMAND api-test "he.+o" "hello world" 1)

ADD_TEST(NAME basic-nomatch
	COMMAND api-test "g.*bye" "hello there!" 0)
