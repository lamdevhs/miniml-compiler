let rec constantes = (1, false, [])
  (* entiers, booléens, liste vide, paires *)
  (* ici ce "triplet" est parsé en (1, (true, [])) *)
and listes = (4, 5) :: [2; true;]
  (* les listes ne sont pas typées *)
  (* trailing semicolon allowed in [...;...;] syntax *)
and arith = [3 + 2; 3 - 2; 3 * 2; 3 / 2; 3 mod 2]
and comparaisons = [3 = 2; 3 <> 2; 3 > 2; 3 >= 2; 3 < 2; 3 <= 2]
and unaires = let x = (1, 2) in let y = [3; 4] in
  [fst x; snd x; head y; head (tail y)]
  (* opérateurs unaires *)
  (* syntaxe `let x = ... in ...` *)
and tests = [is_empty [1;2]; is_empty []; is_empty 4]
  (* is_empty vérifie si son argument est une liste vide *)
  (* ne provoque pas d'erreurs même si la valeur n'est pas une liste *)
and cond = if 3 > 4 then 1 / 0 else 42
  (* évaluation paresseuse des arguments du if *)
and andOr = is_empty [1] && 3 < 4 || (true || 3 / 0 > 2)
  (* évaluation paresseuse *)
  (* (&&) est prioritaire sur (||), ici le résultat est `true` *)
  (* opérations arithmétiques prioritaires sur les comparaisons,
  elles-mêmes prioritaires sur les opérations booléennes *)
and notExp = not true && true || not true
  (* opérateur booléen `not` *)
and plus3 = (fun x y -> x + y) 3
  (* fonctions comme valeurs *) (* currying (application partielle) *)
  (* syntaxe `fun x y z -> ...` *)
and deuxPuissanceQuatre = let f x = x * x in f (f 2)
  (* syntaxe `let f x y z = ... in ...` *)
and fac n = if n <= 0 then 1 else n * fac (n - 1)
  (* récursion classique *) (* syntaxe `let rec f x y z = ... in ...` *)
and even x = x = 0 || odd (x - 1)
and odd x = x <> 0 && even (x - 1)
  (* récursion mutuelle *)
in [
  constantes; listes; arith; comparaisons; unaires; tests; cond;
  (andOr, notExp); plus3; deuxPuissanceQuatre; fac 6; (even 42, odd 100);
] ;;
