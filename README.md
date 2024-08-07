# Time Tracking 

This is a simple command line tool for keeping track of my work times. The tool essentially just stores a list time stamps in a file and allows me to print them out to the console in a human-readable format.

The code for the time tracking itself looks awful right now since I just wanted to establish the basic features and the way I manage the file contents. This will hopefully change over time. There also is no customizability for this tool. It pretty much only works for my use case.

The code for the command line parsing is kind of nice. But I haven't put much thought into it. At some point I might make it a library that I can reuse for other tools.

There is also still some more stuff to work on/add to it. The following is a (non-exhaustive) list of those things.

# TODO

- [ ] Put this on GitHub
- [ ] Add build and usage instructions
- [ ] show_month currently just displays all entries in the record.ttr file. Should I split the files into months? Or do some sort of filtering and just keep one big file?
- [ ] There is way too much code duplication (e.g. opening file and finding pointer to the beginning of last line) 
- [ ] show_month just uses 45 minutes of default pause timing on a day. Should this be customizable? This is very specific to my use case.
- [ ] There is no way of adding times in post (in case I forget to start the timer)
- [ ] There are lots of early returns which cause memory leaks because stuff isn't freed. Might be a good time to finaly look into memory arenas
- [ ] The error handling is super scuffed
- [ ] There are some basics tests for file parsing but thats it 

# How to use the tool

# How to build the tool