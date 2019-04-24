
(* Main function and target of compilation in Makefile *)


let main () =
  if Array.length Sys.argv != 3
  then (print_endline "User Error: expects exactly two arguments, an input filename and an output filename.")
  else (Interf.generate Sys.argv.(1) Sys.argv.(2))
;;

main();;
