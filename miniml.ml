type var = string

(* Unary operators *)
type unop = Fst | Snd
    
(* Binary Arithmetic operators *)
type barith = BAadd | BAsub | BAmul | BAdiv | BAmod

(* Binary Comparison operators *)
type bcompar = BCeq | BCge | BCgt | BCle | BClt | BCne

type binop = 
  BArith of barith
| BCompar of bcompar

type primop =
  UnOp of unop
| BinOp of binop


type mlexp =
  Var of var
| Bool of bool
| Int of int
| PrimOp of primop
| Cond of mlexp * mlexp * mlexp
| Pair of mlexp * mlexp
| App of mlexp * mlexp
| Fn of var * mlexp
| Fix of (var * mlexp) list * mlexp
    
type typedef = string
type prog = Prog of typedef option * mlexp

let mlexp_of_prog (Prog(t, e)) = e

