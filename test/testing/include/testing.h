#ifndef TESTING_H
#define TESTING_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum TestStatus {
    TEST_PASS,
    TEST_FAIL,
    TEST_ERROR,
    TEST_SKIP
} TestStatus;

void __TestAssertion_fail(const char *condition, const char *file, int line, const char *func);
#define TestAssertion(condition) \
          ((condition) ? (void)0 : __TestAssertion_fail(#condition, __FILE__, __LINE__, __func__))

typedef struct Mock {
    void *return_value;
    void (*side_effect)(void);
} Mock;

typedef struct TestCase {
    const char *name;
    TestStatus (*test_func)(void);
    void (*setup)(void);
    void (*teardown)(void);
} TestCase;

typedef struct TestResult {
    TestStatus status;
    const char *message;
    double duration;
} TestResult;

typedef struct TestReport {
    void (*add_result)(TestCase* test_case, TestResult* result);
    void (*generate_report)(void);
} TestReport;

Mock *create_mock(void);
void set_mock_return_value(Mock *mock, void *return_value);
void set_mock_side_effect(Mock *mock, void (*side_effect)(void));

void register_fixture(void (*setup)(void), void (*teardown)(void));
void parameterized_test(void (*test_func)(void *data), void **parameters, int num_parameters);

#define RUN_TESTS \
void run_tests(void);

#define PRINT_REPORT \
void print_report(TestReport *report);

#endif // TESTING_H
