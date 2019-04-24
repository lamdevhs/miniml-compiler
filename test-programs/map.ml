let rec mapPlusUn = fun xss ->
  if fst xss = 0 then (0, 0) else ((fst xss + 10), mapPlusUn (snd xss))
in
mapPlusUn (1, 2, 4, 3, 1, 0, 0)
;;
