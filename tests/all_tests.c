/** This file is stubbed out by pdantler but not managed by it.

    Change it as much as you would like.
*/

#include "munit.h"

static MunitResult test_placeholder(const MunitParameter params[],
                                   void *fixture) {
    return MUNIT_OK;
}

MunitTest main_tests[] = {
    {
        "/test-placeholder", /* name */
        test_placeholder,    /* test */
        NULL,                   /* setup */
        NULL,                      /* tear_down */
        MUNIT_TEST_OPTION_NONE,                /* options */
        NULL                                   /* parameters */
    },
/* Mark the end of the array with an entry where the test
     * function is NULL */
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

const MunitSuite main_suite = {
    "/main-tests",        /* name */
    main_tests,           /* tests */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};
				  

int main(int argc, char *argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
  MunitSuite all_suites[] = {main_suite};

  const MunitSuite all_tests = {
      "/all-tests",           /* name */
      NULL,                   /* tests */
      all_suites,             /* suites */
      1,                      /* iterations */
      MUNIT_SUITE_OPTION_NONE /* options */
  };

  return munit_suite_main(&all_tests, NULL, argc, argv);
}