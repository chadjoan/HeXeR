/// This modules provides preprocessor macros that are distinct from HeXeR's
/// intended focus, but are nonetheless important or requisite for implementing
/// HeXeR's primary features. This module should have no dependencies, and
/// does not depend on HeXeR's compilable-and-executable code (`hexer.c`) or its
/// header (`hexer.h`).
///
/// As such, if one does not desire the features provided in `hexer.c` and
/// `hexer.h`, but wishes to take advantage of HeXeR's preprocessor
/// metaprogramming offerings, then including `pp_core.h` alone, or with other
/// `pp_*.h` modules, is a valid and perhaps sensible strategy.
///
#ifndef HXR_PP_CORE_H
#define HXR_PP_CORE_H


#if defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL
#	define HXR_MSVC_EXPAND_VA(...)  __VA_ARGS__
#	define HXR_MSVC_EXPAND_X(x)     x
#endif

// ========================== Macro overloading ==========================
// Variadic argument counting and extraction taken from these posts:
// https://groups.google.com/g/comp.std.c/c/d-6Mj5Lko_s
// https://stackoverflow.com/a/26408195

#define HXR__ARG_N( \
	_1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
	_11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
	_21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
	_31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
	_41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
	_51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
	_61,_62,_63,N,...) N

// For distinguishing between single-argument and multi-argument cases.
// (Also distinguishes the 0-argument case with "00". Note, however, that
// passing 0-arguments to variadic macros should be avoided, because 0-argument
// variadic macros could lead to undefined behavior in standard C99, even if
// they work on some compilers (ex: GCC).)
//
#define HXR__RSEQ_SINGLE() \
	0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,0,0, \
	0,0,0,0,0,0,0,0,1,00

#define HXR__RSEQ_N() \
	63,62,61,60, \
	59,58,57,56,55,54,53,52,51,50, \
	49,48,47,46,45,44,43,42,41,40, \
	39,38,37,36,35,34,33,32,31,30, \
	29,28,27,26,25,24,23,22,21,20, \
	19,18,17,16,15,14,13,12,11,10, \
	9,8,7,6,5,4,3,2,1,0

#define HXR__RSEQ_NN() \
	63,62,61,60, \
	59,58,57,56,55,54,53,52,51,50, \
	49,48,47,46,45,44,43,42,41,40, \
	39,38,37,36,35,34,33,32,31,30, \
	29,28,27,26,25,24,23,22,21,20, \
	19,18,17,16,15,14,13,12,11,10, \
	09,08,07,06,05,04,03,02,01,00

#if defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL
#	define HXR_NARG_(...)     HXR_MSVC_EXPAND_X(HXR__ARG_N(__VA_ARGS__))
#else
#	define HXR_NARG_(...)     HXR__ARG_N(__VA_ARGS__)
#endif

#define HXR_SARG(...)         HXR_NARG_(__VA_ARGS__,HXR__RSEQ_SINGLE())
#define HXR_NARG(...)         HXR_NARG_(__VA_ARGS__,HXR__RSEQ_N())
#define HXR_NNARG(...)        HXR_NARG_(__VA_ARGS__, HXR__RSEQ_NN())

#define HXR_MACRO_OVERLOAD_NX(name, n) name##n
#define HXR_MACRO_OVERLOAD_N(name, n)  HXR_MACRO_OVERLOAD_NX(name, n)

#if defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL
// The MSVC-traditional versions require additional macro expansions,
// so while they /should/ work in a std-c99 compliant preprocessor,
// we put them on their own path so that the macros for the compliant
// processor can execute faster or define fewer symbols
// (not sure if it matters, but may as well).
#	define HXR_OVERLOAD_CALL(macro_overload, args)          HXR_MSVC_EXPAND_VA(macro_overload args)
#	define HXR_MACRO_OVERLOAD0(macro_name, ...)             HXR_OVERLOAD_CALL(HXR_MACRO_OVERLOAD_N(macro_name, HXR_NARG(__VA_ARGS__)), (__VA_ARGS__))
#	define HXR_MACRO_OVERLOAD(macro_name, ...)              HXR_MACRO_OVERLOAD0(macro_name, __VA_ARGS__)
#	define HXR_MACRO_OVERLOAD00(macro_name, ...)            HXR_OVERLOAD_CALL(HXR_MACRO_OVERLOAD_N(macro_name, HXR_NNARG(__VA_ARGS__)), (__VA_ARGS__))
#	define HXR_MACRO_OVERLOAD_SINGLE_ARG_(macro_name, ...)  HXR_OVERLOAD_CALL(HXR_MACRO_OVERLOAD_N(macro_name, HXR_SARG(__VA_ARGS__)), (__VA_ARGS__))
#	define HXR_MACRO_OVERLOAD_SINGLE_ARG(macro_name, ...)   HXR_MACRO_OVERLOAD_SINGLE_ARG_(macro_name, __VA_ARGS__)
#else

/// Expands as `macro_nameN(...)` where N is the number of variadic macro
/// arguments that were passed (that is, the number of arguments passed to this
/// macro that are following the `macro_name` argument).
///
/// This allows for the implementation of a macro API that looks like it had
/// been created with a feature similar to that of function overloads in some
/// programming languages: functions with the same name and different parameters
/// are considered as distinct functions, allowing the caller to call a different
/// function of the same name by changing the number of arguments passed.
///
/// In simpler settings, this can be used to implement things like optional arguments.
///
/// As many as 64 arguments may be passed using this macro. Passing any more
/// will result in undefined behavior.
///
/// See also: HXR_MACRO_OVERLOAD_SINGLE_ARG, HXR_MACRO_OVERLOAD00
#define HXR_MACRO_OVERLOAD0(macro_name, ...)  HXR_MACRO_OVERLOAD_N(macro_name, HXR_NARG(__VA_ARGS__)) (__VA_ARGS__)

/// ditto
#define HXR_MACRO_OVERLOAD(macro_name, ...)   HXR_MACRO_OVERLOAD0(macro_name, __VA_ARGS__)

/// Expands as `macro_nameNN(...)` where NN is the number of variadic macro
/// arguments that were passed (that is, the number of arguments passed to this
/// macro that are following the `macro_name` argument).
///
/// The end result is almost identical to `HXR_MACRO_OVERLOAD(macro_name,...)`,
/// except that if the number of variadic arguments is less than 10, the number
/// will have a leading 0 on it, so that the NN part of the generated macro's
/// name is always 2 digits wide for any argument count (at least any argument
/// count under 100, which is unreachable anyways because argument counts
/// greater than 64 are unsupported).
///
#define HXR_MACRO_OVERLOAD00(macro_name, ...) HXR_MACRO_OVERLOAD_N(macro_name, HXR_NNARG(__VA_ARGS__)) (__VA_ARGS__)

#define HXR_MACRO_OVERLOAD_SINGLE_ARG_(macro_name, ...) HXR_MACRO_OVERLOAD_N(macro_name, HXR_SARG(__VA_ARGS__)) (__VA_ARGS__)

/// Expands as `macro_name1(...)` if there is a single argument,
/// otherwise expands as `macro_name0(...)` if there is more than one argument.
///
/// If there are 0 arguments, expands as `macro_name00(...)`.
/// Note, however, that 0-arguments to variadic macros should be avoided,
/// because 0-argument variadic macros could lead to undefined behavior in
/// standard C99, even if they work on some compilers (ex: GCC).
///
/// As many as 64 arguments may be passed using this macro. Passing any more
/// will result in undefined behavior.
///
#define HXR_MACRO_OVERLOAD_SINGLE_ARG(macro_name, ...) HXR_MACRO_OVERLOAD_SINGLE_ARG_(macro_name, __VA_ARGS__)

#endif /* defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL */

// =================== Exposing basic macro operators ===================

#define HXR_STRINGIZE_1(token) #token
//#define HXR_STRINGIZE_0(token) HXR_STRINGIZE_1(token)
#define HXR_STRINGIZE(token)   HXR_STRINGIZE_1(token)

#define HXR_CONCAT_16(_F,_E,_D,_C,_B,_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _F##_E##_D##_C##_B##_A##_9##_8##_7##_6##_5##_4##_3##_2##_1##_0
#define HXR_CONCAT_15(   _E,_D,_C,_B,_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)     _E##_D##_C##_B##_A##_9##_8##_7##_6##_5##_4##_3##_2##_1##_0
#define HXR_CONCAT_14(      _D,_C,_B,_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)         _D##_C##_B##_A##_9##_8##_7##_6##_5##_4##_3##_2##_1##_0
#define HXR_CONCAT_13(         _C,_B,_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)             _C##_B##_A##_9##_8##_7##_6##_5##_4##_3##_2##_1##_0
#define HXR_CONCAT_12(            _B,_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)                 _B##_A##_9##_8##_7##_6##_5##_4##_3##_2##_1##_0
#define HXR_CONCAT_11(               _A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)                     _A##_9##_8##_7##_6##_5##_4##_3##_2##_1##_0
#define HXR_CONCAT_10(                  _9,_8,_7,_6,_5,_4,_3,_2,_1,_0)                         _9##_8##_7##_6##_5##_4##_3##_2##_1##_0
#define HXR_CONCAT_09(                     _8,_7,_6,_5,_4,_3,_2,_1,_0)                             _8##_7##_6##_5##_4##_3##_2##_1##_0
#define HXR_CONCAT_08(                        _7,_6,_5,_4,_3,_2,_1,_0)                                 _7##_6##_5##_4##_3##_2##_1##_0
#define HXR_CONCAT_07(                           _6,_5,_4,_3,_2,_1,_0)                                     _6##_5##_4##_3##_2##_1##_0
#define HXR_CONCAT_06(                              _5,_4,_3,_2,_1,_0)                                         _5##_4##_3##_2##_1##_0
#define HXR_CONCAT_05(                                 _4,_3,_2,_1,_0)                                             _4##_3##_2##_1##_0
#define HXR_CONCAT_04(                                    _3,_2,_1,_0)                                                 _3##_2##_1##_0
#define HXR_CONCAT_03(                                       _2,_1,_0)                                                     _2##_1##_0
#define HXR_CONCAT_02(                                          _1,_0)                                                         _1##_0
#define HXR_CONCAT_01(                                             _0)                                                             _0

/// Concatenates up to 16 arguments together using the preprocessor's token
/// concatenation operation (##).
#define HXR_CONCAT(...) HXR_MACRO_OVERLOAD00(HXR_CONCAT_, __VA_ARGS__)
//#define HXR_CONCAT_NN(...) HXR_MACRO_OVERLOAD00(HXR_CONCAT_, __VA_ARGS__)
//#define HXR_CONCAT(...) HXR_CONCAT_NN(__VA_ARGS__)

// ============================= Arithmatic =============================
// ----------------------- Comma-Separated Digits -----------------------
// ......................................................................

// TODO: I might replace this with comma-separated-hex at some point.
// CSD operations would still exist, but they'd be moved out of pp_core.h.
#define HXR_ADD_DIGITS_00 0,0
#define HXR_ADD_DIGITS_01 0,1
#define HXR_ADD_DIGITS_02 0,2
#define HXR_ADD_DIGITS_03 0,3
#define HXR_ADD_DIGITS_04 0,4
#define HXR_ADD_DIGITS_05 0,5
#define HXR_ADD_DIGITS_06 0,6
#define HXR_ADD_DIGITS_07 0,7
#define HXR_ADD_DIGITS_08 0,8
#define HXR_ADD_DIGITS_09 0,9

#define HXR_ADD_DIGITS_10 0,1
#define HXR_ADD_DIGITS_11 0,2
#define HXR_ADD_DIGITS_12 0,3
#define HXR_ADD_DIGITS_13 0,4
#define HXR_ADD_DIGITS_14 0,5
#define HXR_ADD_DIGITS_15 0,6
#define HXR_ADD_DIGITS_16 0,7
#define HXR_ADD_DIGITS_17 0,8
#define HXR_ADD_DIGITS_18 0,9
#define HXR_ADD_DIGITS_19 1,0

#define HXR_ADD_DIGITS_20 0,2
#define HXR_ADD_DIGITS_21 0,3
#define HXR_ADD_DIGITS_22 0,4
#define HXR_ADD_DIGITS_23 0,5
#define HXR_ADD_DIGITS_24 0,6
#define HXR_ADD_DIGITS_25 0,7
#define HXR_ADD_DIGITS_26 0,8
#define HXR_ADD_DIGITS_27 0,9
#define HXR_ADD_DIGITS_28 1,0
#define HXR_ADD_DIGITS_29 1,1

#define HXR_ADD_DIGITS_30 0,3
#define HXR_ADD_DIGITS_31 0,4
#define HXR_ADD_DIGITS_32 0,5
#define HXR_ADD_DIGITS_33 0,6
#define HXR_ADD_DIGITS_34 0,7
#define HXR_ADD_DIGITS_35 0,8
#define HXR_ADD_DIGITS_36 0,9
#define HXR_ADD_DIGITS_37 1,0
#define HXR_ADD_DIGITS_38 1,1
#define HXR_ADD_DIGITS_39 1,2

#define HXR_ADD_DIGITS_40 0,4
#define HXR_ADD_DIGITS_41 0,5
#define HXR_ADD_DIGITS_42 0,6
#define HXR_ADD_DIGITS_43 0,7
#define HXR_ADD_DIGITS_44 0,8
#define HXR_ADD_DIGITS_45 0,9
#define HXR_ADD_DIGITS_46 1,0
#define HXR_ADD_DIGITS_47 1,1
#define HXR_ADD_DIGITS_48 1,2
#define HXR_ADD_DIGITS_49 1,3

#define HXR_ADD_DIGITS_50 0,5
#define HXR_ADD_DIGITS_51 0,6
#define HXR_ADD_DIGITS_52 0,7
#define HXR_ADD_DIGITS_53 0,8
#define HXR_ADD_DIGITS_54 0,9
#define HXR_ADD_DIGITS_55 1,0
#define HXR_ADD_DIGITS_56 1,1
#define HXR_ADD_DIGITS_57 1,2
#define HXR_ADD_DIGITS_58 1,3
#define HXR_ADD_DIGITS_59 1,4

#define HXR_ADD_DIGITS_60 0,6
#define HXR_ADD_DIGITS_61 0,7
#define HXR_ADD_DIGITS_62 0,8
#define HXR_ADD_DIGITS_63 0,9
#define HXR_ADD_DIGITS_64 1,0
#define HXR_ADD_DIGITS_65 1,1
#define HXR_ADD_DIGITS_66 1,2
#define HXR_ADD_DIGITS_67 1,3
#define HXR_ADD_DIGITS_68 1,4
#define HXR_ADD_DIGITS_69 1,5

#define HXR_ADD_DIGITS_70 0,7
#define HXR_ADD_DIGITS_71 0,8
#define HXR_ADD_DIGITS_72 0,9
#define HXR_ADD_DIGITS_73 1,0
#define HXR_ADD_DIGITS_74 1,1
#define HXR_ADD_DIGITS_75 1,2
#define HXR_ADD_DIGITS_76 1,3
#define HXR_ADD_DIGITS_77 1,4
#define HXR_ADD_DIGITS_78 1,5
#define HXR_ADD_DIGITS_79 1,6

#define HXR_ADD_DIGITS_80 0,8
#define HXR_ADD_DIGITS_81 0,9
#define HXR_ADD_DIGITS_82 1,0
#define HXR_ADD_DIGITS_83 1,1
#define HXR_ADD_DIGITS_84 1,2
#define HXR_ADD_DIGITS_85 1,3
#define HXR_ADD_DIGITS_86 1,4
#define HXR_ADD_DIGITS_87 1,5
#define HXR_ADD_DIGITS_88 1,6
#define HXR_ADD_DIGITS_89 1,7

#define HXR_ADD_DIGITS_90 0,9
#define HXR_ADD_DIGITS_91 1,0
#define HXR_ADD_DIGITS_92 1,1
#define HXR_ADD_DIGITS_93 1,2
#define HXR_ADD_DIGITS_94 1,3
#define HXR_ADD_DIGITS_95 1,4
#define HXR_ADD_DIGITS_96 1,5
#define HXR_ADD_DIGITS_97 1,6
#define HXR_ADD_DIGITS_98 1,7
#define HXR_ADD_DIGITS_99 1,8


#define HXR_IMPL_TRIM_CSD_BY_01_0(_N,...)    __VA_ARGS__
#define HXR_IMPL_TRIM_CSD_BY_01_1(_N,...) _N,__VA_ARGS__
#define HXR_IMPL_TRIM_CSD_BY_01_2(_N,...) _N,__VA_ARGS__
#define HXR_IMPL_TRIM_CSD_BY_01_3(_N,...) _N,__VA_ARGS__
#define HXR_IMPL_TRIM_CSD_BY_01_4(_N,...) _N,__VA_ARGS__
#define HXR_IMPL_TRIM_CSD_BY_01_5(_N,...) _N,__VA_ARGS__
#define HXR_IMPL_TRIM_CSD_BY_01_6(_N,...) _N,__VA_ARGS__
#define HXR_IMPL_TRIM_CSD_BY_01_7(_N,...) _N,__VA_ARGS__
#define HXR_IMPL_TRIM_CSD_BY_01_8(_N,...) _N,__VA_ARGS__
#define HXR_IMPL_TRIM_CSD_BY_01_9(_N,...) _N,__VA_ARGS__

#if defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL
#	define HXR_IMPL_TRIM_CSD_BY_01_SWITCH_X(_D,...) \
		HXR_MACRO_OVERLOAD_N(HXR_IMPL_TRIM_CSD_BY_01_,_D) (_D,__VA_ARGS__)
#	define HXR_IMPL_TRIM_CSD_BY_01_SWITCH(args) \
		HXR_MSVC_EXPAND_X(HXR_IMPL_TRIM_CSD_BY_01_SWITCH_X(args))
#else
#	define HXR_IMPL_TRIM_CSD_BY_01_SWITCH(_D,...) \
		HXR_MACRO_OVERLOAD_N(HXR_IMPL_TRIM_CSD_BY_01_,_D) (_D,__VA_ARGS__)
#endif

#define HXR_IMPL_TRIM_CSD_BY_01(...)     HXR_IMPL_TRIM_CSD_BY_01_SWITCH(__VA_ARGS__)
#define HXR_TRIM_CSD_BY_01_LIMIT_00()
#define HXR_TRIM_CSD_BY_01_LIMIT_1(_0)      _0
#define HXR_TRIM_CSD_BY_01_LIMIT_0(_D, ...) HXR_IMPL_TRIM_CSD_BY_01(_D, __VA_ARGS__)
#define HXR_TRIM_CSD_BY_01(...) HXR_MACRO_OVERLOAD_SINGLE_ARG(HXR_TRIM_CSD_BY_01_LIMIT_, __VA_ARGS__)

#define HXR_IMPL_TRIM_CSD_BY_02(...) HXR_IMPL_TRIM_CSD_BY_01(HXR_IMPL_TRIM_CSD_BY_01(__VA_ARGS__))
#define HXR_IMPL_TRIM_CSD_BY_03(...) HXR_IMPL_TRIM_CSD_BY_01(HXR_IMPL_TRIM_CSD_BY_02(__VA_ARGS__))
#define HXR_IMPL_TRIM_CSD_BY_04(...) HXR_IMPL_TRIM_CSD_BY_01(HXR_IMPL_TRIM_CSD_BY_03(__VA_ARGS__))
#define HXR_IMPL_TRIM_CSD_BY_05(...) HXR_IMPL_TRIM_CSD_BY_01(HXR_IMPL_TRIM_CSD_BY_04(__VA_ARGS__))
#define HXR_IMPL_TRIM_CSD_BY_06(...) HXR_IMPL_TRIM_CSD_BY_01(HXR_IMPL_TRIM_CSD_BY_05(__VA_ARGS__))
#define HXR_IMPL_TRIM_CSD_BY_07(...) HXR_IMPL_TRIM_CSD_BY_01(HXR_IMPL_TRIM_CSD_BY_06(__VA_ARGS__))
#define HXR_IMPL_TRIM_CSD_BY_08(...) HXR_IMPL_TRIM_CSD_BY_01(HXR_IMPL_TRIM_CSD_BY_07(__VA_ARGS__))
#define HXR_IMPL_TRIM_CSD_BY_09(...) HXR_IMPL_TRIM_CSD_BY_01(HXR_IMPL_TRIM_CSD_BY_08(__VA_ARGS__))
#define HXR_IMPL_TRIM_CSD_BY_10(...) HXR_IMPL_TRIM_CSD_BY_01(HXR_IMPL_TRIM_CSD_BY_09(__VA_ARGS__))

#define HXR_TRIM_CSD_BY_02(...) HXR_TRIM_CSD_BY_01(HXR_TRIM_CSD_BY_01(__VA_ARGS__))
#define HXR_TRIM_CSD_BY_03(...) HXR_TRIM_CSD_BY_01(HXR_TRIM_CSD_BY_02(__VA_ARGS__))
#define HXR_TRIM_CSD_BY_04(...) HXR_TRIM_CSD_BY_01(HXR_TRIM_CSD_BY_03(__VA_ARGS__))
#define HXR_TRIM_CSD_BY_05(...) HXR_TRIM_CSD_BY_01(HXR_TRIM_CSD_BY_04(__VA_ARGS__))
#define HXR_TRIM_CSD_BY_06(...) HXR_TRIM_CSD_BY_01(HXR_TRIM_CSD_BY_05(__VA_ARGS__))
#define HXR_TRIM_CSD_BY_07(...) HXR_TRIM_CSD_BY_01(HXR_TRIM_CSD_BY_06(__VA_ARGS__))
#define HXR_TRIM_CSD_BY_08(...) HXR_TRIM_CSD_BY_01(HXR_TRIM_CSD_BY_07(__VA_ARGS__))
#define HXR_TRIM_CSD_BY_09(...) HXR_TRIM_CSD_BY_01(HXR_TRIM_CSD_BY_08(__VA_ARGS__))
#define HXR_TRIM_CSD_BY_10(...) HXR_TRIM_CSD_BY_01(HXR_TRIM_CSD_BY_09(__VA_ARGS__))

#define HXR_TRIM_CSD01(                           _0)                                                      _0
#define HXR_TRIM_CSD02(                        _1,_0)   HXR_IMPL_TRIM_CSD_BY_01(                        _1,_0)
#define HXR_TRIM_CSD03(                     _2,_1,_0)   HXR_IMPL_TRIM_CSD_BY_02(                     _2,_1,_0)
#define HXR_TRIM_CSD04(                  _3,_2,_1,_0)   HXR_IMPL_TRIM_CSD_BY_03(                  _3,_2,_1,_0)
#define HXR_TRIM_CSD05(               _4,_3,_2,_1,_0)   HXR_IMPL_TRIM_CSD_BY_04(               _4,_3,_2,_1,_0)
#define HXR_TRIM_CSD06(            _5,_4,_3,_2,_1,_0)   HXR_IMPL_TRIM_CSD_BY_05(            _5,_4,_3,_2,_1,_0)
#define HXR_TRIM_CSD07(         _6,_5,_4,_3,_2,_1,_0)   HXR_IMPL_TRIM_CSD_BY_06(         _6,_5,_4,_3,_2,_1,_0)
#define HXR_TRIM_CSD08(      _7,_6,_5,_4,_3,_2,_1,_0)   HXR_IMPL_TRIM_CSD_BY_07(      _7,_6,_5,_4,_3,_2,_1,_0)
#define HXR_TRIM_CSD09(   _8,_7,_6,_5,_4,_3,_2,_1,_0)   HXR_IMPL_TRIM_CSD_BY_08(   _8,_7,_6,_5,_4,_3,_2,_1,_0)
#define HXR_TRIM_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)   HXR_IMPL_TRIM_CSD_BY_09(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)

#define HXR_TRIM_CSD(...) HXR_MACRO_OVERLOAD00(HXR_TRIM_CSD, __VA_ARGS__)

#define HXR_IMPL_CAST_TO_CSD10_FROM_CSD11(_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _9,_8,_7,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD09_FROM_CSD11(_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)    _8,_7,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD08_FROM_CSD11(_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)       _7,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD07_FROM_CSD11(_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)          _6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD06_FROM_CSD11(_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)             _5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD05_FROM_CSD11(_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)                _4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD04_FROM_CSD11(_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)                   _3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD03_FROM_CSD11(_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)                      _2,_1,_0
#define HXR_IMPL_CAST_TO_CSD02_FROM_CSD11(_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)                         _1,_0
#define HXR_IMPL_CAST_TO_CSD01_FROM_CSD11(_A,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)                            _0

#define HXR_IMPL_CAST_TO_CSD10_FROM_CSD10(   _9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _9,_8,_7,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD09_FROM_CSD10(   _9,_8,_7,_6,_5,_4,_3,_2,_1,_0)    _8,_7,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD08_FROM_CSD10(   _9,_8,_7,_6,_5,_4,_3,_2,_1,_0)       _7,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD07_FROM_CSD10(   _9,_8,_7,_6,_5,_4,_3,_2,_1,_0)          _6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD06_FROM_CSD10(   _9,_8,_7,_6,_5,_4,_3,_2,_1,_0)             _5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD05_FROM_CSD10(   _9,_8,_7,_6,_5,_4,_3,_2,_1,_0)                _4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD04_FROM_CSD10(   _9,_8,_7,_6,_5,_4,_3,_2,_1,_0)                   _3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD03_FROM_CSD10(   _9,_8,_7,_6,_5,_4,_3,_2,_1,_0)                      _2,_1,_0
#define HXR_IMPL_CAST_TO_CSD02_FROM_CSD10(   _9,_8,_7,_6,_5,_4,_3,_2,_1,_0)                         _1,_0
#define HXR_IMPL_CAST_TO_CSD01_FROM_CSD10(   _9,_8,_7,_6,_5,_4,_3,_2,_1,_0)                            _0

#define HXR_IMPL_CAST_TO_CSD10_FROM_CSD09(      _8,_7,_6,_5,_4,_3,_2,_1,_0)  0,_8,_7,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD09_FROM_CSD09(      _8,_7,_6,_5,_4,_3,_2,_1,_0)    _8,_7,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD08_FROM_CSD09(      _8,_7,_6,_5,_4,_3,_2,_1,_0)       _7,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD07_FROM_CSD09(      _8,_7,_6,_5,_4,_3,_2,_1,_0)          _6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD06_FROM_CSD09(      _8,_7,_6,_5,_4,_3,_2,_1,_0)             _5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD05_FROM_CSD09(      _8,_7,_6,_5,_4,_3,_2,_1,_0)                _4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD04_FROM_CSD09(      _8,_7,_6,_5,_4,_3,_2,_1,_0)                   _3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD03_FROM_CSD09(      _8,_7,_6,_5,_4,_3,_2,_1,_0)                      _2,_1,_0
#define HXR_IMPL_CAST_TO_CSD02_FROM_CSD09(      _8,_7,_6,_5,_4,_3,_2,_1,_0)                         _1,_0
#define HXR_IMPL_CAST_TO_CSD01_FROM_CSD09(      _8,_7,_6,_5,_4,_3,_2,_1,_0)                            _0

#define HXR_IMPL_CAST_TO_CSD10_FROM_CSD08(         _7,_6,_5,_4,_3,_2,_1,_0)  0, 0,_7,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD09_FROM_CSD08(         _7,_6,_5,_4,_3,_2,_1,_0)     0,_7,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD08_FROM_CSD08(         _7,_6,_5,_4,_3,_2,_1,_0)       _7,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD07_FROM_CSD08(         _7,_6,_5,_4,_3,_2,_1,_0)          _6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD06_FROM_CSD08(         _7,_6,_5,_4,_3,_2,_1,_0)             _5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD05_FROM_CSD08(         _7,_6,_5,_4,_3,_2,_1,_0)                _4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD04_FROM_CSD08(         _7,_6,_5,_4,_3,_2,_1,_0)                   _3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD03_FROM_CSD08(         _7,_6,_5,_4,_3,_2,_1,_0)                      _2,_1,_0
#define HXR_IMPL_CAST_TO_CSD02_FROM_CSD08(         _7,_6,_5,_4,_3,_2,_1,_0)                         _1,_0
#define HXR_IMPL_CAST_TO_CSD01_FROM_CSD08(         _7,_6,_5,_4,_3,_2,_1,_0)                            _0

#define HXR_IMPL_CAST_TO_CSD10_FROM_CSD07(            _6,_5,_4,_3,_2,_1,_0)  0, 0, 0,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD09_FROM_CSD07(            _6,_5,_4,_3,_2,_1,_0)     0, 0,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD08_FROM_CSD07(            _6,_5,_4,_3,_2,_1,_0)        0,_6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD07_FROM_CSD07(            _6,_5,_4,_3,_2,_1,_0)          _6,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD06_FROM_CSD07(            _6,_5,_4,_3,_2,_1,_0)             _5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD05_FROM_CSD07(            _6,_5,_4,_3,_2,_1,_0)                _4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD04_FROM_CSD07(            _6,_5,_4,_3,_2,_1,_0)                   _3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD03_FROM_CSD07(            _6,_5,_4,_3,_2,_1,_0)                      _2,_1,_0
#define HXR_IMPL_CAST_TO_CSD02_FROM_CSD07(            _6,_5,_4,_3,_2,_1,_0)                         _1,_0
#define HXR_IMPL_CAST_TO_CSD01_FROM_CSD07(            _6,_5,_4,_3,_2,_1,_0)                            _0

#define HXR_IMPL_CAST_TO_CSD10_FROM_CSD06(               _5,_4,_3,_2,_1,_0)  0, 0, 0, 0,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD09_FROM_CSD06(               _5,_4,_3,_2,_1,_0)     0, 0, 0,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD08_FROM_CSD06(               _5,_4,_3,_2,_1,_0)        0, 0,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD07_FROM_CSD06(               _5,_4,_3,_2,_1,_0)           0,_5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD06_FROM_CSD06(               _5,_4,_3,_2,_1,_0)             _5,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD05_FROM_CSD06(               _5,_4,_3,_2,_1,_0)                _4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD04_FROM_CSD06(               _5,_4,_3,_2,_1,_0)                   _3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD03_FROM_CSD06(               _5,_4,_3,_2,_1,_0)                      _2,_1,_0
#define HXR_IMPL_CAST_TO_CSD02_FROM_CSD06(               _5,_4,_3,_2,_1,_0)                         _1,_0
#define HXR_IMPL_CAST_TO_CSD01_FROM_CSD06(               _5,_4,_3,_2,_1,_0)                            _0

#define HXR_IMPL_CAST_TO_CSD10_FROM_CSD05(                  _4,_3,_2,_1,_0)  0, 0, 0, 0, 0,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD09_FROM_CSD05(                  _4,_3,_2,_1,_0)     0, 0, 0, 0,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD08_FROM_CSD05(                  _4,_3,_2,_1,_0)        0, 0, 0,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD07_FROM_CSD05(                  _4,_3,_2,_1,_0)           0, 0,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD06_FROM_CSD05(                  _4,_3,_2,_1,_0)              0,_4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD05_FROM_CSD05(                  _4,_3,_2,_1,_0)                _4,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD04_FROM_CSD05(                  _4,_3,_2,_1,_0)                   _3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD03_FROM_CSD05(                  _4,_3,_2,_1,_0)                      _2,_1,_0
#define HXR_IMPL_CAST_TO_CSD02_FROM_CSD05(                  _4,_3,_2,_1,_0)                         _1,_0
#define HXR_IMPL_CAST_TO_CSD01_FROM_CSD05(                  _4,_3,_2,_1,_0)                            _0

#define HXR_IMPL_CAST_TO_CSD10_FROM_CSD04(                     _3,_2,_1,_0)  0, 0, 0, 0, 0, 0,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD09_FROM_CSD04(                     _3,_2,_1,_0)     0, 0, 0, 0, 0,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD08_FROM_CSD04(                     _3,_2,_1,_0)        0, 0, 0, 0,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD07_FROM_CSD04(                     _3,_2,_1,_0)           0, 0, 0,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD06_FROM_CSD04(                     _3,_2,_1,_0)              0, 0,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD05_FROM_CSD04(                     _3,_2,_1,_0)                 0,_3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD04_FROM_CSD04(                     _3,_2,_1,_0)                   _3,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD03_FROM_CSD04(                     _3,_2,_1,_0)                      _2,_1,_0
#define HXR_IMPL_CAST_TO_CSD02_FROM_CSD04(                     _3,_2,_1,_0)                         _1,_0
#define HXR_IMPL_CAST_TO_CSD01_FROM_CSD04(                     _3,_2,_1,_0)                            _0

#define HXR_IMPL_CAST_TO_CSD10_FROM_CSD03(                        _2,_1,_0)  0, 0, 0, 0, 0, 0, 0,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD09_FROM_CSD03(                        _2,_1,_0)     0, 0, 0, 0, 0, 0,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD08_FROM_CSD03(                        _2,_1,_0)        0, 0, 0, 0, 0,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD07_FROM_CSD03(                        _2,_1,_0)           0, 0, 0, 0,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD06_FROM_CSD03(                        _2,_1,_0)              0, 0, 0,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD05_FROM_CSD03(                        _2,_1,_0)                 0, 0,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD04_FROM_CSD03(                        _2,_1,_0)                    0,_2,_1,_0
#define HXR_IMPL_CAST_TO_CSD03_FROM_CSD03(                        _2,_1,_0)                      _2,_1,_0
#define HXR_IMPL_CAST_TO_CSD02_FROM_CSD03(                        _2,_1,_0)                         _1,_0
#define HXR_IMPL_CAST_TO_CSD01_FROM_CSD03(                        _2,_1,_0)                            _0

#define HXR_IMPL_CAST_TO_CSD10_FROM_CSD02(                           _1,_0)  0, 0, 0, 0, 0, 0, 0, 0,_1,_0
#define HXR_IMPL_CAST_TO_CSD09_FROM_CSD02(                           _1,_0)     0, 0, 0, 0, 0, 0, 0,_1,_0
#define HXR_IMPL_CAST_TO_CSD08_FROM_CSD02(                           _1,_0)        0, 0, 0, 0, 0, 0,_1,_0
#define HXR_IMPL_CAST_TO_CSD07_FROM_CSD02(                           _1,_0)           0, 0, 0, 0, 0,_1,_0
#define HXR_IMPL_CAST_TO_CSD06_FROM_CSD02(                           _1,_0)              0, 0, 0, 0,_1,_0
#define HXR_IMPL_CAST_TO_CSD05_FROM_CSD02(                           _1,_0)                 0, 0, 0,_1,_0
#define HXR_IMPL_CAST_TO_CSD04_FROM_CSD02(                           _1,_0)                    0, 0,_1,_0
#define HXR_IMPL_CAST_TO_CSD03_FROM_CSD02(                           _1,_0)                       0,_1,_0
#define HXR_IMPL_CAST_TO_CSD02_FROM_CSD02(                           _1,_0)                         _1,_0
#define HXR_IMPL_CAST_TO_CSD01_FROM_CSD02(                           _1,_0)                            _0

#define HXR_IMPL_CAST_TO_CSD10_FROM_CSD01(                              _0)  0, 0, 0, 0, 0, 0, 0, 0, 0,_0
#define HXR_IMPL_CAST_TO_CSD09_FROM_CSD01(                              _0)     0, 0, 0, 0, 0, 0, 0, 0,_0
#define HXR_IMPL_CAST_TO_CSD08_FROM_CSD01(                              _0)        0, 0, 0, 0, 0, 0, 0,_0
#define HXR_IMPL_CAST_TO_CSD07_FROM_CSD01(                              _0)           0, 0, 0, 0, 0, 0,_0
#define HXR_IMPL_CAST_TO_CSD06_FROM_CSD01(                              _0)              0, 0, 0, 0, 0,_0
#define HXR_IMPL_CAST_TO_CSD05_FROM_CSD01(                              _0)                 0, 0, 0, 0,_0
#define HXR_IMPL_CAST_TO_CSD04_FROM_CSD01(                              _0)                    0, 0, 0,_0
#define HXR_IMPL_CAST_TO_CSD03_FROM_CSD01(                              _0)                       0, 0,_0
#define HXR_IMPL_CAST_TO_CSD02_FROM_CSD01(                              _0)                          0,_0
#define HXR_IMPL_CAST_TO_CSD01_FROM_CSD01(                              _0)                            _0

// To have _IMPL and non-_IMPL versions of this macro-table might seem redundant,
// but it is quite necessary for avoiding errors like this:
// error: macro "HXR_INCR_AND_WRAP_CSD01_" requires 2 arguments, but only 1 given
//
// Basically, there are times when the preprocessor interprets the result of
// a macro expansion as behaving like a single argument, even if it contains
// commas. In those cases, passing the result to the above "_IMPL" macros
// would result in an analogue of the said error message.
//
// However, if we pass the single "argument" to a macro that accepts just one
// argument, then pass that argument to the intended macro, the argument
// expansion (which happens prior to argument enumeration) will reify all of the
// commas as argument separators. This would give us something like:
// #define HXR_CAST_TO_CSD01_FROM_CSD02(csd) HXR_IMPL_CAST_TO_CSD01_FROM_CSD02(csd)
//
// That works... if the `csd` argument is a macro result. But if it isn't, like
// might be the case if the caller invoked it directly, then there would be trouble.
// So to handle both cases, we make these be variadic:
// * If they receive a single argument, then argument expansion will reify the commas.
// * If they receive multiple arguments, they pass them on as __VA_ARGS__ and become no-ops.
//
// Yes, all that, and just for this seemingly pointless extra macro table.
// We love you, preprocessor. <3
//

#if defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL

#define HXR_CAST_TO_CSD10_FROM_CSD11(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD10_FROM_CSD11(__VA_ARGS__))
#define HXR_CAST_TO_CSD09_FROM_CSD11(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD09_FROM_CSD11(__VA_ARGS__))
#define HXR_CAST_TO_CSD08_FROM_CSD11(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD08_FROM_CSD11(__VA_ARGS__))
#define HXR_CAST_TO_CSD07_FROM_CSD11(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD07_FROM_CSD11(__VA_ARGS__))
#define HXR_CAST_TO_CSD06_FROM_CSD11(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD06_FROM_CSD11(__VA_ARGS__))
#define HXR_CAST_TO_CSD05_FROM_CSD11(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD05_FROM_CSD11(__VA_ARGS__))
#define HXR_CAST_TO_CSD04_FROM_CSD11(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD04_FROM_CSD11(__VA_ARGS__))
#define HXR_CAST_TO_CSD03_FROM_CSD11(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD03_FROM_CSD11(__VA_ARGS__))
#define HXR_CAST_TO_CSD02_FROM_CSD11(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD02_FROM_CSD11(__VA_ARGS__))
#define HXR_CAST_TO_CSD01_FROM_CSD11(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD01_FROM_CSD11(__VA_ARGS__))

#define HXR_CAST_TO_CSD10_FROM_CSD10(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD10_FROM_CSD10(__VA_ARGS__))
#define HXR_CAST_TO_CSD09_FROM_CSD10(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD09_FROM_CSD10(__VA_ARGS__))
#define HXR_CAST_TO_CSD08_FROM_CSD10(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD08_FROM_CSD10(__VA_ARGS__))
#define HXR_CAST_TO_CSD07_FROM_CSD10(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD07_FROM_CSD10(__VA_ARGS__))
#define HXR_CAST_TO_CSD06_FROM_CSD10(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD06_FROM_CSD10(__VA_ARGS__))
#define HXR_CAST_TO_CSD05_FROM_CSD10(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD05_FROM_CSD10(__VA_ARGS__))
#define HXR_CAST_TO_CSD04_FROM_CSD10(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD04_FROM_CSD10(__VA_ARGS__))
#define HXR_CAST_TO_CSD03_FROM_CSD10(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD03_FROM_CSD10(__VA_ARGS__))
#define HXR_CAST_TO_CSD02_FROM_CSD10(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD02_FROM_CSD10(__VA_ARGS__))
#define HXR_CAST_TO_CSD01_FROM_CSD10(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD01_FROM_CSD10(__VA_ARGS__))

#define HXR_CAST_TO_CSD10_FROM_CSD09(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD10_FROM_CSD09(__VA_ARGS__))
#define HXR_CAST_TO_CSD09_FROM_CSD09(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD09_FROM_CSD09(__VA_ARGS__))
#define HXR_CAST_TO_CSD08_FROM_CSD09(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD08_FROM_CSD09(__VA_ARGS__))
#define HXR_CAST_TO_CSD07_FROM_CSD09(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD07_FROM_CSD09(__VA_ARGS__))
#define HXR_CAST_TO_CSD06_FROM_CSD09(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD06_FROM_CSD09(__VA_ARGS__))
#define HXR_CAST_TO_CSD05_FROM_CSD09(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD05_FROM_CSD09(__VA_ARGS__))
#define HXR_CAST_TO_CSD04_FROM_CSD09(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD04_FROM_CSD09(__VA_ARGS__))
#define HXR_CAST_TO_CSD03_FROM_CSD09(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD03_FROM_CSD09(__VA_ARGS__))
#define HXR_CAST_TO_CSD02_FROM_CSD09(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD02_FROM_CSD09(__VA_ARGS__))
#define HXR_CAST_TO_CSD01_FROM_CSD09(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD01_FROM_CSD09(__VA_ARGS__))

#define HXR_CAST_TO_CSD10_FROM_CSD08(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD10_FROM_CSD08(__VA_ARGS__))
#define HXR_CAST_TO_CSD09_FROM_CSD08(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD09_FROM_CSD08(__VA_ARGS__))
#define HXR_CAST_TO_CSD08_FROM_CSD08(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD08_FROM_CSD08(__VA_ARGS__))
#define HXR_CAST_TO_CSD07_FROM_CSD08(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD07_FROM_CSD08(__VA_ARGS__))
#define HXR_CAST_TO_CSD06_FROM_CSD08(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD06_FROM_CSD08(__VA_ARGS__))
#define HXR_CAST_TO_CSD05_FROM_CSD08(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD05_FROM_CSD08(__VA_ARGS__))
#define HXR_CAST_TO_CSD04_FROM_CSD08(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD04_FROM_CSD08(__VA_ARGS__))
#define HXR_CAST_TO_CSD03_FROM_CSD08(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD03_FROM_CSD08(__VA_ARGS__))
#define HXR_CAST_TO_CSD02_FROM_CSD08(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD02_FROM_CSD08(__VA_ARGS__))
#define HXR_CAST_TO_CSD01_FROM_CSD08(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD01_FROM_CSD08(__VA_ARGS__))

#define HXR_CAST_TO_CSD10_FROM_CSD07(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD10_FROM_CSD07(__VA_ARGS__))
#define HXR_CAST_TO_CSD09_FROM_CSD07(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD09_FROM_CSD07(__VA_ARGS__))
#define HXR_CAST_TO_CSD08_FROM_CSD07(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD08_FROM_CSD07(__VA_ARGS__))
#define HXR_CAST_TO_CSD07_FROM_CSD07(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD07_FROM_CSD07(__VA_ARGS__))
#define HXR_CAST_TO_CSD06_FROM_CSD07(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD06_FROM_CSD07(__VA_ARGS__))
#define HXR_CAST_TO_CSD05_FROM_CSD07(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD05_FROM_CSD07(__VA_ARGS__))
#define HXR_CAST_TO_CSD04_FROM_CSD07(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD04_FROM_CSD07(__VA_ARGS__))
#define HXR_CAST_TO_CSD03_FROM_CSD07(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD03_FROM_CSD07(__VA_ARGS__))
#define HXR_CAST_TO_CSD02_FROM_CSD07(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD02_FROM_CSD07(__VA_ARGS__))
#define HXR_CAST_TO_CSD01_FROM_CSD07(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD01_FROM_CSD07(__VA_ARGS__))

#define HXR_CAST_TO_CSD10_FROM_CSD06(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD10_FROM_CSD06(__VA_ARGS__))
#define HXR_CAST_TO_CSD09_FROM_CSD06(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD09_FROM_CSD06(__VA_ARGS__))
#define HXR_CAST_TO_CSD08_FROM_CSD06(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD08_FROM_CSD06(__VA_ARGS__))
#define HXR_CAST_TO_CSD07_FROM_CSD06(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD07_FROM_CSD06(__VA_ARGS__))
#define HXR_CAST_TO_CSD06_FROM_CSD06(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD06_FROM_CSD06(__VA_ARGS__))
#define HXR_CAST_TO_CSD05_FROM_CSD06(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD05_FROM_CSD06(__VA_ARGS__))
#define HXR_CAST_TO_CSD04_FROM_CSD06(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD04_FROM_CSD06(__VA_ARGS__))
#define HXR_CAST_TO_CSD03_FROM_CSD06(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD03_FROM_CSD06(__VA_ARGS__))
#define HXR_CAST_TO_CSD02_FROM_CSD06(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD02_FROM_CSD06(__VA_ARGS__))
#define HXR_CAST_TO_CSD01_FROM_CSD06(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD01_FROM_CSD06(__VA_ARGS__))

#define HXR_CAST_TO_CSD10_FROM_CSD05(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD10_FROM_CSD05(__VA_ARGS__))
#define HXR_CAST_TO_CSD09_FROM_CSD05(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD09_FROM_CSD05(__VA_ARGS__))
#define HXR_CAST_TO_CSD08_FROM_CSD05(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD08_FROM_CSD05(__VA_ARGS__))
#define HXR_CAST_TO_CSD07_FROM_CSD05(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD07_FROM_CSD05(__VA_ARGS__))
#define HXR_CAST_TO_CSD06_FROM_CSD05(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD06_FROM_CSD05(__VA_ARGS__))
#define HXR_CAST_TO_CSD05_FROM_CSD05(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD05_FROM_CSD05(__VA_ARGS__))
#define HXR_CAST_TO_CSD04_FROM_CSD05(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD04_FROM_CSD05(__VA_ARGS__))
#define HXR_CAST_TO_CSD03_FROM_CSD05(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD03_FROM_CSD05(__VA_ARGS__))
#define HXR_CAST_TO_CSD02_FROM_CSD05(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD02_FROM_CSD05(__VA_ARGS__))
#define HXR_CAST_TO_CSD01_FROM_CSD05(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD01_FROM_CSD05(__VA_ARGS__))

#define HXR_CAST_TO_CSD10_FROM_CSD04(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD10_FROM_CSD04(__VA_ARGS__))
#define HXR_CAST_TO_CSD09_FROM_CSD04(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD09_FROM_CSD04(__VA_ARGS__))
#define HXR_CAST_TO_CSD08_FROM_CSD04(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD08_FROM_CSD04(__VA_ARGS__))
#define HXR_CAST_TO_CSD07_FROM_CSD04(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD07_FROM_CSD04(__VA_ARGS__))
#define HXR_CAST_TO_CSD06_FROM_CSD04(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD06_FROM_CSD04(__VA_ARGS__))
#define HXR_CAST_TO_CSD05_FROM_CSD04(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD05_FROM_CSD04(__VA_ARGS__))
#define HXR_CAST_TO_CSD04_FROM_CSD04(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD04_FROM_CSD04(__VA_ARGS__))
#define HXR_CAST_TO_CSD03_FROM_CSD04(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD03_FROM_CSD04(__VA_ARGS__))
#define HXR_CAST_TO_CSD02_FROM_CSD04(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD02_FROM_CSD04(__VA_ARGS__))
#define HXR_CAST_TO_CSD01_FROM_CSD04(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD01_FROM_CSD04(__VA_ARGS__))

#define HXR_CAST_TO_CSD10_FROM_CSD03(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD10_FROM_CSD03(__VA_ARGS__))
#define HXR_CAST_TO_CSD09_FROM_CSD03(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD09_FROM_CSD03(__VA_ARGS__))
#define HXR_CAST_TO_CSD08_FROM_CSD03(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD08_FROM_CSD03(__VA_ARGS__))
#define HXR_CAST_TO_CSD07_FROM_CSD03(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD07_FROM_CSD03(__VA_ARGS__))
#define HXR_CAST_TO_CSD06_FROM_CSD03(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD06_FROM_CSD03(__VA_ARGS__))
#define HXR_CAST_TO_CSD05_FROM_CSD03(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD05_FROM_CSD03(__VA_ARGS__))
#define HXR_CAST_TO_CSD04_FROM_CSD03(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD04_FROM_CSD03(__VA_ARGS__))
#define HXR_CAST_TO_CSD03_FROM_CSD03(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD03_FROM_CSD03(__VA_ARGS__))
#define HXR_CAST_TO_CSD02_FROM_CSD03(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD02_FROM_CSD03(__VA_ARGS__))
#define HXR_CAST_TO_CSD01_FROM_CSD03(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD01_FROM_CSD03(__VA_ARGS__))

#define HXR_CAST_TO_CSD10_FROM_CSD02(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD10_FROM_CSD02(__VA_ARGS__))
#define HXR_CAST_TO_CSD09_FROM_CSD02(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD09_FROM_CSD02(__VA_ARGS__))
#define HXR_CAST_TO_CSD08_FROM_CSD02(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD08_FROM_CSD02(__VA_ARGS__))
#define HXR_CAST_TO_CSD07_FROM_CSD02(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD07_FROM_CSD02(__VA_ARGS__))
#define HXR_CAST_TO_CSD06_FROM_CSD02(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD06_FROM_CSD02(__VA_ARGS__))
#define HXR_CAST_TO_CSD05_FROM_CSD02(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD05_FROM_CSD02(__VA_ARGS__))
#define HXR_CAST_TO_CSD04_FROM_CSD02(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD04_FROM_CSD02(__VA_ARGS__))
#define HXR_CAST_TO_CSD03_FROM_CSD02(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD03_FROM_CSD02(__VA_ARGS__))
#define HXR_CAST_TO_CSD02_FROM_CSD02(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD02_FROM_CSD02(__VA_ARGS__))
#define HXR_CAST_TO_CSD01_FROM_CSD02(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD01_FROM_CSD02(__VA_ARGS__))

#define HXR_CAST_TO_CSD10_FROM_CSD01(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD10_FROM_CSD01(__VA_ARGS__))
#define HXR_CAST_TO_CSD09_FROM_CSD01(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD09_FROM_CSD01(__VA_ARGS__))
#define HXR_CAST_TO_CSD08_FROM_CSD01(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD08_FROM_CSD01(__VA_ARGS__))
#define HXR_CAST_TO_CSD07_FROM_CSD01(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD07_FROM_CSD01(__VA_ARGS__))
#define HXR_CAST_TO_CSD06_FROM_CSD01(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD06_FROM_CSD01(__VA_ARGS__))
#define HXR_CAST_TO_CSD05_FROM_CSD01(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD05_FROM_CSD01(__VA_ARGS__))
#define HXR_CAST_TO_CSD04_FROM_CSD01(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD04_FROM_CSD01(__VA_ARGS__))
#define HXR_CAST_TO_CSD03_FROM_CSD01(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD03_FROM_CSD01(__VA_ARGS__))
#define HXR_CAST_TO_CSD02_FROM_CSD01(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD02_FROM_CSD01(__VA_ARGS__))
#define HXR_CAST_TO_CSD01_FROM_CSD01(...) HXR_MSVC_EXPAND_VA(HXR_IMPL_CAST_TO_CSD01_FROM_CSD01(__VA_ARGS__))

#else /* !( defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL) */

#define HXR_CAST_TO_CSD10_FROM_CSD11(...) HXR_IMPL_CAST_TO_CSD10_FROM_CSD11(__VA_ARGS__)
#define HXR_CAST_TO_CSD09_FROM_CSD11(...) HXR_IMPL_CAST_TO_CSD09_FROM_CSD11(__VA_ARGS__)
#define HXR_CAST_TO_CSD08_FROM_CSD11(...) HXR_IMPL_CAST_TO_CSD08_FROM_CSD11(__VA_ARGS__)
#define HXR_CAST_TO_CSD07_FROM_CSD11(...) HXR_IMPL_CAST_TO_CSD07_FROM_CSD11(__VA_ARGS__)
#define HXR_CAST_TO_CSD06_FROM_CSD11(...) HXR_IMPL_CAST_TO_CSD06_FROM_CSD11(__VA_ARGS__)
#define HXR_CAST_TO_CSD05_FROM_CSD11(...) HXR_IMPL_CAST_TO_CSD05_FROM_CSD11(__VA_ARGS__)
#define HXR_CAST_TO_CSD04_FROM_CSD11(...) HXR_IMPL_CAST_TO_CSD04_FROM_CSD11(__VA_ARGS__)
#define HXR_CAST_TO_CSD03_FROM_CSD11(...) HXR_IMPL_CAST_TO_CSD03_FROM_CSD11(__VA_ARGS__)
#define HXR_CAST_TO_CSD02_FROM_CSD11(...) HXR_IMPL_CAST_TO_CSD02_FROM_CSD11(__VA_ARGS__)
#define HXR_CAST_TO_CSD01_FROM_CSD11(...) HXR_IMPL_CAST_TO_CSD01_FROM_CSD11(__VA_ARGS__)

#define HXR_CAST_TO_CSD10_FROM_CSD10(...) HXR_IMPL_CAST_TO_CSD10_FROM_CSD10(__VA_ARGS__)
#define HXR_CAST_TO_CSD09_FROM_CSD10(...) HXR_IMPL_CAST_TO_CSD09_FROM_CSD10(__VA_ARGS__)
#define HXR_CAST_TO_CSD08_FROM_CSD10(...) HXR_IMPL_CAST_TO_CSD08_FROM_CSD10(__VA_ARGS__)
#define HXR_CAST_TO_CSD07_FROM_CSD10(...) HXR_IMPL_CAST_TO_CSD07_FROM_CSD10(__VA_ARGS__)
#define HXR_CAST_TO_CSD06_FROM_CSD10(...) HXR_IMPL_CAST_TO_CSD06_FROM_CSD10(__VA_ARGS__)
#define HXR_CAST_TO_CSD05_FROM_CSD10(...) HXR_IMPL_CAST_TO_CSD05_FROM_CSD10(__VA_ARGS__)
#define HXR_CAST_TO_CSD04_FROM_CSD10(...) HXR_IMPL_CAST_TO_CSD04_FROM_CSD10(__VA_ARGS__)
#define HXR_CAST_TO_CSD03_FROM_CSD10(...) HXR_IMPL_CAST_TO_CSD03_FROM_CSD10(__VA_ARGS__)
#define HXR_CAST_TO_CSD02_FROM_CSD10(...) HXR_IMPL_CAST_TO_CSD02_FROM_CSD10(__VA_ARGS__)
#define HXR_CAST_TO_CSD01_FROM_CSD10(...) HXR_IMPL_CAST_TO_CSD01_FROM_CSD10(__VA_ARGS__)

#define HXR_CAST_TO_CSD10_FROM_CSD09(...) HXR_IMPL_CAST_TO_CSD10_FROM_CSD09(__VA_ARGS__)
#define HXR_CAST_TO_CSD09_FROM_CSD09(...) HXR_IMPL_CAST_TO_CSD09_FROM_CSD09(__VA_ARGS__)
#define HXR_CAST_TO_CSD08_FROM_CSD09(...) HXR_IMPL_CAST_TO_CSD08_FROM_CSD09(__VA_ARGS__)
#define HXR_CAST_TO_CSD07_FROM_CSD09(...) HXR_IMPL_CAST_TO_CSD07_FROM_CSD09(__VA_ARGS__)
#define HXR_CAST_TO_CSD06_FROM_CSD09(...) HXR_IMPL_CAST_TO_CSD06_FROM_CSD09(__VA_ARGS__)
#define HXR_CAST_TO_CSD05_FROM_CSD09(...) HXR_IMPL_CAST_TO_CSD05_FROM_CSD09(__VA_ARGS__)
#define HXR_CAST_TO_CSD04_FROM_CSD09(...) HXR_IMPL_CAST_TO_CSD04_FROM_CSD09(__VA_ARGS__)
#define HXR_CAST_TO_CSD03_FROM_CSD09(...) HXR_IMPL_CAST_TO_CSD03_FROM_CSD09(__VA_ARGS__)
#define HXR_CAST_TO_CSD02_FROM_CSD09(...) HXR_IMPL_CAST_TO_CSD02_FROM_CSD09(__VA_ARGS__)
#define HXR_CAST_TO_CSD01_FROM_CSD09(...) HXR_IMPL_CAST_TO_CSD01_FROM_CSD09(__VA_ARGS__)

#define HXR_CAST_TO_CSD10_FROM_CSD08(...) HXR_IMPL_CAST_TO_CSD10_FROM_CSD08(__VA_ARGS__)
#define HXR_CAST_TO_CSD09_FROM_CSD08(...) HXR_IMPL_CAST_TO_CSD09_FROM_CSD08(__VA_ARGS__)
#define HXR_CAST_TO_CSD08_FROM_CSD08(...) HXR_IMPL_CAST_TO_CSD08_FROM_CSD08(__VA_ARGS__)
#define HXR_CAST_TO_CSD07_FROM_CSD08(...) HXR_IMPL_CAST_TO_CSD07_FROM_CSD08(__VA_ARGS__)
#define HXR_CAST_TO_CSD06_FROM_CSD08(...) HXR_IMPL_CAST_TO_CSD06_FROM_CSD08(__VA_ARGS__)
#define HXR_CAST_TO_CSD05_FROM_CSD08(...) HXR_IMPL_CAST_TO_CSD05_FROM_CSD08(__VA_ARGS__)
#define HXR_CAST_TO_CSD04_FROM_CSD08(...) HXR_IMPL_CAST_TO_CSD04_FROM_CSD08(__VA_ARGS__)
#define HXR_CAST_TO_CSD03_FROM_CSD08(...) HXR_IMPL_CAST_TO_CSD03_FROM_CSD08(__VA_ARGS__)
#define HXR_CAST_TO_CSD02_FROM_CSD08(...) HXR_IMPL_CAST_TO_CSD02_FROM_CSD08(__VA_ARGS__)
#define HXR_CAST_TO_CSD01_FROM_CSD08(...) HXR_IMPL_CAST_TO_CSD01_FROM_CSD08(__VA_ARGS__)

#define HXR_CAST_TO_CSD10_FROM_CSD07(...) HXR_IMPL_CAST_TO_CSD10_FROM_CSD07(__VA_ARGS__)
#define HXR_CAST_TO_CSD09_FROM_CSD07(...) HXR_IMPL_CAST_TO_CSD09_FROM_CSD07(__VA_ARGS__)
#define HXR_CAST_TO_CSD08_FROM_CSD07(...) HXR_IMPL_CAST_TO_CSD08_FROM_CSD07(__VA_ARGS__)
#define HXR_CAST_TO_CSD07_FROM_CSD07(...) HXR_IMPL_CAST_TO_CSD07_FROM_CSD07(__VA_ARGS__)
#define HXR_CAST_TO_CSD06_FROM_CSD07(...) HXR_IMPL_CAST_TO_CSD06_FROM_CSD07(__VA_ARGS__)
#define HXR_CAST_TO_CSD05_FROM_CSD07(...) HXR_IMPL_CAST_TO_CSD05_FROM_CSD07(__VA_ARGS__)
#define HXR_CAST_TO_CSD04_FROM_CSD07(...) HXR_IMPL_CAST_TO_CSD04_FROM_CSD07(__VA_ARGS__)
#define HXR_CAST_TO_CSD03_FROM_CSD07(...) HXR_IMPL_CAST_TO_CSD03_FROM_CSD07(__VA_ARGS__)
#define HXR_CAST_TO_CSD02_FROM_CSD07(...) HXR_IMPL_CAST_TO_CSD02_FROM_CSD07(__VA_ARGS__)
#define HXR_CAST_TO_CSD01_FROM_CSD07(...) HXR_IMPL_CAST_TO_CSD01_FROM_CSD07(__VA_ARGS__)

#define HXR_CAST_TO_CSD10_FROM_CSD06(...) HXR_IMPL_CAST_TO_CSD10_FROM_CSD06(__VA_ARGS__)
#define HXR_CAST_TO_CSD09_FROM_CSD06(...) HXR_IMPL_CAST_TO_CSD09_FROM_CSD06(__VA_ARGS__)
#define HXR_CAST_TO_CSD08_FROM_CSD06(...) HXR_IMPL_CAST_TO_CSD08_FROM_CSD06(__VA_ARGS__)
#define HXR_CAST_TO_CSD07_FROM_CSD06(...) HXR_IMPL_CAST_TO_CSD07_FROM_CSD06(__VA_ARGS__)
#define HXR_CAST_TO_CSD06_FROM_CSD06(...) HXR_IMPL_CAST_TO_CSD06_FROM_CSD06(__VA_ARGS__)
#define HXR_CAST_TO_CSD05_FROM_CSD06(...) HXR_IMPL_CAST_TO_CSD05_FROM_CSD06(__VA_ARGS__)
#define HXR_CAST_TO_CSD04_FROM_CSD06(...) HXR_IMPL_CAST_TO_CSD04_FROM_CSD06(__VA_ARGS__)
#define HXR_CAST_TO_CSD03_FROM_CSD06(...) HXR_IMPL_CAST_TO_CSD03_FROM_CSD06(__VA_ARGS__)
#define HXR_CAST_TO_CSD02_FROM_CSD06(...) HXR_IMPL_CAST_TO_CSD02_FROM_CSD06(__VA_ARGS__)
#define HXR_CAST_TO_CSD01_FROM_CSD06(...) HXR_IMPL_CAST_TO_CSD01_FROM_CSD06(__VA_ARGS__)

#define HXR_CAST_TO_CSD10_FROM_CSD05(...) HXR_IMPL_CAST_TO_CSD10_FROM_CSD05(__VA_ARGS__)
#define HXR_CAST_TO_CSD09_FROM_CSD05(...) HXR_IMPL_CAST_TO_CSD09_FROM_CSD05(__VA_ARGS__)
#define HXR_CAST_TO_CSD08_FROM_CSD05(...) HXR_IMPL_CAST_TO_CSD08_FROM_CSD05(__VA_ARGS__)
#define HXR_CAST_TO_CSD07_FROM_CSD05(...) HXR_IMPL_CAST_TO_CSD07_FROM_CSD05(__VA_ARGS__)
#define HXR_CAST_TO_CSD06_FROM_CSD05(...) HXR_IMPL_CAST_TO_CSD06_FROM_CSD05(__VA_ARGS__)
#define HXR_CAST_TO_CSD05_FROM_CSD05(...) HXR_IMPL_CAST_TO_CSD05_FROM_CSD05(__VA_ARGS__)
#define HXR_CAST_TO_CSD04_FROM_CSD05(...) HXR_IMPL_CAST_TO_CSD04_FROM_CSD05(__VA_ARGS__)
#define HXR_CAST_TO_CSD03_FROM_CSD05(...) HXR_IMPL_CAST_TO_CSD03_FROM_CSD05(__VA_ARGS__)
#define HXR_CAST_TO_CSD02_FROM_CSD05(...) HXR_IMPL_CAST_TO_CSD02_FROM_CSD05(__VA_ARGS__)
#define HXR_CAST_TO_CSD01_FROM_CSD05(...) HXR_IMPL_CAST_TO_CSD01_FROM_CSD05(__VA_ARGS__)

#define HXR_CAST_TO_CSD10_FROM_CSD04(...) HXR_IMPL_CAST_TO_CSD10_FROM_CSD04(__VA_ARGS__)
#define HXR_CAST_TO_CSD09_FROM_CSD04(...) HXR_IMPL_CAST_TO_CSD09_FROM_CSD04(__VA_ARGS__)
#define HXR_CAST_TO_CSD08_FROM_CSD04(...) HXR_IMPL_CAST_TO_CSD08_FROM_CSD04(__VA_ARGS__)
#define HXR_CAST_TO_CSD07_FROM_CSD04(...) HXR_IMPL_CAST_TO_CSD07_FROM_CSD04(__VA_ARGS__)
#define HXR_CAST_TO_CSD06_FROM_CSD04(...) HXR_IMPL_CAST_TO_CSD06_FROM_CSD04(__VA_ARGS__)
#define HXR_CAST_TO_CSD05_FROM_CSD04(...) HXR_IMPL_CAST_TO_CSD05_FROM_CSD04(__VA_ARGS__)
#define HXR_CAST_TO_CSD04_FROM_CSD04(...) HXR_IMPL_CAST_TO_CSD04_FROM_CSD04(__VA_ARGS__)
#define HXR_CAST_TO_CSD03_FROM_CSD04(...) HXR_IMPL_CAST_TO_CSD03_FROM_CSD04(__VA_ARGS__)
#define HXR_CAST_TO_CSD02_FROM_CSD04(...) HXR_IMPL_CAST_TO_CSD02_FROM_CSD04(__VA_ARGS__)
#define HXR_CAST_TO_CSD01_FROM_CSD04(...) HXR_IMPL_CAST_TO_CSD01_FROM_CSD04(__VA_ARGS__)

#define HXR_CAST_TO_CSD10_FROM_CSD03(...) HXR_IMPL_CAST_TO_CSD10_FROM_CSD03(__VA_ARGS__)
#define HXR_CAST_TO_CSD09_FROM_CSD03(...) HXR_IMPL_CAST_TO_CSD09_FROM_CSD03(__VA_ARGS__)
#define HXR_CAST_TO_CSD08_FROM_CSD03(...) HXR_IMPL_CAST_TO_CSD08_FROM_CSD03(__VA_ARGS__)
#define HXR_CAST_TO_CSD07_FROM_CSD03(...) HXR_IMPL_CAST_TO_CSD07_FROM_CSD03(__VA_ARGS__)
#define HXR_CAST_TO_CSD06_FROM_CSD03(...) HXR_IMPL_CAST_TO_CSD06_FROM_CSD03(__VA_ARGS__)
#define HXR_CAST_TO_CSD05_FROM_CSD03(...) HXR_IMPL_CAST_TO_CSD05_FROM_CSD03(__VA_ARGS__)
#define HXR_CAST_TO_CSD04_FROM_CSD03(...) HXR_IMPL_CAST_TO_CSD04_FROM_CSD03(__VA_ARGS__)
#define HXR_CAST_TO_CSD03_FROM_CSD03(...) HXR_IMPL_CAST_TO_CSD03_FROM_CSD03(__VA_ARGS__)
#define HXR_CAST_TO_CSD02_FROM_CSD03(...) HXR_IMPL_CAST_TO_CSD02_FROM_CSD03(__VA_ARGS__)
#define HXR_CAST_TO_CSD01_FROM_CSD03(...) HXR_IMPL_CAST_TO_CSD01_FROM_CSD03(__VA_ARGS__)

#define HXR_CAST_TO_CSD10_FROM_CSD02(...) HXR_IMPL_CAST_TO_CSD10_FROM_CSD02(__VA_ARGS__)
#define HXR_CAST_TO_CSD09_FROM_CSD02(...) HXR_IMPL_CAST_TO_CSD09_FROM_CSD02(__VA_ARGS__)
#define HXR_CAST_TO_CSD08_FROM_CSD02(...) HXR_IMPL_CAST_TO_CSD08_FROM_CSD02(__VA_ARGS__)
#define HXR_CAST_TO_CSD07_FROM_CSD02(...) HXR_IMPL_CAST_TO_CSD07_FROM_CSD02(__VA_ARGS__)
#define HXR_CAST_TO_CSD06_FROM_CSD02(...) HXR_IMPL_CAST_TO_CSD06_FROM_CSD02(__VA_ARGS__)
#define HXR_CAST_TO_CSD05_FROM_CSD02(...) HXR_IMPL_CAST_TO_CSD05_FROM_CSD02(__VA_ARGS__)
#define HXR_CAST_TO_CSD04_FROM_CSD02(...) HXR_IMPL_CAST_TO_CSD04_FROM_CSD02(__VA_ARGS__)
#define HXR_CAST_TO_CSD03_FROM_CSD02(...) HXR_IMPL_CAST_TO_CSD03_FROM_CSD02(__VA_ARGS__)
#define HXR_CAST_TO_CSD02_FROM_CSD02(...) HXR_IMPL_CAST_TO_CSD02_FROM_CSD02(__VA_ARGS__)
#define HXR_CAST_TO_CSD01_FROM_CSD02(...) HXR_IMPL_CAST_TO_CSD01_FROM_CSD02(__VA_ARGS__)

#define HXR_CAST_TO_CSD10_FROM_CSD01(...) HXR_IMPL_CAST_TO_CSD10_FROM_CSD01(__VA_ARGS__)
#define HXR_CAST_TO_CSD09_FROM_CSD01(...) HXR_IMPL_CAST_TO_CSD09_FROM_CSD01(__VA_ARGS__)
#define HXR_CAST_TO_CSD08_FROM_CSD01(...) HXR_IMPL_CAST_TO_CSD08_FROM_CSD01(__VA_ARGS__)
#define HXR_CAST_TO_CSD07_FROM_CSD01(...) HXR_IMPL_CAST_TO_CSD07_FROM_CSD01(__VA_ARGS__)
#define HXR_CAST_TO_CSD06_FROM_CSD01(...) HXR_IMPL_CAST_TO_CSD06_FROM_CSD01(__VA_ARGS__)
#define HXR_CAST_TO_CSD05_FROM_CSD01(...) HXR_IMPL_CAST_TO_CSD05_FROM_CSD01(__VA_ARGS__)
#define HXR_CAST_TO_CSD04_FROM_CSD01(...) HXR_IMPL_CAST_TO_CSD04_FROM_CSD01(__VA_ARGS__)
#define HXR_CAST_TO_CSD03_FROM_CSD01(...) HXR_IMPL_CAST_TO_CSD03_FROM_CSD01(__VA_ARGS__)
#define HXR_CAST_TO_CSD02_FROM_CSD01(...) HXR_IMPL_CAST_TO_CSD02_FROM_CSD01(__VA_ARGS__)
#define HXR_CAST_TO_CSD01_FROM_CSD01(...) HXR_IMPL_CAST_TO_CSD01_FROM_CSD01(__VA_ARGS__)

/* #if      defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL   */
/* #else !( defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL ) */
#endif

#define HXR_CAST_TO_CSD10(...) HXR_MACRO_OVERLOAD00(HXR_CAST_TO_CSD10_FROM_CSD, __VA_ARGS__)
#define HXR_CAST_TO_CSD09(...) HXR_MACRO_OVERLOAD00(HXR_CAST_TO_CSD09_FROM_CSD, __VA_ARGS__)
#define HXR_CAST_TO_CSD08(...) HXR_MACRO_OVERLOAD00(HXR_CAST_TO_CSD08_FROM_CSD, __VA_ARGS__)
#define HXR_CAST_TO_CSD07(...) HXR_MACRO_OVERLOAD00(HXR_CAST_TO_CSD07_FROM_CSD, __VA_ARGS__)
#define HXR_CAST_TO_CSD06(...) HXR_MACRO_OVERLOAD00(HXR_CAST_TO_CSD06_FROM_CSD, __VA_ARGS__)
#define HXR_CAST_TO_CSD05(...) HXR_MACRO_OVERLOAD00(HXR_CAST_TO_CSD05_FROM_CSD, __VA_ARGS__)
#define HXR_CAST_TO_CSD04(...) HXR_MACRO_OVERLOAD00(HXR_CAST_TO_CSD04_FROM_CSD, __VA_ARGS__)
#define HXR_CAST_TO_CSD03(...) HXR_MACRO_OVERLOAD00(HXR_CAST_TO_CSD03_FROM_CSD, __VA_ARGS__)
#define HXR_CAST_TO_CSD02(...) HXR_MACRO_OVERLOAD00(HXR_CAST_TO_CSD02_FROM_CSD, __VA_ARGS__)
#define HXR_CAST_TO_CSD01(...) HXR_MACRO_OVERLOAD00(HXR_CAST_TO_CSD01_FROM_CSD, __VA_ARGS__)

#define HXR_INCR_AND_CARRY_CSD02_1(_1, carry, _0)                          HXR_ADD_DIGITS_ ## carry ## _1,_0
#define HXR_INCR_AND_CARRY_CSD03_1(_2, carry, _1,_0)                       HXR_ADD_DIGITS_ ## carry ## _2,_1,_0
#define HXR_INCR_AND_CARRY_CSD04_1(_3, carry, _2,_1,_0)                    HXR_ADD_DIGITS_ ## carry ## _3,_2,_1,_0
#define HXR_INCR_AND_CARRY_CSD05_1(_4, carry, _3,_2,_1,_0)                 HXR_ADD_DIGITS_ ## carry ## _4,_3,_2,_1,_0
#define HXR_INCR_AND_CARRY_CSD06_1(_5, carry, _4,_3,_2,_1,_0)              HXR_ADD_DIGITS_ ## carry ## _5,_4,_3,_2,_1,_0
#define HXR_INCR_AND_CARRY_CSD07_1(_6, carry, _5,_4,_3,_2,_1,_0)           HXR_ADD_DIGITS_ ## carry ## _6,_5,_4,_3,_2,_1,_0
#define HXR_INCR_AND_CARRY_CSD08_1(_7, carry, _6,_5,_4,_3,_2,_1,_0)        HXR_ADD_DIGITS_ ## carry ## _7,_6,_5,_4,_3,_2,_1,_0
#define HXR_INCR_AND_CARRY_CSD09_1(_8, carry, _7,_6,_5,_4,_3,_2,_1,_0)     HXR_ADD_DIGITS_ ## carry ## _8,_7,_6,_5,_4,_3,_2,_1,_0
#define HXR_INCR_AND_CARRY_CSD10_1(_9, carry, _8,_7,_6,_5,_4,_3,_2,_1,_0)  HXR_ADD_DIGITS_ ## carry ## _9,_8,_7,_6,_5,_4,_3,_2,_1,_0

#if defined(_MSVC_TRADITIONAL) && _MSVC_TRADITIONAL
#	define HXR_INCR_AND_CARRY_CSD02_0(_1, tuple)    HXR_MSVC_EXPAND_X(HXR_INCR_AND_CARRY_CSD02_1(_1, tuple))
#	define HXR_INCR_AND_CARRY_CSD03_0(_2, tuple)    HXR_MSVC_EXPAND_X(HXR_INCR_AND_CARRY_CSD03_1(_2, tuple))
#	define HXR_INCR_AND_CARRY_CSD04_0(_3, tuple)    HXR_MSVC_EXPAND_X(HXR_INCR_AND_CARRY_CSD04_1(_3, tuple))
#	define HXR_INCR_AND_CARRY_CSD05_0(_4, tuple)    HXR_MSVC_EXPAND_X(HXR_INCR_AND_CARRY_CSD05_1(_4, tuple))
#	define HXR_INCR_AND_CARRY_CSD06_0(_5, tuple)    HXR_MSVC_EXPAND_X(HXR_INCR_AND_CARRY_CSD06_1(_5, tuple))
#	define HXR_INCR_AND_CARRY_CSD07_0(_6, tuple)    HXR_MSVC_EXPAND_X(HXR_INCR_AND_CARRY_CSD07_1(_6, tuple))
#	define HXR_INCR_AND_CARRY_CSD08_0(_7, tuple)    HXR_MSVC_EXPAND_X(HXR_INCR_AND_CARRY_CSD08_1(_7, tuple))
#	define HXR_INCR_AND_CARRY_CSD09_0(_8, tuple)    HXR_MSVC_EXPAND_X(HXR_INCR_AND_CARRY_CSD09_1(_8, tuple))
#	define HXR_INCR_AND_CARRY_CSD10_0(_9, tuple)    HXR_MSVC_EXPAND_X(HXR_INCR_AND_CARRY_CSD10_1(_9, tuple))
#else
#	define HXR_INCR_AND_CARRY_CSD02_0(_1, tuple)                      HXR_INCR_AND_CARRY_CSD02_1(_1, tuple)
#	define HXR_INCR_AND_CARRY_CSD03_0(_2, tuple)                      HXR_INCR_AND_CARRY_CSD03_1(_2, tuple)
#	define HXR_INCR_AND_CARRY_CSD04_0(_3, tuple)                      HXR_INCR_AND_CARRY_CSD04_1(_3, tuple)
#	define HXR_INCR_AND_CARRY_CSD05_0(_4, tuple)                      HXR_INCR_AND_CARRY_CSD05_1(_4, tuple)
#	define HXR_INCR_AND_CARRY_CSD06_0(_5, tuple)                      HXR_INCR_AND_CARRY_CSD06_1(_5, tuple)
#	define HXR_INCR_AND_CARRY_CSD07_0(_6, tuple)                      HXR_INCR_AND_CARRY_CSD07_1(_6, tuple)
#	define HXR_INCR_AND_CARRY_CSD08_0(_7, tuple)                      HXR_INCR_AND_CARRY_CSD08_1(_7, tuple)
#	define HXR_INCR_AND_CARRY_CSD09_0(_8, tuple)                      HXR_INCR_AND_CARRY_CSD09_1(_8, tuple)
#	define HXR_INCR_AND_CARRY_CSD10_0(_9, tuple)                      HXR_INCR_AND_CARRY_CSD10_1(_9, tuple)
#endif

#define HXR_INCR_AND_CARRY_CSD01(_0)                                  HXR_CONCAT_02(HXR_ADD_DIGITS_1, _0)
#define HXR_INCR_AND_CARRY_CSD02(_1,_0)                               HXR_INCR_AND_CARRY_CSD02_0(_1, HXR_ADD_DIGITS_1 ## _0)
#define HXR_INCR_AND_CARRY_CSD03(_2,_1,_0)                            HXR_INCR_AND_CARRY_CSD03_0(_2, HXR_INCR_AND_CARRY_CSD02(_1,_0))
#define HXR_INCR_AND_CARRY_CSD04(_3,_2,_1,_0)                         HXR_INCR_AND_CARRY_CSD04_0(_3, HXR_INCR_AND_CARRY_CSD03(_2,_1,_0))
#define HXR_INCR_AND_CARRY_CSD05(_4,_3,_2,_1,_0)                      HXR_INCR_AND_CARRY_CSD05_0(_4, HXR_INCR_AND_CARRY_CSD04(_3,_2,_1,_0))
#define HXR_INCR_AND_CARRY_CSD06(_5,_4,_3,_2,_1,_0)                   HXR_INCR_AND_CARRY_CSD06_0(_5, HXR_INCR_AND_CARRY_CSD05(_4,_3,_2,_1,_0))
#define HXR_INCR_AND_CARRY_CSD07(_6,_5,_4,_3,_2,_1,_0)                HXR_INCR_AND_CARRY_CSD07_0(_6, HXR_INCR_AND_CARRY_CSD06(_5,_4,_3,_2,_1,_0))
#define HXR_INCR_AND_CARRY_CSD08(_7,_6,_5,_4,_3,_2,_1,_0)             HXR_INCR_AND_CARRY_CSD08_0(_7, HXR_INCR_AND_CARRY_CSD07(_6,_5,_4,_3,_2,_1,_0))
#define HXR_INCR_AND_CARRY_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0)          HXR_INCR_AND_CARRY_CSD09_0(_8, HXR_INCR_AND_CARRY_CSD08(_7,_6,_5,_4,_3,_2,_1,_0))
#define HXR_INCR_AND_CARRY_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)       HXR_INCR_AND_CARRY_CSD10_0(_9, HXR_INCR_AND_CARRY_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0))


#define HXR_INCR_AND_WRAP_CSD01(_0)                             HXR_CAST_TO_CSD01_FROM_CSD02(HXR_INCR_AND_CARRY_CSD01(_0))
#define HXR_INCR_AND_WRAP_CSD02(_1,_0)                          HXR_CAST_TO_CSD02_FROM_CSD03(HXR_INCR_AND_CARRY_CSD02(_1,_0))
#define HXR_INCR_AND_WRAP_CSD03(_2,_1,_0)                       HXR_CAST_TO_CSD03_FROM_CSD04(HXR_INCR_AND_CARRY_CSD03(_2,_1,_0))
#define HXR_INCR_AND_WRAP_CSD04(_3,_2,_1,_0)                    HXR_CAST_TO_CSD04_FROM_CSD05(HXR_INCR_AND_CARRY_CSD04(_3,_2,_1,_0))
#define HXR_INCR_AND_WRAP_CSD05(_4,_3,_2,_1,_0)                 HXR_CAST_TO_CSD05_FROM_CSD06(HXR_INCR_AND_CARRY_CSD05(_4,_3,_2,_1,_0))
#define HXR_INCR_AND_WRAP_CSD06(_5,_4,_3,_2,_1,_0)              HXR_CAST_TO_CSD06_FROM_CSD07(HXR_INCR_AND_CARRY_CSD06(_5,_4,_3,_2,_1,_0))
#define HXR_INCR_AND_WRAP_CSD07(_6,_5,_4,_3,_2,_1,_0)           HXR_CAST_TO_CSD07_FROM_CSD08(HXR_INCR_AND_CARRY_CSD07(_6,_5,_4,_3,_2,_1,_0))
#define HXR_INCR_AND_WRAP_CSD08(_7,_6,_5,_4,_3,_2,_1,_0)        HXR_CAST_TO_CSD08_FROM_CSD09(HXR_INCR_AND_CARRY_CSD08(_7,_6,_5,_4,_3,_2,_1,_0))
#define HXR_INCR_AND_WRAP_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0)     HXR_CAST_TO_CSD09_FROM_CSD10(HXR_INCR_AND_CARRY_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0))
#define HXR_INCR_AND_WRAP_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)  HXR_CAST_TO_CSD10_FROM_CSD11(HXR_INCR_AND_CARRY_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0))

#define HXR_INCR_CSD01(_0)                             HXR_IMPL_TRIM_CSD_BY_01(HXR_INCR_AND_CARRY_CSD01(_0))
#define HXR_INCR_CSD02(_1,_0)                          HXR_IMPL_TRIM_CSD_BY_01(HXR_INCR_AND_CARRY_CSD02(_1,_0))
#define HXR_INCR_CSD03(_2,_1,_0)                       HXR_IMPL_TRIM_CSD_BY_01(HXR_INCR_AND_CARRY_CSD03(_2,_1,_0))
#define HXR_INCR_CSD04(_3,_2,_1,_0)                    HXR_IMPL_TRIM_CSD_BY_01(HXR_INCR_AND_CARRY_CSD04(_3,_2,_1,_0))
#define HXR_INCR_CSD05(_4,_3,_2,_1,_0)                 HXR_IMPL_TRIM_CSD_BY_01(HXR_INCR_AND_CARRY_CSD05(_4,_3,_2,_1,_0))
#define HXR_INCR_CSD06(_5,_4,_3,_2,_1,_0)              HXR_IMPL_TRIM_CSD_BY_01(HXR_INCR_AND_CARRY_CSD06(_5,_4,_3,_2,_1,_0))
#define HXR_INCR_CSD07(_6,_5,_4,_3,_2,_1,_0)           HXR_IMPL_TRIM_CSD_BY_01(HXR_INCR_AND_CARRY_CSD07(_6,_5,_4,_3,_2,_1,_0))
#define HXR_INCR_CSD08(_7,_6,_5,_4,_3,_2,_1,_0)        HXR_IMPL_TRIM_CSD_BY_01(HXR_INCR_AND_CARRY_CSD08(_7,_6,_5,_4,_3,_2,_1,_0))
#define HXR_INCR_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0)     HXR_IMPL_TRIM_CSD_BY_01(HXR_INCR_AND_CARRY_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0))
#define HXR_INCR_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)  HXR_INCR_AND_WRAP_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0)


/// Expands to the number represented by the argument list, in Comma-Separated-Digits (CSD) form,
/// plus one.
///
/// This version always inserts an additional digit at the beginning of the
/// CSD result. This digit will be 0 if the resulting number did not increase
/// in decimal magnitude, and it will be 1 if the resulting number DID increase
/// in decimal magnitude (and thus required the extra digit to be stored losslessly).
///
/// Note that if the CSD represented by the argument list was already the widest
/// supported CSD (e.g. 10 digits, as of this writing), then the result will
/// be wider than what is uniformely handled by the CSD system's operations.
/// This can still be useful if the caller needs to check for carry on such
/// operations. The oversized CSD can be truncated back to a managable size
/// using the `HXR_CAST_TO_CSD*` macros (e.g. `HXR_CAST_TO_CSD10`, as of this
/// writing).
///
/// In most situations, the behavior of `HXR_INCR_CSD` is more desirable and
/// intuitive. However, this version that always inserts a new digit is useful
/// if there is a calling macro that needs to dispatch on the new digit's value
/// using a macro like `HXR_MACRO_OVERLOAD`.
///
/// For example:
///   HXR_STRINGIZE((HXR_INCR_AND_CARRY_CSD(0,3))) == "(0,0,4)"
///   HXR_STRINGIZE((HXR_INCR_AND_CARRY_CSD(1,5))) == "(0,1,6)"
///   HXR_STRINGIZE((HXR_INCR_AND_CARRY_CSD(9,9))) == "(1,0,0)"
///   HXR_STRINGIZE((HXR_INCR_AND_CARRY_CSD(9,9,9,9,9,9,9,9,9,9))) == "(1,0,0,0,0,0,0,0,0,0,0)"
///
#define HXR_INCR_AND_CARRY_CSD(...)  HXR_MACRO_OVERLOAD00(HXR_INCR_AND_CARRY_CSD, __VA_ARGS__)

/// Expands to the number represented by the argument list, in Comma-Separated-Digits (CSD) form,
/// plus one, or expands to a CSD of equivalent width that represents 0 if the
/// arguments' CSD width is unable to store the resulting value (this is the
/// "wraparound", which in this case is the same thing as integer overflow
/// in most programming languages).
///
/// For example:
///   HXR_STRINGIZE((HXR_INCR_CSD(0,3))) == "(0,4)"
///   HXR_STRINGIZE((HXR_INCR_CSD(1,5))) == "(1,6)"
///   HXR_STRINGIZE((HXR_INCR_CSD(9,9))) == "(0,0)"
///
#define HXR_INCR_AND_WRAP_CSD(...)   HXR_MACRO_OVERLOAD00(HXR_INCR_AND_WRAP_CSD, __VA_ARGS__)

/// Expands to the number represented by the argument list, in Comma-Separated-Digits (CSD) form,
/// plus one.
///
/// If the result can't be stored in an argument list of the same length, then
/// an additional digit is prepended to the result (the new most-significant digit).
///
/// If the arguments already represented the widest possible CSD (e.g. 10 digits,
/// as of this writing) and overflow occured, then the result will be the same as
/// if `HXR_INCR_AND_WRAP_CSD` were called.
///
/// For example:
///   HXR_STRINGIZE((HXR_INCR_CSD(0,3))) == "(0,4)"
///   HXR_STRINGIZE((HXR_INCR_CSD(1,5))) == "(1,6)"
///   HXR_STRINGIZE((HXR_INCR_CSD(9,9))) == "(1,0,0)"
///   HXR_STRINGIZE((HXR_INCR_CSD(9,9,9,9,9,9,9,9,9,9))) == "(0,0,0,0,0,0,0,0,0,0)"
///
#define HXR_INCR_CSD(...)            HXR_MACRO_OVERLOAD00(HXR_INCR_CSD, __VA_ARGS__)

#define HXR_ADD

#define HXR_GET_DIGIT09_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _9
#define HXR_GET_DIGIT08_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _8
#define HXR_GET_DIGIT07_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _7
#define HXR_GET_DIGIT06_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _6
#define HXR_GET_DIGIT05_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _5
#define HXR_GET_DIGIT04_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _4
#define HXR_GET_DIGIT03_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _3
#define HXR_GET_DIGIT02_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _2
#define HXR_GET_DIGIT01_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _1
#define HXR_GET_DIGIT00_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _0

#define HXR_GET_DIGIT09_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT08_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0) _8
#define HXR_GET_DIGIT07_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0) _7
#define HXR_GET_DIGIT06_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0) _6
#define HXR_GET_DIGIT05_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0) _5
#define HXR_GET_DIGIT04_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0) _4
#define HXR_GET_DIGIT03_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0) _3
#define HXR_GET_DIGIT02_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0) _2
#define HXR_GET_DIGIT01_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0) _1
#define HXR_GET_DIGIT00_CSD09(_8,_7,_6,_5,_4,_3,_2,_1,_0) _0

#define HXR_GET_DIGIT09_CSD08(_7,_6,_5,_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT08_CSD08(_7,_6,_5,_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT07_CSD08(_7,_6,_5,_4,_3,_2,_1,_0) _7
#define HXR_GET_DIGIT06_CSD08(_7,_6,_5,_4,_3,_2,_1,_0) _6
#define HXR_GET_DIGIT05_CSD08(_7,_6,_5,_4,_3,_2,_1,_0) _5
#define HXR_GET_DIGIT04_CSD08(_7,_6,_5,_4,_3,_2,_1,_0) _4
#define HXR_GET_DIGIT03_CSD08(_7,_6,_5,_4,_3,_2,_1,_0) _3
#define HXR_GET_DIGIT02_CSD08(_7,_6,_5,_4,_3,_2,_1,_0) _2
#define HXR_GET_DIGIT01_CSD08(_7,_6,_5,_4,_3,_2,_1,_0) _1
#define HXR_GET_DIGIT00_CSD08(_7,_6,_5,_4,_3,_2,_1,_0) _0

#define HXR_GET_DIGIT09_CSD07(_6,_5,_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT08_CSD07(_6,_5,_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT07_CSD07(_6,_5,_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT06_CSD07(_6,_5,_4,_3,_2,_1,_0) _6
#define HXR_GET_DIGIT05_CSD07(_6,_5,_4,_3,_2,_1,_0) _5
#define HXR_GET_DIGIT04_CSD07(_6,_5,_4,_3,_2,_1,_0) _4
#define HXR_GET_DIGIT03_CSD07(_6,_5,_4,_3,_2,_1,_0) _3
#define HXR_GET_DIGIT02_CSD07(_6,_5,_4,_3,_2,_1,_0) _2
#define HXR_GET_DIGIT01_CSD07(_6,_5,_4,_3,_2,_1,_0) _1
#define HXR_GET_DIGIT00_CSD07(_6,_5,_4,_3,_2,_1,_0) _0

#define HXR_GET_DIGIT09_CSD06(_5,_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT08_CSD06(_5,_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT07_CSD06(_5,_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT06_CSD06(_5,_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT05_CSD06(_5,_4,_3,_2,_1,_0) _5
#define HXR_GET_DIGIT04_CSD06(_5,_4,_3,_2,_1,_0) _4
#define HXR_GET_DIGIT03_CSD06(_5,_4,_3,_2,_1,_0) _3
#define HXR_GET_DIGIT02_CSD06(_5,_4,_3,_2,_1,_0) _2
#define HXR_GET_DIGIT01_CSD06(_5,_4,_3,_2,_1,_0) _1
#define HXR_GET_DIGIT00_CSD06(_5,_4,_3,_2,_1,_0) _0

#define HXR_GET_DIGIT09_CSD05(_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT08_CSD05(_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT07_CSD05(_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT06_CSD05(_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT05_CSD05(_4,_3,_2,_1,_0)  0
#define HXR_GET_DIGIT04_CSD05(_4,_3,_2,_1,_0) _4
#define HXR_GET_DIGIT03_CSD05(_4,_3,_2,_1,_0) _3
#define HXR_GET_DIGIT02_CSD05(_4,_3,_2,_1,_0) _2
#define HXR_GET_DIGIT01_CSD05(_4,_3,_2,_1,_0) _1
#define HXR_GET_DIGIT00_CSD05(_4,_3,_2,_1,_0) _0

#define HXR_GET_DIGIT09_CSD04(_3,_2,_1,_0)  0
#define HXR_GET_DIGIT08_CSD04(_3,_2,_1,_0)  0
#define HXR_GET_DIGIT07_CSD04(_3,_2,_1,_0)  0
#define HXR_GET_DIGIT06_CSD04(_3,_2,_1,_0)  0
#define HXR_GET_DIGIT05_CSD04(_3,_2,_1,_0)  0
#define HXR_GET_DIGIT04_CSD04(_3,_2,_1,_0)  0
#define HXR_GET_DIGIT03_CSD04(_3,_2,_1,_0) _3
#define HXR_GET_DIGIT02_CSD04(_3,_2,_1,_0) _2
#define HXR_GET_DIGIT01_CSD04(_3,_2,_1,_0) _1
#define HXR_GET_DIGIT00_CSD04(_3,_2,_1,_0) _0

#define HXR_GET_DIGIT09_CSD03(_2,_1,_0)  0
#define HXR_GET_DIGIT08_CSD03(_2,_1,_0)  0
#define HXR_GET_DIGIT07_CSD03(_2,_1,_0)  0
#define HXR_GET_DIGIT06_CSD03(_2,_1,_0)  0
#define HXR_GET_DIGIT05_CSD03(_2,_1,_0)  0
#define HXR_GET_DIGIT04_CSD03(_2,_1,_0)  0
#define HXR_GET_DIGIT03_CSD03(_2,_1,_0)  0
#define HXR_GET_DIGIT02_CSD03(_2,_1,_0) _2
#define HXR_GET_DIGIT01_CSD03(_2,_1,_0) _1
#define HXR_GET_DIGIT00_CSD03(_2,_1,_0) _0

#define HXR_GET_DIGIT09_CSD02(_1,_0)  0
#define HXR_GET_DIGIT08_CSD02(_1,_0)  0
#define HXR_GET_DIGIT07_CSD02(_1,_0)  0
#define HXR_GET_DIGIT06_CSD02(_1,_0)  0
#define HXR_GET_DIGIT05_CSD02(_1,_0)  0
#define HXR_GET_DIGIT04_CSD02(_1,_0)  0
#define HXR_GET_DIGIT03_CSD02(_1,_0)  0
#define HXR_GET_DIGIT02_CSD02(_1,_0)  0
#define HXR_GET_DIGIT01_CSD02(_1,_0) _1
#define HXR_GET_DIGIT00_CSD02(_1,_0) _0

#define HXR_GET_DIGIT09_CSD01(_0)  0
#define HXR_GET_DIGIT08_CSD01(_0)  0
#define HXR_GET_DIGIT07_CSD01(_0)  0
#define HXR_GET_DIGIT06_CSD01(_0)  0
#define HXR_GET_DIGIT05_CSD01(_0)  0
#define HXR_GET_DIGIT04_CSD01(_0)  0
#define HXR_GET_DIGIT03_CSD01(_0)  0
#define HXR_GET_DIGIT02_CSD01(_0)  0
#define HXR_GET_DIGIT01_CSD01(_0)  0
#define HXR_GET_DIGIT00_CSD01(_0) _0

#define HXR_GET_DIGIT09_CSD(...) HXR_MACRO_OVERLOAD00(HXR_GET_DIGIT09_CSD, __VA_ARGS__)
#define HXR_GET_DIGIT08_CSD(...) HXR_MACRO_OVERLOAD00(HXR_GET_DIGIT08_CSD, __VA_ARGS__)
#define HXR_GET_DIGIT07_CSD(...) HXR_MACRO_OVERLOAD00(HXR_GET_DIGIT07_CSD, __VA_ARGS__)
#define HXR_GET_DIGIT06_CSD(...) HXR_MACRO_OVERLOAD00(HXR_GET_DIGIT06_CSD, __VA_ARGS__)
#define HXR_GET_DIGIT05_CSD(...) HXR_MACRO_OVERLOAD00(HXR_GET_DIGIT05_CSD, __VA_ARGS__)
#define HXR_GET_DIGIT04_CSD(...) HXR_MACRO_OVERLOAD00(HXR_GET_DIGIT04_CSD, __VA_ARGS__)
#define HXR_GET_DIGIT03_CSD(...) HXR_MACRO_OVERLOAD00(HXR_GET_DIGIT03_CSD, __VA_ARGS__)
#define HXR_GET_DIGIT02_CSD(...) HXR_MACRO_OVERLOAD00(HXR_GET_DIGIT02_CSD, __VA_ARGS__)
#define HXR_GET_DIGIT01_CSD(...) HXR_MACRO_OVERLOAD00(HXR_GET_DIGIT01_CSD, __VA_ARGS__)
#define HXR_GET_DIGIT00_CSD(...) HXR_MACRO_OVERLOAD00(HXR_GET_DIGIT00_CSD, __VA_ARGS__)

#define HXR_MAKE_CSD10_10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _9,_8,_7,_6,_5,_4,_3,_2,_1,_0
#define HXR_MAKE_CSD10_09(   _8,_7,_6,_5,_4,_3,_2,_1,_0)  0,_8,_7,_6,_5,_4,_3,_2,_1,_0
#define HXR_MAKE_CSD10_08(      _7,_6,_5,_4,_3,_2,_1,_0)  0, 0,_7,_6,_5,_4,_3,_2,_1,_0
#define HXR_MAKE_CSD10_07(         _6,_5,_4,_3,_2,_1,_0)  0, 0, 0,_6,_5,_4,_3,_2,_1,_0
#define HXR_MAKE_CSD10_06(            _5,_4,_3,_2,_1,_0)  0, 0, 0, 0,_5,_4,_3,_2,_1,_0
#define HXR_MAKE_CSD10_05(               _4,_3,_2,_1,_0)  0, 0, 0, 0, 0,_4,_3,_2,_1,_0
#define HXR_MAKE_CSD10_04(                  _3,_2,_1,_0)  0, 0, 0, 0, 0, 0,_3,_2,_1,_0
#define HXR_MAKE_CSD10_03(                     _2,_1,_0)  0, 0, 0, 0, 0, 0, 0,_2,_1,_0
#define HXR_MAKE_CSD10_02(                        _1,_0)  0, 0, 0, 0, 0, 0, 0, 0,_1,_0
#define HXR_MAKE_CSD10_01(                           _0)  0, 0, 0, 0, 0, 0, 0, 0, 0,_0

#define HXR_MAKE_CSD10(...) HXR_MACRO_OVERLOAD00(HXR_MAKE_CSD10_, __VA_ARGS__)

#define HXR_MAKE_CSD09(...) HXR_CAST_TO_CSD09_FROM_CSD10(HXR_MAKE_CSD10(__VA_ARGS__))
#define HXR_MAKE_CSD08(...) HXR_CAST_TO_CSD08_FROM_CSD10(HXR_MAKE_CSD10(__VA_ARGS__))
#define HXR_MAKE_CSD07(...) HXR_CAST_TO_CSD07_FROM_CSD10(HXR_MAKE_CSD10(__VA_ARGS__))
#define HXR_MAKE_CSD06(...) HXR_CAST_TO_CSD06_FROM_CSD10(HXR_MAKE_CSD10(__VA_ARGS__))
#define HXR_MAKE_CSD05(...) HXR_CAST_TO_CSD05_FROM_CSD10(HXR_MAKE_CSD10(__VA_ARGS__))
#define HXR_MAKE_CSD04(...) HXR_CAST_TO_CSD04_FROM_CSD10(HXR_MAKE_CSD10(__VA_ARGS__))
#define HXR_MAKE_CSD03(...) HXR_CAST_TO_CSD03_FROM_CSD10(HXR_MAKE_CSD10(__VA_ARGS__))
#define HXR_MAKE_CSD02(...) HXR_CAST_TO_CSD02_FROM_CSD10(HXR_MAKE_CSD10(__VA_ARGS__))
#define HXR_MAKE_CSD01(...) HXR_CAST_TO_CSD01_FROM_CSD10(HXR_MAKE_CSD10(__VA_ARGS__))

#define HXR_GENERATE_TOKEN_FROM_CSD10(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0) _9 ## _8 ## _7 ## _6 ## _5 ## _4 ## _3 ## _2 ## _1 ## _0
#define HXR_GENERATE_TOKEN_FROM_CSD09(   _8,_7,_6,_5,_4,_3,_2,_1,_0)       _8 ## _7 ## _6 ## _5 ## _4 ## _3 ## _2 ## _1 ## _0
#define HXR_GENERATE_TOKEN_FROM_CSD08(      _7,_6,_5,_4,_3,_2,_1,_0)             _7 ## _6 ## _5 ## _4 ## _3 ## _2 ## _1 ## _0
#define HXR_GENERATE_TOKEN_FROM_CSD07(         _6,_5,_4,_3,_2,_1,_0)                   _6 ## _5 ## _4 ## _3 ## _2 ## _1 ## _0
#define HXR_GENERATE_TOKEN_FROM_CSD06(            _5,_4,_3,_2,_1,_0)                         _5 ## _4 ## _3 ## _2 ## _1 ## _0
#define HXR_GENERATE_TOKEN_FROM_CSD05(               _4,_3,_2,_1,_0)                               _4 ## _3 ## _2 ## _1 ## _0
#define HXR_GENERATE_TOKEN_FROM_CSD04(                  _3,_2,_1,_0)                                     _3 ## _2 ## _1 ## _0
#define HXR_GENERATE_TOKEN_FROM_CSD03(                     _2,_1,_0)                                           _2 ## _1 ## _0
#define HXR_GENERATE_TOKEN_FROM_CSD02(                        _1,_0)                                                 _1 ## _0
#define HXR_GENERATE_TOKEN_FROM_CSD01(                           _0)                                                       _0

#define HXR_GENERATE_TOKEN_FROM_CSD(...) HXR_MACRO_OVERLOAD00(HXR_GENERATE_TOKEN_FROM_CSD, __VA_ARGS__)

#endif
