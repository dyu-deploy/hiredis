#ifndef _TEST_HELPER_H
#define _TEST_HELPER_H 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <errno.h>

/* gai_strerror */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/* local */
#include "../handle.h"

/******************************************************************************/
/* ASSERTS ********************************************************************/
/******************************************************************************/

#define assert_equal(a, b, type, fmt) do {       \
    type a_ = (a);                               \
    type b_ = (b);                               \
    if (a_ != b_) {                              \
        fprintf(stderr,                          \
            "%s:%d: " fmt " != " fmt "\n",       \
            __FILE__, __LINE__, a_, b_);         \
        assert(0);                               \
    }                                            \
} while(0)

#define assert_equal_int(a,b) do {               \
    assert_equal(a, b, int, "%d");               \
} while(0)

#define assert_equal_size_t(a,b) do {            \
    assert_equal(a, b, size_t, "%lu");           \
} while(0)

/* Use long long to avoid compiler warnings about the printf format. */
#define assert_equal_int64_t(a,b) do {           \
    assert_equal(a, b, long long, "%lld");       \
} while(0)

#define assert_equal_double(a,b) do {            \
    assert_equal(a, b, double, "%f");            \
} while(0)

#define assert_equal_string(a,b) do {            \
    if (strcmp(a, b)) {                          \
        fprintf(stderr,                          \
            "%s:%d: %s != %s\n",                 \
            __FILE__, __LINE__, a, b);           \
        assert(0);                               \
    }                                            \
} while(0)

#define ERR_TO_STR_OFFSET 4
static const char *err_to_str[] = {
    [REDIS_OK      + ERR_TO_STR_OFFSET] = "REDIS_OK",
    [REDIS_ESYS    + ERR_TO_STR_OFFSET] = "REDIS_ESYS",
    [REDIS_EGAI    + ERR_TO_STR_OFFSET] = "REDIS_EGAI",
    [REDIS_EPARSER + ERR_TO_STR_OFFSET] = "REDIS_EPARSER",
    [REDIS_EEOF    + ERR_TO_STR_OFFSET] = "REDIS_EEOF"
};

static void assert_equal_return_failure(int actual, int expected, const char *file, int line) {
    const char *actualstr = err_to_str[actual + ERR_TO_STR_OFFSET];
    const char *expectedstr = err_to_str[expected + ERR_TO_STR_OFFSET];
    char reason[128];

    switch (actual) {
    case REDIS_ESYS:
        sprintf(reason, "%d:%s", errno, strerror(errno));
        break;
    case REDIS_EGAI:
        sprintf(reason, "%d:%s", errno, gai_strerror(errno));
        break;
    }

    fprintf(stderr, "%s:%d: ", file, line);
    fprintf(stderr, "%s != %s (%s)\n", actualstr, expectedstr, reason);
}

#define assert_equal_return(a, b) do {           \
    if ((a) != (b)) {                            \
        assert_equal_return_failure(             \
            (a), (b),                            \
            __FILE__, __LINE__);                 \
        assert(0);                               \
    }                                            \
} while(0)

/******************************************************************************/
/* TEST DEFINITION ************************************************************/
/******************************************************************************/

static const char *current_test = NULL;

#define TEST(name)                                                             \
static void test__##name(void);                                                \
static void test_##name(void) {                                                \
    current_test = #name;                                                      \
    test__##name();                                                            \
    printf("%s: PASSED\n", current_test);                                      \
}                                                                              \
static void test__##name(void)

/******************************************************************************/
/* TIMING *********************************************************************/
/******************************************************************************/

static long long usec(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

/******************************************************************************/
/* MISC ***********************************************************************/
/******************************************************************************/

#define REDIS_DEFAULT_PORT 6379

static int redis_port(void) {
    char *env, *eptr;
    long port = REDIS_DEFAULT_PORT;

    env = getenv("REDIS_PORT");
    if (env != NULL) {
        port = strtol(env, &eptr, 10);

        /* Reset to default when the var contains garbage. */
        if (*eptr != '\0') {
            port = REDIS_DEFAULT_PORT;
        }
    }
    return port;
}

#endif
