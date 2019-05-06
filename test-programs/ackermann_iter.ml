(* Higher-order version of Ackermann function with function iteration *)

let rec succ n = n + 1
and iter f n = if n = 0 then (f 1) else f (iter f (n - 1))
and ack m = if m = 0 then succ else iter (ack (m - 1)) in
ack 3 2;;
