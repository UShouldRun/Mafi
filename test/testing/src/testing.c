#include "testing.h"

void __TestAssertion_fail(
  const char *condition,
  const char *file,
  int line,
  const char *func
) {
  fprintf(
    stderr,
    "[FAIL]: In File %s -> Line %d -> Function %s -> Assertion failed: (%s)\n",
    file, line, func, condition
  );
}

Mock *create_mock(void) {
    Mock *mock = (Mock *)malloc(sizeof(Mock));
    if (!mock) return NULL;
    mock->return_value = NULL;
    mock->side_effect = NULL;
    return mock;
}

void set_mock_return_value(Mock *mock, void *value) {
  mock->return_value = value;
}

void set_mock_side_effect(Mock *mock, void (*side_effect)(void)) {
  mock->side_effect = side_effect;
}

void register_fixture(void (*setup)(void), void (*teardown)(void)) {}
void parameterized_test(void (*test_func)(void *data), void **parameters, int num_parameters) {}

void run_tests(void) {}
void print_report(TestReport* report) {}
