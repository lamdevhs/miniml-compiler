(* Instructions of the CAM *)

open Miniml;;


(* staple tools *)
let rec zip : 'a list -> 'b list ->  ('a * 'b) list =
  fun xlist ylist -> match (xlist, ylist) with
  | ([], _) -> []
  | (_, []) -> []
  | (x :: xs, y :: ys) -> (x, y) :: zip xs ys
;;

let rec unzip : ('a * 'b) list -> ('a list * 'b list) =
  function
  | [] -> ([], [])
  | (x, y) :: tail ->
    let (xs, ys) = unzip tail in
    (x :: xs, y :: ys)
;;


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
  | ListCons
  | QuoteEmptyList
and code = instr list


let iSnd = PrimInstr (UnOp Snd);;
let iFst = PrimInstr (UnOp Fst);;


(* compilation *)

type compilation_env = envelem list
and envelem = EVar of var | EDef of var list;;

let rec several : int -> 'a -> 'a list = fun n x ->
  if (n <= 0) then [] else x :: several (n - 1) x
;;

let concat_with : string -> string list -> string = fun sep ->
  let folder acc str = acc ^ sep ^ str in
  function
  | [] ->  ""
  | x :: xs ->  List.fold_left folder x xs
;;

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
;;


let access : string -> compilation_env -> code = fun x env ->
  let has y zs = List.exists (fun z -> z = y) zs in
  let rec go n = (function
    | EDef ds :: tail -> if (has x) ds then [Call x] else go n tail
    | EVar a :: tail -> if a = x then several n iFst @ [iSnd] else go (n + 1) tail
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
    let (defs_names, defs_mlexps) = unzip defs in
    let new_env = EDef defs_names :: env in
    let defs_code = List.map (compile_rec new_env) defs_mlexps in
    let dc = zip defs_names defs_code in
    let ec = compile_rec new_env exp in
    [AddDefs dc] @ ec @ [RmDefs]
    | EmptyList -> [QuoteEmptyList]
    | ListCons(head, tail) ->
    Push :: compile_rec env head @ [Swap] @ compile_rec env tail @ [ListCons]
    | otherwise -> failwith "CompilerBug: mlexp expression unsupported!"
  ) in
  compile_rec env x @ [Halt]
;;

type flat_instr
  = FlatHalt
  | FlatUnary of unop
  | FlatArith of barith
  | FlatCompare of bcompar
  | FlatCons
  | FlatPush
  | FlatSwap
  | FlatReturn
  | FlatQuoteB of bool
  | FlatQuoteI of int
  | FlatCur of string
  | FlatApp
  | FlatBranch of string * string
  (* new for recursive calls *)
  | FlatCall of string
  (* new for lists *)
  | FlatQuoteEmptyList
  | FlatListCons
and flat_code = flat_instr list;;

type referenced_flat_code = (string * flat_code) list;;
type defs = (var * code) list;;
type defs_dict = (var * string) list;;
type flatdefs = (var * string) list;;

let code_namer str n = (str ^ string_of_int n, n + 1);;


let rename_defs : int -> var list -> (int * string list) =
  fun seedN oldNameList ->
  let folder (n, newNames) oldName =
    let (newName, newN) = code_namer "letrec" n in
    (newN, (newName ^ "_" ^ oldName) :: newNames) in
  let seed = (seedN, []) in
  let (outN, outList) = List.fold_left folder seed oldNameList in
  (outN, List.rev outList)
;;

let rec find_def : var -> defs_dict list -> string =
  fun v -> function
  | [] -> failwith "find_def says: compiler bug!"
  | d :: ds -> try List.assoc v d with
    | Not_found -> find_def v ds
;;

(* Putting aside the implementation details, this new function takes a
list of `instr` and returns a list of `flat_instr`.
The difference between them is that the code structure is not recursive
anymore: code does not contain instructions that could contain code.
Instead, we end up with a list of separated pieces of code, and named
references from one piece of code to another. It fits the C model (with
pointers to code fragments, etc) that `flat_code` will get compiled to
eventually. *)
let rec flatten_code
: int -> defs_dict list -> code -> (int * referenced_flat_code * flat_code) =
  fun seedN seedDefsDict code ->
  let folder (n, defsDict, refCode, mainCode) = (function
    | PrimInstr(UnOp op) ->
      (n, defsDict, refCode, mainCode @ [FlatUnary (op)])
    | PrimInstr(BinOp (BArith op)) ->
      (n, defsDict, refCode, mainCode @ [FlatArith(op)])
    | PrimInstr(BinOp (BCompar op)) ->
      (n, defsDict, refCode, mainCode @ [FlatCompare(op)])
    | Cons -> (n, defsDict, refCode, mainCode @ [FlatCons])
    | Push -> (n, defsDict, refCode, mainCode @ [FlatPush])
    | Swap -> (n, defsDict, refCode, mainCode @ [FlatSwap])
    | Return -> (n, defsDict, refCode, mainCode @ [FlatReturn])
    | QuoteB(b) -> (n, defsDict, refCode, mainCode @ [FlatQuoteB(b)])
    | QuoteI(i) -> (n, defsDict, refCode, mainCode @ [FlatQuoteI(i)])
    | QuoteEmptyList -> (n, defsDict, refCode, mainCode @ [FlatQuoteEmptyList])
    | ListCons -> (n, defsDict, refCode, mainCode @ [FlatListCons])
    | Cur(curCode) ->
      let (curN, curRefCode, curFlatCode) = flatten_code n defsDict curCode in
      let (curName, nextN) = code_namer "lambda" curN in
      let nextRefCode = refCode @ curRefCode @ [(curName, curFlatCode)] in
      let nextMainCode = mainCode @ [FlatCur(curName)] in
      (nextN, defsDict, nextRefCode, nextMainCode)
    | App -> (n, defsDict, refCode, mainCode @ [FlatApp])
    | Branch(ifCode, elseCode) ->
      let (ifCodeN, ifRefCode, ifFlatCode) =
        flatten_code n defsDict ifCode in
      let (ifCodeName, n2) = code_namer "if_branch" ifCodeN in
      let (elseCodeN, elseRefCode, elseFlatCode) =
        flatten_code n2 defsDict elseCode in
      let (elseCodeName, nextN) = code_namer "else_branch" elseCodeN in
      let nextRefCode = refCode
        @ ifRefCode @ [(ifCodeName, ifFlatCode)]
        @ elseRefCode @ [(elseCodeName, elseFlatCode)] in
      let nextMainCode = mainCode @ [FlatBranch(ifCodeName, elseCodeName)] in
      (nextN, defsDict, nextRefCode, nextMainCode)
    | AddDefs(newDefs) ->
      let (defsOldNames, defsCode) = unzip newDefs in
      let (newN, defsNewNames) = rename_defs n defsOldNames in
      let defsTranslator = zip defsOldNames defsNewNames in
      let nextDefsDict = defsTranslator :: defsDict in
      let (nextN, defsRefCode, defsFlatCode) =
            flatten_defs newN nextDefsDict defsCode in
      let nextRefCode = refCode @ defsRefCode @ (zip defsNewNames defsFlatCode) in
      (nextN, nextDefsDict, nextRefCode, mainCode)
    | Call(var) ->
      let nameToCall = find_def var defsDict in
      (n, defsDict, refCode, mainCode @ [FlatCall(nameToCall)])
    | RmDefs -> (match defsDict with
      | [] -> failwith "flatten_code says: compiler bug! near RmDefs"
      | (d :: ds) -> (n, ds, refCode, mainCode))
    | Halt -> (n, defsDict, refCode, mainCode @ [FlatHalt])
  ) in
  let seed = (seedN, seedDefsDict, [], []) in
  let (outN, _, outRefCode, outMainCode) = List.fold_left folder seed code in
  (outN, outRefCode, outMainCode)
(* end of flatten_code *)
and flatten_defs
: int -> defs_dict list -> code list -> (int * referenced_flat_code * flat_code list) =
  fun seedN defsDict codeList ->
  let folder (n, refs, flatCodeList) code =
    let (newN, newRefs, flatCode) = flatten_code n defsDict code in
    (newN, refs @ newRefs, (flatCode @ [FlatReturn]) :: flatCodeList) in
  let seed = (seedN, [], []) in
  let (outN, outRefs, outFlatCodeList) = List.fold_left folder seed codeList in
  (outN, outRefs, List.rev outFlatCodeList)
;;

let flatten_program : code -> (referenced_flat_code * flat_code) = fun code ->
  let (_, refs, program) = flatten_code 0 [] code in
  (refs, program)
;;


let sample_program =
  let if1 =
    [QuoteI(3); Return] in
  let else1 =
    let defs = [
      ("f", [Call "g"]);
      ("g", [QuoteI 123]);
      ("h", [Call "i"]);
      ("i", [Call "h"])
    ] in
    [AddDefs(defs); Call("f"); RmDefs; Return] in
  let cur1 =
    [Push; iSnd; Swap; Branch(if1, else1); Cons;
    PrimInstr (BinOp (BArith BAadd)); Return]
  in
  [Push; Cur(cur1); Swap; QuoteI(2); Cons; App]
;;
let qwer = flatten_program sample_program;;

type c_code_fragment = CCodeFragment of (string * int * flat_code);;



let sum : int list -> int = fun xs ->
  List.fold_left (+) 0 xs
;;

let flat_program_to_c_code_fragments
: (referenced_flat_code * flat_code) -> c_code_fragment list =
  fun (refsCode, mainCode) ->
  let allCode = refsCode @ [("main_code", mainCode)] in
  let flat_instr_length = (function
    | FlatUnary _ -> 2
    | FlatArith _ -> 2
    | FlatCompare _ -> 2
    | FlatQuoteB _ -> 2
    | FlatQuoteI _ -> 2
    | FlatCur _ -> 2
    | FlatBranch (_, _) -> 3
    | FlatCall _ -> 2
    | otherwise -> 1
  ) in
  let f (name, flatCode) =
    let len = sum (List.map flat_instr_length flatCode) in
    CCodeFragment (name, len, flatCode) in
  List.map f allCode
;;

let qwer2 = flat_program_to_c_code_fragments (flatten_program sample_program);;

let operation_of_unary = function
  | Fst -> "Fst"
  | Snd -> "Snd"
  | Head -> "Head"
  | Tail -> "Tail"
;;

let lines_of_C_code : c_code_fragment list -> (string list * string list) =
  fun fragments ->
  let union_field field content = "{." ^field^ " = " ^content^ "}," in
  let write_instruction content = union_field "instruction" content in
  let write_operation content = union_field "operation" content in
  let write_reference content = union_field "reference" content in
  let write_data content = union_field "data" content in
  let string_of_flat_instr = (function
    | FlatHalt -> write_instruction "Halt"
    | FlatUnary op ->
        write_instruction "Unary" ^ write_operation (operation_of_unary op)
    | FlatArith op ->
        let opstr = (match op with
          BAadd->"Plus" |BAsub->"Sub" |BAmul->"Mul" |BAdiv->"Div" |BAmod->"Mod") in
        write_instruction "Arith" ^ write_operation opstr
    | FlatCompare op ->
        let opstr = (match op with
          BCeq->"Eq" |BCge->"Ge" |BCgt->"Gt"
          |BCle->"Le" |BClt->"Lt" |BCne->"Neq") in
        write_instruction "Compare" ^ write_operation opstr
    | FlatCons -> write_instruction "Cons"
    | FlatPush -> write_instruction "Push"
    | FlatSwap -> write_instruction "Swap"
    | FlatReturn -> write_instruction "Return"
    | FlatQuoteB b -> write_instruction "QuoteBool" ^
        write_data (if b then "True" else "False")
    | FlatQuoteI i -> write_instruction "QuoteInt" ^
        write_data (string_of_int i ^ "L")
    | FlatQuoteEmptyList -> write_instruction "QuoteEmptyList"
    | FlatListCons -> write_instruction "MakeList"
    | FlatCur ref -> write_instruction "Curry" ^ write_reference ref
    | FlatApp -> write_instruction "Apply"
    | FlatBranch (ifref, elseref) ->
        write_instruction "Branch" ^
        write_reference ifref ^ write_reference elseref
    | FlatCall ref -> write_instruction "Call" ^ write_reference ref
  ) in
  let with_indent n lines =
    let indent = String.make n ' ' in
    List.map (fun line -> indent ^ line) lines in
  let lines_of_fragment (CCodeFragment(name, size, instrs)) =
    let first_line = "CodeT " ^ name ^ "[] =" in
    let frag_content = with_indent 4 (List.map string_of_flat_instr instrs) in
    first_line :: ["{"] @ frag_content @ ["};"] in
  let declaration_of_fragment (CCodeFragment(name, size, instrs)) =
    "CodeT " ^ name ^ "[" ^ string_of_int size ^ "];" in
  let declarations = List.map declaration_of_fragment fragments in
  let definitions = List.map lines_of_fragment fragments in
  (declarations, List.concat definitions)
;;

let fold_left_one : ('a -> 'a -> 'a) -> 'a -> 'a list -> 'a =
  fun f default_value -> function
  | [] -> default_value
  | x :: xs -> List.fold_left f x xs
;;

let lines_to_string : string list -> string = fun xs ->
  fold_left_one (fun acc line -> acc ^ "\n" ^ line) "" xs
;;

let flat_program_to_C
: (referenced_flat_code * flat_code) -> string = fun flat_program ->
  let (declarations, definitions) =
    lines_of_C_code (flat_program_to_c_code_fragments flat_program) in
  let include_vm = "#include \"virtual-machine.h\"" in
  let code_accessor = "CodeT *get_main_code()" :: "{" :: "    return main_code;" :: "}" :: [] in
  let c_file = include_vm :: "" :: declarations @ ("" :: definitions @ ("" :: code_accessor)) in
  (lines_to_string c_file ^ "\n")
;;

let qwer3 =
  let (a, b) = lines_of_C_code qwer2 in
  lines_to_string a ^ "\n" ^ lines_to_string b
;;

let rec factorial =
  fun n -> if n = 0 then 1 else n * factorial (n - 1)
in factorial 10
;;

let factorial_mlexp =
  let eq a b = Miniml.App (PrimOp (BinOp (BCompar BCeq)), Pair(a, b)) in
  let mul a b = Miniml.App (PrimOp (BinOp (BArith BAmul)), Pair(a, b)) in
  let sub a b = Miniml.App (PrimOp (BinOp (BArith BAsub)), Pair(a, b)) in
  let app var x = Miniml.App(Var var, x) in
  let body =
    let cnd = eq (Var "n") (Int 0) in
    let ifbranch = Int 1 in
    let elsebranch = mul (Var "n") (app "factorial" (sub (Var "n") (Int 1))) in
    Cond(cnd, ifbranch, elsebranch) in
  let def = Fn("n", body) in
  Fix ([("factorial", def)], app "factorial" (Int 10))
;;

let factorial_code = compile [] factorial_mlexp;;
let factorial_flat_code = flatten_program factorial_code;;
let factorial_C = flat_program_to_c_code_fragments factorial_flat_code;;
let factorial_str =
  let (a, b) = lines_of_C_code factorial_C in
  lines_to_string a ^ "\n" ^ lines_to_string b
;;
