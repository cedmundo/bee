" Vim syntax file
" Language: Bee
" Maintainer: Carlos Martínez
" Latest Revision: 13 Apr 2020

if exists("b:current_syntax")
  finish
endif

syn keyword beeKeywords def let if then else elif in for reduce with lambda it skipwhite
syn region beeBlock start="(" end=")" fold transparent
syn region beeList start="\[" end="\]" fold transparent
syn region beeString oneline start='"' skip=/\\./ end='"'

let b:current_syntax = "bee"

hi def link beeKeywords     Keyword
hi def link beeString       Constant
