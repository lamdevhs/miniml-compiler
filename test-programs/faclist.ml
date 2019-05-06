let rec faclist n =
  if n = 0 then [1] else
  let xs = faclist (n - 1) in
  (n * head xs) :: xs
in faclist 15
;;
