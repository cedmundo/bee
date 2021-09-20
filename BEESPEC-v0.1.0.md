# BEESPEC

Version 0.1.0 - Road to cellophane.

_TODO: Add here a better description_

_TODO: Add here section for must, should and other naming conventions_

## Syntax

```
expr              = logical_expr
logical_expr      = comparision_expr spaces ("&&" comparision_expr | "||" comparision_expr)
comparision_expr  = inequality_expr spaces ("==" inequality_expr | "!=" inequality_expr)*
inequality_expr   = bitwise_expr spaces (">" bitwise_expr | ">=" bitwise_expr | "<" bitwise_expr | "<=" bitwise_expr)*
bitwise_expr      = bitshift_expr spaces ("&" bitshift_expr | "|" bitshift_expr | "^" bitshift_expr)
bitshift_expr     = factor_expr spaces ("<<" factor_expr | ">>" factor_expr )*
factor_expr       = term_expr spaces ("+" term_expr | "-" term_expr)*
term_expr         = primary_expr spaces ("*" term_expr | "/" term_expr | "%" term_expr)*
primary_expr      = spaces "(" expr ")"
                  | spaces number_lit
number_lit        = digit+
digit             = "0".."9"
space             = " " | "\t"
spaces            = space*
```

_TODO: Add here section for abstract syntax_
_TODO: Add here section for sematics_

## Execution

Once program loaded, bee interpreter must output the result of the integer
operations interpreted.
