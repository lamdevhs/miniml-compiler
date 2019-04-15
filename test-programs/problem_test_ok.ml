(fun a ->
  let rec f = fun a -> fun x ->
    if x = fst a
    then 0
    else f a (x - 1) in
  f a 6)
(3, false)
;;