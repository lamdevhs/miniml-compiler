#load "miniml.cmo";;
#load "parser.cmo" ;;
#load "lexer.cmo" ;;
#load "instrs.cmo";;
#load "interf.cmo";;
open Miniml ;;
open Interf ;;
open Instrs ;;

(* For using the parser:

- Evaluate this file (use.ml)
- parse "Tests/test.ml" ;;

* For code generation:

- compile_prog (parse "Tests/test.ml") ;;

* For execution:

- run (initial_cfg (compile_prog (parse "Tests/test.ml"))) ;;

*)

let do_compilation filename =
  let parsed = parse filename in print_endline (pp parsed);
  let Prog (_, instrs) = parsed in
  compile [] instrs ;;

let full_test filename =
  let compiled = do_compilation filename in
  eval_prog compiled ;;

let tfss = "Tests/test.ml" ;;