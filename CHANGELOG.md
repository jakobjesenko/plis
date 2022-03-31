# Changelog

## Alpha 4.2 (2022-3-31)

Memory is now available for use, that means an array of int variables, that can be used without passing them to functions.  
[improved first actual program](examples/statments1.plis).  
[easy programming exercise](examples/statments3.plis).  
While loop is now a thing, likely making plis a proper programming language.

### New Operations:
| operation                | usage                              | description   |
| :----------------------: | :--------------------------------- | :------------ |
| `memset(index, val)`     | memset(int:index, int\|char:val) -> none | set 8-byte aligned memory array at index to value |
| `memget(index)`          | memget(int:index) -> int           | get value from memory at index |
| `while(condition, code)` | while(bool:condition, none:code) -> none | repeat code until contition is false; example: [counting](examples/statments2.plis) |

### Possible improvements:
- [ ] strings do not support escaped and special characters
- [ ] follow register purpose convention and rename labels
- [x] inttostr does not work for 1 digit numbers


## Alpha 4.1 (2022-3-24)

How do we get multi character input? [that's how](examples/math2.plis).  
Also [first actual program](examples/statments0.plis) that does anything.

### New Operations:
| operation                   | usage                                         | description   |
| :-------------------------: | :-------------------------------------------- | :------------ |
| `if(condition, opt1, opt2)` | if(bool:condition, any:opt1, any:opt2) -> any[^a4.1-1] | guides flow of the program in a way indicated by the condition |

### Possible improvements:
- [ ] strings do not support escaped and special characters
- [ ] follow register purpose convention and rename labels
- [ ] inttostr does not work for 1 digit numbers

[^a4.1-1]: the any type in this statement includes none, but has to match for all of them

## Alpha 4.0 (2022-3-23)

The logics update.  
I am aware that syntax is very cursed.

### New Operations:
| operation              | usage                             | description   |
| :--------------------: | :-------------------------------- | :------------ |
| `!(a)`                 | !(bool:a) -> bool                 | !a            |
| `&&(a, b)`             | &&(bool:a, bool:b) -> bool        | a && b        |
| `||(a, b)`             | \|\|(bool:a, bool:b) -> bool      | a \|\| b      |
| `==(a, b)`             | ==(bool:a, bool:b) -> bool        | a == b        |
| `!=(a, b)`             | !=(bool:a, bool:b) -> bool        | a != b        |
| `<(a, b)`              | <(bool:a, bool:b) -> bool         | a < b         |
| `>(a, b)`              | >(bool:a, bool:b) -> bool         | a > b         |
| `<=(a, b)`             | <=(bool:a, bool:b) -> bool        | a <= b        |
| `>=(a, b)`             | >=(bool:a, bool:b) -> bool        | a >= b        |

### Possible improvements:
- [ ] strings do not support escaped and special characters
- [ ] follow register purpose convention and rename labels

## Alpha 3.3 (2022-3-23)

A few more binary operations were added to plis. This ends the math functions implementations for the time being.

### New Operations:
| operation              | usage                             | description   |
| :--------------------: | :-------------------------------- | :------------ |
| `div(a, b)`            | div(int:a, int:b) -> int          | a / b         |
| `mod(a, b)`            | mod(int:a, int:b) -> int          | a % b         |
| `shiftl(a, offset)`    | shiftl(int:a, int:offset) -> int  | a << offset   |
| `shiftr(a, offset)`    | shiftr(int:a, int:offset) -> int  | a >> offset   |
| `shiftla(a, offset)`   | shiftla(int:a, int:offset) -> int | shift a left by offset, preserving sign |
| `shiftra(a, offset)`   | shiftra(int:a, int:offset) -> int | shift a right by offset, preserving sign |
| `inttostr(n)`          | inttostr(int:n) -> str            | returns string representation of an integer   |

### Possible improvements:
- [x] look into how div instruction works on x86_64
- [X] make a function, that prints integers **(suplemented by inttostr and prints)**
- [ ] strings do not support escaped and special characters
- [ ] follow register purpose convention and rename labels


## Alpha 3.2 (2022-3-21)

More quality of life. Printing strings and conversion to integers. Those changes bring an easier implementation of [hello world program](examples/hello1.plis). String values are now passed around as null terminated srings, instead of also passing their length.

### New Operations:
| operation              | usage                       | description   |
| :--------------------: | :-------------------------- | :------------ |
| `prints(msg)`          | prints(str:msg) -> none     | prints string variable to stdout |
| `parseint(string)`     | parseint(str:string) -> int | converts string to an integer |

### Possible improvements:
- [x] character literals of numerical values get parsed as integers, **which they should not**
- [ ] look into how div instruction works on x86_64
- [ ] make a function, that prints integers
- [ ] strings do not support escaped and special characters

## Alpha 3.1 (2022-3-17)

Reading characters from stdin. Putc is now buffered, to not waste time on write syscalls. Assembly output gets reformatted to look prettier. Also fixed zero-argument function compiling problems.  
mostly quality of life changes.

### New Operations:
| operation              | usage                       | description   |
| :--------------------: | :-------------------------- | :------------ |
| `getc()`               | getc() -> int               | reads one character from stdin |


### Possible improvements:
- [x] make nop waste a cpu cycle
- [ ] character literals of numerical values get parsed as integers, **which they should not**
- [ ] look into how div instruction works on x86_64
- [ ] make a function, that prints integers

## Alpha 3.0 (2022-3-16)

Math comming to plis!  
Some arithmetic and bitwise operations were added to plis, this introduced a few bugs, but hey it's a start. Also there is no way to print integers out easily.  
[simple example](examples/math0.plis)

for reference: ~18446744073709551550 = 65

### New Operations:
| operation              | usage                       | description   |
| :--------------------: | :-------------------------- | :------------ |
| `bitnot(x)`            | bitnot(int:x) -> int        | bitwise not operator |
| `bitand(a, b)`         | bitand(int:a, int:b) -> int | bitwise and operator |
| `bitor(a, b)`          | bitor(int:a, int:b) -> int  | bitwise or operator |
| `add(a, b)`            | and(int:a, int:b) -> int    | a + b |
| `sub(a, b)`            | sub(int:a, int:b) -> int    | a - b |
| `mul(a, b)`            | mul(int:a, int:b) -> int    | a \* b |

### Bugfixes:
- Corrected code in `printAsmProgram`, that cosed operations with multiple arguments to print more than once during compilation

### Possible improvements:
- [ ] make nop waste a cpu cycle
- [ ] character literals of numerical values get parsed as integers, **which they should not**
- [ ] look into how div instruction works on x86_64
- [ ] make a function, that prints integers

## Alpha 2.0 (2022-3-14)

Plis switched to more appropriate calling convention using stack to transfer arguments. Character literals were added. First [hello world program](examples/hello0.plis) is now easily achivable.

### Modified Operations:
| operation              | usage               | description   |
| :--------------------: | :------------------ | :------------ |
| `putc(n)`              | putc(int\|char:n[^a2.0-1]) -> none | print character to stdout by its ascii code |

### Problems:
`nop()` does even less than expected (does not get added to AST)

### Possible improvements:
- [ ] make nop waste a cpu cycle
- [x] add character literals for use with putc

[^a2.0-1]: int\|cher:arg means either type is accepted

## Alpha 1.1 (2022-3-14)

Exit statement is no longer necessary at the ent of each execution flow. On successfull execution `exit(0)` is called automatically.

### New Operations:
| operation              | usage               | description   |
| :--------------------: | :------------------ | :------------ |
| `putc(n)`              | putc(int:n) -> none | print character to stdout by its ascii code |
| `chain(expr1, epr2)`   | chain(none:expr1, none:expr2) | chain creates two successive folws in program execution[^a1.1-1] [example](examples/chain0.plis)

### Problems:
`nop()` does even less than expected (does not get added to AST),  
`testingop()` should probably hava a return value of *any*

### Possible improvements:
- [ ] make nop waste a cpu cycle
- [ ] add character literals for use with putc
- [x] automatically exit with code 0 on successfull execution
- [x] check for correct number of arguments in function calls

[^a1.1-1]: the same way two consecutive lines of code are interpreted by most other languages

## Alpha 1.0 (2022-3-14)

Plis can do nothing successfully.  

### New Operations:
| operation              | usage                                 | description   |
| :--------------------: | :------------------------------------ | :------------ |
| `nop()`                | nop() -> none[^a1.0-1]                         | does nothing |
| `exit(code)`           | exit(int:code[^a1.0-2]) -> none       | exits program execution with code |
| `testingop(x, y, ...)` | testingop(n*any:arg[^a1.0-3]) -> none | does nothing ang is ignored by compiler **not ment for use in programs** |

[^a1.0-1]: -> none indicates return value, *none* means no return value
[^a1.0-2]: int:arg means argument of type int
[^a1.0-3]: n*arg means n arguments

### Problems:
`nop()` does even less than expected (does not get added to AST)

### Possible improvments:
- [ ] make nop waste a cpu cycle
- [ ] automatically exit with code 0 on successfull execution