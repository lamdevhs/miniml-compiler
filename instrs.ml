(* Instructions of the CAM *)

open Miniml;;

type instr =
  PrimInstr of primop
| Cons
| Push
| Swap
| Return
| QuoteB of bool
| QuoteI of int
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
type stack = stackelem list

let iSnd = PrimInstr (UnOp Snd);;
let iFst = PrimInstr (UnOp Fst);;

let eval_arith : barith -> int -> int -> int = function
  BAadd -> (+) | BAsub -> (-) | BAmul -> ( * ) | BAdiv -> (/) | BAmod -> (mod)
;;

let eval_compar : bcompar -> 'a -> 'a -> bool = function
  BCeq -> (=) | BCge -> (>=) | BCgt -> (>) | BCle -> (<=) | BClt -> (<) | BCne -> (<>)
;;

let eval_one
: (value * code * stack) -> (value * code * stack) =
  function
  | (_, QuoteB(x) :: c, st) -> (BoolV(x), c, st)
  | (_, QuoteI(x) :: c, st) -> (IntV(x), c, st)
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

let test_prog = [Push; Cur [Push; iSnd; Swap; QuoteI(1); Cons;
              PrimInstr (BinOp (BArith BAadd)); Return];
   Swap; QuoteI(2); Cons; App]
;;
let eval_prog instrs = till_no_change eval_one (initial_cfg instrs)
;;

(* compilation *)

type compilation_env = string list ;;

let rec access : string -> compilation_env -> code =
  fun x env -> match env with
  | a :: tail -> if a = x then [iSnd] else iFst :: access x tail
  | [] -> failwith ("this compiler is somehow buggy - var = " ^ x)
;;

let rec compile : compilation_env -> mlexp -> code =
  fun env x -> match x with
  | Var v -> access v env
  | Bool x -> [QuoteB x]
  | Int x -> [QuoteI x]
  | Pair(a, b) -> Push :: compile env a @ [Swap] @ compile env b @ [Cons]
  | App(PrimOp op, e) -> compile env e @ [PrimInstr op]
  | App(f, x) -> Push :: compile env f @ [Swap] @ compile env x @ [Cons; App]
  | Fn(arg, body) -> Cur (compile (arg :: env) body @ [Return]) :: []
  | Cond(cond, if_then, if_else) ->
    let compiled_br x = compile env x @ [Return] in
    let branches = Branch(compiled_br if_then, compiled_br if_else) in
    Push :: compile env cond @ [branches]
  (* | Fix(defs, exp) -> *)
  | otherwise -> failwith "this compiler is somehow buggy 1"
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
and flat_code = flat_instr list;;

type referenced_flat_code = (string * flat_code) list;;
type defs = (var * code) list;;
type defs_dict = (var * string) list;;
type flatdefs = (var * string) list;;

let code_namer str n = (str ^ string_of_int n, n + 1);;

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
    | RmDefs _ -> (match defsDict with
      | [] -> failwith "flatten_code says: compiler bug! near RmDefs"
      | (d :: ds) -> (n, ds, refCode, mainCode))
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
  (refs, program @ [FlatHalt])
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
    [AddDefs(defs); Call("f"); RmDefs(42); Return] in
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

let flat_program_to_C
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

let qwer2 = flat_program_to_C (flatten_program sample_program);;

let lines_of_C_code : c_code_fragment list -> string list = fun fragments ->
  let union_field field content = "{." ^field^ " = " ^content^ "}," in
  let write_instruction content = union_field "instruction" content in
  let write_operation content = union_field "operation" content in
  let write_reference content = union_field "reference" content in
  let write_data content = union_field "data" content in
  let string_of_flat_instr = (function
    | FlatHalt -> write_instruction "Halt"
    | FlatUnary op ->
        let opstr = if (op = Fst) then "Fst" else "Snd" in
        write_instruction "Unary" ^ write_operation opstr
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
    | FlatCur ref -> write_instruction "Cur" ^ write_reference ref
    | FlatApp -> write_instruction "App"
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
  declarations @ [""] @ List.concat definitions
;;

let fold_left_one : ('a -> 'a -> 'a) -> 'a -> 'a list -> 'a =
  fun f default_value -> function
  | [] -> default_value
  | x :: xs -> List.fold_left f x xs
;;

let lines_to_string : string list -> string = fun xs ->
  fold_left_one (fun acc line -> acc ^ "\n" ^ line) "" xs
;;

let qwer3 = lines_to_string (lines_of_C_code qwer2);;