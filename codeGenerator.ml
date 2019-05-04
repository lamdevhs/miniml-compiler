
open Miniml;;
open Flattener;;

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
    let len = Tools.sum (List.map flat_instr_length flatCode) in
    CCodeFragment (name, len, flatCode) in
  List.map f allCode
;;

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
    | FlatMakeList -> write_instruction "MakeList"
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
  let include_vm = "#include \"ccam.h\"" in
  let code_accessor = "CodeT *get_main_code()" :: "{" :: "    return main_code;" :: "}" :: [] in
  let c_file = include_vm :: "" :: declarations @ ("" :: definitions @ ("" :: code_accessor)) in
  (lines_to_string c_file ^ "\n")
;;