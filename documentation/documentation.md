# Documentation
This is a high-level documentation mostly for personal note taking on my end. I wanted to have a place where I can write down my thoughts on how I'd like to structure the code and solve certain problem.

## Format
As written in the [Motivation](motivation.md), the format for tracking times is a major piece of the program and the old implementation put a lot of restrictions on what I could do. While I could technically still use the old format and simply rewrite the parser for it, I've been meaning to take a look into more traditional parsing. A slightly more nuanced format would let me explore that in more detail while also having the added benifit of actually being human-readable. 

The format I ended up with looks like this:

```
[Day   ] [Tracked times                                 ]
           [Start/End  ] [Pauses                       ]
                           [Start/End  ] [Start/End  ]
25/03/23 { (07:52,16:52) [ (08:45,09:00) (11:30,12:00) ]};
```

It uses a little more punctuation than is necessary, but I'm happy with it. Since it's mostly going to be used by the time tracking tool (and human-readability is only a secondary requirement) I've stripped any kind of identifier (such as "pauses", "start" or "end" ) to not make it too verbose. This also made it look a lot less like JSON, so writing a parser for it wasn't that much of a wasted effort. :D

### Parsing
When writing the parser, I took lots of "inspiration" from the [Handmade Hero episode on introspection](https://www.youtube.com/watch?v=1IwYEJsvdcs&t=4976s). Other videos that helped me a lot were [Jon Blow's video on Programming Language Parsers](https://www.youtube.com/watch?v=MnctEW1oL-E) and [VoxelRifts video on making C easy](https://www.youtube.com/watch?v=3IAlJSIjvH0).

What I ended up with is a (not really) recursive descent parser that breaks each record down to it's basic components. I fill a predefined structure (ttr_record_list) with it and return it as the final result. I now also load the entire record file into memory and work on that, which makes certain operations (such as editing times in post) possible. 

#### Error handling
Error Handling was more about me playing around with parsing. Since the files should really only be written by the tool itself, there isn't much need for error handling but I wanted to see what I could do with the new parser. I didn't really have any idea on how to do good error handling and reporting but I'm happy with what I came up with. 

What I'm essentially doing, is testing for a required token or range of required tokens. If I encounter a token I didn't expect, I forward the tokenizer to the end of the file and fall through the chain of parsing. I also print out the location of the error that occured during parsing. 

### Writing
Writing the file back to disc is about as simple as it gets. I just iterate through all records and write them out using a formatted string.