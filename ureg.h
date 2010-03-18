/** @file ureg.h 
 *  @brief libureg public API.
 *
 * Copyright 2010 Matteo Panella. All Rights Reserved.
 * Based on code by Russ Cox.
 * Use of this code is governed by a BSD-style license.
 */

#ifndef INCLUDED_ureg_h
#define INCLUDED_ureg_h

/**
 * @addtogroup types Types and variables
 */
/** @{ */

/** @brief Opaque handler to a compiled regexp.
 *
 *  This struct should be used only in conjunction with ureg_* methods.
 *  @sa ureg_compile(), ureg_free()
 */
typedef struct ureg_regexp_t *ureg_regexp;

/** @brief Error codes.
 *  @sa ureg_errno
 */
typedef enum ureg_error_t
{
	/** @brief No error */
	UREG_NOERROR = 0,
	/** @brief Invalid argument (NULL pointer) */	
	UREG_ERR_NULL,
	/** @brief Out of memory */
	UREG_ERR_NOMEM,
	/** @brief Syntax error */
	UREG_ERR_SYNTAX,
	/** @brief Compiler error, please report this */
	UREG_ERR_COMPILE
} ureg_error_t;

/** @brief Last error code */
extern ureg_error_t ureg_errno;

/** @} */

/**
 * @addtogroup functions
 */

/** @{ */

/** @brief Compile a regexp.
 *
 *  @param pattern pattern being compiled.
 *  @param flags flags for parser/compiler (currently unused)
 *  @return A compiled regexp handler.
 *  @sa ureg_free(), ureg_match()
 */
extern ureg_regexp ureg_compile(const char *pattern, unsigned int flags);

/** @brief Destroy a previously compiled regexp and free its memory.
 *  @param handle Regexp handle being free()'d.
 *  @sa ureg_compile()
 */
extern void ureg_free(ureg_regexp handle);

/** @brief Match a string against a given compiled regexp.
 *  @param handle Handle to regexp.
 *  @param str string being tested.
 *  @return 1 if str matches, 0 if it does not match, -1 on error.
 */
extern int ureg_match(ureg_regexp handle, const char *str);

/** @brief Get the original pattern for a given compiled regexp.
 *
 *  The returned pointer is valid until the underlying regexp object
 *  is freed with ureg_free().
 *  @param handle A regexp handle
 *  @return Pointer to a copy of the original pattern.
 */
extern const char *ureg_txt(ureg_regexp handle);

/** @} */

#endif /* INCLUDED_ureg_h */
