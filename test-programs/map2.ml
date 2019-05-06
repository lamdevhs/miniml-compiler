let rec map f xs =
  if is_empty xs
    then []
    else f (head xs) :: map f (tail xs)
in
let g x = x + 100 in
map g [1;2;3;4;5;6;7;8]
;;
