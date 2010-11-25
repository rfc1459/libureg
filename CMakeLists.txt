############################################################
# libureg CMake specifications                             #
############################################################
# Copyright 2010 Matteo Panella.                           #
# Distributed under a BSD-style license (see LICENSE file) #
############################################################

PROJECT(libureg)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

#### External CMake modules ####
INCLUDE(TestMacros)
INCLUDE(LemonMacros)
INCLUDE(CheckIncludeFile)


#### Platform tests ####
CHECK_INCLUDE_FILE(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILE(strings.h HAVE_STRINGS_H)
CHECK_INCLUDE_FILE(string.h HAVE_STRING_H)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/setup.h.cmake ${PROJECT_BINARY_DIR}/setup.h)

#### Enable CTest framework ####
ENABLE_TESTING()

#### Include paths et. al. ####
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR})

#### Lemon bootstrap ####
ADD_EXECUTABLE(lemon lemon/lemon.c)

#### lemon parser ####
LEMON_PARSER(parse.y)

#### libureg target ####
SET(ureg_LIB_SRCS
ast.c
compile.c
parse.c
thompsonvm.c
ureg.c
)
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
ADD_TEST(basic-match api-test "he.+o" "hello world" 1)
ADD_TEST(basic-nomatch api-test "g.*bye" "hello there!" 0)
ADD_TEST(basic-alt-match api-test "(?:hello|goodbye) world" "hello world" 1)
ADD_TEST(basic-alt-nomatch api-test "le hai mostrato il pupp?aruolo\\\\?" "no ma ho buone possibilita'" 0)
ADD_TEST(basic-count-match api-test "a{7}b" "aaaaaaab" 1)
ADD_TEST(basic-count-nomatch api-test "a{7}b" "ab" 0)
ADD_TEST(basic-count-nohi-match api-test "a{5,}b" "aaaaaaab" 1)
ADD_TEST(basic-count-nohi-nomatch api-test "a{5,}b" "aaaab" 0)
ADD_TEST(basic-count-nolo-match api-test "a{,5}b" "b" 1)
ADD_TEST(complex-count-match api-test "(antani ?){5}" "antani antani antani antani antani" 1)
ADD_TEST(complex-count-nomatch api-test "(antani ?){5}" "antani sbiriguda antani antani antani" 0)
ADD_TEST(FFFFUUUUUUUU api-test "F{4}U{8,}" "FFFFUUUUUUUUUUUUUUUUU" 1)
ADD_TEST(FFFFUUUUUUUU-nomatch api-test "F{4}U{8,}" "FFFUUUUUUUU" 0)
