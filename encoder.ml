
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

(* encoding: mlexp -> instr list *)

type compilation_env = envelem list
and envelem = EVar of var | EDef of var list;;
(* 
let rec str_of_env : compilation_env -> string = function
  | [] -> "[]"
  | EVar x :: xs -> "EVar(" ^ x ^ ")::" ^ str_of_env xs
  | EDef ds :: xs -> "EDef[" ^ concat_with "," ds ^ "]::" ^ str_of_env xs

let rec str_of_access : code -> string = function
  | [] -> ""
  | [Call x] -> "Call(" ^ x ^ ")"
  | PrimInstr (UnOp Fst) :: xs -> "Tail," ^ str_of_access xs
  | PrimInstr (UnOp Snd) :: xs -> "Head," ^ str_of_access xs
  | _ -> failwith "str_of_access: won't ever happen"
;; *)

let access : string -> compilation_env -> code = fun x env ->
  let has y zs = List.exists (fun z -> z = y) zs in
  let instr_Fst = PrimInstr (UnOp Fst) in
  let instr_Snd = PrimInstr (UnOp Snd) in
  let rec go n = (function
    | EDef ds :: tail -> if (has x) ds then [Call x] else go n tail
    | EVar a :: tail -> if a = x then Tools.several n instr_Fst @ [instr_Snd] else go (n + 1) tail
    | [] -> failwith ("this compiler is somehow buggy - var = " ^ x)
  ) in
  (* let msg = "looking for " ^ x ^ " in env = " ^ str_of_env env in
  print_endline msg; *)
  let res = go 0 env in
  (* print_endline ("got:" ^ str_of_access res) ; *)
  res
;;

let compile : compilation_env -> mlexp -> code = fun env x ->
  let rec compile_rec env x =
  (
    match x with
    | Var v -> access v env
    | Bool x -> [QuoteB x]
    | Int x -> [QuoteI x]
    | Pair(a, b)
      -> Push :: compile_rec env a @ [Swap] @ compile_rec env b @ [Cons]
    | App(PrimOp op, e) -> compile_rec env e @ [PrimInstr op]
    | App(f, x)
      -> Push :: compile_rec env f @ [Swap] @ compile_rec env x @ [Cons; App]
    | Fn(arg, body)
      -> Cur (compile_rec (EVar arg :: env) body @ [Return]) :: []
    | Cond(cond, if_then, if_else) ->
    let compiled_br x = compile_rec env x @ [Return] in
    let branches = Branch(compiled_br if_then, compiled_br if_else) in
    Push :: compile_rec env cond @ [branches]
    | Fix(defs, exp) ->
    let (defs_names, defs_mlexps) = Tools.unzip defs in
    let new_env = EDef defs_names :: env in
    let defs_code = List.map (compile_rec new_env) defs_mlexps in
    let dc = Tools.zip defs_names defs_code in
    let ec = compile_rec new_env exp in
    [AddDefs dc] @ ec @ [RmDefs]
    | EmptyList -> [QuoteEmptyList]
    | ListCons(head, tail) ->
    Push :: compile_rec env head @ [Swap] @ compile_rec env tail @ [MakeList]
    | otherwise -> failwith "CompilerBug: mlexp expression unsupported!"
  ) in
  compile_rec env x @ [Halt]
;;