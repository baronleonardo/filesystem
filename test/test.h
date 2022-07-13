#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include <stdalign.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <malloc.h>
#endif // _WIN32

#ifndef o_fprintf
#include <stdio.h>
#define o_fprintf fprintf
#endif // o_fprintf

#ifndef o_printf
#include <stdio.h>
#define o_printf printf
#endif // o_printf

#ifndef USE_NO_COLOR
#define __TEST_COLOR_CHECK_FAILED__     "\x1b[33m"
#define __TEST_COLOR_REQUIRE_FAILED__   "\x1b[31m"
#define __TEST_COLOR_TITLES__           "\x1b[33m"
#define __TEST_COLOR_PASSED__           "\x1b[32m"
#define __TEST_COLOR_FAILED__           "\x1b[31m"
#define __TEST_COLOR_RESET__            "\x1b[0m"
#else
#define __TEST_COLOR_CHECK_FAILED__
#define __TEST_COLOR_REQUIRE_FAILED__
#define __TEST_COLOR_TITLES__
#define __TEST_COLOR_PASSED__
#define __TEST_COLOR_FAILED__
#define __TEST_COLOR_RESET__
#endif // USE_NO_COLOR

typedef void (*test_case_handler)();

typedef struct {
    const char* name;
    test_case_handler handler;
} Test_Case;

typedef struct {
    uint32_t max_test_cases_num;
    uint32_t test_cases_num;
    Test_Case* test_cases;
    uint32_t current_test_case_index;
    const char* targeted_test_case_name;
    uint32_t checks_positive;
    uint32_t checks_negative;
    uint32_t requires_positive;
    uint32_t requires_negative;
} Unit_Test;

typedef enum { TEST_TYPE_CHECK, TEST_TYPE_REQUIRE } Test_Check_Type;

Unit_Test* __unit_test__ = NULL;

static void __test_on_fatal_error__(Unit_Test* self, int signal_number);
static void __test_post_run__(Unit_Test* self);

void
__test_on_singal__(int signal_num)
{
    if(__unit_test__)
    {
        signal(signal_num, SIG_IGN);
        __unit_test__->requires_negative++;
        __test_on_fatal_error__(__unit_test__, signal_num);

        // __signal_status__ = signal_num;
        // printf("%d\n", errno);
        // exit(0);
        // puts("opaaaaaaaaaaa");
        // abort();
    }
    else
    {
        signal(signal_num, SIG_DFL);
    }
}

static Unit_Test
test_init(size_t max_test_cases, int argc, const char** argv)
{
    Unit_Test unit_test = {
        .max_test_cases_num = max_test_cases,
#ifdef _WIN32
        .test_cases = _aligned_malloc(alignof(Test_Case), sizeof(Test_Case) * max_test_cases)
#else
        .test_cases = aligned_alloc(alignof(Test_Case), sizeof(Test_Case) * max_test_cases)
#endif // _WIN32
        // .test_cases = malloc(sizeof(Test_Case) * max_test_cases)
    };

    if(argc > 1)
    {
        unit_test.targeted_test_case_name = argv[1];
    }

    signal(SIGILL, __test_on_singal__);
    signal(SIGABRT, __test_on_singal__);
    signal(SIGFPE, __test_on_singal__);
    signal(SIGSEGV, __test_on_singal__);
    signal(SIGTERM, __test_on_singal__);

    return unit_test;
}

static void
__test_deinit__(Unit_Test* self)
{
    free(self->test_cases);
    self->test_cases = NULL;
}

#define test_register(unit_test, handler) \
    void handler(Unit_Test* test); \
    _Generic((unit_test), \
    Unit_Test*: _Generic((handler), test_case_handler: __test_register__))(unit_test, handler, #handler)

static void
__test_register__(Unit_Test* self, test_case_handler handler, const char* name)
{
    Test_Case test_case = {
        .name = name,
        .handler = handler
    };

    if(self->test_cases_num == self->max_test_cases_num)
    {
        o_fprintf(stderr, "You can not add more test cases, you already have %d\n", self->test_cases_num); 
        return;
    }

    self->test_cases[self->test_cases_num] = test_case;
    self->test_cases_num++;
}

void __test_on_fatal_error__(Unit_Test* self, int sig_number)
{
    const char* sig_names[] = {
        NULL,
        NULL,
        NULL,
        NULL,
        "SIGILL",
        NULL,
        "SIGABRT",
        NULL,
        "SIGFPE",
        NULL,
        NULL,
        "SIGSEGV",
        NULL,
        NULL,
        NULL,
        "SIGTERM"
    };

    o_fprintf(stderr,
        __TEST_COLOR_FAILED__
        "FATAL ERROR: signal %s: Test case `%s` failed.\n"
        __TEST_COLOR_RESET__,
        sig_names[sig_number],
        self->test_cases[self->current_test_case_index].name
    );

    __test_post_run__(self);
    exit(EXIT_FAILURE);
}

void
__test_post_run__(Unit_Test* self)
{
    o_printf("\n"
             "=======================\n"
             __TEST_COLOR_TITLES__
             "Test cases:\n"
             __TEST_COLOR_PASSED__
             "\tpassed: %d\n"
             __TEST_COLOR_FAILED__
             "\tfailed: %d\n"
             __TEST_COLOR_TITLES__
             "Checks:\n"
             __TEST_COLOR_PASSED__
             "\tpassed: %d\n"
             __TEST_COLOR_FAILED__
             "\tfailed: %d\n"
             __TEST_COLOR_RESET__,
             self->requires_positive,
             self->requires_negative,
             self->checks_positive,
             self->checks_negative);

    __test_deinit__(self);
}

static void
test_run(Unit_Test* self)
{
    __unit_test__ = self;

    if(self->targeted_test_case_name)
    {
        for(uint32_t iii = 0; iii < self->test_cases_num; iii++)
        {
            if(strcmp(self->targeted_test_case_name, self->test_cases[iii].name) == 0)
            {
                self->current_test_case_index = iii;
                self->test_cases[iii].handler(self);
            }
        }
    }
    else
    {
        for(uint32_t iii = 0; iii < self->test_cases_num; iii++)
        {
            self->current_test_case_index = iii;
            self->test_cases[iii].handler(self);
        }
    }

    __test_post_run__(self);
    exit(EXIT_SUCCESS);
}

static bool
__test_check__(Unit_Test* self,
               Test_Check_Type check_type,
               bool expr,
               const char* expr_str,
               const char* file,
               uint32_t line)
{
    if(check_type == TEST_TYPE_CHECK)
    {
        self->requires_positive++;

        if(!expr)
        {
            fprintf(stderr,
                __TEST_COLOR_CHECK_FAILED__
                "-- %s:%d\n\t\t%s\n\n"
                __TEST_COLOR_RESET__,
                file, line, expr_str);

            self->checks_negative++;
        }
        else
        {
            self->checks_positive++;
        }
    }
    else if(check_type == TEST_TYPE_REQUIRE)
    {
        if(!expr)
        {
            fprintf(stderr,
                __TEST_COLOR_REQUIRE_FAILED__
                "-- %s:%d\n\t\t%s\n\n"
                __TEST_COLOR_RESET__,
                file, line, expr_str);

            self->requires_negative++;
            return false;
        }
        else
        {
            self->requires_positive++;
        }
    }

    return true;
}

#define TEST_CHECK(expr) \
    if(!__test_check__(self, TEST_TYPE_CHECK, expr, #expr, __FILE__, __LINE__)) return;
#define TEST_CHECK_MESSAGE(expr, msg) \
    if(!__test_check__(self, TEST_TYPE_CHECK, expr, msg, __FILE__, __LINE__)) return;
#define TEST_REQUIRE(expr)  \
    if(!__test_check__(self, TEST_TYPE_REQUIRE, expr, #expr, __FILE__, __LINE__)) return;
#define TEST_REQUIRE_MESSAGE(expr, msg) \
    if(!__test_check__(self, TEST_TYPE_REQUIRE, expr, msg, __FILE__, __LINE__)) return;

#undef __TEST_COLOR_CHECK_FAILED__
#undef __TEST_COLOR_REQUIRE_FAILED__
#undef __TEST_COLOR_TITLES__
#undef __TEST_COLOR_PASSED__
#undef __TEST_COLOR_FAILED__
#undef __TEST_COLOR_RESET__

#endif // TEST_H