let rec factorialList n =
  if n = 0 then [1] else
  let xs = factorialList (n - 1) in
  (n * head xs) :: xs
in factorialList 15
;;
