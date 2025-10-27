#ifndef base_testing_HPP
#define base_testing_HPP

namespace base::testing
{

#if 0

// NOTE(speciial): Up this number based on max test cases in a single test suit.
#define MAX_FAILED_TEST_COUNT 20

struct failed_test
{
    char *TestName;
};

struct test_setup
{
    char *TestSuitName;
    char *CurrentTestName;
    bool CurrentTestFailed;

    clock_t StartTime;
    clock_t EndTime;

    int FailedTestCount;
    failed_test FailedTests[MAX_FAILED_TEST_COUNT];
};

#define TEST_CASE(NAME)     void NAME(test_setup *TestSetup, arena *Arena)
#define RUN_TEST_CASE(NAME) RunTest(Setup, &Arena, &NAME, #NAME)

#define ASSERT(Expression) if (!(Expression)) { \
    TestSetup->CurrentTestFailed = true; \
    if (TestSetup->FailedTestCount < MAX_FAILED_TEST_COUNT) { \
        TestSetup->FailedTests[TestSetup->FailedTestCount++].TestName = TestSetup->CurrentTestName; \
    } \
}

test_setup *SetupTest(arena *Arena, char *TestName);

void StartTestTimer(test_setup *TestSetup);

void EndTestTimer(test_setup *TestSetup);

void RunTest(test_setup *TestSetup, arena *Arena, void (*Function)(test_setup *, arena *), char *FunctionName);

void PrintTestResult(test_setup *TestSetup);

#endif

} // namespace base

#endif // base_testing_HPP