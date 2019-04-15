let rec even = fun n -> (n = 0) || odd (n - 1)
     and odd = fun n -> (n <> 0) && even (n - 1)
in (even 7, odd 9) ;;
