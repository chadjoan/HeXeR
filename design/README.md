
In the chasm of C there is a fractal void. As I try to create one construct,
I realize it needs many others. And then, those endeavors require yet more
below them. And down and down I go. Eventually I realize I am building
something from nothing. Just as a dream, it exists, yet lacks substance.

I accept the absurdity of the depths, but not defeat. We need to start somewhere.
My consciousness waning, I cast a hex upon the (void).

Awaken, HeXeR!

HeXeR
=====

HeXeR is a highly portable C (ISO C99) library that provides a novel replacement
for typical exception handling mechanisms, along with structured logging and
reporting features.

HeXeR is intended to be very accessible, so that it can be adopted without
significant downsides. To accomplish this, HeXeR has these attributes:
* Few or no mandatory dependencies.
* Avoids features that could be slow or complicate software integration and
    language interop (ex: HeXeR does not use setjmp+longjmp, and HeXeR's
    interface avoids C data types with ambiguities, such as `int` or `unsigned`).
* Highly permissive Boost Software License (BSL-1.0).
* Narrow-ish scope: HeXeR is not a general purpose library or framework, so
    including it in your project will not introduce large amounts of features
    that could be redundant with (or incompatible with) other libraries that
    you are using.

[//]: # (The intent is to make it as easy)  
[//]: # (as possible to write good error messages, aggregate relevant information from)  
[//]: # (all levels of the call stack, and perform sensible recovery with)  
[//]: # (less boilerplate code or unappealing trade-offs.)  

The goal is to encourage more humane error messages and logging information
in C programs by providing authors with an easy and widely-accessible way
to make their programs present information that is comprehensive, relevant,
actionable, and succinct.

[//]: # (HeXeR is a C (ISO C99) library for enabling programs and libraries to be)  
[//]: # (more humane by making it easy to write good error messages, keep the user)  
[//]: # (informed, and also to determine where bugs and problems come from.)  

[//]: # (HeXeR is a low-level library with high-level implications. It eases the)  
[//]: # (process of recording and annotating program state. It provides a conduit)  
[//]: # (for human-relevant information to flow from algorithms to interfaces.)  
[//]: # (It allows us developers to tell ourselves and our users not only *what just)  
[//]: # (happened*, but also *what to do about it*.)  

Features:
* Errors can be returned through a common thread object (`hxr_thread`)
    instead of as a return value. Now your C functions that return values
    can actually *return values*.
* Easy ways to signal more than one error during a function's execution
    (ex: so simple parsers don't have to stop on the first error).
* Structured message/error methodology makes it easier to avoid log-spam
    and related anti-patterns while still providing detailed information
    that includes parameters and function/program state.
* Message-sending can replace misuses of `printf`, thus allowing algorithmic
    code to decouple itself from user interface logic. Algorithms should be
    responsible for creating appropriate text as feedback, but should not be
    responsible for getting that text to the user's eyeballs.
* An automatic resource cleanup feature (TODO: link) similar to the
    D programming language's [`scope` guard](https://tour.dlang.org/tour/en/gems/scope-guards)
    statements  or the Go language's [`defer` statements](https://gobyexample.com/defer).
* HeXeR can be built without a `libc` implementation, though if this is necessary,
    it is strongly advised that the caller provides substitutes for
    basic functions such as `malloc` and `printf`, so that text formatting
    and storage is possible.
* HeXeR can be used in small parts of large projects and expanded slowly as
    needs require, and as comfort levels allow. For example: initialization
    within `main` is not necessary (though it does provide some small
    optimizations, if you can do it), and messages can be sent between
    functions that are separated on the call stack by other functions that
    are not aware of HeXeR.

HeXeR also has some secondary features and offerings. These were implemented
as necessary building blocks for HeXeR's primary features, but these also
solve common programming problems and are useful on their own merit.

Secondary features:
* C preprocessor metaprogramming modules that implement features such as:
    macro dispatch on variadic argument count, arithmetic using
    comma-separated-digits (CSD) or comma-separated-hexadigits (CSH),
    and lists (TODO! ... could be useful for in-expansion ON_ABORT/ON_SUCCESS/ON_EXIT).
* Some unittesting constructs (ex: `HXR_ASSERT`). HeXeR won't feel bad if
    you want to use something like [Seatest](https://github.com/keithn/seatest)
    instead, but making HeXeR reliable (and low on dependencies) means it needs
    its own (minimal) unittesting capabilities.
* Some variable|value constraints (ex: `HXR_NEVER_NULL(my_variable);`).
    These are similar to the unittesting constructs in terms of (minimal)
    complexity and purpose.

#### Scope

HeXeR seeks to fill small, but long-overlooked, gaps in functionality that
have made basic engineering discipline and code quality unnecessarily difficult
to acheive in C projects of any size.

HeXeR's feature scope is fairly restricted and avoids overlap with other
C libraries.

HeXeR will never feature:
- Exception handling.
- Regular expressions.
- Timing functions.
- Network I/O.
- Serialization/deserialization.
- etc.

There's nothing wrong with those features, but if HeXeR implemented them,
it could inadvertantly experience dependency creep or create tight coupling
between unrelated features, which would make release engineering and
software integration more painful than they already are. With any luck, it
should already be easy to do many of those things using libraries that
existed long before HeXeR.

There are a few features that are implemented commonly in other places but
are re-implemented in HeXeR, which happens only because HeXeR itself needs
them to function and be humane (ex: constraints, assertions).

#### User Choice

HeXeR will make use of whatever you can provide, or improvise if you can't.
Thus, HeXeR has many optional macros and functions that can be used to
make it slightly faster or provide additional redundancy and robustness.

Usage
-----

HeXeR is versatile: it can benefit from comprehensive initialization in
`main`, but it can also be used in library or legacy code with very little
initialization. Some macros, like `HXR_ENTER_FUNCTION`, are completely
optional, but can be used to provide redundancies that allow HeXeR to
catch mistakes in its usage, or simply provide more detailed information
after an exception.

#### Initialization

HeXeR's ideal initialization, in summary, looks like this:

```C
int main (int argc, const char *argv[])
{
    hxr_start(); // Important!

    hxr_thread  *t;
    HXR_THREAD_ALLOCATE(&t); // Optional

    int rc = inner_main(t, argc, argv); // Reliability++

    hxr_print_messages(t); // Important!

    return rc;
}

int inner_main(hxr_thread *t, int argc, const char *argv[])
{
    // Your code goes here.

    printf("Hello world!\n");
    return 0;
}
```

The above is presented first because, if you're curious about HeXeR
and just want to bang out a simple program as fast as possible, then
you can just start by copy-pasting that and have no regrets.
After pasting, visit the [??? TODO] section to learn the important helpful
bits.

Now, if you want to know what all of that means, here is a semantically
identical version with far more annotation:

```C
int main (int argc, const char *argv[])
{
    // Libraries that don't use HeXeR can be initialized here, if so desired.

    // ...

    // `hxr_start` will create a `hxr_process` object internally and ensure
    // that all of HeXeR's initial memory is properly initialized.
    hxr_start();

    // Optionally, use the `HXR_THREAD_ALLOCATE` macro to create a `hxr_thread`
    // object on the stack (if possible) and give it some working memory.
    //
    // This isn't a big deal, but it does potentially avoid a few calls
    // to `malloc`, and perhaps more importantly, makes the `hxr_thread`
    // object more likely to benefit from locality of reference. In other
    // words: it's more likely to be cached when the CPU tries to read it
    // from memory, and it's internals are more likely to be in-memory
    // adjacent to the `hxr_thread` struct, which further improves odds on
    // cache hits whenever `hxr_thread`'s internal pointers are dereferenced.
    //
    // It's safe to use `HXR_THREAD_ALLOCATE` after a `hxr_thread`
    // object was already created somewhere else (ex: in another library).
    // At worst, this will just forfeit the advantages of using
    // `HXR_THREAD_ALLOCATE` while incuring the small cost in stack
    // memory that using `HXR_THREAD_ALLOCATE` entails.
    //
    hxr_thread  *t;
    HXR_THREAD_ALLOCATE(&t);

    // Libraries that DO use HeXeR should be initialized here, if possible.
    //
    // If you worry that you might not know which libraries do or don't use
    // HeXeR, then just ignore this and put library initialization wherever
    // you want.
    //
    // Out-of-order initialization *might* be slightly less optimal, but
    // it won't change HeXeR's behavior and it should be reasonably efficient
    // anyways.
    //

    // ...

    // To ensure that hxr_print_messages() gets called, we make our `main`
    // function be a wrapper and move the usual `main` logic into the function
    // it wraps: `inner_main`.
    int rc = inner_main(t, argc, argv);

    // Now that we know we'll reach this place in code, we can display any
    // messages (including errors) that haven't been handled already
    // (e.g. because nothing knew how to handle them).
    //
    // The `hxr_print_messages` function sends these things to `stdout`.
    // If your program communicates in another way, then it would make sense
    // to call another function instead, but it is important to process
    // messages *somehow*. (If you need some other library for that, then it
    // would make sense to put that library's initailization/finalization
    // before and after HeXeR's initialization.)
    //
    // Ensuring message display will avoid that ugly situation where someone
    // runs your program and it just exits "immediately" without printing any
    // feedback whatsoever, and you get emails/tickets/etc from the user
    // stating that your program "didn't work".
    //
    // Don't let that happen: ALWAYS display the remaining messages.
    //
    hxr_print_messages(t);

    // ...

    // Close/free/finalize any other libraries here.

    return rc;
}

int inner_main(hxr_thread *t, int argc, const char *argv[])
{
    // Your code goes here.

    printf("Hello world!\n");
    return 0;
}
```

This still leaves unanswered questions about how to handle situations
where `main` is inaccessible or there is no obvious choice for
initialization placement. We'll get to that in later in the (TODO: link)
Integration section, because it will help to first understand how HeXeR
is used and what its idiomatic usage looks like.

#### Writing Messages

Perhaps the biggest reason to use HeXeR is to be able to easily write
structured error messages, and then pass the responsibility for displaying
these messages off to *other* code that knows how to display text to users
in a natural way.

Here's the abridged example:

```C
#include <hexer.h>

void tremendous_potential(hxr_thread *t)
{
	int elephant_mass = 6000; // kg
	int sliding_object_mass = 3; // kg

	// ... Stuff happens ...

	// ... and then:
	if ( there_is_an_elephant_in_the_way )
	HXR_BEGIN_ERROR(t);

		hxr_summary(t, "Object did not continue to move. There is an elephant in the way.");
		hxr_message_id(t, "elephant_in_way");

		hxr_details_fmt(t,
			"There is a %d kg elephant in front of the frictionless ramp.\n"
			"The %d kg object is unable to proceed towards the ramp.\n",
			elephant_mass, sliding_object_mass);

		hxr_suggestion(t,
			"Either lure the elephant away with some food, request the help "
			"of a staff assistant, or do something else for a few hours "
			"before coming back to this.");
	HXR_END(t);
}
```


And now, in greater detail:

```C
#include <hexer.h>

void tremendous_potential(hxr_thread *t)
{
	int elephant_mass = 6000; // kg
	int sliding_object_mass = 3; // kg

	// ... Stuff happens ...

	// ... and then:
	if ( there_is_an_elephant_in_the_way )
	HXR_BEGIN_ERROR(t);

		// The summary of a message is a short one or two sentence description
		// of what happened or briefly explains what the program is trying to
		// tell the user (or developer/sysadmin).
		//
		// This is the most important part of a message. If you call no other
		// message building functions, then at least call this one.
		//
		// This function comes in a `hxr_summary` version for plain text,
		// a `hxr_summary_fmt` version for formatted text, and a `hxr_summary_va`
		// version for forwarding formatted text with va_list arguments.
		//
		hxr_summary(t, "Object did not continue to move. There is an elephant in the way.");

		// The `hxr_message_id` function provides a way to programmatically
		// identify this message.
		//
		// Conventionally, this will not be displayed in a human-readable
		// error report, or will be hidden and not prominent.
		//
		// A message's idname is useful for things like text internationalisation,
		// log filtering, and repetition detection (logspam prevention).
		//
		// This is optional, but recommended.
		//
		hxr_message_id(t, "elephant_in_way");

TODO: HeXeR probably just should not have _fmt versions of functions.
And if it does, they should be a format that gets delayed until after any
filtering/compression of the message stream, and the formatting elements
should be specified in the message-values section. This structuring, of course,
makes it easy to suppress logspam and ensure relevancy. But it's defeated by
allowing conventional format strings at all levels. Callers can subvert this
of course, but we shouldn't make it easy to do Wrong Things. The easy+happy
path should be the correct one.
		// The `hxr_details` function, and its variants `hxr_details_fmt` and
		// `hxr_details_va`, add a details section to the message.
		//
		// Note that it is perfectly valid to place newlines ('\n') in the
		// text to specify where non-paragraph line returns should go.
		//
		// It is not necessary to ever use other newline sequences, such as
		// "\r\n" (common on the Windows platform). The formatter on the
		// display end is responsible for any line-ending conversion.
		//
		hxr_details_fmt(t,
			"There is a %d kg elephant in front of the frictionless ramp.\n"
			"The %d kg object is unable to proceed towards the ramp.\n",
			elephant_mass, sliding_object_mass);

		// The `hxr_suggestion` function (and variants) produces a section
		// just like `hxr_details`, but informs the downstream formatter that
		// this information suggests a course of action for the user/operator
		// to take that will resolve, rollback, or work around the error that
		// just happened.
		//
		// This element is appropriate for non-error messages too. In this case
		// of informational messages, this could be used to tell the user what
		// they need to do next to proceed through a workflow, or to provide
		// information that is more detailed than, and complementary to,
		// information provided on other channels (ex: labels in a GUI).
		//
		hxr_suggestion(t,
			"Either lure the elephant away with some food, request the help "
			"of a staff assistant, or do something else for a few hours "
			"before coming back to this.");

	HXR_END(t);
}
```


#### Integration

TODO: fix this text
All of the above is great if you're starting from scratch and want to play
a perfect game.

But if you're in a more difficult place, like you're writing a library and
want HeXeR's help, or upgrading a legacy system that spans hundreds of
thousands or millions of lines of code that existed long before HeXeR,
then flexibility is paramount.

If you can't put a `hxr_thread *t` parameter at the beginning of a function
that you're writing or maintaining, then you can still easily use HeXeR
like so:
```C
#include <hexer.h>

void legacy_function(/* some parameter list */)
{
	hxr_start();
	hxr_thread *t = hxr_get_current_thread();

	// Original code goes here, but now you can also do HeXeR things.
}
```

That's it.

As it turns out, the `hxr_thread *t` parameter isn't critical,
though depending on your 

If you're paranoid, you might worry that if `legacy_function` gets called
multiple times, it will result in `hxr_start()` being called multiples,
and that just seems Not Right. Don't worry, `hxr_start` will not do
anything after the first time it's called in a process (a property known
as "idempotency"). Also, after it has completed once, it will be
thread-safe to call it repeatedly.

If you're rewriting more functions, and you want to move to a situation
where that `hxr_thread *t` object is passed along on the stack, then
you can do this:

```C
#include <hexer.h>

void legacy_function(/* some parameter list */)
{
	hxr_start();
	hxr_thread *t = hxr_get_current_thread();

	// Original code goes here, but now you can also do HeXeR things.
}

void replacement_function(hxr_thread *t, /* some parameter list */)
{
	// Original code goes here, but now you can also do HeXeR things.
}
```


Now, whenever you encounter `legacy_function` in code, you can convert
the caller to use `replacement_function` after ensuring that there are
`hxr_start()` and `hxr_get_current_thread()` calls near the top of the
caller's function body.

It's also important to notice that the above pattern allows HeXeR to
be used 
If the legacy system has an accessible `main` function, then you might
still be able to benefit from calling `hxr_start()` there, at which point
you no longer need to call it in any other functions. Additionally,
that also allows you to use 




Configuration (advanced)
------------------------

(TODO: This is kind of an advanced topic, so there should definitely be
more basic examples and documentation first. Gotta get the reader rolling
ASAP and all.)

HeXeR has compile-time configuration settings that are determined by the
definition (if defined) of various preprocessor macros. These can be set
however you wish (ex: compiler arguments, wrapper header file), but must be
"in effect" at the point where the `#include "hexer.h"` directive appears
in your source file, and they should be consistent across all of your
source files where HeXeR is included (some of these options affect important
things like binary layouts of structs or array lengths, so Bad Things could
happen if configurations are not consistent).

Here is a list of current HeXeR configuration settings:
HXR_TESTING_MODE             : boolean constant (default: 0 or undefined)
HXR_DOCUMENTATION_BUILD      : boolean constant (default: 0 or undefined)
HXR_PROCESS_STATIC_EMBEDS(X) : X-macro
HXR_THREAD_STATIC_EMBEDS(X)  : X-macro
HXR_ENABLE_LIBC              : boolean, (default: 1)
HXR_ENABLE_FILE_IO           : boolean, (default: 1)
HXR_ENABLE_SYSLOG            : boolean, (default: 1)
HXR_VFPRINTF_DEFAULT         : function identifier (default: `vfprintf`)
HXR_VSNPRINTF_DEFAULT        : function identifier (default: `vsnprintf`)
HXR_VSYSLOG_DEFAULT          : function identifier (default: `vsyslog`)
HXR_DEBUGF_DEFAULT           : function identifier (default: `printf`)
HXR_MALLOC_DEFAULT           : function identifier (default: `malloc`)
HXR_REALLOC_DEFAULT          : function identifier (default: `realloc`)
HXR_FREE_DEFAULT             : function identifier (default: `free`)
HXR_ALLOW_VLAS               : boolean, (default: 1)   TODO: This should be no longer used, now that ON_ABORT is being rewritten.
HXR_CALL_HISTORY_FNCLASSES   : constant expression of `HXR_FNCLASS_*` values (default: HXR_FNCLASS_NORMAL)
HXR_CALL_HISTORY_MAX         : uint64_t constant
HXR_STACK_TRACE_EXCLUDES     : constant expression of `HXR_FNCLASS_*` values (default: depends on native stack trace availability)
HXR_LINKAGE_PREFIX           : identifier fragment; defaults to `hxr_`

Many C projects use Makefiles to pass feature flags to the C compiler
building their project and its dependencies.

If your project is small or you've had negative experiences with build
systems, you can configure HeXeR for your project by making a `.h` config
file, like so:
```C
// my_hexer_config.h
#ifndef MY_HEXER_CONFIG_H
#define MY_HEXER_CONFIG_H 1

// Note: If you're writing a library, it's a good idea to guard your
// configuration changes in `#ifndef-#endif` blocks and allow your caller
// to override your settings, at least whenever this can be done without
// breaking your own library's code.
#ifndef HXR_ENABLE_FILE_IO
#define HXR_ENABLE_FILE_IO 0
#endif

// As above, if you're writing a library, respect your caller's wishes
// by including their process+thread embeds with your own (if you need
// to embed anything at all, that is) :
#ifndef HXR_THREAD_STATIC_EMBEDS
#define HXR_THREAD_STATIC_EMBEDS(X)
#endif
#define HXR_THREAD_STATIC_EMBEDS(X) \
    HXR_THREAD_EMBES(X) \
    X(my_thread_struct, my_thread_state)
// The above invocation of `HXR_THREAD_STATIC_EMBEDS` from within itself will not
// cause recursion, but will instead simply substitute the previous
// definition at that point in the macro, which may be important here.

#endif
```

Then, you can generate your own version of the HeXeR header:
```C
// my_hexer.h
#ifndef MY_HEXER_H
#define MY_HEXER_H

#include "my_hexer_config.h"
#include "hexer.h"
#endif
```

And in your entry point, near your `main` function, or any other place that
only gets compiled once, summon the instance of your HeXeR:
```C
// main.h

// This must appear first.
#include "my_hexer.h"

// HeXeR, come forth!
#include "hexer.c"

int main(int argc, char *argv[]) {
    // do stuff
}
```
