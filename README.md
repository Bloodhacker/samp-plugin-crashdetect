[CrashDetect][github] 
=====================

[![Donate][donate_button]][donate]

This plugin helps you debug runtime errors and server crashes. When something
goes wrong you see a more or less detailed error message that contains description
of the error, the stack trace of the offending script and additionally the system
call stack in case of a server crash. This can greatly reduce th amount of time
spent spotting the erroneous code and fixing it.

See the [original][forum] topic on the SA-MP Forums for more information and
examples.

Download
--------

Get latest binaries for Windows and Linux [here][download].

FAQ
---

### How do I get function names, line numbers, etc in stack trace?

You have to tell the Pawn compiler to include debugging information in the
resulting AMX file. This can be accomplished by passing either `-d2` or `-d3`
flag to it at the command line. The easiest way is:

* Create a file called `pawn.cfg` inside your `pawno` directory if it
  does not already exist

* Open it in your favourite text editor and insert `-d3` (preceding with a space
  it the file was not empty)

This file is basically a list of flags that will be passed to the compiler
whenever you hit F5 in Pawno. If you use someting other than Pawno you might
need to put that file in the same directory as the script you compile lives
in because most editors don't set current working directory to `pawno`.
It might be easier to add the exta flags via your editor's options.

### Still doesn't work! Why??

If you put the AMX file in some custom directory other than `gamemodes` or
`filterscripts` or in ia subdirectory of these CrashDetect will not be able to
find it when loading debug info. In order to fix this you have to specify the
path manually via the `AMX_PATH` environment variable which is a
semicolon-separated (or colon-separated on Linux) list of paths, similar to
the `PATH` variable. The path can be absolute or relative to the server root.

### Is it possible to perform some action whenever a runtime error occurs?

Yes, use the `OnRuntimeError(error_code, &bool:suppress)` callback. Set the
`suppress` parameter to `true` to suppress error message.

[github]: https://github.com/Zeex/samp-plugin-crashdetect
[forum]: http://forum.sa-mp.com/showthread.php?t=262796
[download]: https://github.com/Zeex/samp-plugin-crashdetect/releases 
[donate]: http://pledgie.com/campaigns/19750
[donate_button]: http://www.pledgie.com/campaigns/19750.png
