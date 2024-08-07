#include "test_time_tracking.h"

#include "../source/time_tracking.h"

#include <assert.h>

static void testParseEmptyLine()
{
    TTRTimeTracking result = ttr_internal_parseLine("");

    assert((result.start == 0));
    assert((result.end == 0));
    assert((result.pauseCount == 0));
}

static void testParseStartLine()
{
    TTRTimeTracking result = ttr_internal_parseLine("1722871741,");

    assert((result.start == 1722871741));
    assert((result.end == 0));
    assert((result.pauseCount == 0));
}

static void testParseStartAndSinglePauseLine()
{
    TTRTimeTracking result = ttr_internal_parseLine("1722871741,p1722871728");

    assert((result.start == 1722871741));
    assert((result.end == 0));
    assert((result.pauseCount == 1));
    assert((result.pauses[0] == 1722871728));
}

static void testParseStartAndTwoPauseLine()
{
    TTRTimeTracking result = ttr_internal_parseLine("1722871741,p1722871728,p1722871728");

    assert((result.start == 1722871741));
    assert((result.end == 0));
    assert((result.pauseCount == 2));
    assert((result.pauses[0] == 1722871728));
    assert((result.pauses[1] == 1722871728));
}

static void testParseStartEndAndSinglePauseLine()
{
    TTRTimeTracking result = ttr_internal_parseLine("1722871741,p1722871728,1722871728");

    assert((result.start == 1722871741));
    assert((result.end == 1722871728));
    assert((result.pauseCount == 1));
    assert((result.pauses[0] == 1722871728));
}

static void testParseStopAtNewLine()
{
    TTRTimeTracking result = ttr_internal_parseLine("1722871741,p1722871728,\n1722871728");

    assert((result.start == 1722871741));
    assert((result.end == 0));
    assert((result.pauseCount == 1));
    assert((result.pauses[0] == 1722871728));
}

static void testParseMaxPauseTimer()
{
    TTRTimeTracking result = ttr_internal_parseLine("1722871741,p1722871728,p1722871728,p1722871728,p1722871728,p1722871728,p1722871728,");

    assert((result.start == 1722871741));
    assert((result.end == 0));
    assert((result.pauseCount == 6));
    assert((result.pauses[0] == 1722871728));
    assert((result.pauses[1] == 1722871728));
    assert((result.pauses[2] == 1722871728));
    assert((result.pauses[3] == 1722871728));
    assert((result.pauses[4] == 1722871728));
    assert((result.pauses[5] == 1722871728));
}

static void testParsePauseTimerOverflow()
{
    TTRTimeTracking result = ttr_internal_parseLine("1722871741,p1722871728,p1722871728,p1722871728,p1722871728,p1722871728,p1722871728,p1722871728,");

    assert((result.start == 1722871741));
    assert((result.end == 0));
    assert((result.pauseCount == 6));
    assert((result.pauses[0] == 1722871728));
    assert((result.pauses[1] == 1722871728));
    assert((result.pauses[2] == 1722871728));
    assert((result.pauses[3] == 1722871728));
    assert((result.pauses[4] == 1722871728));
    assert((result.pauses[5] == 1722871728));
}


void runTTRTests()
{
    testParseEmptyLine();
    testParseStartLine();
    testParseStartAndSinglePauseLine();
    testParseStartAndTwoPauseLine();
    testParseStartEndAndSinglePauseLine();
    testParseStopAtNewLine();
    testParseMaxPauseTimer();
    testParsePauseTimerOverflow();
}