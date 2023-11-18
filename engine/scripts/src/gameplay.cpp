#include "events.hpp"
#include <api.h>
using namespace api;
extern void benchmark_multiprocessing();

int main()
{
	/* This gets called before anything else, on each machine */
	return 0;
}

/* These are used for benchmarking */
static void empty_function() {}

static void full_thread_function()
{
	microthread::create([](int, int, int) { /* ... */ }, 1, 2, 3);
}

static void oneshot_thread_function()
{
	microthread::oneshot([](int, int, int) { /* ... */ }, 1, 2, 3);
}

static void direct_thread_function()
{
	microthread::direct([] { /* ... */ });
}

static void opaque_dyncall_handler()
{
	sys_empty();
	sys_empty();
	sys_empty();
	sys_empty();
	return_fast();
}

static void inline_dyncall_handler()
{
	isys_empty();
	isys_empty();
	isys_empty();
	isys_empty();
	return_fast();
}

static void opaque_dyncall_args_x4()
{
	sys_empty();
	sys_testing123(4, 5, 6, 7.0, 8.0, 9.0);
	sys_empty();
	sys_testing123(4, 5, 6, 7.0, 8.0, 9.0);
	sys_empty();
	sys_testing123(4, 5, 6, 7.0, 8.0, 9.0);
	sys_empty();
	sys_testing123(4, 5, 6, 7.0, 8.0, 9.0);
	return_fast();
}

static void inline_dyncall_args_x4()
{
	isys_empty();
	isys_testing123(4, 5, 6, 7.0, 8.0, 9.0);
	isys_empty();
	isys_testing123(4, 5, 6, 7.0, 8.0, 9.0);
	isys_empty();
	isys_testing123(4, 5, 6, 7.0, 8.0, 9.0);
	isys_empty();
	isys_testing123(4, 5, 6, 7.0, 8.0, 9.0);
	return_fast();
}

PUBLIC(void public_donothing())
{
	/* nothing */
}

PUBLIC(void benchmarks())
{
	try
	{
		throw GameplayException("This is a test!");
	}
	catch (const GameplayException& ge)
	{
		print("Exception caught: ", ge.what(), "!\n");
		print(
			"Exception thrown from: ", ge.location().function_name(),
			", line ", ge.location().line(), "\n");
	}

	/* This function makes thousands of calls into this machine,
	   while preserving registers, and then prints some statistics. */
	measure("VM function call overhead",
		[] {
			return_fast();
		});
	measure("Full thread creation overhead", full_thread_function);
	measure("Oneshot thread creation overhead", oneshot_thread_function);
	measure("Direct thread creation overhead", direct_thread_function);
	measure("Dynamic call handler x4 (inline)", inline_dyncall_handler);
	measure("Dynamic call handler x4 (call)", opaque_dyncall_handler);
	measure("Dynamic call args x4 (inline)", inline_dyncall_args_x4);
	measure("Dynamic call args x4 (call)", opaque_dyncall_args_x4);

	//benchmark_multiprocessing();
}

struct C
{
	std::string to_string() const
	{
		return std::string(&c, 1) + "++";
	}

  private:
	char c;
};

PUBLIC(void cpp_function(const char* a, const C& c, const char* b))
{
	/* Hello C++ World */
	print(a, " ", c.to_string(), " ", b, "\n");
}

/* Example game object logic */
#include <embedded_string.hpp>

struct GameObject
{
	EmbeddedString<32> name;
	bool alive;
};

PUBLIC(void myobject_death(GameObject& object))
{
	EXPECT(object.alive);
	print("Object '", object.name.to_string(), "' is dying!\n");
	/* SFX: Ugh... */
	object.alive = false;
}

PUBLIC(void test_dynamic_functions())
{
	// See: dynamic_calls.json
	// 1: A simple no-argument dynamic call
	sys_testing();
	// 2: A dynamic call that takes a zero-terminated string, and then a string with length
	sys_testing_strings("Hello World!", "Hello World!", 12);
	// 3: A dynamic call that takes a few integers and floats
	sys_testing123(4, 5, 6, 7.0, 8.0, 9.0);
}
