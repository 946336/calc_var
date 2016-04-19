# calc_var
A simple calculator, like [calc](https://github.com/946336/calc "The inferior version"), but offers variables.

This simple calculator takes after `bc` but provides only a basic subset of `bc`'s capabilities.

You may or may not care about the following decisions I have made:
<ul>
  <li>
    log has been turned into the infix operator |, where (number)|(base) is the syntax
  </li>
  <li>
    Parenthesized operations adjacent to literals or other parenthesized operations will assume multiplication if an operator is not
    present
  </li>
</ul>

Use either the included makefile or `compile.sh` to compile. Note that `compile.sh` script aggregates all the source files into a single source file before compiling from that. `compile.sh` currently still requires that the headers be available during compilation.

## Syntax

### Expressions
Any complete mathematical expression

    >>> (2^5)|2
    = 5
    >>> 2+(3+5)7
    = 58
    >>> 2+(3+5)8|2
    = 24

### Variables
Variables declared in the following fashion:

    >>> let <Name_1> = <Expression_1>

The variable `<Name_1>` is assigned the result of evaluating `<Expression_1>` and is available for use afterwards.

### Intermediate Variables
    let <Name_1> = <Exp_1> where <Name_2> = <Exp_2> and <Name_3> = <Exp_3> where <Name_4> = <Exp_4>
    <Exp_5> where <Name_6> = <Exp_6> and <Name_7> = <Exp_7> where <Name_8> = <Exp_8>

Intermediate variables can be introduced in the scope of another expression. These variables are destroyed after `<Exp_1>` is evaluated. You can introduce multiple intermediate variables at once.

Intermediate variables are be mutually recursive (Note: super extensive testing has not been done on this front), so the following examples are correct and equivalent. After either or both of these statements, `a` is correctly bound to the value `5`, and neither `b` nor `c` are bound as variables.

    >>> let a = b + 0 where b = c and c = 5
    = 5
    >>> let a = b + 0 where c = 5 and b = c
    = 5
    >>> a
    = 5
    >>> b
    ... [...] Runtime Error: Name [b] nout bound

Similarly, intermediate bindings in the scope of an expression do not persist.

    >>> a where b = 5 and a = b
    = 5
    >>> a
    ... [...] Runtime Error: Name [a] not bound
    >>> b
    ... [...] Runtime Error: Name [b] not bound

### Strings
Strings exist and can be bound to variables. String literals are quoted with double quotes as in 

    >>> "This is a string literal"
    = "This is a string literal"
    >>> let string = "This is a string"
    = "This is a string"

String concatenation is accomplished via the `+` operator.

    >>> let a = b + c where b = "Hello " and c = "world"
    = "Hello world"

## Command Line
### Options
`calc` can read from files. Provide the filename as the last option on the command line.

Additionally, `calc` accepts a handful of command line options.

`-q`: Quiet - Suppresses most output

`-v`: Verbose - Produce more output

`--no-echo`: No echo - Don't echo back the parsed expression
