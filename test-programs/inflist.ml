let rec f = fun x -> (1, g)
and g = fun x -> (2, f)
and h = fun n x -> (n, h (n + 1))
and take = fun n xss ->
  if n = 1 then fst xss else (fst xss, take (n - 1) (snd xss false))
in (take 10 (f false), take 10 (g false), take 10 ((h 0) false))
;;
