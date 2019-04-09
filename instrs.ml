(* Instructions of the CAM *)

open Miniml;;

type instr =
  PrimInstr of primop
| Cons
| Push
| Swap
| Return
| Quote of value
| Cur of code
| App
| Branch of code * code
(* new for recursive calls *)
| Call of var
| AddDefs of (var * code) list
| RmDefs of int
and value =
  NullV 
| VarV of Miniml.var
| IntV of int
| BoolV of bool
| PairV of value * value
| ClosureV of code * value
and code = instr list
  
type stackelem = Val of value | Cod of code

let iSnd = PrimInstr (UnOp Snd);;
let iFst = PrimInstr (UnOp Fst);;

let eval_arith : barith -> int -> int -> int = function
  BAadd -> (+) | BAsub -> (-) | BAmul -> ( * ) | BAdiv -> (/) | BAmod -> (mod)
;;

let eval_compar : bcompar -> 'a -> 'a -> bool = function
  BCeq -> (=) | BCge -> (>=) | BCgt -> (>) | BCle -> (<=) | BClt -> (<) | BCne -> (<>)
;;

let eval_one
: (value * instr list * stackelem list) -> (value * instr list * stackelem list) =
  function
  | (_, Quote(v) :: c, st) -> (v, c, st)
  (* --------- paires --------- *)
  | (x, Cons :: c, Val(y) :: st) -> (PairV(y, x), c, st)
  | (x, Push :: c, st) -> (x, c, Val x :: st)
  | (x, Swap :: c, Val(y) :: st) -> (y, c, Val (x) :: st)
  (* --------- clotures --------- *)
  | (x, Cur (c1) :: c, st) -> (ClosureV(c1, x), c, st)
  | (PairV(ClosureV(cd, y), z), App :: c, st) -> (PairV(y, z), cd, Cod(c) :: st)
  | (x, Return :: c, Cod(c1) :: st) -> (x, c1, st)
  (* --------- primInstr --------- *)
  | (PairV(x, y), PrimInstr (UnOp Fst) :: c, st) -> (x, c, st)
  | (PairV(x, y), PrimInstr (UnOp Snd) :: c, st) -> (y, c, st)
  | (PairV(IntV x, IntV y), PrimInstr (BinOp (BArith op)) :: c, st) ->
    (IntV (eval_arith op x y), c, st)
  | (PairV(IntV x, IntV y), PrimInstr (BinOp (BCompar op)) :: c, st) ->
    (BoolV (eval_compar op x y), c, st)
  | (PairV(BoolV x, BoolV y), PrimInstr (BinOp (BCompar op)) :: c, st) ->
    (BoolV (eval_compar op x y), c, st)
  (* --------- Branch --------- *)
  | (BoolV(b), Branch (if_then, if_else) :: c, Val(x) :: st) ->
    (x, (if b then if_then else if_else), Cod(c) :: st)
  (* --------- otherwise --------- *)
  | otherwise -> otherwise
;;

let rec till_no_change : ('a -> 'a) -> 'a -> 'a = fun f x ->
  let y = f x in if y = x then y else till_no_change f y
;;

let initial_cfg x = (NullV, x, []);;

let test_prog = initial_cfg
  [Push; Cur [Push; iSnd; Swap; Quote (IntV 1); Cons;
              PrimInstr (BinOp (BArith BAadd)); Return];
   Swap; Quote (IntV 2); Cons; App]
;;
let eval_prog instrs = till_no_change eval_one (initial_cfg instrs)
;;

(* compilation *)

type compilation_env = string list ;;

let rec access : string -> compilation_env -> instr list =
  fun x env -> match env with
  | a :: tail -> if a = x then [iSnd] else iFst :: access x tail
  | [] -> failwith ("this compiler is somehow buggy - var = " ^ x)
;;

let rec compile : compilation_env -> mlexp -> instr list =
  fun env x -> match x with
  | Var v -> access v env
  | Bool x -> [Quote (BoolV x)]
  | Int x -> [Quote (IntV x)]
  | Pair(a, b) -> Push :: compile env a @ [Swap] @ compile env b @ [Cons]
  | App(PrimOp op, e) -> compile env e @ [PrimInstr op]
  | App(f, x) -> Push :: compile env f @ [Swap] @ compile env x @ [Cons; App]
  | Fn(arg, body) -> Cur (compile (arg :: env) body @ [Return]) :: []
  | Cond(cond, if_then, if_else) ->
    let compiled_br x = compile env x @ [Return] in
    let branches = Branch(compiled_br if_then, compiled_br if_else) in
    Push :: compile env cond @ [branches]
  | otherwise -> failwith "this compiler is somehow buggy 1"
;;
  