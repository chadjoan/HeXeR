#ifndef HXR_HEXER_H
#define HXR_HEXER_H

#include "hxr/pp_core.h"

#if defined(HXR_DOCUMENTATION_BUILD) && (HXR_DOCUMENTATION_BUILD)
#undef  HXR_DOCUMENTATION_BUILD

/// The `HXR_DOCUMENTATION_BUILD` flag causes simplified-but-representative
/// versions of the more complicated or inaccessible HeXeR definitions to be
/// chosen by the preprocessor, with the sole intent of making the resulting
/// documentation easier to read.
///
/// When actually *compiling* HeXeR, which is the most likely scenario,
/// it is best to ignore this flag or set it to `0` (false).
///
/// Otherwise, this flag should (only) be set to 1 when the compiler
/// (or other document generator) is processing HeXeR's definitions to
/// generate documentation.
///
/// This should never be set (ex: to `1`) when working code is expected to
/// result. The documentation-specific preprocessor path might not generate
/// a very useful binary, object file, or header.
///
/// If not specified in calling code, this will default to `0` (false).
///
#define HXR_DOCUMENTATION_BUILD  (0)

// We messed with it to make the document generator see the correct default
// value (if it even matters). Now set it back, otherwise the almost-everything
// won't get documented.
#undef  HXR_DOCUMENTATION_BUILD
#define HXR_DOCUMENTATION_BUILD  (1)

#endif

#ifndef HXR_DOCUMENTATION_BUILD
#define HXR_DOCUMENTATION_BUILD  (0)
#endif

// ===== HXR_CALL_HISTORY_FNCLASSES =====
#if defined(HXR_CALL_HISTORY_FNCLASSES) && HXR_DOCUMENTATION_BUILD
#undef HXR_CALL_HISTORY_FNCLASSES
#endif

#ifndef HXR_CALL_HISTORY_FNCLASSES
/// `HXR_CALL_HISTORY_FNCLASSES` is used to determine which `HXR_ENTER_FUNCTION`
/// statements contribute function call history entries.
///
/// The exact mechanism is as follows:
/// Whether a function call is included in function call history is determined
/// by the `func_classification` parameter of the `HXR_ENTER_FUNCTION` macro
/// and by the value of `HXR_CALL_HISTORY_FNCLASSES`.
/// If the expression `func_classification & HXR_CALL_HISTORY_FNCLASSES`
/// evaluates to a non-zero value, then all calls to that function are added
/// to the function call history.
/// If the expression `func_classification & HXR_CALL_HISTORY_FNCLASSES`
/// evaluates to zero, then calls to that function will not be included
/// in function call history.
///
/// In principle, as long as `HXR_CALL_HISTORY_FNCLASSES` and `HXR_ENTER_FUNCTION`'s
/// `func_classification` parameter are both constant expressions, then
/// the compiler will evaluate the constant expressions and eliminate the
/// unreachable code, thus making this a no-cost operation when the function
/// is not included in function call history.
///
#define HXR_CALL_HISTORY_FNCLASSES  (1)

#endif

// ===== HXR_ENABLE_LIBC =====
#if defined(HXR_ENABLE_LIBC) && HXR_ENABLE_LIBC
#undef HXR_ENABLE_LIBC
#endif

#ifndef HXR_ENABLE_LIBC
/// The `HXR_ENABLE_LIBC` macro determines whether HeXeR is allowed to use
/// ANY standard C library (libc) functionality.
///
/// Define it as (1) to enable libc usage. (The default.)
/// Define it as (0) to disable libc usage.
///
/// NOTE: This macro is currently unimplemented! (Well, partially implemented.)
///
/// This configuration macro may be helpful if you are writing an embedded
/// system, OS kernel, or working in any other environment where the C
/// standard library is not present.
///
/// In any other, more normal situation where a C standard library (libc) is
/// available (ex: applications development on Windows, Linux, MacOS, etc),
/// then this configuration macro should be left undefined or set to (1).
/// Setting it to (0) without any plan to replace the few necessary low-level
/// `libc` features that HeXeR requires will result in a build of HeXeR that
/// is, or at least seems, very broken.
///
/// This is a meta-macro that controls the default values of other configuration
/// macros, especially function-and-feature-substitution macros like
/// `HXR_VSNPRINTF_DEFAULT` and `HXR_MALLOC_DEFAULT`. It will cause these
/// macros to provide values that either do nothing or (more commonly) print
/// error messages using printf/debugf when called. Since `HXR_DEBUGF_DEFAULT`
/// will be set to `HXR(debugf_noop)` by default in this configuration, you
/// will need to set `HXR_DEBUGF_DEFAULT` to some `printf` equivalent to be
/// able to see any of the said error messages. Likewise, having a working
/// `HXR_VSNPRINTF_DEFAULT` and default allocator (`malloc` and friends)
/// may be imperative for getting basic functionality out of HeXeR.
///
/// This macro also affects configuration macro defaults in a way that forbids
/// HeXeR from using any C stardard library headers.
///
/// If this is set to (0), it is expected that the caller will define other
/// configuration macros as necessary to provide any functionality that
/// HeXeR will need to implement the caller's requirements.
///
/// All of the above effects can be accomplished by very carefully manipulating
/// other configuration macros, but setting `HXR_ENABLE_LIBC` to (0) is an
/// easier and less-error-prone way to make HeXeR libc-free by default.
///
/// Notable, this will not affect subsystem toggle macros such as
/// `HXR_ENABLE_FILE_IO` and `HXR_ENABLE_SYSLOG`, because those things can
/// still be done without standard libraries, assuming replacement functions
/// are provided to HeXeR.
///
#define HXR_ENABLE_LIBC (1)
// TODO: finish implementing this.

#endif

// ===== HXR_ENABLE_FILE_IO =====
#if defined(HXR_ENABLE_FILE_IO) && HXR_DOCUMENTATION_BUILD
#undef HXR_ENABLE_FILE_IO
#endif

#ifndef HXR_ENABLE_FILE_IO
/// The value of the `HXR_ENABLE_FILE_IO` macro determines whether any functions
/// or struct fields related to file-I/O (including output to `stdout` and
/// `stderr`) are included in HeXeR during compilation.
///
/// By default, this is defined as (1).
///
/// If you are writing a library or writing a program that depends on any
/// libraries that use HeXeR (either directly or indirectly), then it is a good
/// idea to just leave this one alone or set it to `1`. This will keep your
/// version of HeXeR compatible with others at an ABI-level.
///
/// If you are writing a small program, or maybe embedded code or firmware, then
/// this option *might* be helpful for reducing HeXeR's code size (in addition
/// to being a fairly severe way to disable the optional `vfprintf` dependency).
///
#define HXR_ENABLE_FILE_IO (1)

#endif

// ===== HXR_ENABLE_SYSLOG =====
#if defined(HXR_ENABLE_SYSLOG) && HXR_DOCUMENTATION_BUILD
#undef HXR_ENABLE_SYSLOG
#endif

#ifndef HXR_ENABLE_SYSLOG
/// The value of the `HXR_ENABLE_SYSLOG` macro determines whether any functions
/// or struct fields related to syslog-style logging are included in HeXeR
/// during compilation.
///
/// By default, this is defined as (1).
///
/// If you are writing a library or writing a program that depends on any
/// libraries that use HeXeR (either directly or indirectly), then it is a good
/// idea to just leave this one alone or set it to `1`. This will keep your
/// version of HeXeR compatible with others at an ABI-level.
///
/// If you are writing a small program, or maybe embedded code or firmware, then
/// this option *might* be helpful for reducing HeXeR's code size (in addition
/// to being a fairly severe way to disable the optional `vsyslog` dependency).
///
#define HXR_ENABLE_SYSLOG (1)

#endif

// ===== HXR_VFPRINTF_DEFAULT =====
#if defined(HXR_VFPRINTF_DEFAULT) && HXR_DOCUMENTATION_BUILD
#undef HXR_VFPRINTF_DEFAULT
#endif

#ifndef HXR_VFPRINTF_DEFAULT

/// `HXR_VFPRINTF_DEFAULT` defines the function that HeXeR calls whenever
/// vfprintf would be (or is) invoked.
///
/// By default, this expands to `vfprintf`.
///
/// By defining this macro to another function with the same signature as
/// `vfprintf`, it is possible to replace this function with a substitute,
/// and in such a complete way that compiling HeXeR will create a link-time
/// dependency on the substitute and NOT on `vfprintf`.
///
/// Additionally, defining this macro may allow HeXeR to avoid `#include`ing
/// standard C library headers (ex: `#include <stdio.h>`), though the precise
/// effectiveness of this will also depend on the values of other
/// function-and-feature-substitution macros.
///
/// If you are running HeXeR in an environment where `vfprintf` or a reasonable
/// equivalent isn't available (ex: some embedded or firmware systems), then
/// any file I/O normally done through `vfprintf` can be disabled entirely by
/// defining this to reference a do-nothing function with the same signature
/// as `vfprintf`:
///
/// ```C
/// #include "hexer.h"
/// #define HXR_VFPRINTF_DEFAULT  HXR(vfprintf_noop)
/// // ...
/// #include "hexer.c"
/// ```
#define HXR_VFPRINTF_DEFAULT  vfprintf

#endif

// ===== HXR_VSNPRINTF_DEFAULT =====
#if defined(HXR_VSNPRINTF_DEFAULT) && HXR_DOCUMENTATION_BUILD
#undef HXR_VSNPRINTF_DEFAULT
#endif

#ifndef HXR_VSNPRINTF_DEFAULT

/// `HXR_VSNPRINTF_DEFAULT` defines the function that HeXeR calls whenever
/// vsnprintf would be (or is) invoked.
///
/// By default, this expands to `vsnprintf`.
///
/// By defining this macro to another function with the same signature as
/// `vsnprintf`, it is possible to replace this function with a substitute,
/// and in such a complete way that compiling HeXeR will create a link-time
/// dependency on the substitute and NOT on `vsnprintf`.
///
/// It is not recommended to replace `vsnprintf` with a do-nothing version
/// because many string-handling portions of HeXeR expect to have this
/// function available.
#define HXR_VSNPRINTF_DEFAULT  vsnprintf

#endif

// ===== HXR_VSYSLOG_DEFAULT =====
#if defined(HXR_VSYSLOG_DEFAULT) && HXR_DOCUMENTATION_BUILD
#undef HXR_VSYSLOG_DEFAULT
#endif

#ifndef HXR_VSYSLOG_DEFAULT

/// `HXR_VSYSLOG_DEFAULT` defines the function that HeXeR calls whenever
/// vsyslog would be (or is) invoked.
///
/// By default, this expands to `vsyslog`.
///
/// By defining this macro to another function with the same signature as
/// `vsyslog`, it is possible to replace this function with a substitute,
/// and in such a complete way that compiling HeXeR will create a link-time
/// dependency on the substitute and NOT on `vsyslog`.
///
/// If you are running HeXeR in an environment where `vsyslog` or a reasonable
/// equivalent isn't available (ex: some embedded or firmware systems), then
/// any logging normally done through `vsyslog` can be disabled entirely by
/// defining this to reference a do-nothing function with the same signature
/// as `vsyslog`:
/// ===
/// #include "hexer.h"
/// #define HXR_VSYSLOG_DEFAULT  HXR(vsyslog_noop)
/// // ...
/// #include "hexer.c"
/// ===
#define HXR_VSYSLOG_DEFAULT  vsyslog

#endif

// ===== HXR_DEBUGF_DEFAULT =====
#if defined(HXR_DEBUGF_DEFAULT) && HXR_DOCUMENTATION_BUILD
#undef HXR_DEBUGF_DEFAULT
#endif

#ifndef HXR_DEBUGF_DEFAULT

/// `HXR_DEBUGF_DEFAULT` defines the function that the `hxr_debugf_`
/// internal function pointer will point to right after HeXeR initializes.
///
/// This function is used by HeXeR wherever it would otherwise call `printf`.
/// This isn't called very often really, but it does happen in some places
/// where valid process+thread state isn't available and HeXeR needs to print
/// information regarding whatever situation lead to such a condition.
///
/// If you are running HeXeR in an environment where `printf` or a reasonable
/// equivalent isn't available (ex: some embedded or firmware systems), then
/// this printing can be disabled entirely by defining this to reference a
/// do-nothing function with the same signature as `printf`:
/// ===
/// #include "hexer.h"
/// #define HXR_DEBUGF_DEFAULT  HXR(debugf_noop)
/// // ...
/// #include "hexer.c"
/// ===
#define HXR_DEBUGF_DEFAULT  printf

#endif

// ===== HXR_MALLOC_DEFAULT =====
#if defined(HXR_MALLOC_DEFAULT) && HXR_DOCUMENTATION_BUILD
#undef HXR_MALLOC_DEFAULT
#endif

#ifndef HXR_MALLOC_DEFAULT

/// `HXR_MALLOC_DEFAULT` defines the function that `hxr_process` and
/// `hxr_thread` will use by default for memory allocation.
///
/// By default, this expands to `malloc`.
///
/// By defining this macro to another function with the same signature as
/// `malloc`, it is possible to replace this function with a substitute,
/// and in such a complete way that compiling HeXeR will create a link-time
/// dependency on the substitute and NOT on `malloc`.
///
#define HXR_MALLOC_DEFAULT  malloc

#endif

// ===== HXR_REALLOC_DEFAULT =====
#if defined(HXR_REALLOC_DEFAULT) && HXR_DOCUMENTATION_BUILD
#undef HXR_REALLOC_DEFAULT
#endif

#ifndef HXR_REALLOC_DEFAULT

/// `HXR_REALLOC_DEFAULT` defines the function that `hxr_process` and
/// `hxr_thread` will use by default for reallocating memory that was
/// initially allocated using `HXR_MALLOC_DEFAULT`.
///
/// By default, this expands to `realloc`.
///
/// By defining this macro to another function with the same signature as
/// `realloc`, it is possible to replace this function with a substitute,
/// and in such a complete way that compiling HeXeR will create a link-time
/// dependency on the substitute and NOT on `realloc`.
///
#define HXR_REALLOC_DEFAULT  realloc

#endif

// ===== HXR_FREE_DEFAULT =====
#if defined(HXR_FREE_DEFAULT) && HXR_DOCUMENTATION_BUILD
#undef HXR_FREE_DEFAULT
#endif

#ifndef HXR_FREE_DEFAULT

/// `HXR_FREE_DEFAULT` defines the function that `hxr_process` and
/// `hxr_thread` will use by default for releasing any memory that was
/// allocated using `HXR_MALLOC_DEFAULT` or `HXR_REALLOC_DEFAULT`.
///
/// By default, this expands to `free`.
///
/// By defining this macro to another function with the same signature as
/// `free`, it is possible to replace this function with a substitute,
/// and in such a complete way that compiling HeXeR will create a link-time
/// dependency on the substitute and NOT on `free`.
///
#define HXR_FREE_DEFAULT  free

#endif

// ===== HXR_ALLOW_VLAS =====
#if defined(HXR_ALLOW_VLAS) && HXR_DOCUMENTATION_BUILD
#undef HXR_ALLOW_VLAS
#endif

#ifndef HXR_ALLOW_VLAS

/// `HXR_ALLOW_VLAS` determines whether or not HeXeR may use the standard C99
/// "variable length arrays" feature.
///
/// VLAs are used to optimize scope guards (e.g. `ON_ABORT`, `ON_EXIT`, etc)
/// by attempting to stack-allocate only the precise amount of memory necessary.
/// This can only be done after a function has been executed once and usage
/// counts obtained, but it does guarantee that subsequent calls will be
/// able to handle, with great efficiency, as many scope guards as any of the
/// previous calls to the function have encountered.
///
/// This can be disabled because some developers feel that VLAs may result in
/// damaged debugging output, excessive stack allocation, and even security
/// vulnerabilities (due to the runtime-influence on stack layout). As far
/// as HeXeR's usage patterns are concerned, only the concern about
/// debugging output should be valid: there is a finite maximum amount of
/// stack memory that may be allocated for HeXeR's features, with protections
/// in place to ensure that the same dynamic stack allocations cannot
/// be executed repeatedly within the same function call frame. The only reason
/// things like VLAs are necessary is because this "finite maximum" is
/// impossible to determine at compile-time due to limitations of the C
/// programming language (e.g. there is no preprocessor feature that allows
/// us to track the number of scope guard macro expansions in between their
/// flanking begin-end macro expansions, and even if there were, it would be
/// difficult or impossible to insert this information at the 'BEGIN' macro
/// where it would be needed).
///
/// Before discussing further, keep in mind that the stack memory cost for
/// each scope guard statement (e.g. `ON_ABORT`, `ON_EXIT`, etc) is generally
/// very small, being around 6-12 bytes each, depending on other available
/// features (C11 atomics, __COUNTER__, etc). Individually, this is not
/// significant. The worry that leads to optimizations, such as VLAs, is that
/// allocating stack-based fixed-size arrays large enough to handle common
/// cases efficiently (ex: 4-16 handler structs) will result in consuming
/// 24-to-192 bytes on every function call, which might seem small, but adds up
/// if the stack becomes deep. While this might not overflow a stack on its own,
/// it is significant. It is desirable to be able to allocate ONLY the amount
/// of memory necessary to support exactly the number of scope guard statements
/// found in a function.
///
/// Also, stack allocation of these features is important for encouraging
/// locality of reference. If scope guard handlers are stored in thread local
/// memory or, god forbid, heap memory, then simply calling a function and
/// executing a stack guard statement could result in a cache miss. HeXeR works
/// diligently on your behalf to avoid such misfortunes!
///
/// Generally, the scope guard implementation will attempt these allocation
/// strategies, in order:
/// * Small fixed-size array allocated for given function call by `HXR_CLEANUP_SCOPE_BEGIN`.
///     In the future, it might be possible to use profiling-guidance or
///     special preprocessing to precisely allocate these, which would make the
///     subsequent fall-back strategies unnecessary at the cost of more complicated
///     build/release strategies.
/// * Variable length array, if memory requirements were recorded from previous calls.
/// * Thread-local-storage, if fast options are available (ex: C11 or compiler extensions, when implemented).
/// * Calls to `malloc` and `realloc`. Allocated memory will be persisted (not `free`d),
///     unless the function is called recursively (memory used in excess of the initial
///     call will be `free`d as the nested calls return).
///
/// 
/// 
///
#define HXR_ALLOW_VLAS  (1)

#endif

// ===== HXR_LINKAGE_PREFIX =====
#if defined(HXR_LINKAGE_PREFIX) && HXR_DOCUMENTATION_BUILD
#undef HXR_LINKAGE_PREFIX
#endif

#ifndef HXR_LINKAGE_PREFIX

/// `HXR_LINKAGE_PREFIX` defines the characters placed at the beginning of any
/// HeXeR definitions that appear in object code emitted by the compiler.
///
/// The default of `hxr_` should be fine in most conceivable scenarios.
/// However, should you use HeXeR along with another library that uses symbols
/// (function names, global extern variables) starting with `hxr_`, then this
/// setting allows you to change HeXeR's prefix to avoid link-time collisions.
/// This could also be useful if two instances of HeXeR's object code end
/// up linked into the same executable: if, for whatever reason, it is
/// absolutely impossible to avoid such a misfortune (ex: because the
/// configurations are different and need to be different, or because build
/// systems), then changing the prefix of one of the instances would allow
/// linking to proceed without generating ALL of the symbol collisions.
///
/// Example:
/// ===
/// // ----- Before: -----
///
/// #include "hexer.h"
/// #include "hexer.c"
///
/// int main(int argc, char *argv[]) {
///     hxr_process* proc = hxr_start();
///     // ... do things ...
///     hxr_stop(proc);
///     return 0;
/// }
///
/// // ----- After: -----
///
/// #define HXR_LINKAGE_PREFIX  mypfx_
/// #include "hexer.h"
/// #include "hexer.c"
///
/// int main(int argc, char *argv[]) {
///     // Now we call "mypfx_start" instead of "hxr_start".
///     // "hxr_start" is undefined at this point.
///     hxr_process* hxproc = mypfx_start();
///
///     // Identifiers without linkage, such as struct names, will still have
///     // the `hxr_` prefix. However, to make it possible to write alt-prefix
///     // code without a confusing mix of symbols, macro definitions in HeXeR
///     // create aliases of these that have the prefix replaced:
///     mypfx_process* myproc = proc;
///     hxr_process*   proc = myproc;
///
///     // ... do things ...
///
///     // As above, any HeXeR function that, for example, accepts a `hxr_process*`,
///     // will also accept a `mypfx_process*`:
///     mypfx_stop(proc);
///     // These types will be the same after preprocessing.
/// }
///
/// // ---------------
/// // If you need adjustable control the symbols emitted in calling code,
/// // you can use the `HXR` macro to delegate identifier generation:
///
/// #define HXR_LINKAGE_PREFIX  mypfx_
/// #include "hexer.h"
/// #include "hexer.c"
///
/// int main(int argc, char *argv[]) {
///     HXR(process)* proc = HXR(start)();
///     // ... do things ...
///     HXR(stop)(proc);
/// }
///
/// // This is notably more ugly than just using function names directly,
/// // so it might only be worth doing if you really need this feature.
/// ===
///
#define HXR_LINKAGE_PREFIX hxr_

#define HXR__PREFIX_ALIAS(symbol)
#else
#define HXR__PREFIX_ALIAS(symbol)  typedef symbol HXR(symbol);
#endif

#define HXR__(prefix, symbol)  prefix ## symbol

/// The `HXR` macro is used to apply the `HXR_LINKAGE_PREFIX` to identifiers.
/// See the `HXR_LINKAGE_PREFIX` documentation for more details.
#define HXR(symbol)    HXR__(HXR_LINKAGE_PREFIX, symbol)

// Internal-use function that is probably usually `printf`. Avoid using it if
// possible. However, if there is no valid `hxr_process` or `hxr_thread` to
// report errors with, then it is appropriate to use this instead.
extern int  (HXR(debugf_)*)(const char *str, ...);

#if (HXR_ENABLE_FILE_IO) || (HXR_DOCUMENTATION_BUILD)
/// Set `HXR_VFPRINTF_DEFAULT` to this to discard any text sent to fprintf
/// through HeXeR's file I/O functions.
///
/// Example:
/// ===
/// #include "hexer.h"
/// #define HXR_VFPRINTF_DEFAULT  HXR(vfprintf_noop)
/// // ...
/// #include "hexer.c"
/// ===
///
/// This is a way to completely remove `vfprintf` as a dependency of HeXeR, but
/// it is preferable, if possible, to provide a replacement that sends this
/// text or data to somewhere where it can be accessed later.
///
/// If you know that file I/O (including the standard descriptors `stdout` and
/// `stderr`) is not used by your project and you are trying to reduce compiled
/// code size, then it might be more productive to define `HXR_ENABLE_FILE_IO`
/// to 0. This is a more severe change that could result in binary incompability
/// with other HeXeR configurations, but it does prevent those functions from
/// being emitted at all, and removes any associated state from HeXeR objects.
///
/// Similarly, if you are writing a system that must have no dependencies
/// on the standard C library whatsoever, then the `HXR_ENABLE_LIBC`
/// configuration macro may also be of interest.
///
int HXR(vfprintf_noop)(FILE *fd, const char *fmtstr, va_list vargs);

/// Set `HXR_VFPRINTF_DEFAULT` to this to have any text sent to fprintf
/// redirected to the `hxr_debugf` function.
///
/// Example:
/// ```C
/// #include "hexer.h"
/// #define HXR_VFPRINTF_DEFAULT  HXR(vfprintf_redirect_debugf)
/// // ...
/// #include "hexer.c"
/// ```
///
/// This is a way to completely remove `vfprintf` as a dependency of HeXeR.
///
/// This implementation will ignore the `fd` parameter.
///
int HXR(vfprintf_redirect_debugf)(FILE *fd, const char *fmtstr, va_list vargs);

// TODO: HXR_ENABLE_LIBC disabling will need a config macro for replacing
// <stdio.h> types like FILE.
#endif

/// Set `HXR_VSNPRINTF_DEFAULT` to this to cause rare and
/// difficult-to-troubleshoot bugs in your programs.
///
/// Example:
/// ===
/// #include "hexer.h"
/// #define HXR_VSNPRINTF_DEFAULT  HXR(vsnprintf_noop)
/// // ...
/// #include "hexer.c"
/// ===
///
/// The above description is omninous for a reason:
/// HeXeR has to have SOME way to format strings, both for when one of its
/// string formatting functions is called, and for reporting its own internal
/// state. And when HeXeR can't delegate formatting to some other
/// I/O subsystem, such as `vfprintf` or `vsyslog`, then `vsnprintf` is used
/// instead. This is likely to happen even when the other I/O functions are
/// enabled, because it is sometimes necessary to analyze an already-formatted
/// string (ex: to align columns, rewrap, etc) before sending it elsewhere.
///
/// So while it *is* possible to just toss those bytes into the dark abyss,
/// it will likely just make HeXeR randomly fail to output messages of every
/// kind, regardless of said messages' origin(s) or destination(s).
///
/// If a dependency on `vsnprintf` is unacceptable for your program, then it
/// is strongly recommended that you find a replacement. ProTip: [TODO:LINK: musl libc]
/// has one under a non-restrictive license (MIT) in its [vsnprintf (TODO:LINK) https://git.musl-libc.org/cgit/musl/tree/src/stdio/vsnprintf.c] and [vfprintf (TODO:LINK) https://git.musl-libc.org/cgit/musl/tree/src/stdio/vfprintf.c]
/// source files.
///
int HXR(vsnprintf_noop)(char *s, size_t n, const char *format, va_list vargs);
// TODO: maybe make a minimal version that just handles %s and %zd, or whatever
// is *just barely* sufficient for HeXeR to print its own messages?

#if (HXR_ENABLE_SYSLOG) || (HXR_DOCUMENTATION_BUILD)
/// Set `HXR_VSYSLOG_DEFAULT` to this to discard any text sent to syslog
/// through HeXeR's logging functions.
///
/// Example:
/// ===
/// #include "hexer.h"
/// #define HXR_VSYSLOG_DEFAULT  HXR(vsyslog_noop)
/// // ...
/// #include "hexer.c"
/// ===
///
/// This is a way to completely remove `vsyslog` as a dependency of HeXeR, but
/// it is preferable, if possible, to provide a replacement that sends log
/// messages to a logger of your choosing.
///
/// If you know that syslog-style logging is not used by your project and you
/// are trying to reduce compiled code size, then it might be more productive
/// to define `HXR_ENABLE_SYSLOG` to 0. This is a more severe change that could
/// result in binary incompability with other HeXeR configurations, but it does
/// prevent those functions from being emitted at all, and removes any
/// associated state from HeXeR objects.
///
void HXR(vsyslog_noop)(int priority, const char *format, va_list ap);
#endif

/// Set `HXR_DEBUGF_DEFAULT` to this to completely disable the few `printf`-like
/// debugging statements used in HeXeR.
///
/// Example:
/// ===
/// #include "hexer.h"
/// #define HXR_DEBUGF_DEFAULT  HXR(debugf_noop)
/// // ...
/// #include "hexer.c"
/// ===
///
/// This is a way to completely remove `printf` as a dependency of HeXeR, but
/// it is preferable, if possible, to provide a replacement that sends text
/// to some place you can read. After all, this is the thing that gets called
/// when HeXeR's state or environment is so corrupt or wrong that it can't
/// trust itself to relay error messages to your code and must instead use
/// directly dump it to stdout using a primitive I/O function.
///
int HXR(debugf_noop)(const char *str, ...);

/// Stores HeXeR configuration and metadata related to the current running process.
///
/// This includes default values for various C-library functions:
/// - `malloc`, `realloc`, and `free` (the allocator interface)
/// - `vsnprintf` (buffered string formatting)
/// - `vfprintf` (file I/O)
/// - `vsyslog` (syslog-style logging)
///
/// Other default settings may be found here too, such as format specifiers
/// that determine how various endpoints present feedback messages.
///
/// Any newly created `hxr_thread` objects will inherit or construct their
/// settings and references from the values in the `hxr_process` object, but
/// it is possible to modify the `hxr_thread` objects to have different
/// configurations after they have been instantiated.
///
/// Unless you need to change any of those things, it might be entirely
/// unnecessary to retrieve and interact with the `hxr_process` object.
typedef struct S_HXR_PROCESS
{
	void **dynamic_embeds;

#ifdef HXR_PROCESS_STATIC_EMBEDS
#define HXR_X(embed_type, embed_name) embed_type embed_name;
	HXR_PROCESS_STATIC_EMBEDS(HXR_X)
#undef HXR_X
#endif
} hxr_process;
HXR__PREFIX_ALIAS(process);

/// The `hxr_thread` object is at the center of much of HeXeR's functionality.
///
/// Here are some major points;
/// - It buffers data during the creation of sructured feedback messages
///     (ex: error messages).
/// - It persists user feedback (ex: error messages) between function calls, so
///     that this information can be displayed using proper means (instead of
///     just being `printf'd` from algorithms).
/// - Stores HeXeR configuration and metadata related to the current thread.
typedef struct S_HXR_THREAD
{
	// TODO: implement this.
	void **dynamic_embeds;

#ifdef HXR_THREAD_STATIC_EMBEDS
#define HXR_X(embed_type, embed_name) embed_type embed_name;
	HXR_THREAD_STATIC_EMBEDS(HXR_X)
#undef HXR_X
#endif
} hxr_thread;
HXR__PREFIX_ALIAS(thread);

/// Initializes the HeXeR library and creates the `hxr_process*` object.
hxr_process  *HXR(start)();

/// Returns the process-wide instance of the `hxr_process` object.
hxr_process  *HXR(get_current_process)();


/// Retreives a reference to the current thread's `hxr_thread` object by using
/// the given `proc` object to accelerate retrieval, if possible (it may or may not help).
///
/// Once a `hxr_thread*` object is retrieved, it is recommended to simply pass
/// it to any other functions, typically as their first argument. This methodology
/// has fairly predictable (and lightweight) performance implications. By
/// comparison, calling `hxr_process_get_current_thread` repeatedly might,
/// each time, invoke a `pthread` function to retrieve the pointer.
/// This may or may not be fast (depending on implementation), and while
/// it's *probably* reasonably fast, it also probably isn't as cache-friendly
/// as simply looking at a pointer that's already on the call stack
/// (as the enclosing function's first argument).
///
/// Note that the caller never needs to explicitly create these objects.
/// They are created lazily (on an as-needed basis), and there is always
/// only one per POSIX thread.
hxr_thread *HXR(process_get_current_thread)(hxr_process *proc);

/// Retreives a reference to the current thread's `hxr_thread` object from
/// it's thread-local storage.
///
/// Once a `hxr_thread*` object is retrieved, it is recommended to simply pass
/// it to any other functions, typically as their first argument. This methodology
/// has fairly predictable (and lightweight) performance implications. By
/// comparison, calling `hxr_get_current_thread` repeatedly will, each time,
/// invoke a `pthread` function to retrieve the pointer. This may or may not
/// be fast (depending on implementation), and while it's *probably* reasonably
/// fast, it also probably isn't as cache-friendly as simply looking at
/// a pointer that's already on the call stack (as the enclosing function's
/// first argument).
///
/// Note that the caller never needs to explicitly create these objects.
/// They are created lazily (on an as-needed basis), and there is always
/// only one per POSIX thread.
hxr_thread *HXR(get_current_thread)();

/// Implementing this callback allows calling code to print/handle messages,
/// errors, etc, as they happen within a called function, instead fo waiting
/// for the function to finish. Most of the time this won't matter, but it is
/// particularly important when calling any Zeo functions that could take
/// a while to return.
typedef void (*hxr_feedback_handler)(hxr_thread*, hxr_feedback_message*);
HXR__PREFIX_ALIAS(feedback_handler);

/// Returns the (global, read-only) function representing the do-nothing
/// feedback handler.
hxr_feedback_handler HXR(feedback_noop)();

/// Retrieves the current thread's feedback handler. If none is assigned,
/// returns `hxr_feedback_noop()` instead.
hxr_feedback_handler HXR(thread_get_feedback_handler)(hxr_thread *t);

/// Sets the current thread's feedback handler.
///
/// If NULL is passed for `callback`, then the thread's feedback handler will
/// be set to the "ignore all messages" handler defined by `hxr_feedback_noop()`.
/// (Rationale: This prevents a NULL-dereference possibility while still providing
/// a sentinel value. We are also reducing the number of ways in which this
/// function can fail.)
///
/// See the `hxr_feedback_handler`'s definition for details about the
/// feedback handler itself and an explanation of why anyone would want to use one.
void HXR(thread_set_message_handler(hxr_thread *t, hxr_feedback_handler callback, void *callback_context);

size_t  HXR(error_count)(hxr_thread *t);
size_t  HXR(message_count)(hxr_thread *t);
void    HXR(print_message)(hxr_thread *t, hxr_feedback_message *msg, FILE *fd);
void    HXR(log_message)(hxr_thread *t, hxr_feedback_message *msg);

/// Returns: The number of messages printed.
size_t  HXR(print_messages)(hxr_thread *t, FILE *fd);

/// Returns: The number of messages logged.
size_t  HXR(log_messages)(hxr_thread *t);

/// Returns: The number of messages deleted.
size_t  HXR(clear_messages)(hxr_thread *t);


// Message formatting:
//
// must handle multi-line messages
// first line:   prefix, text
// middle lines: prefix, text
// last line:    prefix, text
//
// must handle line re-wrapping.
// setting for line length maximum
//
// Prefix elements:
//    Timestamping. (date, time, datetime, ISO8601, formatted, etc)
//    Hostname.
//    pid (process id)
//    uid (user)
//    gid (group)
//    Executable name
//    Current working directory
//
// Option to append spaces to any prefix that isn't as long as the others.
// Option to establish a minimum (padded) prefix length (e.g. so that an entire
//   stringized message stream will have the same indentation/alignment).
//
// Also might need to consider message structure. Maybe.



typedef struct S_HXR__SOURCE_LOCATION
{
	const char *file;
	const char *func;
	size_t     line;
}
hxr_source_location_;

static inline hxr_source_location_
	HXR(make_source_loc_)(const char *file, const char *func, size_t line)
{
	hxr_source_location_  result;
	result.file = file;
	result.func = func;
	result.line = line;
	return result;
}

#define HXR_SOURCE_LOCATION_HERE_ \
	(hxr_make_source_loc_(__FILE__, __FUNCTION__, __LINE__))

#define HXR_STACK_FRAME_PTR_HERE_  (&__func__)




// The `frame_id` parameter should be passed a pointer to the calling stack
// frame's copy of the current thread's `hxr_thread` pointer. Thus, the outer
// pointer uniquely identifies the stack frame that called `HXR_ENTER_FUNCTION(...)`.
//
// The `func_classification` parameter is used to pass information about the
// nature of the function being annotated. As of 2020-10-03, this function will
// ignore the `func_classification` parameter, because any processing on those
// values will already have been completed by macro preprocessing (right now
// it's just used to turn the HXR_ENTER_FUNCTION expansion on or off based on
// whether the developer needs a class of functions to be logged or traced at
// that time).
void HXR(thread_frame_entrance_)(
		hxr_thread **frame_id,
		size_t     func_classification,
		const char *file_name,
		const char *function_name,
		int        line_number);

void HXR(thread_frame_exit_)(
		hxr_thread **frame_id,
		const char *file_name,
		const char *function_name,
		int        line_number);

/// Function classes used to identify distinct uses of HXR_ENTER_FUNCTION.
/// These MUST be macro definitions. These are expanded in a preprocessor #if
/// statement to acheive compile-time conditional compilation. C variables will
/// not be interpreted by the preprocessor correctly, and thus cannot be used
/// for these.
#define  HXR_FNCLASS_NORMAL   ((size_t)0x0001ULL)
#define  HXR_FNCLASS_WRAPPER  ((size_t)0x0002ULL) /// ditto
#define  HXR_FNCLASS_HOTPATH  ((size_t)0x0004ULL) /// ditto
#define  HXR_FNCLASS_GETTER   ((size_t)0x0008ULL) /// ditto
#define  HXR_FNCLASS_SETTER   ((size_t)0x0010ULL) /// ditto

/// This is a special function class used on HeXeR's internal functions that
/// are in codepaths that handle errors or message delivery.
///
/// For code outside of HeXeR, this might not be very useful.
///
/// Inside message delivery code, this is important for preventing
/// infinite loops and any other manner of message storm.
/// 
#define  HXR_FNCLASS_CANT_MESSAGE ((size_t)0x0020ULL)

/// This definition can be used to define custom function classes, should the
/// existing `HXR_FNCLASS_*` set be insufficient for your needs.
///
/// The process of defining new functions classes would look like this:
/// ===
/// #define MY_FNCLASS_RETAIL  (HXR_FNCLASS_UNUSED << 0)
/// #define MY_FNCLASS_CHUUNI  (HXR_FNCLASS_UNUSED << 1)
/// #define MY_FNCLASS_GUI     (HXR_FNCLASS_UNUSED << 2)
/// ===
///
/// Be aware that there are only so many flags available, and that this number
/// is limited by the number of bits in the `size_t` integer type. To ensure
/// your code does what you expect, it is strongly recommended that you have
/// a unittest or constraint check for overflow like so:
/// ===
/// #define MY_FNCLASS_LAST    (MY_FNCLASS_GUI)
/// void my_unittest(hxr_thread *t)
/// {
///     HXR_ASSERT( MY_FNCLASS_LAST == 0 )
/// }
/// ===
///
#define  HXR_FNCLASS_UNUSED   ((size_t)0x0040ULL)



#ifdef HXR_DOCUMENTATION_BUILD

/// The `HXR_ENTER_FUNCTION` macro can be placed at the top of function
/// definitions to allow HeXeR to do cross-platform compiler-agnostic
/// function call history reporting, call-count-profiling, and stack trace printing.
/// (HeXeR stack unwinding algorithms will still prefer to use native
/// mechanisms if they are available and the integration has been implemented.)
///
/// This macro is a variadic macro that can take either 1 or 2 arguments.
///
/// A greatly simplified call history printout looks like this:
/// ===
/// main.c, line  32:     main
/// foo.c,  line  12:        foo
/// foo.c,  line  57:          foo_inner_loop  (50 times)
/// bar.c,  line  15:        bar
/// baz.c,  line  37:          baz (20 times)
/// ui.c,   line 123:             ui_interact
/// baz.c,  line  37:          baz
/// ui.c,   line 123:             ui_interact (3 errors!)
/// baz.c,  line  37:          baz
/// ui.c,   line 123:             ui_interact
/// ui.c,   line 460:               ui_exit
/// main.c:               hxr_stop was called
/// ===
///
/// The call-count profiling information for the above would look like so:
/// ===
/// Function,    Count,  File,   Line Number
/// baz,            22,  baz.c,  37
/// ui_interact,    22,  ui.c,   123
/// bar,             1,  bar.c,  15
/// foo,             1,  foo.c,  12
/// main,            1,  main.c, 32
/// ui_exit,         1,  ui.c,   460
/// ===
///
/// Parameters:
///
/// t - The first parameter, `t`, is mandatory.
///     The `t` parameter must be the caller's `hxr_thread*` object.
///     In other words, pass it a pointer to a `hxr_thread`.
///
/// func_classification - The second parameter, `func_classification`, is optional.
///     If it is not passed, it will be defaulted to `HXR_FNCLASS_NORMAL`.
///     Typically it will only make sense to pass one flag from the `HXR_FNCLASS_*`
///     set of flags. However, some of these flags are orthogonal in nature,
///     so it might be occasionally useful to pass something like
///         `HXR_FNCLASS_WRAPPER | HXR_FNCLASS_HOTPATH`
///     which would indicate that the function is a wrapper function that also
///     exists on a hotpath.
///
///     Technically, any constant expression containing flags from the
///     `HXR_FNCLASS_*` set (or custom caller-defined derivatives created using
///     the `HXR_FNCLASS_UNUSED` constant) is valid, though most arithmetic
///     operations will not make sense in this context.
///
///     Anything passed to the `func_classification` parameter MUST be a preprocessor macro
///     definition that expands to a *constant expression*. Passing C variables or
///     macros expanding to things that contrain C variables *will* cause problems.
///     That's because these are expanded in a preprocessor #if statement to acheive
///     compile-time conditional compilation. C variables will not be interpreted by
///     the preprocessor correctly, and thus cannot be used
#	define HXR_ENTER_FUNCTION(t, func_classification) (0)
// TODO: Update documentation to reflect that having a constant expression is not
// *necessary*, but is still pretty important for optimization reasons.

#else

#	define HXR_ENTER_FUNCTION2(t, func_classification) \
		do { \
			if ( (func_classification) && (HXR_CALL_HISTORY_FNCLASSES) ) \
			{
				HXR_CHECK_AND_ENSURE_THREAD(t); \
				HXR(thread_frame_entrance_)(&t, (func_classification), __FILE__, __FUNCTION__, __LINE__); \
			} \
		} while(0)

#	define HXR_ENTER_FUNCTION1(t) HXR_ENTER_FUNCTION2(t, HXR_FNCLASS_NORMAL)

#	define HXR_ENTER_FUNCTION(...)  HXR_MACRO_OVERLOAD(HXR_ENTER_FUNCTION, __VA_ARGS__)

#endif

#define HXR_UPDATE_CALL_HISTORY(t) (hxr_update_call_history_(&t, __FILE__, __FUNCTION__, __LINE__))

#if 0
// TODO: conditionally compile hxr_thread_record_return_?
#define HXR_RETURN1(t) \
	do { \
		hxr_thread_frame_exit_(t, __FILE__, __FUNCTION__, __LINE__); \
		return; \
	} while(0)

#define HXR_RETURN2(t, val) \
	do { \
		hxr_thread_frame_exit_(t, __FILE__, __FUNCTION__, __LINE__); \
		return (val); \
	} while(0)

#define HXR_RETURN(...)  HXR_MACRO_OVERLOAD(HXR_RETURN, __VA_ARGS__)
#endif

#define HXR_CHECK_AND_ENSURE_THREAD(t) \
	do { \
		if ((t) == NULL ) { \
			HXR(debugf_)("%s : %s, line %d: Thread context " #t " is NULL, which should never happen. " \
				"This is a bug. This function's caller must provide a non-NULL instance of this, " \
				"such as one provided by hxr_get_current_thread(hxr_process *proc).", \
					__FILE__, __FUNCTION__, __LINE__); \
			(t) = HXR(create_temporary_thread_)(); \
			HXR(constraint_violated_)(t); \
		} \
	} while (0)

#define HXR_NEVER_NULL(t, var) \
	do { \
		HXR_CHECK_AND_ENSURE_THREAD(t); \
		if ((var) == NULL) \
			HXR(debugf_)("%s : %s, line %d: Value " #var " is NULL, which violates a NEVER_NULL constraint.", __FILE__, __FUNCTION__, __LINE__); \
		HXR(constraint_violated_)(t); \
	} while(0)

#ifdef HXR_TESTING_MODE
#	define HXR_IF_NULL_ABORT(t, var) \
		do { \
			HXR_CHECK_AND_ENSURE_THREAD(t); \
			if ((var) == NULL) \
				HXR(debugf_)("%s : %s, line %d: Value " #var " is NULL, which violates an IF_NULL_ABORT constraint.", __FILE__, __FUNCTION__, __LINE__); \
			HXR(constraint_violated_)(t); \
		} while(0)
#else
#	define HXR_IF_NULL_ABORT(t, var) \
		do { \
			HXR_CHECK_AND_ENSURE_THREAD(t); \
			HXR(constraint_violated_)(t); \
		} while(0)
#endif

// We diverge a bit from the usual "do { ... } while(0)" pattern here, because
// these blocks will contain the caller's (mostly) arbitrary code, and we
// don't want to change the way the `break` and `continue` statements work.
// Hence, we need to use the "if(1) { ... } else (void)0" pattern to ensure
// that `break` and `continue` can escape our block-ends.
//
// At that point we could just wrap everything in a single "if(1) {...} else (void)0"
// block and be done with it. There would be no need for the `HXR_BEGIN_INNER_BLOCK_`
// macro. Yet it exists, and for a reason.
//
// The `HXR_*_INNER_BLOCK` macros capture any `break` or `continue` statements
// that the caller used. This is initially contrary to the original goal of
// leaving `break` and `continue` behavior unaltered. However, if we want
// to really really ensure that the end-of-block code gets executed, then
// we have to risk it. Otherwise, the enclosing code could skip out on the
// end-of-block finalization code by jumping over it using `break` or `continue`.
// Once we're done executing our end-of-block code, we repeat any original
// `break` or `continue` statements that the enclosed code attempted. This does
// mean that we have to differentiate between the two, otherwise we wouldn't
// know which one to do at the end of it all.
//
// Identifying whether the enclosing code exited the block using `break`,
// `continue`, or normally, is what the for-loop in `HXR_*_INNER_BLOCK` is
// used for.
//
// We'll explain the detection of `continue` first, because `break` is actually
// easier to identify and can be detected regardless of statement used.
//
// So to detect `continue`, we will need to use one of C's looping statements.
// Things like `if` and `switch-case` won't do, because they do not use the
// `continue` statement to control flow of execution. The looping constructs
// all evaluate an expression before-or-after each iteration, which is an
// important requirement: we will use the iteration/condition evaluation to
// tell us when enclosing code has executed a `continue` statement.
//
// The looping constructs of 'do-whille', 'while', and 'for' were all tested
// in '../design/detecting-break-and-continue.c'. As it turns out, they are
// all suitable for this. 'do-while' is chosen because it seems to require
// persisting slightly less state in a `hxr_block_` object (which is hidden
// within `hxr_thread`, because declaring variables in these macros would make
// them not nestable or poorly nestable).
// 
#define HXR_BEGIN_OUTER_BLOCK_(t) \
	if(1) {

#define HXR_BEGIN_INNER_BLOCK_(t) \
		do { \
			HXR(enter_block_)(t, HXR_STACK_FRAME_PTR_HERE_, __LINE__);

#define HXR_END_INNER_BLOCK_(t) \
			HXR(normal_block_exit_)(t, HXR_STACK_FRAME_PTR_HERE_, __LINE__);); \
			break; \
		} \
		while( HXR(continue_was_used_(t, HXR_STACK_FRAME_PTR_HERE_, __LINE__);)) ) \
		HXR(nesting_block_exit_);

// Note that the HXR(relay_...) functions all implicitly call HXR(exit_block_).
// (HXR_RETURN and HXR_GOTO do as well.)
#define HXR_END_OUTER_BLOCK_(t) \
		\
		if ( HXR(relay_continue_stmt_)(t, HXR_STACK_FRAME_PTR_HERE_, __LINE__);) ) \
			continue; \
		else \
		if ( HXR(relay_break_stmt_)(t, HXR_STACK_FRAME_PTR_HERE_, __LINE__);) ) \
			break; \
		else \
			HXR(exit_block_)(t, HXR_STACK_FRAME_PTR_HERE_, __LINE__); \
	} \
	else (void)0

// For sake of API simplicity (fewer function arguments) and memory conservation,
// flags and types share the same integer.

/// The HEXER flag indicates that the message originated from within HeXeR's
/// internal mechanisms.
///
/// Messages with this flag are generated whenever HeXeR is able to catch
/// usage mistakes (ex: raw return statement from within HXR_BEGIN-HXR_END,
/// HXR_BEGIN-HXR_END with no message building functions called, and anything
/// else of that nature).
#define HXR_MSG_FLAG_HEXER    ((uint32_t)0x80000000)
#define HXR_MSG_FLAG_PROTOCOL ((uint32_t)0x40000000) /** For not-human-readable messages, ex: XML, JSON, binary. */

/// `HXR_MSG_FLAG_HTML` indicates that the message contains HTML-formatted text
/// exclusively.
///
/// Please note that HeXeR does not do HTML text display and does
/// not plan on implementing or depending on an HTML/CSS parser.
///
/// This flag is intended to be used by programs or libraries that want to
/// originate HTML text and know that there is an HTML-capable message
/// formatter at the message's destination (such as if they provided one).
#define HXR_MSG_FLAG_HTML     ((uint32_t)0x20000000)

#define HXR_MSG_FLAG_MASK     ((uint32_t)0xFFF00000)

#define HXR_MSG_TYPE_INFO     ((uint32_t)0x00000001)
#define HXR_MSG_TYPE_WARNING  ((uint32_t)0x00000002)
#define HXR_MSG_TYPE_ERROR    ((uint32_t)0x00000003)

#define HXR_MSG_TYPE_MASK     ((uint32_t)0x0000003F)

#define HXR_MSG_FLAG_EXTRACT(packed_i32) ((packed_i32) & HXR_MSG_FLAG_MASK)
#define HXR_MSG_TYPE_EXTRACT(packed_i32) ((packed_i32) & HXR_MSG_TYPE_MASK)

#define HXR_MSG_SET_FLAGS(taf, new_flags) ((taf) = (((taf) & ~HXR_MSG_FLAG_MASK) | (new_flags)))
#define HXR_MSG_SET_TYPE(taf,  new_type)  ((taf) = (((taf) & ~HXR_MSG_TYPE_MASK) | (new_type)))
#define HXR_MSG_SET_TYPE_AND_FLAGS(taf, new_type, new_flags) \
		((taf) = (((taf) & ~(HXR_MSG_FLAG_MASK | HXR_MSG_TYPE_MASK)) | (new_type) | (new_flags)))


// =============================
// Block-nest methodology
// =============================

// These flags are packed into block events to encode what happens at any given
// point in the program. They are compressed somewhat so that they can share
// a 32-bit integer with the __LINE__ information for that event (because the
// ordering of a sequence of __LINE__ values can be used to verify proper nesting).
//
//
#define HXR_BLK_EXIT_NONE_     (((uint32_t)0x00) << 24)
#define HXR_BLK_EXIT_END_      (((uint32_t)0x10) << 24)
#define HXR_BLK_EXIT_CONTINUE_ (((uint32_t)0x20) << 24)
#define HXR_BLK_EXIT_GOTO_     (((uint32_t)0x30) << 24)
#define HXR_BLK_EXIT_RETURN_   (((uint32_t)0x40) << 24)
#define HXR_BLK_EXIT_MASK_     (((uint32_t)0xF0) << 24)

#define HXR_BLK_EXTRACT_EXIT_(packed_i32) ((packed_i32) & HXR_BLK_EXIT_MASK_)
#define HXR_BLK_EXTRACT_LINE_(packed_i32) ((packed_i32) & ~HXR_BLK_EXIT_MASK_)

// Admittedly, this could make things get weird if someone has a C source file
// that is more than approximately 256 million lines long (before preprocessing).
// We could try to mask out the upper 4 bits of the `line` parameter, but that
// would waste a precious cycle or two, and it wouldn't help us much anyways
// because it would cause line numbers to wrap and break any nesting on that
// modulus boundary. Whatever are we to do?!?
// I'm... not going to lose an sleep over it.
#define HXR_BLK_PACK_LINE_EXIT_(line, exit_type) ((line) | (exit_type))

#define HXR_BLK_SET_LINE(lae, new_line) ((lae) = (((lae) & HXR_BLK_EXIT_MASK_)  | (new_line)))
#define HXR_BLK_SET_EXIT(lae, new_exit) ((lae) = (((lae) & ~HXR_BLK_EXIT_MASK_) | (new_exit)))

// The answer to a certain stack overflow question (https://stackoverflow.com/a/32382763)
// cites a section of the C99 standard that guarantees something very useful
// for us regarding the __func__ identifier:
//
// > From the C99 standard:
// >
// > 6.4.2.2 Predefined identifiers
// > 
// > Semantics:
// > 
// > 1. The identifier __func__ shall be implicitly declared by the translator
// >    as if, immediately following the opening brace of each function
// >    definition, the declaration
// > 
// >        static const char __func__[] = "function-name";
// > 
// >    appeared, where function-name is the name of the lexically-enclosing function.
// > 
// > 2. This name is encoded as if the implicit declaration had been written
// >    in the source character set and then translated into the execution
// >    character set as indicated in translation phase 5.
// >
// > 3. EXAMPLE Consider the code fragment
// > 
// >        #include <stdio.h>
// >         void myfunc(void) {
// >           printf("%s\n", __func__);
// >           /* ... */
// >        }
// > 
// >    Each time the function is called, it will print to the standard output stream:
// > 
// >        myfunc
// > 
// > You'll also see __FUNCTION__ in many C compilers, but it was never standard.
//
// Of particular important is point #1: every function will have a single
// instance of that string. If __func__ appears in multiple locations, each
// one will be a pointer to the SAME string. There is no chance that two
// instances of __func__ (from the same function) will point to different strings.
// (Unless the C compiler is non-compliant, but we must handle such things on
// on a case-by-case basis and might be able to exploit non-standard extensions
// in such situations.) This means that we can use pointer comparison to know
// if we're still in the same function or not! This eliminates any need to do
// string-scanning, hashing, etc etc.  It's actually quite wonderful.
//
typedef struct S_HXR_BLOCK_
{
	// `stack_frame` uniquely identifies the function call in which
	// this event happened.
	//
	// It is derived from the __func__ identifier, as placed in a macro
	// expansion (the `HXR_STACK_FRAME_PTR_HERE_` macro, specifically).
	//
	// As above, this is (indirectly) guaranteed by the C99 to be the same
	// pointer whenever it appears in the same function.
	//
	// We still have to consider if there are any situations where __func__
	// could be an identical pointer when we want to instead find an inequality.
	//
	// There are two situations I can think of, as of this writing (2020-10-07) :
	//
	// - Two static functions have the same name but live in different files.
	//   Initially they might point to the different strings, but the C compiler
	//   or linker optimizes these duplicates into one string.
	//
	// - A function recurses into itself. __func__ will point to the same
	//   static string, but we want our comparison to have a negatory result
	//   because we are in different stack frames. In other words: we're in
	//   the same function, but not the same function *call*, and it's the
	//   latter that we're trying to distinguish.
	//
	// To handle this, we will take the address of the implicit __func__
	// variable instead of the string that it points to. This address will
	// uniquely identify the stack FRAME and not the FUNCTION.
	//
	// A bit of caveat: NEVER dereference the &__func__ pointer after it's
	// been stored somewhere over the span of some unknown calls/returns.
	// It might point to something completely different than what it did
	// before. To prevent this from happening accidentally, we will have already
	// casted this value into a uintptr_t, which removes pointer semantics.
	// (and it really is being used as more of an ID anyways).
	//
	uintptr_t stack_frame_coords;

	// This stores `HXR_MSG_FLAG_*` and `HXR_MSG_TYPE_*` values.
	//
	// Either of those definition sets may grow or change, so there's no
	// definitive bit-allocation. However, the flags will occupy the
	// high bits and grow downward, while the type definitions will occupy
	// the low bits and grow upwards.
	//
	// The `HXR_MSG_FLAG_MASK` and `HXR_MSG_TYPE_MASK` macros define the
	// bit-allocation used in this version of HeXeR. These do not occupy
	// all bits, and the remaining bits may, in the future, be used to
	// provide a mechanism for defining custom types and flags.
	//
	// To actually extract the FLAG And TYPE values, use the
	// `HXR_MSG_FLAG_EXTRACT(p)` and `HXR_MSG_TYPE_EXTRACT(p)` macros.
	//
	// Message TYPEs and FLAGs can be ORed together to populate this
	// value. Use the `HXR_MSG_SET_*` macros to assign these bitfields
	// new values without affecting the other contents of this packed
	// integer.
	uint32_t   type_and_flags;

	// This is actually a 28-bit integer (for holding the __LINE__ number)
	// along with an 4-bit integer for storing exit-type information
	// (which are given by the `HXR_BLK_EXIT_*` definitions).
	//
	// The two different pieces of information can be obtained with the
	// `HXR_BLKEV_EXTRACT_TYPE_(stuff)` and `HXR_BLKEV_EXTRACT_LINE_(stuff)`
	// macros.
	//
	// The two different pieces of information can be packed into this field
	// using the `HXR_BLKEV_PACK_STUFF_(line, event)` macro.
	uint32_t   line_and_exit;

} hxr_block_;



static inline void HXR(enter_block_)(
	hxr_thread  *t,
	const char  **stack_frame,
	uint32_t    type_and_flags,
	uint32_t    line
	)
{
	hxr_block_ block;
	block.stack_frame_coords = (uintptr_t)stack_frame;
	block.type_and_flags = type_and_flags;
	block.line_and_exit  = line;
	HXR(add_block_)(t, block);
}

static inline int HXR(continue_was_used_)(
	hxr_thread  *t,
	const char  **stack_frame,
	uint32_t    line
	)
{
	// TODO: detect mismatch (if ( block->stack_frame_coords != (uintptr_t)stack_frame )) and check line number too
	// It may be necessary to wind back more than once... at least if there were unmatched BEGINs since this block's BEGIN.
	// Like, ensure that all of these are handled:
	// * BEGIN - BEGIN - END - [END] - msg_dispatch
	// * BEGIN - BEGIN - [END] - END
	// * BEGIN - BEGIN - [END] - [END] - msg_dispatch
	// * BEGIN - call - BEGIN - BEGIN - [END] - [END] return - END
	//
	// I was tempted to do error detection in HXR(nesting_block_exit_), but
	// it might be necessary to do it here AS WELL. That's because it's important
	// for applying the HXR_BLK_EXIT_* attribute to the correct block.
	//
	hxr_thread_impl_ *timpl = HXR(thread_get_impl_)(t);
	size_t i = timpl->open_block_count - 1;
	hxr_block_ *block = &timpl->open_blocks[i];
	HXR_BLK_SET_LINE(block->line_and_exit, HXR_BLK_EXIT_CONTINUE_);

	// Return false to make the enclosing loop statement terminate.
	return 0;
}

static inline void HXR(normal_block_exit_)(
	hxr_thread  *t,
	const char  **stack_frame,
	uint32_t    line
	)
{
	hxr_thread_impl_ *timpl = HXR(thread_get_impl_)(t);
	size_t i = timpl->open_block_count - 1;
	hxr_block_ *block = &timpl->open_blocks[i];
	HXR_BLK_SET_LINE(block->line_and_exit, HXR_BLK_EXIT_CONTINUE_);
}

static inline void HXR(nesting_block_exit_)(hxr_thread *t)
{
	HXR(reify_blocks_)(t);
}




// =============================
// Block-event methodology
// =============================


// These flags are packed into block events to encode what happens at any given
// point in the program. They are compressed somewhat so that they can share
// a 32-bit integer with the __LINE__ information for that event (because the
// ordering of a sequence of __LINE__ values can be used to verify proper nesting).
//
//
#define HXR_BLKEV_ENTER_    (((uint32_t)0x10) << 24)
#define HXR_BLKEV_END_      (((uint32_t)0x20) << 24)
#define HXR_BLKEV_CONTINUE_ (((uint32_t)0x30) << 24)
#define HXR_BLKEV_GOTO_     (((uint32_t)0x40) << 24)
#define HXR_BLKEV_RETURN_   (((uint32_t)0x50) << 24)
#define HXR_BLKEV_MASK_     (((uint32_t)0xF0) << 24)

#define HXR_BLKEV_EXTRACT_TYPE_(stuff) (stuff & HXR_BLKEV_MASK_)
#define HXR_BLKEV_EXTRACT_LINE_(stuff) (stuff & ~HXR_BLKEV_MASK_)

// Admittedly, this could make things get weird if someone has a C source file
// that is more than approximately 256 million lines long (before preprocessing).
// We could try to mask out the upper 4 bits of the `line` parameter, but that
// would waste a precious cycle or two, and it wouldn't help us much anyways
// because it would cause line numbers to wrap and break any nesting on that
// modulus boundary. Whatever are we to do?!?
// I'm... not going to lose an sleep over it.
#define HXR_BLKEV_PACK_STUFF_(line, event) (event | line)

// We can do better mistake-detection if we have a full list of begin-end
// events before we do all of our push-and-pop nesting logic, because the
// nesting logic will lose some information (in an ideal world, there is no
// information to lose, because everything is perfectly nested).
//
// The answer to a certain stack overflow question (https://stackoverflow.com/a/32382763)
// cites a section of the C99 standard that guarantees something very useful
// for us regarding the __func__ identifier:
//
// > From the C99 standard:
// >
// > 6.4.2.2 Predefined identifiers
// > 
// > Semantics:
// > 
// > 1. The identifier __func__ shall be implicitly declared by the translator
// >    as if, immediately following the opening brace of each function
// >    definition, the declaration
// > 
// >        static const char __func__[] = "function-name";
// > 
// >    appeared, where function-name is the name of the lexically-enclosing function.
// > 
// > 2. This name is encoded as if the implicit declaration had been written
// >    in the source character set and then translated into the execution
// >    character set as indicated in translation phase 5.
// >
// > 3. EXAMPLE Consider the code fragment
// > 
// >        #include <stdio.h>
// >         void myfunc(void) {
// >           printf("%s\n", __func__);
// >           /* ... */
// >        }
// > 
// >    Each time the function is called, it will print to the standard output stream:
// > 
// >        myfunc
// > 
// > You'll also see __FUNCTION__ in many C compilers, but it was never standard.
//
// Of particular important is point #1: every function will have a single
// instance of that string. If __func__ appears in multiple locations, each
// one will be a pointer to the SAME string. There is no chance that two
// instances of __func__ (from the same function) will point to different strings.
// (Unless the C compiler is non-compliant, but we must handle such things on
// on a case-by-case basis and might be able to exploit non-standard extensions
// in such situations.) This means that we can use pointer comparison to know
// if we're still in the same function or not! This eliminates any need to do
// string-scanning, hashing, etc etc.  It's actually quite wonderful.
//
typedef struct S_HXR_BLOCK_EVENT_
{
	// `stack_frame` uniquely identifies the function call in which
	// this event happened.
	//
	// It is derived from the __func__ identifier, as placed in a macro
	// expansion (the `HXR_STACK_FRAME_PTR_HERE_` macro, specifically).
	//
	// As above, this is (indirectly) guaranteed by the C99 to be the same
	// pointer whenever it appears in the same function.
	//
	// We still have to consider if there are any situations where __func__
	// could be an identical pointer when we want to instead find an inequality.
	//
	// There are two situations I can think of, as of this writing (2020-10-07) :
	//
	// - Two static functions have the same name but live in different files.
	//   Initially they might point to the different strings, but the C compiler
	//   or linker optimizes these duplicates into one string.
	//
	// - A function recurses into itself. __func__ will point to the same
	//   static string, but we want our comparison to have a negatory result
	//   because we are in different stack frames. In other words: we're in
	//   the same function, but not the same function *call*, and it's the
	//   latter that we're trying to distinguish.
	//
	// To handle this, we will take the address of the implicit __func__
	// variable instead of the string that it points to. This address will
	// uniquely identify the stack FRAME and not the FUNCTION.
	//
	// A bit of caveat: NEVER dereference the &__func__ pointer after it's
	// been stored somewhere over the span of some unknown calls/returns.
	// It might point to something completely different than what it did
	// before. To prevent this from happening accidentally, we will have already
	// casted this value into a uintptr_t, which removes pointer semantics.
	// (and it really is being used as more of an ID anyways).
	//
	uintptr_t stack_frame_coords;

	// Sorry for the vague name.
	// This is actually a 28-bit integer (for holding the __LINE__ number)
	// along with an 4-bit integer for storing event-type information.
	//
	// The two different pieces of information can be obtained with the
	// `HXR_BLKEV_EXTRACT_TYPE_(stuff)` and `HXR_BLKEV_EXTRACT_LINE_(stuff)`
	// macros.
	//
	// The two different pieces of information can be packed into this field
	// using the `HXR_BLKEV_PACK_STUFF_(line, event)` macro.
	uint32_t   stuff;

} hxr_block_event_;


static inline void HXR(enter_block_)(
	hxr_thread  *t,
	const char  **stack_frame,
	uint32_t    line
	)
{
	hxr_block_event_ blkev;
	blkev.stack_frame_coords = (uintptr_t)stack_frame;
	blkev.stuff = HXR_BLKEV_PACK_STUFF_(line, HXR_BLKEV_ENTER_);
	HXR(add_block_event_)(t, blkev);
}

static inline int HXR(continue_was_used_)(
	hxr_thread  *t,
	const char  **stack_frame,
	uint32_t    line
	)
{
	hxr_block_event_ blkev;
	blkev.stack_frame_coords = (uintptr_t)stack_frame;
	blkev.stuff = HXR_BLKEV_PACK_STUFF_(line, HXR_BLKEV_EXIT_ | HXR_BLKEV_CONTINUE_);
	HXR(add_block_event_)(t, blkev);
	return 0; // Return false to make the enclosing loop statement terminate.
}

static inline void HXR(normal_block_exit_)(
	hxr_thread  *t,
	const char  **stack_frame,
	uint32_t    line
	)
{
	hxr_block_event_ blkev;
	blkev.stack_frame_coords = (uintptr_t)stack_frame;
	blkev.stuff = HXR_BLKEV_PACK_STUFF_(line, HXR_BLKEV_EXIT_ | HXR_BLKEV_END_);
	HXR(add_block_event_)(t, blkev);
}

static inline void HXR(nesting_block_exit_)(hxr_thread *t)
{
	HXR(reify_blocks_)(t);
}

void HXR(reify_blocks_)(hxr_thread *t)
{
	hxr_thread_impl_ *timpl = &((hxr_thread_wrapper_*)t)->impl;

	size_t  events_len = timpl->block_event_count;
	ssize_t nesting_depth = 0;

#define HXR_BLKEV_IS_EXIT_EVENT_(blkev_) \
		(HXR_BLKEV_EXIT_ & HXR_BLKEV_EXTRACT_TYPE_(blkev_.stuff))

#define HXR_BLKEV_IS_ENTER_EVENT_(blkev_) \
		(!HXR_BLKEV_IS_EXIT_EVENT_(blkev_))

	// Start at the most recent event.
	hxr_block_event_  *earliest = timpl->block_events;
	hxr_block_event_  *cursor = &timpl->block_events[events_len - 1];

	// Scan past any open blocks. We can't do anything with them on this run.
	while ( cursor >= earliest && !HXR_BLKEV_IS_EXIT_EVENT_(*cursor) )
		cursor--;

	// If ( cursor < earliest ), then there are no block-exit events. Which means
	// we have nothing to do.
	// If ( cursor == earliest ), then we have one block-exit event, and it
	// is not preceded by any block-enter events. That means we should flag
	// an internal error, but the later loop already does this, so we'll let
	// it slide at this particular IF-statement.
	if ( cursor < earliest )
		return;

	// We mark the most recent event that we are going to be processing,
	// which has now been proven to be some kind of exit event.
	// This will represent the HXR_END command for the *outermost* block.
	// (Unless it has no matching HXR_BEGIN, in which case we'll squawk and
	// chose an outermost block that IS complete.)
	hxr_block_event_  *most_recent_ptr = cursor;
	hxr_block_event_  most_recent = *most_recent_ptr;

	cursor--;
	nesting_depth++;

	while ( cursor >= earliest )
	{
		hxr_block_event_ blkev = *cursor;
		blkev.stack_frame_coords == most_recent.stack_frame_coords
	}

#undef HXR_BLKEV_IS_ENTER_EVENT_
#undef HXR_BLKEV_IS_EXIT_EVENT_
}

#define HXR_DEBUGF_LINE_BUFFER_SZ (128)
static const char *hxr_debugf_line_buffer[HXR_DEBUGF_LINE_BUFFER_SZ];
static size_t hxr_debugf_count;
static int  (hxr_debugf_save*)(const char *str, ...);
static hxr_allocator *hxr_test_allocator;


// We're going to need this so that we can detect critical internal errors
// (which would normally just be dumped to the user's stdout/tty/whatever)
// and fail tests if they happen.
//
// This probably isn't necessary for all unittests, but it is pretty
// important whenever we are testing any core functionality that
// has failure modes which call debugf.
//
static int hxr_test_vdebugf(const char *fmtstr, va_list vargs_orig)
{	
#define HXR_TEST_STRINGIZE_(str) #str
#define HXR_TEST_STRINGIZE(str)  HXR_TEST_STRINGIZE_(str)

	int (hxr_vsnprintf*)(char *, size_t, const char *, va_list);
	hxr_vsnprintf = &HXR_VSNPRINTF_DEFAULT;
	va_list vargs;

	// Figure out how large the formatted string will be.
	va_copy(vargs, vargs_orig);
	int sz = hxr_vsnprintf(NULL, 0, fmtstr, vargs);
	if ( sz <= 0 ) {
		hxr_debugf_save(
			HXR_TEST_STRINGIZE(HXR_DEFAULT_VSNPRINTF)
			" returned %d, but for testing to be done, it needs to "
			"instead successfully format the string and then return the size "
			"needed to store the formatted string.\n"
			"The format string was \"%s\"",
			sz, fmtstr);
		return -1;
	}

	// Allocate and format the string.
	char *buf = hxr_test_allocator->allocate(sz+1);
	if ( buf == NULL ) {
		hxr_debugf_save(
			HXR_TEST_STRINGIZE(HXR_DEFAULT_ALLOCATOR)
			" returned NULL. This indicates an allocation failure or broken "
			"allocator. A working allocator and *some* (even a little) "
			"available memory is necessary to run HeXeR's unittests.");
		return -1;
	}

	va_copy(vargs, vargs_orig);
	int rc = hxr_vsnprintf(NULL, 0, fmtstr, vargs);
	if ( rc <= 0 ) {
		hxr_debugf_save(
			HXR_TEST_STRINGIZE(HXR_DEFAULT_VSNPRINTF)
			" returned %d, but for testing to be done, it needs to "
			"instead successfully format the string.\n"
			"It had calculated the size of the formatted string earlier, "
			"and that came out to be %d bytes (null terminator not included).\n"
			"The format string was \"%s\"",
			rc, sz, fmtstr);
		return -1;
	}

	return rc;

#undef HXR_TEST_STRINGIZE
#undef HXR_TEST_STRINGIZE_
}

static int HXR(test_debugf)(const char *fmtstr, ...)
{
	va_list vargs;
	va_start(vargs, fmtstr);
	int rc = hxr_test_vdebugf(fmtstr, vargs);
	va_end(vargs);
	return rc;
}

static void HXR(begin_test_debugf)()
{
	hxr_debugf_count = 0;
	hxr_debugf_save = HXR(debugf);
	HXR(debugf_) = &hxr_test_debugf;
}

static void HXR(end_test_debugf)()
{
	HXR(debugf_) = hxr_debugf_save;
}

void HXR(block_test_bad_return_)(hxr_thread *t)
{
	HXR_BEGIN_INFO(t);
		hxr_message_id(t, "unmatched");
		return;
	HXR_END(t);
}

// Very simple strlen implementation used for assertion expressions and such.
// By writing this, we avoid depending on libc or adding another config macro.
// Considering the strings this will encounter (small test cases),
// no optimization should be necessary.
static size_t HXR(strlen_)(const char *str)
{
	size_t i = 0;
	while ( str[i] != '\0' ) i++;
	return i;
}

// Very simple strcmp implementation used for assertion expressions and such.
// By writing this, we avoid depending on libc or adding another config macro.
// Considering the strings this will encounter (small test cases),
// no optimization should be necessary.
static int HXR(strcmp_)(const char *lhs, const char *rhs)
{
	// NULL strings shall behave as empty strings.
	const char *empty_str = "";
	if ( lhs == NULL )
		lhs = empty_str;
	else
	if ( rhs == NULL )
		rhs = empty_str;

	// Scan strings for differences.
	size_t i = 0;
	while (1)
	{
		char lchar = lhs[i];
		char rchar = rhs[i];
		i++;

		if ( lchar < rchar )
			return -1;
		else
		if ( lchar > rchar )
			return 1;

		// lchar == rchar.
		if ( lchar == '\0' )
			return 0;
	}

	// This should be unreachable.
}

#if defined(HXR_EXTRACT_UNITTESTS) && (0 != HXR_EXTRACT_UNITTESTS)
void HXR(strcmp_unittest)(hxr_thread *t)
{
	HXR_ASSERT( 0, ==, HXR(strcmp_)(NULL, NULL) );
	HXR_ASSERT( 0, ==, HXR(strcmp_)(NULL,   "") );
	HXR_ASSERT( 0, ==, HXR(strcmp_)(  "", NULL) );
	HXR_ASSERT( 0, ==, HXR(strcmp_)(  "",   "") );
	HXR_ASSERT(-1, ==, HXR(strcmp_)(NULL,  "a") );
	HXR_ASSERT( 1, ==, HXR(strcmp_)( "a", NULL) );
	HXR_ASSERT(-1, ==, HXR(strcmp_)(  "",  "a") );
	HXR_ASSERT( 1, ==, HXR(strcmp_)( "a",   "") );
	HXR_ASSERT( 0, ==, HXR(strcmp_)( "a",  "a") );
	HXR_ASSERT(-1, ==, HXR(strcmp_)( "a",  "b") );
	HXR_ASSERT( 1, ==, HXR(strcmp_)( "b",  "a") );
	HXR_ASSERT(-1, ==, HXR(strcmp_)( "A",  "a") );
	HXR_ASSERT( 1, ==, HXR(strcmp_)( "a",  "A") );
	HXR_ASSERT(-1, ==, HXR(strcmp_)( "B",  "a") );
	HXR_ASSERT( 1, ==, HXR(strcmp_)( "a",  "B") );
	HXR_ASSERT( 0, ==, HXR(strcmp_)("aa", "aa") );
	HXR_ASSERT(-1, ==, HXR(strcmp_)( "a", "aa") );
	HXR_ASSERT( 1, ==, HXR(strcmp_)("aa",  "a") );
	HXR_ASSERT(-1, ==, HXR(strcmp_)("aa",  "b") );
	HXR_ASSERT( 1, ==, HXR(strcmp_)( "b", "aa") );
}
#endif

#define HXR_IF_STR3(lhs, op, rhs) \
	( (1 op 1) ? ( \
		( (1 op 0) ? (\
			HXR_ASSERT_STR_ELSE_GTE(lhs, op, rhs) \
		) : ( \
		( (0 op 1) ? ( \
			HXR_ASSERT_STR_ELSE_LTE(lhs, op, rhs) \
		) : ( \
			HXR_ASSERT_STR_ELSE_EQ(lhs, op, rhs) \
		)))) \
	) : ( \
		( (1 op 0) ? ( \
			( (0 op 1) ? ( \
				HXR_ASSERT_STR_ELSE_NE(lhs, op, rhs) \
			) : ( \
				HXR_ASSERT_STR_ELSE_GT(lhs, op, rhs) \
			)) \
		) : ( \
			HXR_ASSERT_STR_ELSE_LT(lhs, op, rhs) \
		)) \
	))

#define HXR_ASSERT_STR_ELSE3(lhs, op, rhs) \
	if ( HXR_IF_STR3(lhs, op, rhs) /* || TODO: register constraint violation? */ ) \
		(void)0; \
	else
		/* Caller's code goes here. */

// Use an inline function to prevent macros from evaluating an expression more
// than once.
static inline int HXR(if_str_)(const char *str) {
	return ( (str) != NULL && HXR(strlen_)(str) );
}

#define HXR_ASSERT_STR3(lhs, op, rhs) \
	( HXR_IF_STR3(lhs, op, rhs) /* || TODO: register constraint violation? */ )

#define HXR_IF_STR1(expr) \
	( HXR(if_str_)(expr) )

#define HXR_ASSERT_STR_ELSE1(expr) \
	if ( HXR_IF_STR1(expr) /* || TODO: register constraint violation? */ ) \
		(void)0; /* Assertion succeeded. Do nothing. */ \
	else
		/* Caller's code goes here. */

#define HXR_ASSERT_STR_ELSE(...) HXR_MACRO_OVERLOAD(HXR_ASSERT_STR_ELSE, __VA_ARGS__)

#define HXR_ASSERT_STR(...) HXR_MACRO_OVERLOAD(HXR_ASSERT_STR, __VA_ARGS__)

void HXR(block_unittest)(hxr_thread *t_but_real)
{
	hxr_thread  t0;
	hxr_thread  *t = &t0;
	hxr_message msg;
	//

	// ................................ //
	hxr_thread_init_(t);

	HXR(begin_test_debugf)();
	HXR_BEGIN_INFO(t);
		hxr_message_id(t, "single_msg");
	HXR_END(t);
	HXR(end_test_debugf)();

	HXR_ASSERT( hxr_debugf_count, ==, 0 );
	do {
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 1 )       break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )         break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "single_msg" ) break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 0 )       break;
	} while (0);

	hxr_thread_free_(t);

	// ................................ //
	hxr_thread_init_(t);

	HXR(begin_test_debugf)();
	HXR_BEGIN_INFO(t);
		HXR_BEGIN_INFO(t);
			hxr_message_id(t, "deep");
		HXR_END(t);
		hxr_message_id(t, "shallow");
	HXR_END(t);
	HXR(end_test_debugf)();

	HXR_ASSERT( hxr_debugf_count, ==, 0 );
	do {
		// The ordering should always be based on when HXR_END(t) is reached.
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 2 )    break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )      break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "deep" )    break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 1 )    break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )      break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "shallow" ) break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 0 )    break;
	} while (0);

	hxr_thread_free_(t);

	// ................................ //
	hxr_thread_init_(t);

	HXR(begin_test_debugf)();
	HXR_BEGIN_INFO(t);
		hxr_message_id(t, "shallow");
		HXR_BEGIN_INFO(t);
			hxr_message_id(t, "deep");
		HXR_END(t);
	HXR_END(t);
	HXR(end_test_debugf)();

	// Again: The ordering should always be based on when HXR_END(t) is reached,
	// and NOT on how deep things are.
	HXR_ASSERT( hxr_debugf_count, ==, 0 );
	do {
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 2 )    break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )      break
		HXR_ASSERT_STR_ELSE( msg->id, ==, "deep" )    break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 1 )    break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )      break
		HXR_ASSERT_STR_ELSE( msg->id, ==, "shallow" ) break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 0 )    break;
	} while (0);

	hxr_thread_free_(t);

	// ................................ //
	hxr_thread_init_(t);

	HXR(begin_test_debugf)();
	HXR_BEGIN_INFO(t);
		hxr_message_id(t, "shallow");
		HXR_BEGIN_INFO(t);
			hxr_message_id(t, "deep01");
		HXR_END(t);
		HXR_BEGIN_INFO(t);
			hxr_message_id(t, "deep02");
		HXR_END(t);
	HXR_END(t);
	HXR(end_test_debugf)();

	HXR_ASSERT( hxr_debugf_count, ==, 0 );
	do {
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 3 )    break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )      break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "deep01" )  break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 2 )    break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )      break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "deep02" )  break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 1 )    break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )      break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "shallow" ) break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 0 )    break;
	} while (0);

	hxr_thread_free_(t);

	// ................................ //
	hxr_thread_init_(t);

	HXR(begin_test_debugf)();
	HXR_BEGIN_INFO(t);
		hxr_message_id(t, "shallow");
		HXR_BEGIN_INFO(t);
			HXR_BEGIN_INFO(t);
				hxr_message_id(t, "deep");
			HXR_END(t);
			hxr_message_id(t, "middle");
		HXR_END(t);
	HXR_END(t);
	HXR(end_test_debugf)();

	HXR_ASSERT( hxr_debugf_count, ==, 0 );
	do {
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 3 )     break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )       break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "deep" )     break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 2 )     break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )       break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "middle" )   break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 1 )     break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )       break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "shallow" )  break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 0 )     break;
	} while (0);

	hxr_thread_free_(t);

	// ................................ //
	hxr_thread_init_(t);

	HXR(begin_test_debugf)();
	HXR_BEGIN_INFO(t);
		HXR_BEGIN_INFO(t);
			HXR_BEGIN_INFO(t);
				hxr_message_id(t, "deep01");
			HXR_END(t);
			hxr_message_id(t, "middle01");
		HXR_END(t);
		hxr_message_id(t, "shallow01");
	HXR_END(t);

	HXR_BEGIN_INFO(t);
		HXR_BEGIN_INFO(t);
			HXR_BEGIN_INFO(t);
				hxr_message_id(t, "deep02");
			HXR_END(t);
			hxr_message_id(t, "middle02");
		HXR_END(t);
		hxr_message_id(t, "shallow02");
	HXR_END(t);
	HXR(end_test_debugf)();

	HXR_ASSERT( hxr_debugf_count, ==, 0 );
	do {
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 6 )      break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )        break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "deep01" )    break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 5 )      break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )        break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "middle01" )  break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 4 )      break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )        break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "shallow01" ) break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 3 )      break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )        break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "deep02" )    break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 2 )      break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )        break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "middle02" )  break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 1 )      break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )        break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "shallow02" ) break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 0 )      break;
	} while (0);

	hxr_thread_free_(t);

	// ................................ //
	hxr_thread_init_(t);

	HXR(begin_test_debugf)();
	HXR_BEGIN_INFO(t);
		hxr_message_id(t, "shallow01");
	HXR_END(t);
	HXR_BEGIN_INFO(t);
		HXR_BEGIN_INFO(t);
			HXR_BEGIN_INFO(t);
				hxr_message_id(t, "deep01");
			HXR_END(t);
			HXR_BEGIN_INFO(t);
				hxr_message_id(t, "deep02");
			HXR_END(t);
			hxr_message_id(t, "middle01");
		HXR_END(t);
		HXR_BEGIN_INFO(t);
			hxr_message_id(t, "middle02");
		HXR_END(t);
		HXR_BEGIN_INFO(t);
			HXR_BEGIN_INFO(t);
				hxr_message_id(t, "deep03");
			HXR_END(t);
			hxr_message_id(t, "middle03");
		HXR_END(t);
		hxr_message_id(t, "shallow02");
	HXR_END(t);
	HXR_BEGIN_INFO(t);
		hxr_message_id(t, "shallow03");
	HXR_END(t);
	HXR(end_test_debugf)();

	HXR_ASSERT( hxr_debugf_count, ==, 0 );
	do {
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 9 )       break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )         break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "shallow01" )  break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 8 )       break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )         break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "deep01" )     break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 7 )       break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )         break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "deep02" )     break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 6 )       break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )         break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "middle01" )   break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 5 )       break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )         break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "middle02" )   break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 4 )       break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )         break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "deep03" )     break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 3 )       break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )         break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "middle03" )   break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 2 )       break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )         break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "shallow02" )  break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 1 )       break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )         break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "shallow03" )  break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 0 )       break;
	} while (0);

	hxr_thread_free_(t);

	// ................................ //
	hxr_thread_init_(t);
	
	HXR(begin_test_debugf)();
	HXR(block_test_bad_return_)(hxr_thread *t);
	HXR(end_test_debugf)();
	
	// It won't be able to send a message because the code generating an error
	// is inside the message sending mechanism. So we listen for debugf
	// calls instead.
	TODO: Maybe I should have it actually send messages, albeit by manually
	constructing them whenever there is an unmatched event like this.
	HXR_ASSERT( hxr_debugf_count, >=, 1 );
	HXR_ASSERT( hxr_msg_count(t), ==, 0 );

	hxr_thread_free_(t);

	// ................................ //
	hxr_thread_init_(t);
	
	HXR(begin_test_debugf)();
	HXR_BEGIN_INFO(t);
		HXR(block_test_bad_return_)(hxr_thread *t);
		hxr_message_id(t, "outer");
	HXR_END(t);
	HXR(end_test_debugf)();

	hxr_thread_free_(t);

	HXR_ASSERT( hxr_debugf_count, >=, 1 );
	do {
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 1 )       break;
		HXR_ASSERT_ELSE( hxr_msg_next(t, &msg) )         break;
		HXR_ASSERT_STR_ELSE( msg->id, ==, "outer" )      break;
		HXR_ASSERT_ELSE( hxr_msg_count(t), ==, 0 )       break;
	} while (0);

	// ................................ //
	
	TODO: expand these tests further once the mismatch reporting is figured out
}

#if defined(HXR_EXTRACT_UNITTESTS) && (0 != HXR_EXTRACT_UNITTESTS)
void HXR(init_unittests)(hxr_thread *t)
{
	hxr_test_allocator = HXR(thread_get_allocator)(t);
}
#endif

static inline void HXR(reify_blocks_as_needed_)(
	hxr_thread           *t,
	hxr_block_event_     blkev
	)
{
	// A positive result for this test indicates that we called HXR_ENTER_FUNCTION(...)
	// after HXR_BEGIN*(...) and before HXR_END*(...). That is, we are nesting
	// blocks, but on the call stack, and not within the same function.
	// As long as nesting is going on, we can't reify (or at least, we sacrifice
	// some error-detection capabilities if we do it in this state).
	if ( blkev->stack_depth < t->stack_depth )
		return;

	// This tests asks if we are in a different function.
	// If this is true, and we're not just /called by/ the enclosing blkev
	// (the test above), then we've proven that the BEGIN-END we're entering
	// has nothing to do with any of the prior ones. At that point, it makes
	// sense to process all of the previous blkev events, as this will
	// decrease memory usage, and possibly make the reifier's implemention
	// easier.
	TODO: strcmp?
	if ( blkev->begin_loc.func != loc.func
	||   blkev->begin_loc.file != loc.file )
		HXR(reify_blocks_)(t);
}

static inline void HXR(exit_block_)(
	hxr_thread           *t,
	hxr_block_event_     *blkev,
	hxr_source_location_ loc)
{
	if ( t->block_begin_count == (t->block_end_count + 1)
	&&   
}

void HXR(pull_messages_)(hxr_thread *t, hxr_source_location_ loc)
{
	
}

#define HXR_BEGIN_INFO(t) \
	HXR_BEGIN_OUTER_BLOCK_(t) \
		HXR(begin_)(t, HXR_MESSAGE_TYPE_INFO, HXR_SOURCE_LOCATION_HERE_); \
		HXR_BEGIN_INNER_BLOCK_(t)

#define HXR_BEGIN_WARNING(t) \
	HXR_BEGIN_OUTER_BLOCK_(t) \
		HXR(begin_)(t, HXR_MESSAGE_TYPE_WARNING, HXR_SOURCE_LOCATION_HERE_); \
		HXR_BEGIN_INNER_BLOCK_(t)

#define HXR_BEGIN_ERROR(t) \
	HXR_BEGIN_OUTER_BLOCK_(t) \
		HXR(begin_)(t, HXR_MESSAGE_TYPE_ERROR, HXR_SOURCE_LOCATION_HERE_); \
		HXR_BEGIN_INNER_BLOCK_(t)

#define HXR_END(t)  \
		HXR_END_INNER_BLOCK_(t) \
		HXR(end_)(t, HXR_SOURCE_LOCATION_HERE_); \
	HXR_END_OUTER_BLOCK_(t)

#define HXR_END_AND_RETURN1(t) \
		HXR_END_INNER_BLOCK_(t) \
		HXR(end_)(t, HXR_SOURCE_LOCATION_HERE_); \
		return;     \
	HXR_END_OUTER_BLOCK_(t)

#define HXR_END_AND_RETURN2(t, val) \
		HXR_END_INNER_BLOCK_(t) \
		HXR(end_)(t, HXR_SOURCE_LOCATION_HERE_); \
		return (val); \
	HXR_END_OUTER_BLOCK_(t)

#define HXR_END_AND_RETURN(...) \
	HXR_MACRO_OVERLOAD(HXR_END_AND_RETURN, __VA_ARGS__)

/// `HXR_MSG_COMMIT` immediately enqueues the message that has so far been
/// constructed in the current message block (HXR_BEGIN_[message type]-HXR_END).
///
/// This is useful in situations where the code in a HXR_BEGIN-HXR_END block
/// has already completed building a message, yet still needs to do other things
/// that could affect the thread's message queue or block stack (such as
/// calling other functions or expanding macros that contain raw `return`
/// or `goto` statements).
///
/// The `HXR_RETURN` and `HXR_GOTO` macros use this macro internally, and are
/// conceptuallt similar to expanding `HXR_MSG_COMMIT` before executing
/// the desired `return` or `goto` statement. In cases where it is necessary
/// to `return` or `goto` from a `HXR_BEGIN-HXR_END` block, the `HXR_RETURN`
/// and `HXR_GOTO` macros should be used instead of `HXR_MSG_COMMIT`.
/// Those macros are more specific and could, in the future, provide
/// functionality that `HXR_MSG_COMMIT` can not provide. And if a return or
/// goto is desired, then using `HXR_RETURN` or `HXR_GOTO` will also produce
/// more accurate warning and error messages if anything goes wrong.
///
/// `HXR_MSG_COMMIT`'s effect is shallow and only applies to the innermost
/// (most immediate) `HXR_BEGIN-HXR_END` block that it appears within.
/// It will not enqueue messages in any enclosing blocks.
///
/// Example:
/// ```C
#if HXR_DOCTEST
#include <hexer.h>
#include <hexer.h>

void example_func(hxr_thread *t)
{
    hxr_set_stdout_formatter(&hxr_fmt_minimal_plaintest);
    HXR_BEGIN_INFO(t);
        hxr_msg_summary(t, "outer");
        HXR_BEGIN_INFO(t)
            hxr_msg_summary(t, "inner");
            HXR_MSG_COMMIT(t);

            // Prints "inner\n"
            hxr_send_msg_copy(t);
        HXR_END(t)
        // Prints "inner\n"
        hxr_send_msg_copy(t);
    HXR_END(t);

    // Prints:
    // "inner\n"
    // "outer\n"
    hxr_send_msg(t);
}
#endif
/// ```
///
/// This macro is only valid within a HXR_BEGIN_[message type]-HXR_END block.
/// Using it outside of a block will result in a warning message flagged
/// with `HXR_MSG_FLAG_HEXER`.
///
/// Any `hxr_msg_*` functions (ex: `hxr_msg_details`) called after
/// `HXR_MSG_COMMIT` within the same block will have no effect on the
/// original message (which was already queued and possibly sent), but will,
/// after block exit, trigger an error message flagged with `HXR_MSG_FLAG_HEXER`.
///
/// Example:
/// ```C
#if HXR_DOCTEST
#include <hexer.h>
void example_func(hxr_thread *t)
{
	hxr_stdout_formatter(&hxr_fmt_minimal_plaintest);
	HXR_BEGIN_INFO(t)
		hxr_msg_summary(t, "test");
		HXR_MSG_COMMIT(t);

		hxr_msg_details(t, "This text never makes it."); // <- Error: data loss

		// Prints:
		// "test\n"
		// "`hxr_msg_details` called after `HXR_MSG_COMMIT`\n"
		// "The information provided to this/these function(s) had to be discarded.\n"
		// "Please ensure that the message is completely finished before using `HXR_MSG_COMMIT`\n"
		// "  tags: \"data loss\"\n"
		hxr_send_msg(t);
	HXR_END(t)
}
#endif
/// ```

TODO:
There seems to be a workflow to document:
write message -> commit message -> send message -> receive message -> display message
HXR_BEGIN_XYZ(t); do_stuff() -> HXR_END(t) | HXR_MSG_COMMIT(t) -> hxr_send_msg(t) -> hxr_receive_msg(t,??) -> printf/fprintf/etc (hxr_convenience_functions?)

#define HXR_MSG_COMMIT(t)


TODO: ON_EXIT version will be a little more complicated; it has to handle
divergence/convergence and two counters
#define HXR_ON_ABORT(t, counter, prev_counter) \
	HXR(_on_abort_ ## counter): \
		for(int done = 0; !done && HXR(cleanup_status_) == HXR_CLEANUP_STATUS_ABORTING; done = 1) \
			if (done) { goto HXR(_on_abort_ ## prev_counter); } \
			else

// hxr/on_abort.h


TODO: Is it possible to detect if we're in a different function from the last one?
If so, we could elide the requirement of #include'ing a function header for this feature.

#ifndef HXR_ON_ABORT_COUNTER_CSD 
#define HXR_ON_ABORT_COUNTER_CSD 0,0
#else
#define HXR_ON_ABORT_COUNTER_PREV_CSD  HXR_ON_ABORT_COUNTER_CSD
#undef  HXR_ON_ABORT_COUNTER_CSD
#define HXR_ON_ABORT_COUNTER_CSD  HXR_INCR_CSD(HXR_ON_ABORT_COUNTER_PREV_CSD)
#undef  HXR_ON_ABORT_COUNTER_PREV_CSD
#endif

#ifdef  HXR_ON_ABORT_COUNTER
#undef  HXR_ON_ABORT_COUNTER
#endif

#define HXR_ON_ABORT_COUNTER  HXR_GENERATE_TOKEN_FROM_CSD(HXR_ON_ABORT_COUNTER_CSD)

