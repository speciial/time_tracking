#ifndef CMD_LINE_OPT_H
#define CMD_LINE_OPT_H

struct clop_arg_options
{
    const char *name;
    const int index;
};
typedef struct clop_arg_options CLOPArgOptions;

int clop_matchArgList(const char *arg, CLOPArgOptions *argOptions, int argCount);

int clop_matchArg(const char *arg, CLOPArgOptions argOption);

#endif // CMD_LINE_OPT_H