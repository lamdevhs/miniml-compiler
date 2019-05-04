(* staple tools *)


let rec zip : 'a list -> 'b list ->  ('a * 'b) list =
  fun xlist ylist -> match (xlist, ylist) with
  | ([], _) -> []
  | (_, []) -> []
  | (x :: xs, y :: ys) -> (x, y) :: zip xs ys
;;

let rec unzip : ('a * 'b) list -> ('a list * 'b list) =
  function
  | [] -> ([], [])
  | (x, y) :: tail ->
    let (xs, ys) = unzip tail in
    (x :: xs, y :: ys)
;;


let rec several : int -> 'a -> 'a list = fun n x ->
  if (n <= 0) then [] else x :: several (n - 1) x
;;

let sum : int list -> int = fun xs ->
  List.fold_left (+) 0 xs
;;