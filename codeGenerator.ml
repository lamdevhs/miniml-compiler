(* flattener: flat_code --> .c file *)

open Miniml;;
open Flattener;;

type c_code_fragment = CCodeFragment of (string * int * flat_code);;
(* ^ a c_code_fragment represents an array of CodeT values (cf the CCAM).
the string represents the name that will be given to the array,
the int represents the size of the array
and the flat_code is the code that will be stored inside the CodeT array. *)

(* first step of code generation: determine the size of each code fragment based
on how much space each flat instruction takes up when compiled to a C array of CodeT
(c.f. CCAM) *)
let flat_program_to_c_code_fragments
: (referenced_flat_code * flat_code) -> c_code_fragment list =
  fun (refsCode, mainCode) ->
  let allCode = refsCode @ [("main_code", mainCode)] in
  let flat_instr_length = (function
    | FlatUnary _ -> 2
    | FlatArith _ -> 2
    | FlatCompare _ -> 2
    | FlatQuoteBool _ -> 2
    | FlatQuoteInt _ -> 2
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

let string_of_unary = function
  | Fst -> "Fst"
  | Snd -> "Snd"
  | Head -> "Head"
  | Tail -> "Tail"
;;

let lines_of_C_code : c_code_fragment list -> (string list * string list) =
  fun fragments ->
  let union_field field content = "{." ^field^ " = " ^content^ "}," in
  let string_of_instruction content = union_field "instruction" content in
  let string_of_operation content = union_field "operation" content in
  let string_of_reference content = union_field "reference" content in
  let string_of_data content = union_field "data" content in
  let string_of_flat_instr = (function
    | FlatHalt -> string_of_instruction "Halt"
    | FlatUnary op ->
        string_of_instruction "Unary" ^ string_of_operation (string_of_unary op)
    | FlatArith op ->
        let opstr = (match op with
          BAadd->"Plus" |BAsub->"Sub" |BAmul->"Mul" |BAdiv->"Div" |BAmod->"Mod") in
        string_of_instruction "Arith" ^ string_of_operation opstr
    | FlatCompare op ->
        let opstr = (match op with
          BCeq->"Eq" |BCge->"Ge" |BCgt->"Gt"
          |BCle->"Le" |BClt->"Lt" |BCne->"Neq") in
        string_of_instruction "Compare" ^ string_of_operation opstr
    | FlatCons -> string_of_instruction "Cons"
    | FlatPush -> string_of_instruction "Push"
    | FlatSwap -> string_of_instruction "Swap"
    | FlatReturn -> string_of_instruction "Return"
    | FlatQuoteBool b -> string_of_instruction "QuoteBool" ^
        string_of_data (if b then "True" else "False")
    | FlatQuoteInt i -> string_of_instruction "QuoteInt" ^
        string_of_data (string_of_int i ^ "L")
    | FlatQuoteEmptyList -> string_of_instruction "QuoteEmptyList"
    | FlatMakeList -> string_of_instruction "MakeList"
    | FlatCur ref -> string_of_instruction "Curry" ^ string_of_reference ref
    | FlatApp -> string_of_instruction "Apply"
    | FlatBranch (ifref, elseref) ->
        string_of_instruction "Branch" ^
        string_of_reference ifref ^ string_of_reference elseref
    | FlatCall ref -> string_of_instruction "Call" ^ string_of_reference ref
  )
  in
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

(* like a normal fold_left but takes the first element of the list
as initial value for the folding *)
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
  let include_ccam = "#include \"ccam.h\"" in
  let code_accessor = "CodeT *get_main_code()" :: "{" :: "    return main_code;" :: "}" :: [] in
  let c_file = include_ccam :: "" :: declarations @ ("" :: definitions @ ("" :: code_accessor)) in
  (lines_to_string c_file ^ "\n")
;;