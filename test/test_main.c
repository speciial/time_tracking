#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <assert.h>

#include <arena.h>
#include <time_tracking.h>

void TestReadRecordFile()
{
    const char *RecordFileName = "test_record.ttr";

    arena Arena = NewArena(KB(30));
    char *ArenaStart = Arena.Begin;

    ttr_record_list *Records = ReadRecordFile(&Arena, RecordFileName);

    int AllocatedMemory = (int)(Arena.Begin - ArenaStart);
    printf("Allocated Arena Space: %dkb\n", AllocatedMemory / 1024);

    while (Records)
    {
        printf("Year: %d, Month: %d\n", Records->Year, Records->Month);

        for (int RecordIndex = 0; RecordIndex < DAYS_PER_MONTH; ++RecordIndex)
        {
            ttr_record CurrentRecord = Records->Records[RecordIndex];
            if (CurrentRecord.Used)
            {
                printf("\tStart: %d, End: %d - [", CurrentRecord.Work.Start, CurrentRecord.Work.End);
                for (int PauseIndex = 0; PauseIndex < CurrentRecord.PauseCount; ++PauseIndex)
                {
                    printf("Start: %d, End: %d; ", CurrentRecord.Pauses[PauseIndex].Start, CurrentRecord.Pauses[PauseIndex].End);
                }
                printf("]\n");
            }
        }

        Records = Records->NextMonth;
    }

}

int main()
{
    printf("Time Tracking Test\n");

    TestReadRecordFile();

    {
        time_t Now = time(0);
        printf("Now:   %jd\n", Now);

        struct tm NowTm = *localtime(&Now);
        printf("NowTm: %s\n", asctime(&NowTm));

        NowTm.tm_mon -= 1;
        NowTm.tm_mday = 20;
        printf("NowTm: %s\n", asctime(&NowTm));

        time_t ConvertedNow = mktime(&NowTm);
        printf("Now:   %jd\n", ConvertedNow);


        struct tm ConstructedTm = { 0 };
        ConstructedTm.tm_year = 25 + (2000 - 1900);
        ConstructedTm.tm_mon = (3 - 1);
        ConstructedTm.tm_mday = 25;
        ConstructedTm.tm_hour = 16;
        ConstructedTm.tm_min = 40;
        time_t ConstructedTime = mktime(&ConstructedTm);

        printf("ConstructedTm:   %s\n", asctime(&ConstructedTm));
        printf("ConstructedTime: %jd\n", ConstructedTime);
    }

    return 0;
}