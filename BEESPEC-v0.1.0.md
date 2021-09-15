# BEESPEC

Version 0.1.0 - Road to cellophane.

_TODO: Add here a better description_

_TODO: Add here section for must, should and other naming conventions_

## Syntax

```
expr       = comp_expr
comp_expr  = ineq_expr spaces ("==" ineq_expr | "!=" ineq_expr)*
ineq_expr  = bitw_expr spaces (">" bitw_expr | ">=" bitw_expr | "<" bitw_expr | "<=" bitw_expr)*
bitw_expr  = bshf_expr spaces ("&" bshf_expr | "|" bshf_expr | "^" bshf_expr)
bshf_expr  = fact_expr spaces ("<<" fact_expr | ">>" fact_expr )*
fact_expr  = term_expr spaces ("+" term_expr | "-" term_expr)*
term_expr  = prim_expr spaces ("*" term_expr | "/" term_expr | "%" term_expr)*
prim_expr  = spaces "(" expr ")"
           | spaces num_lit
num_lit    = digit+
digit      = "0".."9"
space      = " " | "\t"
spaces     = space*
```

_TODO: Add here section for abstract syntax_
_TODO: Add here section for sematics_

## Execution

Once program loaded, bee interpreter must output the result of the integer
operations interpreted.
