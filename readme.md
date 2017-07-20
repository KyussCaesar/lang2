This is an interpreter for a very small programming language.

The repo is called lang2 because 'lang' was already taken in my projects folder.

Current Status: Working... kinda. On Hold
-

There was an attempt to add a Boolean type and if statements, but because of the way I've done things it's all turned into a bit of a mess.

This project is now on hold, since it's the end of the holidays, uni is starting back up and I won't really have time to work on it.

'notesfornexttime' details some of my ramblings, reflections and thoughts about how I would do things differently next time.

What *is* working is the Number type and variable declarations, as well as arithmetic stuff for numbers (+ - * /).

Variables are a bit like cells in an excel sheet;

    Number hello;
    Number hi = 3;
	hello = hi + 1;
	hi = 4;

at line three, the variable $hello will be 4, but when the next line is executed, $hi will be equal to 4 AND $hello will now be equal to 5.

essentially; assignment statements don't copy; they declare that (in this example), $hello  *is* $hi + 1.
