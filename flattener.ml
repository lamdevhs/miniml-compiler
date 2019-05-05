(* flattener: code --> flat_code *)

open Miniml;;
open Encoder;;

type flat_instr
  = FlatHalt
  | FlatUnary of unop
  | FlatArith of barith
  | FlatCompare of bcompar
  | FlatCons
  | FlatPush
  | FlatSwap
  | FlatReturn
  | FlatQuoteBool of bool
  | FlatQuoteInt of int
  | FlatCur of string
  | FlatApp
  | FlatBranch of string * string
  (* new for recursive calls *)
  | FlatCall of string
  (* new for lists *)
  | FlatQuoteEmptyList
  | FlatMakeList
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
    | QuoteBool(b) -> (n, defsDict, refCode, mainCode @ [FlatQuoteBool(b)])
    | QuoteInt(i) -> (n, defsDict, refCode, mainCode @ [FlatQuoteInt(i)])
    | QuoteEmptyList -> (n, defsDict, refCode, mainCode @ [FlatQuoteEmptyList])
    | MakeList -> (n, defsDict, refCode, mainCode @ [FlatMakeList])
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
      let (defsOldNames, defsCode) = Tools.unzip newDefs in
      let (newN, defsNewNames) = rename_defs n defsOldNames in
      let defsTranslator = Tools.zip defsOldNames defsNewNames in
      let nextDefsDict = defsTranslator :: defsDict in
      let (nextN, defsRefCode, defsFlatCode) =
            flatten_defs newN nextDefsDict defsCode in
      let nextRefCode = refCode @ defsRefCode @ (Tools.zip defsNewNames defsFlatCode) in
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


