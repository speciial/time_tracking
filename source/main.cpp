#include <stdio.h>

#include <base_arena.hpp>
#include <base_string.hpp>

#include <ttr.hpp>

int main(int argc, char **argv)
{
    printf("Time Tracking\n");

    if (argc > 1)
    {
        const char *command = argv[1];

        base::arena arena;
        ttr::record_list recordList;

        if (base::string_equals(command, "start"))
        {
            printf("Start case\n");
            if (argc == 2)
            {
                ttr::return_code result = ttr::start_now(&arena, &recordList);
            }
            else
            {
                // TODO(speciial): parse command line arguments to start at a specific time
            }
        }
        else if (base::string_equals(command, "end"))
        {
            printf("End case\n");
            if (argc == 2)
            {
                ttr::return_code result = ttr::end_now(&recordList);
            }
            else
            {
                // TODO(speciial): parse command line arguments to start at a specific time
            }
        }
        else if (base::string_equals(command, "pause"))
        {
            printf("Pause case\n");
            if (argc == 2)
            {
                ttr::return_code result = ttr::pause_now(&recordList);
            }
        }
        else if (base::string_equals(command, "unpause"))
        {
            printf("Unpause case\n");
            if (argc == 2)
            {
                ttr::return_code result = ttr::unpause_now(&recordList);
            }
        }
        else if (base::string_equals(command, "show"))
        {
            printf("Show case\n");
            if (argc == 2)
            {
                base::string output = ttr::show_now(&arena, &recordList);
            }
        }
        else
        {
            printf("Else case\n");
        }
    }
    else
    {
        // TODO(speciial): Log program usage
    }

    return 0;
}