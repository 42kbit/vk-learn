#ifndef __H_SRC_MAIN_INCLUDE_ISDEFINED_H
#define __H_SRC_MAIN_INCLUDE_ISDEFINED_H

/* Very smart Linus, very smart! */

#define __ARG_PLACEHOLDER_1 0,
#define ___config_enabled(__ignored, val, ...) val
#define __config_enabled(arg1_or_junk) ___config_enabled(arg1_or_junk 1, 0)
#define _config_enabled(value) __config_enabled(__ARG_PLACEHOLDER_##value)
#define config_enabled(cfg) _config_enabled(cfg)

#define IS_DEFINED(cfg) config_enabled (cfg)

/* MSVC without /Zc:preprocessor responds on __config_enabled(__ARG_PLACEHOLDER_1)
 * with 2 tokens "0, 1" and "1" (first coma is ignored), so strict preprocessor option
 * is needed.
 * 
 * Following code validates __config_enabled(__ARG_PLACEHOLDER_1), if validation fails:
 * throws #error
*/
#ifdef __RESERVED_TOKEN_TESTER
#error "__RESERVED_TOKEN_TESTER should NOT be defined."
#endif /* __RESERVED_TOKEN_TESTER */

#define __RESERVED_TOKEN_TESTER 1
#if config_enabled (__RESERVED_TOKEN_TESTER) != 1
#error "Preprocessor token order is INVALID! If you use MSVC make sure to compile with /Zc:preprocessor"
#endif
#undef __RESERVED_TOKEN_TESTER

#endif /* __H_SRC_MAIN_INCLUDE_ISDEFINED_H */