
(* Main function and target of compilation in Makefile *)


let generate infilename outfilename =
  (*
  let outf = open_out outfile in
  let str = Instrs.print_gen_class_to_java (Instrs.compile_prog (parse infile)) in
  output_string outf str ; flush outf;
  *)
  let abstract_tree = Miniml.mlexp_of_prog (Interf.parse infilename) in
  let code = Encoder.encode abstract_tree in
  let flat_code = Flattener.flatten_program code in
  let c_file = CodeGenerator.flat_program_to_C flat_code in
  let outf_c_file = open_out outfilename in
  output_string outf_c_file c_file ; flush outf_c_file ;
  print_string "Finished compiling miniml to C.\n"
;;


let main () =
  if Array.length Sys.argv != 3
  then (print_endline "usage error: expected exactly two arguments (an input filename and an output filename)")
  else (generate Sys.argv.(1) Sys.argv.(2))
;;

main();;
