type var = string

(* Unary operators *)
type unop = Fst | Snd | Head | Tail

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
| ListCons of mlexp * mlexp
| EmptyList

type typedef = string
type prog = Prog of typedef option * mlexp

let mlexp_of_prog (Prog(t, e)) = e



(* -------------------- pretty printer: ---------------- *)

let pp_unop : unop ->  string = function
  Fst -> "fst" | Snd -> "snd" | Head -> "head" | Tail -> "tail"
;;

let pp_barith : barith -> string = function
  BAadd -> "+" | BAsub -> "-" | BAmul -> "*" | BAdiv -> "/" | BAmod -> "mod"
;;


let pp_bcompar : bcompar ->  string = function
  BCeq -> "=" | BCge -> ">=" | BCgt -> ">" | BCle -> "<=" | BClt -> "<" | BCne -> "<>"
;;

let pp_binop : binop ->  string = function
    BArith b -> pp_barith b
  | BCompar b -> pp_bcompar b
;;


let pp_primop : primop ->  string = function
  | UnOp x -> pp_unop x
  | BinOp x -> pp_binop x
;;


let rec pp_exp : mlexp -> string = function
  | Var v -> v
  | Bool b -> string_of_bool b
  | Int i -> string_of_int i
  | PrimOp po -> pp_primop po
  | Cond (i, t, e) -> "(if " ^ pp_exp i ^ " then " ^ pp_exp t ^ " else " ^ pp_exp e ^ ")"
  | Pair (x, y) -> "(" ^ pp_exp x ^ "," ^ pp_exp y ^ ")"
  | App (PrimOp op, Pair (x, y)) -> "(" ^ pp_exp x ^ " " ^ pp_primop op ^ " " ^ pp_exp y ^ ")"
  | App (f, x) -> "(" ^ pp_exp f ^ " " ^ pp_exp x ^ ")"
  | Fn (v, b) -> "(fun " ^ v ^ " -> " ^ pp_exp b ^ ")"
  | Fix (d :: defs, exp) -> "let rec " ^ pp_def d ^ pp_defs defs ^ " in " ^ pp_exp exp
  | Fix _ -> "(empty let rec ???!!!)"
  | ListCons (x, y) -> "(" ^ pp_exp x ^ " :: " ^ pp_exp y ^ ")"
  | EmptyList -> "[]"
and pp_def : (var * mlexp) -> string = fun (name, value) ->
  name ^ " = " ^ pp_exp value
and pp_defs : (var * mlexp) list -> string = function
  | [] -> ""
  | d :: ds -> " and " ^ pp_def d ^ pp_defs ds
;;

(* pretty print a program;
the result is in truth not really pretty due to an overdose of parentheses.
the point is to get a (vaguely readable) idea of what the parser interpreted
out of a miniml file.  *)
let pp : prog -> string = fun x -> pp_exp (mlexp_of_prog x) ;;