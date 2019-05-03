let rec factolist =
  fun n -> if n = 0 then [1] else
  let xs = factolist (n - 1) in
  (n * head xs) :: xs
in factolist 15
;;
