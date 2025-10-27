#include "base_testing.hpp"

#include <stdio.h>
#include <time.h>

namespace base::testing
{
#if 0
test_setup *SetupTest(arena *Arena, char *TestName)
{
    test_setup *Result = (test_setup *)ArenaAlloc(Arena, sizeof(test_setup), 1);
    Result->TestSuitName = TestName;
    return Result;
}

void StartTestTimer(test_setup *TestSetup)
{
    TestSetup->StartTime = clock();
}

void EndTestTimer(test_setup *TestSetup)
{
    TestSetup->EndTime = clock();
}

void RunTest(test_setup *TestSetup, arena *Arena, void (*Function)(test_setup *, arena *), char *FunctionName)
{
    TestSetup->CurrentTestName = FunctionName;
    printf("Running %s\n", FunctionName);
    (*Function)(TestSetup, Arena);

    if (TestSetup->CurrentTestFailed)
    {
        printf("%s FAILED\n", FunctionName);
    }
    else
    {
        printf("%s SUCCEEDED\n", FunctionName);
    }

    TestSetup->CurrentTestFailed = false;
    TestSetup->CurrentTestName = "";
}

void PrintTestResult(test_setup *TestSetup)
{
    double TimeSpent = (double)(TestSetup->EndTime - TestSetup->StartTime) / CLOCKS_PER_SEC;
    printf("%s took %fs\n", TestSetup->TestSuitName, TimeSpent);

    if (TestSetup->FailedTestCount == 0)
    {
        printf("All tests in %s succeeded\n", TestSetup->TestSuitName);
    }
    else
    {
        printf("The following %d tests in %s failed: \n", TestSetup->FailedTestCount, TestSetup->TestSuitName);
        for (int FailedTests = 0; FailedTests < TestSetup->FailedTestCount; ++FailedTests)
        {
            printf("\t%s\n", TestSetup->FailedTests[FailedTests].TestName);
        }
    }
}
#endif
} // namespace base
