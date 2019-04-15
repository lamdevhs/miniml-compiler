(fun a ->
  let rec f = fun x ->
    if x = fst a then 0 else f (x - 1) in
  f 6)
(3, false)
;;