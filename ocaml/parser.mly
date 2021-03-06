%{
open Miniml

let primop_of_token = (function
  | ADD -> BinOp (BArith BAadd)
  | SUB -> BinOp (BArith BAsub)
  | MUL -> BinOp (BArith BAmul)
  | DIV -> BinOp (BArith BAdiv)
  | MOD -> BinOp (BArith BAmod)
  | EQ -> BinOp (BCompar BCeq)
  | GE -> BinOp (BCompar BCge)
  | GT -> BinOp (BCompar BCgt)
  | LE -> BinOp (BCompar BCle)
  | LT -> BinOp (BCompar BClt)
  | NE -> BinOp (BCompar BCne)
  | _ -> failwith "in primop_of_token: unknown token")

let binary_exp e1 oper e2 = App(PrimOp (primop_of_token oper), Pair(e1, e2))

let rec has_item x = function
  | [] -> false
  | y :: ys -> (x = y) || has_item x ys
;;

%}

%token <string> IDENTIFIER
%token <bool> BCONSTANT
%token <int> INTCONSTANT
%token FST SND
%token ADD SUB MUL DIV MOD
%token EQ GE GT LE LT NE
%token BLAND BLOR NOT
%token LPAREN RPAREN LBRACE RBRACE
%token COMMA SEMICOLON COLON QMARK
%token IF THEN ELSE WHILE FOR RETURN
%token AND ARROW FUN IN LET REC TYPE
%token EOF
%token LIST_CONS HEAD TAIL LBRACKET RBRACKET
%token IS_EMPTY

%right ELSE

%start start
%type <Miniml.prog> start

%%

start: prog { $1 }
;

prog:
  typedef let_rec_exp end_marker_opt EOF
  { Prog ($1, $2) }
;

end_marker_opt:
   /* empty */
  { }
| SEMICOLON SEMICOLON
  { }
;

/* TODO: */
typedef:
   /* empty */
     { None }
|  TYPE IDENTIFIER
  {  Some ($2) }
;

/* LET REC is only allowed as the topmost level
   expression of a program */
let_rec_exp
  : LET REC let_rec_definitions IN main_exp { Fix ($3, $5) }
  | main_exp { $1 }
;

let_rec_definitions
  : let_binding { [$1] }
  | let_binding AND let_rec_definitions
    { let x = fst $1 in
      if (has_item x (List.map fst $3))
      then (failwith ("Variable " ^ x ^ " bound twice in the same let-rec construct."))
      else ($1 :: $3) }
;

/* used both by let-rec bindings and normal let bindings: */
let_binding
  : IDENTIFIER EQ main_exp { ($1, $3) }
  | IDENTIFIER let_binding
    /* to handle the syntactic sugar
       let f x y = e ---> let f = fun x -> fun y -> e */
    { let (v, body) = $2 in ($1, Fn(v, body)) }
;

main_exp
  : let_in_exp { $1 }
  | FUN IDENTIFIER func_body { Fn($2, $3) }
  | IF main_exp THEN main_exp ELSE main_exp { Cond($2, $4, $6) }
  | pair_exp { $1 }
;


/* (let x = e in a) gets translated into ((fun x -> a) e) */
let_in_exp
  : LET let_binding IN main_exp
  { let (x, e) = $2 in App(Fn(x, $4), e) }
;

/* allows the syntax fun x y z -> e */
func_body
  : IDENTIFIER func_body { Fn ($1, $2) }
  | ARROW main_exp { $2 }
;;

/* right associative (kinda like lists).
  allows the absence of parentheses:
  (1, 2, 3) == (1, (2, 3)) */
pair_exp
  : or_exp COMMA pair_exp { Pair ($1, $3) }
  | or_exp { $1 }
;

/* (||), (&&): right associative for the short-circuiting to work
  from left to right:
    a || b || c -> a || (b || c)
  same for (&&).
*/
or_exp
  : and_exp BLOR or_exp { Cond($1, Bool(true), $3) }
  | and_exp { $1 }
;
and_exp
  : compare_exp BLAND and_exp { Cond($1, $3, Bool(false)) }
  | compare_exp { $1 }
;

/* a == b == c is deemed invalid */
compare_exp
  : list_exp compare_op list_exp { binary_exp $1 $2 $3 }
  | list_exp { $1 }
;

compare_op
  : EQ { EQ } | GE { GE } | GT { GT }
  | LE { LE } | LT { LT } | NE { NE }
;

/* e.g.: (1, 2) :: (5, 6) :: [] */
list_exp
  : plus_exp LIST_CONS list_exp { ListCons($1, $3) }
  | plus_exp { $1 }
;

/* left associative, says ocaml's doc */
plus_exp
  : plus_exp ADD mult_exp { binary_exp $1 ADD $3 }
  | plus_exp SUB mult_exp { binary_exp $1 SUB $3 }
  | mult_exp { $1 }
;

/* left associative, says ocaml's doc */
mult_exp
  : mult_exp MUL app_exp { binary_exp $1 MUL $3 }
  | mult_exp DIV app_exp { binary_exp $1 DIV $3 }
  | mult_exp MOD app_exp { binary_exp $1 MOD $3 }
  | app_exp { $1 }
;

/* application is left associative: f a b -> (f a) b */
app_exp
  : app_exp lowest_exp { App($1, $2) }
  | NOT lowest_exp { Cond($2, Bool(false), Bool(true)) }
  | lowest_exp { $1 }
;

/* terminal values and parentheses */
lowest_exp
  : LPAREN main_exp RPAREN { $2 }
  | LBRACKET sugary_list RBRACKET { $2 }
  | IDENTIFIER { Var($1) }
  | BCONSTANT { Bool($1) }
  | INTCONSTANT { Int($1) }
  | unary { PrimOp (UnOp($1)) }
  | test { PrimOp (TestOp($1)) }
  /* OMITTED: string-literal */
;

/*
  syntax sugar for lists:
  [1; 2; 3]
  [2] -- singleton
  [1;2;]
  (trailing semicolon is allowed)
*/
sugary_list
  : { EmptyList } /* nothing in between the brackets */
  | main_exp { ListCons($1, EmptyList) }
  | main_exp SEMICOLON sugary_list { ListCons($1, $3) }
;

unary
  : FST { Fst }
  | SND { Snd }
  | HEAD { Head }
  | TAIL { Tail }
;

test
  : IS_EMPTY { IsEmpty }
;
