/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

// This minimal Azure Sphere app repeatedly toggles GPIO 8, which is the red channel of RGB
// LED 1 on the MT3620 RDB. Use this app to test that device and SDK installation succeeded
// that you can build, deploy, and debug a CMake app with Visual Studio.
//
// It uses the API for the following Azure Sphere application libraries:
// - gpio (digital input for button)
// - log (messages shown in Visual Studio's Device Output window during debugging)

#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <applibs/log.h>
#include <applibs/gpio.h>
#include <applibs/networking.h>

// By default, this sample targets hardware that follows the MT3620 Reference
// Development Board (RDB) specification, such as the MT3620 Dev Kit from
// Seeed Studio.
//
// To target different hardware, you'll need to update CMakeLists.txt. See
// https://github.com/Azure/azure-sphere-samples/tree/master/Hardware for more details.
//
// This #include imports the sample_hardware abstraction from that hardware definition.
#include <hw/sample_hardware.h>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/compile.h>
#include <mruby/dump.h>
#include <mruby/proc.h>
#include <mruby/variable.h>

int run_mruby_code(int argc, char **argv, const uint8_t *code, const char *cmdline)
{
	mrb_state *mrb;
	struct RProc *n;
	struct mrb_irep *irep;
	mrb_value ARGV;
	mrbc_context *c;
	mrb_value v;
	mrb_sym zero_sym;
	int result = 0;

	/* mrubyの初期化 */
	mrb = mrb_open();
	if (mrb == NULL)
		return -1;

	int ai = mrb_gc_arena_save(mrb);
	ARGV = mrb_ary_new_capa(mrb, argc);
	for (int i = 0; i < argc; i++) {
		mrb_ary_push(mrb, ARGV, mrb_str_new_cstr(mrb, argv[i]));
	}
	mrb_define_global_const(mrb, "ARGV", ARGV);

	c = mrbc_context_new(mrb);
	c->dump_result = TRUE;

	/* Set $0 */
	zero_sym = mrb_intern_lit(mrb, "$0");
	mrbc_filename(mrb, c, cmdline);
	mrb_gv_set(mrb, zero_sym, mrb_str_new_cstr(mrb, cmdline));

	irep = mrb_read_irep(mrb, code);
	n = mrb_proc_new(mrb, irep);
	v = mrb_run(mrb, n, mrb_nil_value());

	mrb_gc_arena_restore(mrb, ai);
	mrbc_context_free(mrb, c);
	if (mrb->exc) {
		if (!mrb_undef_p(v)) {
			mrb_print_error(mrb);
		}
		result = -1;
	}

	mrb_close(mrb);
	return result;
}

/// <summary>
/// Exit codes for this application. These are used for the
/// application exit code. They must all be between zero and 255,
/// where zero is reserved for successful termination.
/// </summary>
typedef enum {
    ExitCode_Success = 0,

    ExitCode_Main_Args = 1,
    ExitCode_Main_ScopeId,
	ExitCode_Main_Ruby
} ExitCode;

extern char* scopeId;
extern const uint8_t client_code[];

mrb_value mrb_kernel_sleep(mrb_state *mrb, mrb_value self)
{
	mrb_float interval;
	const struct timespec sleepTime = {.tv_sec = 1, .tv_nsec = 0};

	mrb_get_args(mrb, "f", &interval);

	nanosleep(&sleepTime, NULL);

	return mrb_nil_value();
}

void mrb_mruby_others_gem_init(mrb_state *mrb)
{
	mrb_define_method(mrb, mrb->kernel_module, "sleep", mrb_kernel_sleep, MRB_ARGS_REQ(1));
}

void mrb_mruby_others_gem_final(mrb_state *mrb)
{
}

int main(int argc, char *argv[])
{
    char *ruby_argv[] = {
        ""
    };

    Log_Debug("Azure IoT Application starting.\n");

    bool isNetworkingReady = false;
    if ((Networking_IsNetworkingReady(&isNetworkingReady) == -1) || !isNetworkingReady) {
        Log_Debug("WARNING: Network is not ready. Device cannot connect until network is ready.\n");
    }

    if (argc > 1) {
        scopeId = argv[1];
        Log_Debug("Using Azure IoT DPS Scope ID %s\n", scopeId);
    } else {
        Log_Debug("ScopeId needs to be set in the app_manifest CmdArgs\n");
        return ExitCode_Main_Args;
    }

    if (scopeId == NULL) {
        Log_Debug("Not set scope id.\n");
        return ExitCode_Main_ScopeId;
    }

    if (run_mruby_code(0, ruby_argv, client_code, "iothub") != 0) 
		return ExitCode_Main_Ruby;

	return ExitCode_Success;
}