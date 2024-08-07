#include <stdio.h>

#include "cmd_line_opt.h"
#include "time_tracking.h"

enum possible_arg_index
{
    START = 0,
    END = 1,
    START_PAUSE = 2,
    END_PAUSE = 3,
    SHOW = 4,
    SHOW_MONTH = 5,
    CLEAR = 6
};
typedef enum possible_arg_index PossibleArgIndex;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Wrong number of arguments.\n");
        return -1;
    }

    CLOPArgOptions commandOptions[] = {
        {"start", START},
        {"end", END},
        {"start_pause", START_PAUSE},
        {"end_pause", END_PAUSE},
        {"show", SHOW},
        {"show_month", SHOW_MONTH},
        {"clear", CLEAR},
    };

    PossibleArgIndex commandIndex = clop_matchArgList(argv[1], commandOptions, 7);
    switch (commandIndex)
    {
    case START:
    {
        printf("START\n");
        ttr_start();
    }
    break;
    case END:
    {
        printf("END\n");
        ttr_end();
    }
    break;
    case START_PAUSE:
    {
        printf("START_PAUSE\n");
        ttr_startPause();
    }
    break;
    case END_PAUSE:
    {
        printf("END_PAUSE\n");
        ttr_endPause();
    }
    break;
    case SHOW:
    {
        printf("SHOW\n");
        ttr_show();
    }
    break;
    case SHOW_MONTH:
    {
        printf("SHOW_MONTH\n");
        ttr_showMonth();
    }
    break;

    default:
    {
        printf("UNKNOWN\n");
    }
    break;
    }
}