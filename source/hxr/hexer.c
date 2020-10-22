#include "hexer.h"

#include <stdarg.h>


// Define _HXR_TLS and _HXR_HAVE_TLS so we can use faster thread-local-storage
// mechanisms where they are available, and detect when they aren't.
#if defined(_MSC_VER)
#	define _HXR_TLS(ty)  __declspec(thread) ty
#	define _HXR_HAVE_TLS 1
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#	define _HXR_TLS(ty)  __thread ty
#	define _HXR_HAVE_TLS 1
#else
	// No TLS attributes. We'll have to use things like pthread_getspecific instead.
#	define _HXR_TLS(ty)  ty
#	define _HXR_HAVE_TLS 0
#endif

TODO: Don't just check for HXR_ENABLE_FILE_IO being defined, or for being non-zero.
It might be undefined, or it might be set to 0. Check for both. If that's too
tricky to do everywhere, then define a derivative macro (or macros) that can
be checked using #if like is done here. Undefine such things at exit from
the file to preserve hygiene.
#if (HXR_ENABLE_FILE_IO) || (HXR_DOCUMENTATION_BUILD)
#include <stdlib.h>
#include <stdio.h>
#endif

typedef struct S_HXR__LIBC_VTBL
{
	// Internal-use functions that will usually point to the corresponding libc
	// or system function, but may be individually replaced by the caller if needed.

	#if (HXR_ENABLE_FILE_IO) || (HXR_DOCUMENTATION_BUILD)
	int  (vfprintf*)(FILE *, const char *, va_list);
	#endif

	int  (vsnprintf*)(char *, size_t, const char *, va_list);

	#if (HXR_ENABLE_SYSLOG) || (HXR_DOCUMENTATION_BUILD)
	void (vsyslog*)(int, const char *, va_list);
	#endif

} hxr_libc_vtbl_;

// Internal-use function that is probably usually `printf`. Avoid using it if
// possible. However, if there is no valid `hxr_process` or `hxr_thread` to
// report errors with, then it is appropriate to use this instead.
//
// We don't put this in `hxr_libc_vtbl_` because this needs to be accessible
// when we don't have viable `hxr_process` or `hxr_thread` objects.
int  (HXR(debugf_)*)(const char *str, ...);

static hxr_libc_vtbl_  hxr_libc_vtbl_instance_;

static void hxr_init_libc_vtbl_()
{
	hxr_libc_vtbl_instance_.vfprintf  = &HXR_VFPRINTF_DEFAULT;
	hxr_libc_vtbl_instance_.vsnprintf = &HXR_VSNPRINTF_DEFAULT;
	hxr_libc_vtbl_instance_.vsyslog   = &HXR_VSYSLOG_DEFAULT;
	HXR(debugf_) = &HXR_DEBUGF_DEFAULT;
}

#if (HXR_ENABLE_FILE_IO) || (HXR_DOCUMENTATION_BUILD)
int HXR(vfprintf_noop)(FILE *fd, const char *fmtstr, va_list vargs)
{
	return -1;
}
#endif

int HXR(vsnprintf_noop)(char *s, size_t n, const char *format, va_list vargs)
{
	return -1;
}

#if (HXR_ENABLE_SYSLOG) || (HXR_DOCUMENTATION_BUILD)
void HXR(vsyslog_noop)(int priority, const char *format, va_list ap)
{
	// Do nothing.
}
#endif

int HXR(debugf_noop)(const char *str, ...)
{
	return -1;
}

// -------------------------------------

typedef struct S_HXR__MESSENGER_VTBL
{
	const char *(get_class_name)(hxr_thread *t);
	uint8_t (deliver*)(hxr_thread *t, hxr_feedback_message *msg);

} hxr_messenger_vtbl_;

typedef struct S_HXR__MESSENGER_IMPL
{
	void                  *impl;
	hxr_messenger_vtbl_   *vtable;
	hxr_source_location_  init_loc;
	hxr_source_location_  final_loc;

} hxr_messenger_impl_;

static const char *HXR(invalid_messenger_class_name)(hxr_thread *t);
{
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_CANT_MESSAGE | HXR_FNCLASS_GETTER);
	return "hxr_invalid_messenger";
}

static uint8_t HXR(invalid_messenger_deliver)(hxr_thread *t, hxr_feedback_message *msg)
{
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_CANT_MESSAGE | HXR_FNCLASS_NORMAL);

	// To avoid infinite looping possibilities, we won't call HXR_BEGIN_ERROR
	// from within this function, at least if it was handling an error.
	// Explanation: If we were to create an error because the caller attempted
	// to print an error the wrong way, and then the caller tries to print that
	// error the wrong way too, it creates another error for us to fail to
	// print, and on we go forever. This is why we should call HXR(debugf_)
	// instead during such situations, even if that is normally undesirable.
	if ( hxr_message_type(msg) == HXR_MESSAGE_TYPE_ERROR )
	{
		HXR(debugf_)(
			"hxr_feedback_receiver->handle_msg() called on an invalid feedback receiver.\n"
			""
	}
	else
	HXR_BEGIN_ERROR(t);
		hxr_message_id(t, "invalid_messenger_deliver");
		hxr_summary(t, "hxr_feedback_receiver->handle_msg() called on an invalid feedback receiver.");
		hxr_details_fmt(t,
			"This stream was initialized in file \"%s\", function \"%s\", and line %zd. "
			"The stream was finalized in file \"%s\", function \"%s\", and line %zd. "
			"The text that was to be printed is as follows: %c%s%c",
			init_loc.file,  init_loc.func,  init_loc.line,
			final_loc.file, final_loc.func, final_loc.line,
			tp.need_newline_before, text, tp.need_newline_after);
	HXR_END(t);

	return 0; // Always fail; nothing should call this.
}

// -------------------------------------


typedef struct S_HXR__ALLOCATOR
{
	void *allocator_state;
	void *(allocate*)(hxr_thread *, size_t);
	void *(reallocate*)(hxr_thread *, void *, size_t);
	void (free*)(hxr_thread *, void *);

} hxr_allocator;

static void *hxr_default_malloc(hxr_thread* t, size_t num_bytes) {
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_WRAPPER);
	return HXR_MALLOC_DEFAULT(num_bytes);
}

static void *hxr_default_realloc(hxr_thread* t, void *original, size_t num_bytes) {
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_WRAPPER);
	return HXR_REALLOC_DEFAULT(original, num_bytes);
}

static void *hxr_default_free(hxr_thread* t, void *memptr) {
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_WRAPPER);
	HXR_FREE_DEFAULT(memptr);
}

static void hxr_allocator_init_(hxr_allocator *new_allocator)
{
	new_allocator->allocator_state = NULL;
	new_allocator->allocate   = &hxr_default_malloc;
	new_allocator->reallocate = &hxr_default_realloc;
	new_allocator->free       = &hxr_default_free;
}

// -------------------------------------

TODO: Thinking of just eliminating hxr_process. It seems pointless.
hxr_thread objects should inherit settings from the thread that spawned
their current thread.
For the odd cases where the parent thread can't be determined,
have a process-wide instance of hxr_thread that is used as a "template"
for new thread creation.

typedef struct S_HXR__PROCESS_IMPL
{
	hxr_allocator             default_allocator;
	hxr_logger                default_logger;
	hxr_message_format        default_msg_format;

} hxr_process_impl_;

typedef hxr_process  hxr_process_embed_container_;

typedef struct S_HXR__PROCESS_WRAPPER
{
	hxr_process_impl_             impl;
	hxr_process_embed_container_  embeds;
} hxr_process_wrapper_;

static void hxr_process_init_(hxr_process *proc)
{
	hxr_allocator_init_(&proc->default_allocator);
	hxr_logger_init_(&...); TODO
	TODO: how do we organize message formatters? Shouldn't these be per-output?
}


typedef struct S_HXR__THREAD_IMPL
{
	hxr_process_wrapper_      *process;
	hxr_allocator             *allocator;
	hxr_logger                *logger;
	hxr_message_format        *msg_format;

	size_t                    error_count;
	hxr_feedback_message      *message_queue;
	hxr_feedback_handler      message_handler_func_ptr;
	void                      *message_handler_context;
}
hxr_thread_impl_;

typedef hxr_thread  hxr_thread_embed_container_;

/*
typedef struct S_HXR__THREAD_EMBED_CONTAINER
{
	void **dynamic_embeds;

#ifdef HXR_THREAD_STATIC_EMBEDS
#define HXR_X(embed_type, embed_name) embed_type embed_name;
	HXR_THREAD_STATIC_EMBEDS(HXR_X)
#undef HXR_X
#endif
} hxr_thread_embed_container_;
*/

typedef struct S_HXR__THREAD_WRAPPER
{
	hxr_thread_impl_             impl;
	hxr_thread_embed_container_  embeds;
}
hxr_thread_wrapper_;

#define HXR_OFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

static hxr_thread_impl_ *HXR(thread_get_impl_)(hxr_thread* t)
{
	// The hxr_thread* pointer given to caller-space points to the `embeds`
	// member of the hxr_thread_wrapper_ struct.
	//
	// Graphically, it looks like this:
	//
	//                  [ hxr_thread_wrapper_ ]
	//                    [ hxr_thread_impl_ ]
	//                      [ hxr_process_wrapper_* ]
	//                      [ hxr_allocator_* ]
	//                      [ ... ]
	// hxr_thread* t ---> [ hxr_embed_container_ ]
	//                      [ void **dynamic_embeds ]
	//                      [ caller's library's struct ]
	//                        [ library_context_part_01 ]
	//                        [ library_context_part_02 ]
	//                        [ ... ]
	//                      [ caller's program's struct ]
	//                        [ program_context_part_01 ]
	//                        [ program_context_part_02 ]
	//                        [ ... ]
	//
	// This might seem unintuitive at first, as pointers typically point to the
	// beginning of the memory allocated for the object they reference.
	//
	// Note that the hxr_thread* pointer must point directly to the caller's
	// static embeds. If hxr_thread were declared as a struct that included
	// hxr_thread_impl_ before the embeds, then this would leak implemention
	// details into the API.
	//
	// A more intuitive way around this might be to place the embeds *before*
	// the hxr_thread_impl_ object, then allocate enough bytes for both and
	// omit hxr_thread_impl_ from the header's version of the definition.
	// However, this has downsides, the more severe of which stem from the fact
	// that we would be making assumptions about the correctness of the caller's
	// `HXR_THREAD_STATIC_EMBEDS` macro definition, and also about the caller's
	// ability to avoid buffer overruns.
	//
	// So instead, we opt to place the hxr_thread_impl_ object at the lowest
	// address, place the caller's static embeds beyond that, and then just
	// deal with having to do an extra subtract operation (maybe) whenever
	// accessing our internal state.
	//
	// This has beneficial features:
	//
	// * If the caller has a buffer overrun in a static embed, then this
	//     will NOT corrupt HeXeR's state. If the embeds came first instead,
	//     then any overrun would almost certainly bake the hxr_thread_impl_ object.
	//
	// * If two binary-incompable versions of a hxr_thread somehow end up in
	//     the same executable/library/etc, then the resulting chaos will
	//     NOT corrupt HeXeR's state (the hxr_thread_impl_ object), just the
	//     embedded objects. If the embeds came first instead, then the
	//     hxr_thread_impl_ object would likely be corrupted, AND the embedded
	//     objects would still face corruption.
	//
	// * This places the frequently-accessed hxr_thread_impl_ stuff close to
	//     the beginning of the hxr_thread object's memory allocation. This
	//     may or may not matter at all, but it's more likely to help than
	//     to hurt.
	//
	return (hxr_thread_impl_*)(((char*)t) - HXR_OFFSETOF(hxr_thread_wrapper_, embeds));
}

// Forward declaration, because this function is much easier to write when
// it's placed towards the bottom of the file.
static void hxr_process_level_early_init();

static hxr_process  hxr_process_instance_;

/// Initializes the HeXeR library and creates the `hxr_process*` object.
hxr_process  *HXR(start)()
{
	hxr_process_level_early_init();
	hxr_process_init_(&hxr_process_instance_);
}

/// Returns the process-wide instance of the `hxr_process` object.
hxr_process  *HXR(get_current_process)();

// -------------------------------------

typedef struct S_HXR__TEXT_PLACEMENT_INFO
{
	char need_newline_before;
	char need_newline_after;

} hxr_text_placement_info_;

static void hxr_get_text_placement_info_(
		hxr_thread *t,  hxr_text_placement_info_ *result,  const char *text)
{
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_NORMAL);

	size_t newline_count = 0;
	for ( size_t i = 0; text[i] != '\0'; i++ )
		if ( text[i] == '\n' )
			newline_count++;
	size_t len = i;

	char newline_after = '\n';
	if ( len > 0 && text[len-1] == '\n' ) {
		newline_after = '\0'; // Don't put a redundant line ending.
		newline_count--; // The last one doesn't count.
	}

	char newline_before = '\0';
	if ( newline_count > 0 )
		newline_before = '\n';

	result->need_newline_before = newline_before;
	result->need_newline_after  = newline_after;
}

// ===== Stream Structure : hxr_stream_ =====
/// Very simple internal-use stream object that allows output-writing code
/// to have less code duplication.

#include <stdarg.h>

typedef struct S_HXR__STREAM_VTBL
{
	ssize_t (write_line*)  (hxr_thread*, hxr_stream*, const char* text);
	ssize_t (write_text*)  (hxr_thread*, hxr_stream*, const char* text);
	ssize_t (write_fmtstr*)(hxr_thread*, hxr_stream*, const char* fmtstr, va_list);
} hxr_stream_vtbl_;

typedef struct S_HXR__STREAM
{
	void                  *impl;
	hxr_stream_vtbl_      *vtable;
	hxr_source_location_  init_loc;
	hxr_source_location_  final_loc;
} hxr_stream_;

// The canary stream (defined further down) is used to represent an abstract
// stream's default state of "I'm not programmed to do ANYTHING". This allows
// some programming mistakes to turn into error messages instead of segfaults.
static ssize_t canary_stream_write_line(hxr_thread* t,  hxr_stream_* stream,  const char* text);
static ssize_t canary_stream_write_text(hxr_thread* t,  hxr_stream_* stream,  const char* text);
static ssize_t canary_stream_write_fmtstr(hxr_thread* t,  hxr_stream_* stream,  const char* fmtstr, va_list vargs);

static hxr_stream_vtbl_  hxr_canary_stream_vtbl_;

static void hxr_stream_module_init_()
{
	hxr_canary_stream_vtbl_.write_line   = &canary_stream_write_line;
	hxr_canary_stream_vtbl_.write_text   = &canary_stream_write_text;
	hxr_canary_stream_vtbl_.write_fmtstr = &canary_stream_write_fmtstr;
}

static void hxr_stream_init_(hxr_thread *t, hxr_stream_ *stream, hxr_source_location_ loc)
{
	stream->init_loc  = loc;
	stream->final_loc = hxr_make_source_loc_("N/A", "N/A", 0);
	stream->vtable    = &hxr_canary_stream_vtbl_;
	stream->impl      = NULL;
}

void hxr_stream_finalize_(hxr_thread *t, hxr_stream_ *stream, hxr_source_location_ loc)
{
	stream->final_loc = loc;
	stream->vtable    = &hxr_canary_stream_vtbl_;
	stream->impl      = NULL;
}

// ===== Stream Interface : hxr_stream_* =====

static ssize_t stream_write_line(hxr_thread* t,  hxr_stream_* stream,  const char* text) {
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_WRAPPER);
	return stream->write_line(t, stream, text);
}

static ssize_t stream_write_text(hxr_thread* t,  hxr_stream_* stream,  const char* text) {
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_WRAPPER);
	return stream->write_text(t, stream, text);
}

static ssize_t stream_write_text_fmt(hxr_thread* t,  hxr_stream_* stream,  const char* fmtstr, ...) {
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_WRAPPER);
	va_list vargs;
	va_start(vargs, fmtstr);
	ssize_t rc = stream->write_fmtstr(t, stream, fmtstr);
	va_end(vargs);
	return rc;
}

static ssize_t stream_write_text_fmt_va(hxr_thread* t,  hxr_stream_* stream,  const char* fmtstr, va_list vargs) {
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_WRAPPER);
	return stream->write_fmtstr(t, stream, fmtstr, vargs);
}

// ===== Canary Stream : canary_stream_* =====
// This is used to raise errors whenever a finalized stream is used.

static ssize_t canary_stream_write_line(hxr_thread* t,  hxr_stream_* stream,  const char* text)
{
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_NORMAL);

	hxr_source_location_  init_loc  = stream->init_loc;
	hxr_source_location_  final_loc = stream->final_loc;

	HXR_BEGIN_ERROR(t);
		hxr_message_id(t, "canary_stream_write_line");
		hxr_summary(t, "write_line() called on an expired stream.");
		hxr_details_fmt(t,
			"This stream was initialized in file \"%s\", function \"%s\", and line %zd. "
			"The stream was finalized in file \"%s\", function \"%s\", and line %zd. "
			"The text that was to be printed is as follows: \"%s\"",
			init_loc.file,  init_loc.func,  init_loc.line,
			final_loc.file, final_loc.func, final_loc.line,
			text);
	HXR_END(t);

	return -1;
}

static ssize_t canary_stream_write_text(hxr_thread* t,  hxr_stream_* stream,  const char* text)
{
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_NORMAL);
	hxr_source_location_  init_loc  = stream->init_loc;
	hxr_source_location_  final_loc = stream->final_loc;

	hxr_text_placement_info_  tp;
	hxr_get_text_placement_info_(t, &tp, text);

	HXR_BEGIN_ERROR(t);
		hxr_message_id(t, "canary_stream_write_text");
		hxr_summary(t, "write_line() called on an expired stream.");
		hxr_details_fmt(t,
			"This stream was initialized in file \"%s\", function \"%s\", and line %zd. "
			"The stream was finalized in file \"%s\", function \"%s\", and line %zd. "
			"The text that was to be printed is as follows: %c%s%c",
			init_loc.file,  init_loc.func,  init_loc.line,
			final_loc.file, final_loc.func, final_loc.line,
			tp.need_newline_before, text, tp.need_newline_after);
	HXR_END(t);

	return -1;
}

static ssize_t canary_stream_write_fmtstr(hxr_thread* t,  hxr_stream_* stream,  const char* fmtstr, va_list vargs)
{
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_NORMAL);
	hxr_source_location_  init_loc  = stream->init_loc;
	hxr_source_location_  final_loc = stream->final_loc;

#define BUFSIZE  (1024)
	char printbuf[BUFSIZE];
	size_t buflen = BUFSIZE;
	char *finalstr = printbuf;

	va_list vargs_consumable;
	va_copy(vargs_consumable, vargs);
	int rc = vsnprintf(finalstr, buflen, fmtstr, vargs_consumable);
	if ( rc > buflen ) {
		buflen = rc+1;
		finalstr = hxr_malloc(t, buflen);
		va_copy(vargs_consumable, vargs);
		rc = snprintf(finalstr, buflen, fmtstr, vargs_consumable);
	}

	hxr_text_placement_info_  tp_raw;
	hxr_text_placement_info_  tp_fmt;
	hxr_get_text_placement_info_(t, &tp_raw, fmtstr);
	hxr_get_text_placement_info_(t, &tp_fmt, fmtstr);

	HXR_BEGIN_ERROR(t);
		hxr_message_id(t, "canary_stream_write_fmtstr");
		hxr_summary(t, "write_line() called on an expired stream.");
		hxr_details_fmt(t,
			"This stream was initialized in file \"%s\", function \"%s\", and line %zd. "
			"The stream was finalized in file \"%s\", function \"%s\", and line %zd. "
			"The format string that was to be printed is as follows: %c%s%c\n"
			"The resulting text after formatting is: %c%s%c",
			init_loc.file,  init_loc.func,  init_loc.line,
			final_loc.file, final_loc.func, final_loc.line,
			tp_raw.need_newline_before, fmtstr,   tp_raw.need_newline_after,
			tp_fmt.need_newline_before, finalstr, tp_fmt.need_newline_after);
	HXR_END(t);

	if ( finalstr != printbuf )
		hxr_free(t, finalstr);

	return -1;
}

// ===== File Stream : fstream_* =====

#ifdef HXR_ENABLE_FILE_IO
#include <stdio.h>

static ssize_t fstream_write_line(hxr_thread* t,  hxr_stream_* stream,  const char* text) {
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_NORMAL);
	FILE *fd = stream->impl;
	int rc = fprintf(fd, "%s\n", text);
	if ( rc < 0 )
		return -1;
	else
		return rc;
}

static ssize_t fstream_write_text(hxr_thread* t,  hxr_stream_* stream,  const char* text) {
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_NORMAL);
	FILE *fd = stream->impl;
	int rc = fprintf(fd, "%s", text);
	if ( rc < 0 )
		return -1;
	else
		return rc;
}

static ssize_t fstream_write_fmtstr(hxr_thread* t,  hxr_stream_* stream,  const char* text, va_list vargs) {
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_NORMAL);
	FILE *fd = stream->impl;
	int rc = fprintf(fd, fmtstr, vargs);
	if ( rc < 0 )
		return -1;
	else
		return rc;
}

static hxr_stream_vtbl_  hxr_fstream_vtbl_;

static void fstream_module_init()
{
	hxr_fstream_vtbl_.write_line   = &fstream_write_line;
	hxr_fstream_vtbl_.write_text   = &fstream_write_text;
	hxr_fstream_vtbl_.write_fmtstr = &fstream_write_fmtstr;
}

void fstream_init(hxr_thread *t, hxr_stream_ *stream, hxr_source_location_ loc)
{
	hxr_stream_init_(t, stream, loc);
	stream->vtable = &hxr_fstream_vtbl_;
	stream->impl   = stdout;
}

#define FSTREAM_INIT(t, stream) \
	(fstream_init(t, stream, HXR_SOURCE_LOCATION_HERE_))

void fstream_set_fd(hxr_thread *t, hxr_stream_ *stream, FILE *fd)
{
	stream->impl = fd;
}

void fstream_finalize(hxr_thread *t, hxr_stream_ *stream, hxr_source_location_ loc)
{
	hxr_stream_finalize_(t, stream, loc);
}

#define FSTREAM_FINALIZE(t, stream) \
	(fstream_finalize(t, stream, HXR_SOURCE_LOCATION_HERE_))

#endif // HXR_ENABLE_FILE_IO

// ===== Message Printing =====

#ifdef HXR_ENABLE_FILE_IO
void hxr_print_message(hxr_thread *t, FILE *fd, hxr_feedback_message *msg)
{
	HXR_ENTER_FUNCTION(t, HXR_FNCLASS_NORMAL);
	HXR_NEVER_NULL(t, t);
	HXR_IF_NULL_ABORT(t, msg);
	HXR_IF_NULL_ABORT(t, fd);
	if ( hxr_constraint_violation_count(t) )
		HXR_RETURN(t);

	hxr_stream_  stream;
	FSTREAM_INIT(t, &stream);
	fstream_set_fd(t, &stream, fd);
	hxr_send_message_(t, &stream, msg);
	FSTREAM_FINALIZE(t, &stream);
	HXR_RETURN(t);
}
#endif // HXR_ENABLE_FILE_IO




// Earlier in the init process, we can't assume that there are any valid
// hxr_thread* objects, hence why this function does not accept one as
// an argument, and why all of the functions called here do not accept
// hxr_thread* objects. If you have to change this for any reason, please
// be careful about preventing unbounded recursion and accessing of
// uninitialized memory+resources.
static void hxr_process_level_early_init()
{
	static uint8_t hxr_early_init_done = 0;
	if ( hxr_early_init_done )
		return;

	hxr_init_libc_vtbl_();
	hxr_stream_module_init_();
	hxr_fstream_module_init_();
	hxr_early_init_done = 1;
}
