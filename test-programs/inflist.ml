let rec f = fun x -> (1, g)
and g = fun x -> (2, f)
and h = fun n x -> (n, h (n + 1))
and dummy = false
and take = fun n xss ->
  if n = 0 then [] else fst xss :: take (n - 1) (snd xss dummy)
in (take 6 (f dummy), take 6 (g dummy), take 6 ((h 42) dummy))
;;
