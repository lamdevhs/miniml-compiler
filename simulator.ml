open Miniml;;
open Encoder;;

type value
  = NullV
  | IntV of int
  | BoolV of bool
  | PairV of value * value
  | ListV of value_list
  | ClosureV of code * value
and value_list = EmptyListV | ListConsV of (value * value_list)

(* pretty print value: at the end of the simulation, to print the result in a repl-caml-like way *)
let pp_value : value -> string = fun v ->
  let rec go_value =
  (
    function
      | NullV -> ("unit", "()")
      | IntV i -> ("int", string_of_int i)
      | BoolV b -> ("bool", string_of_bool b)
      | PairV (x, y) ->
        let (type_x, value_x) = go_value x in
        let (type_y, value_y) = go_value y in
        let type_pair = "(" ^ type_x ^ " * " ^ type_y ^ ")" in
        let value_pair = "(" ^ value_x ^ ", " ^ value_y ^ ")" in
        (type_pair, value_pair)
      | ListV l -> ("list", "[" ^ go_value_list l ^ "]")
      | ClosureV (code, x) ->
        let (_, value_x) = go_value x in
        ("closure", "Closure(" ^ value_x ^ ", <code>)")
  )
  and go_value_list =
  (
    function
      | EmptyListV -> ""
      | ListConsV (x, tail) ->
        let (_, value_x) = go_value x in
        if tail = EmptyListV then value_x else
        let value_tail = go_value_list tail in
        value_x ^ "; " ^ value_tail
  ) in
  let (type_, value_) = go_value v in
  ("- : " ^ type_ ^ " = " ^ value_)
;;


type stackelem = Val of value | Cod of code ;;
type stack = stackelem list ;;
type rec_binding = (var * code);;
type defstack = rec_binding list list;;

let rec get_definition : var -> defstack -> code option = fun to_find -> function
  | [] -> None
  | binding_list :: tail ->
    let res = List.assoc_opt to_find binding_list in
    if res = None then get_definition to_find tail else res
;;

let eval_arith : barith -> int -> int -> int = function
  BAadd -> (+) | BAsub -> (-) | BAmul -> ( * ) | BAdiv -> (/) | BAmod -> (mod)
;;

let eval_compar : bcompar -> 'a -> 'a -> bool = function
  BCeq -> (=) | BCge -> (>=) | BCgt -> (>) | BCle -> (<=) | BClt -> (<) | BCne -> (<>)
;;

let rec exec : (value * code * stack * defstack) -> value = function
  | (_, QuoteBool(x) :: c, st, fds) -> exec (BoolV(x), c, st, fds)
  | (_, QuoteInt(x) :: c, st, fds) -> exec (IntV(x), c, st, fds)
  (* --------- pairs --------- *)
  | (x, Cons :: c, Val(y) :: st, fds) -> exec (PairV(y, x), c, st, fds)
  | (_, Cons :: c, _, fds) -> failwith "CompilerBug: can't construct pair: stacktop is not a value"
  | (x, Push :: c, st, fds) -> exec (x, c, Val x :: st, fds)
  | (x, Swap :: c, Val(y) :: st, fds) -> exec (y, c, Val (x) :: st, fds)
  | (_, Swap :: c, _, fds) -> failwith "CompilerBug: can't swap: stacktop is not a value"
  (* --------- closures --------- *)
  | (x, Cur (c1) :: c, st, fds) -> exec (ClosureV(c1, x), c, st, fds)
  | (PairV(ClosureV(cd, y), z), App :: c, st, fds) -> exec (PairV(y, z), cd, Cod(c) :: st, fds)
  | (_, App :: c, _, fds) -> failwith "CompilerBug: can't apply: invalid term"
  | (x, Return :: c, Cod(c1) :: st, fds) -> exec (x, c1, st, fds)
  | (_, Return :: c, _, fds) -> failwith "CompilerBug: can't return: stacktop is not code"
  (* --------- primInstr --------- *)
  | (PairV(x, y), PrimInstr (UnOp Fst) :: c, st, fds) -> exec (x, c, st, fds)
  | (_, PrimInstr (UnOp Fst) :: c, _, fds) -> failwith "TypeError: can't get first: not a pair"
  | (PairV(x, y), PrimInstr (UnOp Snd) :: c, st, fds) -> exec (y, c, st, fds)
  | (_, PrimInstr (UnOp Snd) :: c, _, fds) -> failwith "TypeError: can't get second: not a pair"
  | (PairV(IntV x, IntV y), PrimInstr (BinOp (BArith op)) :: c, st, fds)
    -> exec (IntV (eval_arith op x y), c, st, fds)
  | (_, PrimInstr (BinOp (BArith op)) :: c, _, fds) -> failwith "TypeError: can't operate: operands are not integers"
  | (PairV(IntV x, IntV y), PrimInstr (BinOp (BCompar op)) :: c, st, fds)
    -> exec (BoolV (eval_compar op x y), c, st, fds)
  | (PairV(BoolV x, BoolV y), PrimInstr (BinOp (BCompar op)) :: c, st, fds)
    -> exec (BoolV (eval_compar op x y), c, st, fds)
  | (_, PrimInstr (BinOp (BCompar op)) :: c, _, fds)
    -> failwith "TypeError: can't compare: operands are neither two integers nor two booleans"
  (* --------- Branch --------- *)
  | (BoolV(b), Branch (if_then, if_else) :: c, Val(x) :: st, fds)
    -> exec (x, (if b then if_then else if_else), Cod(c) :: st, fds)
  | (_, Branch (_,_) :: c, Val(x) :: st, fds)
    -> failwith "TypeError: in conditional branch: condition is not a boolean"
  | (_, Branch (_,_) :: c, _, fds)
    -> failwith "CompilerBug: in conditional branch: stacktop is not a value"
  (* --------- let rec --------- *)
  | (x, Call called :: c, st, fds)
    -> (
      match get_definition called fds with
      | Some called_code -> exec (x, called_code @ c, st, fds)
      | None -> failwith "CompilerBug: can't call, definition not found"
    )
  | (x, AddDefs defs :: c, st, fds) -> exec (x, c, st, defs :: fds)
  | (x, RmDefs :: c, st, head :: fds) -> exec (x, c, st, fds)
  | (x, RmDefs :: c, st, _) -> failwith "CompilerBug: can't remove definition, defstack is empty"
  (* -------- lists -------- *)
  | (ListV (tail), MakeList :: c, Val(head) :: st, fds)
    -> exec (ListV(ListConsV (head, tail)), c, st, fds)
  | (_, MakeList :: c, Val(head) :: st, fds)
    -> failwith "CompilerBug: can't construct list: tail is not a list"
  | (_, MakeList :: c, st, fds)
    -> failwith "CompilerBug: can't construct list: stacktop is not a value"
  | (_, QuoteEmptyList :: c, st, fds) -> exec (ListV(EmptyListV), c, st, fds)
  | (l, PrimInstr (UnOp Head) :: c, st, fds) ->
    (
      match l with
      | ListV EmptyListV -> failwith "RuntimeError: can't take the head of an empty list"
      | ListV (ListConsV (h, t)) -> exec (h, c, st, fds)
      | otherwise -> failwith "TypeError: can't take the head: not a list"
    )
  | (l, PrimInstr (UnOp Tail) :: c, st, fds) ->
    (
      match l with
      | ListV EmptyListV -> failwith "RuntimeError: can't take the tail of an empty list"
      | ListV (ListConsV (h, t)) -> exec (ListV t, c, st, fds)
      | otherwise -> failwith "TypeError: can't take the head: not a list"
    )
  (* -------- the end -------- *)
  | (final_value, Halt :: c, stack, fds) ->
    let allok = stack = [] && fds = [] in
    (if not allok then print_endline "CompilerWarning: CAM terminated with weird final state" else ());
    final_value
  | (_, [], _, fds) -> failwith "CompilerBug: code list empty"
  (* | otherwise -> failwith "CompilerBug: undefined instruction" *)
;;

let initial_cfg code = (NullV, code, [], []);;

let run_simulation code = exec (initial_cfg code);;

let main () =
  if Array.length Sys.argv != 2
  then (print_endline "usage error: expected exactly one argument (an input filename)")
  else
    let prog = Interf.parse Sys.argv.(1) in
    let code = Encoder.encode_program prog in
    let final_value = run_simulation code in
    print_endline (pp_value final_value)
;;

main ();;