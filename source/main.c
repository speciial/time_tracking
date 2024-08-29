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
    CONFIG = 6,
};
typedef enum possible_arg_index PossibleArgIndex;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("TTR expects at least 1 argument.\n");
        return -1;
    }

    CLOPArgOptions commandOptions[] = {
        {"start", START},
        {"end", END},
        {"start_pause", START_PAUSE},
        {"end_pause", END_PAUSE},
        {"show", SHOW},
        {"show_month", SHOW_MONTH},
        {"config", CONFIG},
    };

    PossibleArgIndex commandIndex = (PossibleArgIndex)clop_matchArgList(argv[1], commandOptions, 7);
    switch (commandIndex)
    {
    case START:
    {
        if (argc == 2)
        {
            ttr_start();
            printf("Started time tracking.\n");
        }
        else
        {
            printf("start doesn't take additional parameters.\n");
            return -1;
        }
    }
    break;
    case END:
    {
        if (argc == 2)
        {
            ttr_end();
            printf("Ended time tracking.\n");
        }
        else
        {
            printf("end doesn't take additional parameters.\n");
            return -1;
        }
    }
    break;
    case START_PAUSE:
    {
        if (argc == 2)
        {
            ttr_startPause();
            printf("Paused time tracking.\n");
        }
        else
        {
            printf("start_pause doesn't take additional parameters.\n");
            return -1;
        }
    }
    break;
    case END_PAUSE:
    {
        if (argc == 2)
        {
            ttr_endPause();
            printf("Unpaused time tracking.\n");
        }
        else
        {
            printf("end_pause doesn't take additional parameters.\n");
            return -1;
        }
    }
    break;
    case SHOW:
    {
        if (argc == 2)
        {
            ttr_show();
        }
        else
        {
            printf("show doesn't take additional parameters.\n");
            return -1;
        }
    }
    break;
    case SHOW_MONTH:
    {
        if (argc == 2)
        {
            ttr_showMonth(0);
        }
        else if (argc == 3)
        {
            int filterMonth = clop_parseIntArg(argv[2]);
            ttr_showMonth(filterMonth);
        }
        else
        {
            printf("show_month either takes 0 or 1 additional parameter.");
            return -1;
        }
    }
    break;
    case CONFIG:
    {
        if (argc == 4)
        {
            ttr_config(argv[2], argv[3]);
        }
        else 
        {
            printf("config takes 2 additional parameters.");
            return -1;
        }
    }
    break;

    default:
    {
        printf("Unknown command. Try help for a list of available commands.\n");
    }
    break;
    }

    return 0;
}