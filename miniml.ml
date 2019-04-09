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


(* -------------------- pretty printer: ---------------- *)

let pp_unop : unop ->  string = function
  Fst -> "first" | Snd -> "second"
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
  | Cond (i, t, e) -> "if " ^ pp_exp i ^ " then " ^ pp_exp t ^ " else " ^ pp_exp e
  | Pair (x, y) -> "(" ^ pp_exp x ^ "," ^ pp_exp y ^ ")"
  | App (PrimOp op, Pair (x, y)) -> "(" ^ pp_exp x ^ " " ^ pp_primop op ^ " " ^ pp_exp y ^ ")"
  | App (f, x) -> "(" ^ pp_exp f ^ " " ^ pp_exp x ^ ")"
  | Fn (v, b) -> "(fun " ^ v ^ " -> " ^ pp_exp b ^ ")"
  | Fix _ -> "fix"
;;

let pp : prog -> string = function
  Prog (_, exp) -> pp_exp exp
;;