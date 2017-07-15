this is a very small programming language

at the moment there is only one type: Number

variables are a bit like cells in an excel sheet;

    Number hello;
    Number hi = 3;
	hello = hi + 1;
	hi = 4;

at line three, the variable $hello will be 4, but when the next line is executed, $hi will be equal to 4 AND $hello will now be equal to 5.
essentially; assignment statements don't copy; they declare that (in this example), $hello  *is* $hi + 1.

no functions (yet)
no custom data types (yet)
