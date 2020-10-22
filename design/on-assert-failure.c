#include <stdio.h>

// This idea is canned
//
// Most of the pieces are here. I can even generate a unique label when
// expanding HXR_ON_EVENT_DO. What I can't do is have other macros jump to
// that macro. Basically I'd need a way for a macro to generate the
// `goto hxr_event_lbl_35;` and `goto hxr_event_lbl_39;` statements below.
// Ideally there would be a way to make it "fall through" back to the
// macro firing the event, instead of having it just dump execution
// wherever the ON_X_DO_Y macro happened to be expanded.
//
// I think it's possible with that GCC extension called
// "labels as values" which uses a unary && operator to take the address
// of a label (the instruction pointer perhaps?).
// It was mentioned here:
// https://mort.coffee/home/obscure-c-features/
//
// I don't want to make the library only compile on GCC though, so this
// feature isn't going to happen unless I need it badly enough to "invent"
// another way or someone gives me a good-nasty idea.
//
// And then there's setjmp/longjmp. Hmmmmm. Maybe if I (or someone) REALLY
// wants this.
//

#define HXR_ON_EVENT_DO__(block, uid) \
	do { if(0) { \
	hxr_event_lbl_ ## uid: \
		block; \
	} } while (0)

#define HXR_ON_EVENT_DO_(block, uid) HXR_ON_EVENT_DO__(block, uid)
#define HXR_ON_EVENT_DO(block)       HXR_ON_EVENT_DO_(block, __LINE__)


int main(int argc, const char *argv[])
{
	HXR_ON_EVENT_DO(printf("a\n"); goto foo;);
	goto hxr_event_lbl_35;

foo:
	HXR_ON_EVENT_DO(printf("b\n"); goto bar;);
	goto hxr_event_lbl_39;

bar:
	while(0)
	{
	onfail:
		printf("It worked! (But it doesn't matter.)\n");
		goto leave;
	}
	goto onfail;
	printf("It didn't work.\n");

leave:
	return 0;
}
