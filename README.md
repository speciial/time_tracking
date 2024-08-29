# Time Tracking 

This is a simple command line tool for keeping track of my work times. The tool essentially just stores a list time stamps in a file and allows me to print them out to the console in a human-readable format.

The code for the time tracking itself looks awful right now since I just wanted to establish the basic features and the way I manage the file contents. This will hopefully change over time. There also is no customizability for this tool. It pretty much only works for my use case.

The code for the command line parsing is kind of nice. But I haven't put much thought into it. At some point I might make it a library that I can reuse for other tools.

There is also still some more stuff to work on/add to it. The following is a (non-exhaustive) list of those things.

# TODO

- [x] Put this on GitHub
- [ ] Add build and usage instructions
- [ ] Add more compiler warnings 
- [x] show_month improvements
  - [x] Add filtering for specific months (like an additional paramter)
  - [x] Add generic setting for how long the default break is (in my case 45 mins)
  - [x] Add generic setting for how to display the time (hh:mm or decimal)
- [x] config command for settings
- [ ] mail command for printing the email subject
- [ ] help command for options.
- [x] There is way too much code duplication (e.g. opening file and finding pointer to the beginning of last line) 
- [ ] There is no way of adding times in post (in case I forget to start the timer)
- [ ] Can we just memory map the record and settings files? I don't really need them to be human readable if I can edit contents in post
- [x] There are lots of early returns which cause memory leaks because stuff isn't freed
- [ ] Might be a good time to finaly look into memory arenas/string reprensentation
- [ ] Try and remove as much standard library stuff as possible
- [x] The error handling is super scuffed (NOTE: I don't really do error handling anymore. The systems just works if you get back a zero value.)
- [x] On first start no file is created. The utility only works if a record file already exists
- [x] Split out file api
- [ ] There are some basics tests for file parsing but thats it 
- [ ] Add ctest or similar for testing. More important now since the setup with the test directory broke the build.
- [ ] Documenation?

# How to use the tool

# How to build the tool