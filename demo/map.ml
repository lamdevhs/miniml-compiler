let rec map f xs =
  if is_empty xs
    then []
    else f (head xs) :: map f (tail xs) in
let f x = x + 100 in
map f [1;2;3;4;5;6;] ;;
