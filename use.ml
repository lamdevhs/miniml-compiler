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
