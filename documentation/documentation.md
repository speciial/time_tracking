# Documentation
I've been using this little utility for about half a year now and while using it is infinitely better than any kind of spreadsheet, there are many limitation to it. I would like to address all of them bit by bit and hopefully end up with a nice application that does all the time tracking I need. 

## Limitations 
There are lots of them. From the way I structured the program to missing features that are just hard to implement into the current system. Even just the file format that I chose to store the tracked times in is a limitation by itself. The following is a evolving list of topics I'll tackle over time to improve this application.

- File handling is just abusing the fact that I can append new line at the end
  - It means I can't edit anything in post. New times are just added.
- If anything bad happens during writing to the file, it's gone (which would be really bad)
- The print-outs are really complicated and if I want anything more fancy it's a lot of work
- The format isn't super hard to parse but can't support features I'd like to have
  - Sometimes I want to use the default break time as part of a pause but they are exclusive 
- When I'm in office, there is a delay between clocking in and starting the time tracking
  - This should be accounted for so my tracking and SAP are in sync

## Features
I'd like to get a little more specific about what kinds of features I want for this tool. Over time, I'll add more to this because I have some crazy ideas on how to make this a really cool application.

1. Starting and stopping time tracking 
   1. Start and stop "right now"
   2. Start and stop at a specific time / time offset, 7:30 or 10 minutes ago? 
2. Pausing and unpausing time tracking 
   1. If applicable, use "default pause time" for pausing
3. Automatically consider "default pause time"
4. Show current time tracking
   1. Start, total time, net time (total - pauses)
5. Show list of trackings for a month
   1. Start, end, total time, net time
   2. Adjust end time for additional pauses 
   3. Show overtime 
6. Edit times in post
7. Have some more printing options 
   1. Mail subject line
8. Make certain things configurable
   1. Default pause time 
   2. Print string templates?

## Requirements
These aren't hard requirements but I'd like to set some standard for this application to meet.

- More or less human-readable file format with proper parser
  - I'd like to be able to edit / restore the file if the program screws it up 
- Testing for most of the important code
- Proper cmake setup 
  - Install the app under linux 