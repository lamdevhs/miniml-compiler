(* encoder: mlexp --> code *)

open Miniml;;

type instr
  =  PrimInstr of primop
  | Cons
  | Push
  | Swap
  | Return
  | QuoteB of bool
  | QuoteI of int
  | Cur of code
  | App
  | Branch of code * code
  | Halt
  (* new for recursive calls *)
  | Call of var
  | AddDefs of (var * code) list
  | RmDefs
  (* new for lists *)
  | MakeList
  | QuoteEmptyList
and code = instr list ;;


type compilation_env = envelem list
and envelem = EVar of var | EDef of var list;;

let access : string -> compilation_env -> code = fun x env ->
  let has y zs = List.exists (fun z -> z = y) zs in
  let instr_Fst = PrimInstr (UnOp Fst) in
  let instr_Snd = PrimInstr (UnOp Snd) in
  let rec go n = (function
    | EDef ds :: tail -> if (has x) ds then [Call x] else go n tail
    | EVar a :: tail -> if a = x then Tools.several n instr_Fst @ [instr_Snd] else go (n + 1) tail
    | [] -> failwith ("this compiler is somehow buggy - var = " ^ x)
  ) in
  go 0 env
;;

let encode : mlexp -> code =
  let rec encode_rec env x =
  (
    match x with
    | Var v -> access v env
    | Bool x -> [QuoteB x]
    | Int x -> [QuoteI x]
    | Pair(a, b)
      -> Push :: encode_rec env a @ [Swap] @ encode_rec env b @ [Cons]
    | App(PrimOp op, e) -> encode_rec env e @ [PrimInstr op]
    | App(f, x)
      -> Push :: encode_rec env f @ [Swap] @ encode_rec env x @ [Cons; App]
    | Fn(arg, body)
      -> Cur (encode_rec (EVar arg :: env) body @ [Return]) :: []
    | Cond(cond, if_then, if_else) ->
    let encoded_br x = encode_rec env x @ [Return] in
    let branches = Branch(encoded_br if_then, encoded_br if_else) in
    Push :: encode_rec env cond @ [branches]
    | Fix(defs, exp) ->
    let (defs_names, defs_mlexps) = Tools.unzip defs in
    let new_env = EDef defs_names :: env in
    let defs_code = List.map (encode_rec new_env) defs_mlexps in
    let dc = Tools.zip defs_names defs_code in
    let ec = encode_rec new_env exp in
    [AddDefs dc] @ ec @ [RmDefs]
    | EmptyList -> [QuoteEmptyList]
    | ListCons(head, tail) ->
    Push :: encode_rec env head @ [Swap] @ encode_rec env tail @ [MakeList]
    | otherwise -> failwith "CompilerBug: mlexp expression unsupported!"
  ) in
  fun x -> encode_rec [] x @ [Halt]
;;

let encode_program : prog -> code =
  fun x -> encode (mlexp_of_prog x)
;;
  