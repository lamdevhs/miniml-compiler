#load "tools.cmo";;
#load "miniml.cmo";;
#load "parser.cmo" ;;
#load "lexer.cmo" ;;
#load "interf.cmo";;
#load "encoder.cmo";;
#load "flattener.cmo";;
#load "codeGenerator.cmo";;
#load "simulator.cmo";;
open Miniml ;;
open Interf ;;
open Encoder ;;
open Flattener ;;
open CodeGenerator ;;
open Simulator ;;

(* To use the parser:
- parse "foo/bar.ml" ;;

* For code generation:
- encode_program (parse "foo/bar.ml") ;;

* For simulated execution, with trace:
- run_simulation (encode_program (parse "foo/bar.ml")) true ;;

* and without trace:
- run_simulation (encode_program (parse "foo/bar.ml")) false ;;

*)

let test_file = "test-programs/factorial.ml";;

let get_prog () = parse test_file;;

let get_code () = encode_program (get_prog ());;

let run_sim verbose = run_simulation (get_code ()) verbose;;
