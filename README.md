HeXeR
=====

## Note: This library is a work-in-progress and isn't ready yet.
## Most features are only partially implemented and aren't usable right now. Sorry.

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

The goal is to encourage more humane error messages and logging information
in C programs by providing authors with an easy and widely-accessible way
to make their programs present information that is comprehensive, relevant,
actionable, and succinct.

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
* C preprocessor metaprogramming module(s) that implement features such as
    macro dispatch on variadic argument count and arithmetic using
    comma-separated-digits (CSD).
* Some unittesting constructs (ex: `HXR_ASSERT`). HeXeR won't feel bad if
    you want to use something like [Seatest](https://github.com/keithn/seatest)
    instead, but making HeXeR reliable (and low on dependencies) means it needs
    its own (minimal) unittesting capabilities.
* Some variable|value constraints (ex: `HXR_NEVER_NULL(my_variable);`).
    These are similar to the unittesting constructs in terms of (minimal)
    complexity and purpose.
