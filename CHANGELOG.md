# Changelog

## Alpha 1.0 (2022-3-14)

Plis can do nothing successfully.  

### New Operations:
| operation              | usage                                 | description   |
| :--------------------: | :------------------------------------ | :------------ |
| `nop()`                | nop() -> none                         | does nothing |
| `exit(code)`           | exit(int:code[^a1.0-1]) -> none       | exits program execution with code |
| `testingop(x, y, ...)` | testingop(n*any:arg[^a1.0-2]) -> none | does nothing ang is ignored by compiler **not ment for use in programs** |

[^a1.0-1]: int:arg means argument of type int
[^a1.0-2]: n*arg means n arguments

### Problems:
`nop()` does even less than expected (does not get added to AST)

### Possible improvments:
- make nop waste a cpu cycle
- automatically exit with code 0 on successfull execution