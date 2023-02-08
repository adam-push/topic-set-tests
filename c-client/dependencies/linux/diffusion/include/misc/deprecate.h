/*
 * @file deprecate.h
 *
 * Macros to be used when deprecating functions.
 *
 * Copyright © 2018, 2021 Push Technology Ltd., All Rights Reserved.
 *
 * Use is subject to license terms.
 *
 * NOTICE: All information contained herein is, and remains the
 * property of Push Technology. The intellectual and technical
 * concepts contained herein are proprietary to Push Technology and
 * may be covered by U.S. and Foreign Patents, patents in process, and
 * are protected by trade secret or copyright law.
 */

#ifndef _diffusion_deprecate_h_
#define _diffusion_deprecate_h_ 1


#if (defined __GNUC__) || (defined __llvm__)
        #define DEPRECATED(FUNC) FUNC __attribute__ ((deprecated));
#elif defined(_MSC_VER)
        #define DEPRECATED(FUNC) __declspec(deprecated) FUNC;
#else
        #define DEPRECATED(FUNC) FUNC; /* passthrough */
#endif


#if (defined __GNUC__) || (defined __llvm__)
        #if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
                #define DECLARATION_DEPRECATED(FUNC)                                        \
                        _Pragma("GCC diagnostic push")                                      \
                        _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")     \
                        FUNC __attribute__ ((deprecated));                                  \
                        _Pragma("GCC diagnostic pop")
        #else
                #define DECLARATION_DEPRECATED(FUNC)                                        \
                        _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")     \
                        FUNC __attribute__ ((deprecated));
        #endif
#elif defined(_MSC_VER)
        #define DECLARATION_DEPRECATED(FUNC)                                        \
                __pragma("GCC diagnostic push")                                     \
                __pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")    \
                __declspec(deprecated) FUNC;                                        \
                __pragma("GCC diagnostic pop")
#else
        #define DECLARATION_DEPRECATED(FUNC) FUNC; /* passthrough */
#endif


#if (defined __GNUC__) || (defined __llvm__)
        #define ATTR_DEPRECATED __attribute__((deprecated))
#else
        #define ATTR_DEPRECATED  /* passthrough */
#endif


#if (defined __llvm__)
        #define ENUM_DEPRECATED(msg) __deprecated_enum_msg(msg)
#else
        #define ENUM_DEPRECATED(msg) __attribute__ ((deprecated (msg)))
#endif


#if (defined __GNUC__) || (defined __llvm__)
        #define TYPEDEF_DEPRECATED(RETURN_TYPE, FUNC) typedef RETURN_TYPE __attribute__ ((deprecated)) FUNC;
#elif defined(_MSC_VER)
        #define TYPEDEF_DEPRECATED(RETURN_TYPE, FUNC) typedef __declspec(deprecated) RETURN_TYPE FUNC;
#else
        #define TYPEDEF_DEPRECATED(RETURN_TYPE, FUNC) typedef RETURN_TYPE FUNC; /* passthrough */
#endif


#endif
