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

type stackelem = Val of value | Cod of code
and stack = stackelem list ;;

type rec_binding = (var * code)
and defstack = rec_binding list;;

type machine_state = (value * code * stack * defstack);;

type status
  = AllOk of machine_state
  | Stopped of final_status
and final_status
  = Halted of value
  | Crashed of string
;;



(* pretty print value: used at the end of the simulation *)
let pp_value : value -> string = fun v ->
  let rec go_value = function
    | NullV -> "()"
    | IntV i -> string_of_int i
    | BoolV b -> string_of_bool b
    | PairV (x, y) ->
      let value_x = go_value x in
      let value_y = go_value y in
      "(" ^ value_x ^ ", " ^ value_y ^ ")"
    | ListV l -> "[" ^ go_value_list l ^ "]"
    | ClosureV (code, x) ->
      let value_x = go_value x in
      "Closure(<code>, " ^ value_x ^ ")"
  and go_value_list = function
    | EmptyListV -> ""
    | ListConsV (x, tail) ->
      let value_x = go_value x in
      if tail = EmptyListV then value_x else
      let value_tail = go_value_list tail in
      value_x ^ "; " ^ value_tail
  in go_value v
;;

let pp_stacktop : stack -> string = function
  | [] -> "<stack is empty>"
  | Cod _ :: _ -> "<code>"
  | Val v :: _ -> pp_value v
;;

let pp_unary = function
  | Fst -> "Fst"
  | Snd -> "Snd"
  | Head -> "Head"
  | Tail -> "Tail"
;;

let pp_arith = function
  | BAadd -> "Plus"
  | BAsub -> "Sub"
  | BAmul -> "Mul"
  | BAdiv -> "Div"
  | BAmod -> "Mod"
;;

let pp_compare = function
  | BCeq -> "Eq"
  | BCge -> "Ge"
  | BCgt -> "Gt"
  | BCle -> "Le"
  | BClt -> "Lt"
  | BCne -> "Neq"
;;

let pp_test = function
  | IsEmpty -> "TestIsEmpty"
;;

let pp_code : code -> string =
  let parens x = "(" ^ x ^ ")" in
  function
  | [] -> "<code list is empty>"
  | head :: _ ->
  match head with
  | PrimInstr (UnOp op) -> "Unary" ^ parens (pp_unary op)
  | PrimInstr (BinOp (BArith op)) -> "Arith" ^ parens (pp_arith op)
  | PrimInstr (BinOp (BCompar op)) -> "Compare" ^ parens (pp_compare op)
  | PrimInstr (TestOp op) -> "Test" ^ parens (pp_test op)
  | Cons -> "Cons"
  | Push -> "Push"
  | Swap -> "Swap"
  | Return -> "Return"
  | QuoteBool b -> "QuoteBool" ^ parens (string_of_bool b)
  | QuoteInt i -> "QuoteInt" ^ parens (string_of_int i)
  | Cur c -> "Curry" ^ parens "<code>"
  | App -> "Apply"
  | Branch _ -> "Branch" ^ parens "<code>,<code>"
  | Halt -> "Halt"
  (* new for recursive calls *)
  | Call v -> "Call" ^ parens v
  | AddDefs xs -> "AddDefs"
      ^ parens ("<" ^ string_of_int (List.length xs) ^ "defs>")
  | RmDefs n -> "Rmdefs" ^ parens (string_of_int n)
  (* new for lists *)
  | MakeList -> "MakeList"
  | QuoteEmptyList -> "QuoteEmptyList"
;;

let eval_arith : barith -> int -> int -> int = function
  | BAadd -> (+)
  | BAsub -> (-)
  | BAmul -> ( * )
  | BAdiv -> (/)
  | BAmod -> (mod)
;;

let eval_compar : bcompar -> 'a -> 'a -> bool = function
  | BCeq -> (=)
  | BCge -> (>=)
  | BCgt -> (>)
  | BCle -> (<=)
  | BClt -> (<)
  | BCne -> (<>)
;;

let eval_test : (testop * value) -> bool = function
  | (IsEmpty, ListV EmptyListV) -> true
  | (IsEmpty, _) -> false
;;

let execute_next_instruction : machine_state -> status =
  let crashed msg = Stopped (Crashed msg) in
  function
  | (_, QuoteBool(x) :: c, st, fds) -> AllOk (BoolV(x), c, st, fds)
  | (_, QuoteInt(x) :: c, st, fds) -> AllOk (IntV(x), c, st, fds)
  (* --------- pairs --------- *)
  | (x, Cons :: c, Val(y) :: st, fds) -> AllOk (PairV(y, x), c, st, fds)
  | (_, Cons :: c, _, fds) -> crashed
      "MachineFailure: can't construct pair: stacktop is not a value"
  | (x, Push :: c, st, fds) -> AllOk (x, c, Val x :: st, fds)
  | (x, Swap :: c, Val(y) :: st, fds) -> AllOk (y, c, Val (x) :: st, fds)
  | (_, Swap :: c, _, fds) -> crashed
      "MachineFailure: can't swap: stacktop is not a value"
  (* --------- closures --------- *)
  | (x, Cur (c1) :: c, st, fds) -> AllOk (ClosureV(c1, x), c, st, fds)
  | (PairV(ClosureV(cd, y), z), App :: c, st, fds) -> AllOk
      (PairV(y, z), cd, Cod(c) :: st, fds)
  | (_, App :: c, _, fds) -> crashed
      "MachineFailure: can't apply: invalid term"
  | (x, Return :: c, Cod(c1) :: st, fds) -> AllOk (x, c1, st, fds)
  | (_, Return :: c, _, fds) -> crashed
      "MachineFailure: can't return: stacktop is not code"
  (* --------- primInstr --------- *)
  | (term, PrimInstr (TestOp op) :: c, st, fds) -> AllOk
      (BoolV(eval_test (op, term)), c, st, fds)
  | (PairV(x, y), PrimInstr (UnOp Fst) :: c, st, fds) -> AllOk (x, c, st, fds)
  | (_, PrimInstr (UnOp Fst) :: c, _, fds) -> crashed
      "TypeError: can't get first: not a pair"
  | (PairV(x, y), PrimInstr (UnOp Snd) :: c, st, fds) -> AllOk (y, c, st, fds)
  | (_, PrimInstr (UnOp Snd) :: c, _, fds) -> crashed
      "TypeError: can't get second: not a pair"
  | (PairV(IntV x, IntV y), PrimInstr (BinOp (BArith op)) :: c, st, fds) ->
      AllOk (IntV (eval_arith op x y), c, st, fds)
  | (_, PrimInstr (BinOp (BArith op)) :: c, _, fds) -> crashed
      "TypeError: can't operate: operands are not integers"
  | (PairV(IntV x, IntV y), PrimInstr (BinOp (BCompar op)) :: c, st, fds) ->
      AllOk (BoolV (eval_compar op x y), c, st, fds)
  | (PairV(BoolV x, BoolV y), PrimInstr (BinOp (BCompar op)) :: c, st, fds) ->
      AllOk (BoolV (eval_compar op x y), c, st, fds)
  | (_, PrimInstr (BinOp (BCompar op)) :: c, _, fds) -> crashed
      ("TypeError: can't compare: operands are neither "
      ^ "two integers nor two booleans")
  (* --------- Branch --------- *)
  | (BoolV(b), Branch (if_then, if_else) :: c, Val(x) :: st, fds) -> AllOk
      (x, (if b then if_then else if_else), Cod(c) :: st, fds)
  | (_, Branch (_,_) :: c, Val(x) :: st, fds) -> crashed
      "TypeError: in conditional branch: condition is not a boolean"
  | (_, Branch (_,_) :: c, _, fds) -> crashed
      "MachineFailure: in conditional branch: stacktop is not a value"
  (* --------- let rec --------- *)
  | (x, Call to_call :: c, st, fds)
    -> (
      match Tools.assoc_opt to_call fds with
      | Some called_code -> AllOk (x, called_code @ c, st, fds)
      | None -> crashed "MachineFailure: can't Call, definition not found"
    )
  | (x, AddDefs defs :: c, st, fds) -> AllOk (x, c, st, defs @ fds)
  | (x, RmDefs(n) :: c, st, fds) -> AllOk (x, c, st, Tools.chop n fds)
  (* -------- lists -------- *)
  | (ListV (tail), MakeList :: c, Val(head) :: st, fds) -> AllOk
      (ListV(ListConsV (head, tail)), c, st, fds)
  | (_, MakeList :: c, Val(head) :: st, fds) -> crashed
      "MachineFailure: can't construct list: tail is not a list"
  | (_, MakeList :: c, st, fds) -> crashed
      "MachineFailure: can't construct list: stacktop is not a value"
  | (_, QuoteEmptyList :: c, st, fds) -> AllOk (ListV(EmptyListV), c, st, fds)
  | (l, PrimInstr (UnOp Head) :: c, st, fds)
    -> (
      match l with
      | ListV EmptyListV -> crashed
          "RuntimeError: can't take the head of an empty list"
      | ListV (ListConsV (h, t)) -> AllOk (h, c, st, fds)
      | otherwise -> crashed "TypeError: can't take the head: not a list"
    )
  | (l, PrimInstr (UnOp Tail) :: c, st, fds) ->
    (
      match l with
      | ListV EmptyListV -> crashed
          "RuntimeError: can't take the tail of an empty list"
      | ListV (ListConsV (h, t)) -> AllOk (ListV t, c, st, fds)
      | otherwise -> crashed "TypeError: can't take the head: not a list"
    )
  (* -------- the end -------- *)
  | (final_value, Halt :: c, stack, fds) ->
    let final_state_ok = (stack = [] && fds = []) in
    (if not final_state_ok then
      print_endline "Warning: CAM terminated with weird final state"
      else ());
    Stopped (Halted final_value)
  | (_, [], _, fds) -> crashed "MachineFailure: code list empty"
;;

let print_trace : machine_state -> unit = fun (term, code, stack, _) ->
  print_endline ("# term = " ^ pp_value term);
  print_endline ("# stacktop = " ^ pp_stacktop stack);
  print_endline ("# next instruction = " ^ pp_code code);
  print_endline "---";
;;

let rec run_machine : machine_state -> bool -> final_status = fun ms verbose ->
  if verbose then (print_trace ms) else ();
  match execute_next_instruction ms with
  | AllOk new_ms -> run_machine new_ms verbose
  | Stopped final_status -> final_status
;;

let blank_state : code -> machine_state =
  fun code -> (NullV, code, [], [])
;;

let run_simulation code verbose =
  match run_machine (blank_state code) verbose with
  | Halted final_value -> print_endline (pp_value final_value)
  | Crashed msg -> print_endline msg
;;

let main () =
  let len = Array.length Sys.argv in
  let valid_usage =
    len = 2 || (len = 3 && Sys.argv.(1) = "verbose") in
  if not valid_usage
  then print_endline ("usage error: correct usage: ./simu [verbose] <filename>")
  else
    let verbose = (len = 3) in
    let prog = Interf.parse Sys.argv.(if verbose then 2 else 1) in
    let code = Encoder.encode_program prog in
    run_simulation code verbose
;;

main ();;
