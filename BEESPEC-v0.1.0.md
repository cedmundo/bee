# BEESPEC

Version 0.1.0 - Road to cellophane.

_TODO: Add here a better description_

_TODO: Add here section for must, should and other naming conventions_

## Syntax

```
expr       = term_expr space* ("+" term_expr | "-" term_expr)*
term_expr  = prim_expr space* ("*" prim_expr | "/" prim_expr)*
prim_expr  = space* "(" expr ")"
           | space* num_lit
num_lit    = digit+
digit      = "0".."9"
space      = " " | "\t"
```

_TODO: Add here section for abstract syntax_
_TODO: Add here section for sematics_

## Execution

Once program loaded, bee interpreter must output the result of the integer
operations interpreted.
