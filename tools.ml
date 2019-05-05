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

(* like a normal fold_left but takes the first element of the list
as initial value for the folding *)
let fold_left_one : ('a -> 'a -> 'a) -> 'a -> 'a list -> 'a =
  fun f default_value -> function
  | [] -> default_value
  | x :: xs -> List.fold_left f x xs
;;

(* takes a list of strings and treat them as lines in a multilined string,
adding newlines in-between each line appropriately *)
let lines_to_string : string list -> string = fun xs ->
  fold_left_one (fun acc line -> acc ^ "\n" ^ line) "" xs
;;

(* append `n` spaces at the beginning of each string in `lines`,
aka indent the `lines` with `n` spaces *)
let with_indent n lines =
  let indent = String.make n ' ' in
  List.map (fun line -> indent ^ line) lines
;;

let rec chop n xs = if n <= 0 then xs else
  match xs with
  | [] -> []
  | _ :: tail -> chop (n - 1) tail
;;
