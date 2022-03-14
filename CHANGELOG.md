# Changelog

## Alpha 2.0 (2022-3-14)

Plis switched to more appropriate calling convention using stack to transfer arguments. Character literals were added. First hello [world program](examples/hello0.plis) is now easily achivable.

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