# Time Tracking 
A command line tool for tracking work hours. 

# How to use the tool
The following `ttr` commands are available:

| Implemented | Command                   | Description                                                |
| ----------- | ------------------------- | ---------------------------------------------------------- |
| No          | start [-t <start_time>]   | Starts time tracking                                       |
| No          | stop [-t <start_time>]    | Ends time tracking                                         |
| No          | restart [-t <start_time>] | Restarts time tracking in case workday was already stopped |
| No          | pause                     | Starts pause in active time tracking                       |
| No          | unpause                   | Starts pause in active time tracking                       |
| No          | show [-m <month>]         | Prints current time tracking                               |
| No          | config                    | Configure settings                                         |


# How to build the tool

# Roadmap
There are lots of things I'd like to implement here. Some of the most important bits are documented [here](documentation/motivation.md). Others will be added here over time.

- [ ] Feature Parity with the old implementation
  - [ ] Reading record files
  - [ ] Writing record files
  - [ ] Commands
  - [ ] Configuration
- [ ] Testing
- [ ] Smarter default pauses
- [ ] Better output formatting
- [ ] Automated test pipeline on GitHub