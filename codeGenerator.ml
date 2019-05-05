(* flattener: flat_code --> .c file *)

open Miniml;;
open Flattener;;

let c_of_unary = function
  | Fst -> "Fst"
  | Snd -> "Snd"
  | Head -> "Head"
  | Tail -> "Tail"
;;

let c_of_arith = function
  | BAadd -> "Plus"
  | BAsub -> "Sub"
  | BAmul -> "Mul"
  | BAdiv -> "Div"
  | BAmod -> "Mod"
;;

let c_of_compare = function
  | BCeq -> "Eq"
  | BCge -> "Ge"
  | BCgt -> "Gt"
  | BCle -> "Le"
  | BClt -> "Lt"
  | BCne -> "Neq"
;;

let c_of_flat_instr : flat_instr -> string =
  let union_field field content = "{." ^ field ^ " = " ^ content ^ "}," in
  let c_of_instruction content = union_field "instruction" content in
  let c_of_operation content = union_field "operation" content in
  let c_of_reference content = union_field "reference" content in
  let c_of_data content = union_field "data" content in
  function
  | FlatHalt -> c_of_instruction "Halt"
  | FlatUnary op -> c_of_instruction "Unary"
    ^ c_of_operation (c_of_unary op)
  | FlatArith op -> c_of_instruction "Arith"
    ^ c_of_operation (c_of_arith op)
  | FlatCompare op -> c_of_instruction "Compare"
    ^ c_of_operation (c_of_compare op)
  | FlatCons -> c_of_instruction "Cons"
  | FlatPush -> c_of_instruction "Push"
  | FlatSwap -> c_of_instruction "Swap"
  | FlatReturn -> c_of_instruction "Return"
  | FlatQuoteBool b -> c_of_instruction "QuoteBool"
      ^ c_of_data (if b then "True" else "False")
  | FlatQuoteInt i -> c_of_instruction "QuoteInt"
      ^ c_of_data (string_of_int i ^ "L")
  | FlatQuoteEmptyList -> c_of_instruction "QuoteEmptyList"
  | FlatMakeList -> c_of_instruction "MakeList"
  | FlatCur ref -> c_of_instruction "Curry" ^ c_of_reference ref
  | FlatApp -> c_of_instruction "Apply"
  | FlatBranch (ifref, elseref) ->
      c_of_instruction "Branch"
      ^ c_of_reference ifref ^ c_of_reference elseref
  | FlatCall ref -> c_of_instruction "Call" ^ c_of_reference ref
;;

let c_declaration_of_fragment (name, _) = "CodeT " ^ name ^ "[];" ;;

let c_definition_of_fragment (name, instrs) =
  let first_line = "CodeT " ^ name ^ "[] =" in
  let frag_content = Tools.with_indent 4 (List.map c_of_flat_instr instrs) in
  first_line :: ["{"] @ frag_content @ ["};"]
;;

let c_of_flat_program : fragment list -> string = fun fragments ->
let include_ccam = "#include \"ccam.h\"" in
  let declarations = List.map c_declaration_of_fragment fragments in
  let definitions = List.map c_definition_of_fragment fragments in
  let code_accessor =
    "CodeT *get_main_code()"
    :: "{"
    :: "    return main_code;"
    :: ["}"] in
  let c_lines
    = [include_ccam]
    @ [""]
    @ declarations
    @ [""]
    @ List.concat definitions
    @ [""]
    @ code_accessor in
  Tools.lines_to_string c_lines ^ "\n"
;;
