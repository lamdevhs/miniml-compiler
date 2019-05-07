let rec map f xs =
  if head xs = 0 (*marks the end of list*)
    then [0]
    else f (head xs) :: map f (tail xs)
in
let g x = x + 3 in
map g [1;2;3;4;5;6;7;8]
;;