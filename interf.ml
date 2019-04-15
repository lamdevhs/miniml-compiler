(* Interface with parser *)

exception ParseLexError of exn * (string * int * int * string * string)

let parse_file infile = 
  let lexbuf = Lexing.from_channel (open_in infile) in
  try 
    Parser.start Lexer.token lexbuf
  with exn ->
    begin
      let curr = lexbuf.Lexing.lex_curr_p in
      let line = curr.Lexing.pos_lnum in
      let cnum = curr.Lexing.pos_cnum - curr.Lexing.pos_bol in
      let tok = Lexing.lexeme lexbuf in
      let tail = Lexer.ruleTail "" lexbuf in
      raise (ParseLexError (exn,(infile, line,cnum,tok,tail)))
    end
;;

let print_parse_error (filename, line,cnum,tok,tail) =
  print_string ("Parsing error in file: " ^ filename ^ 
                      " on line: " ^ (string_of_int line) ^ 
                      " column: " ^ (string_of_int cnum) ^
                      " token: "  ^ tok ^
                      "\nrest: "  ^ tail ^ "\n")
;;

let parse infile = 
  try parse_file infile
  with ParseLexError (e, r) -> 
    print_parse_error r;
    failwith "Stopped execution."
;;


let generate infilename outfilename =
  (*
  let outf = open_out outfile in
  let str = Instrs.print_gen_class_to_java (Instrs.compile_prog (parse infile)) in
  output_string outf str ; flush outf;
  *)
  let abstract_tree = Miniml.mlexp_of_prog (parse infilename) in
  print_endline (Miniml.pp_exp abstract_tree);
  let code = Instrs.compile [] abstract_tree in
  let flat_code = Instrs.flatten_program code in
  let c_file = Instrs.flat_program_to_C flat_code in
  let outf_c_file = open_out (outfilename ^ ".c") in
  output_string outf_c_file c_file ; flush outf_c_file ;
  print_string "finished\n"
;;

