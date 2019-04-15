let emptyList = fun xxx -> fst xxx = 0 in
let sum = fun xss ->
  let rec go = fun xxs acc ->
    if emptyList xxs then acc else
    go (snd xxs) (fst xxs + acc) in
  go xss 0
in
sum (1, 2, 4, 3, 1, 0, 0)
;;