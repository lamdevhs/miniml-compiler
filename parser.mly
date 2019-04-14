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
%}

%token <string> IDENTIFIER
%token <bool> BCONSTANT
%token <int> INTCONSTANT
%token FST SND
%token ADD SUB MUL DIV MOD
%token EQ GE GT LE LT NE
%token BLAND BLOR
%token LPAREN RPAREN LBRACE RBRACE
%token COMMA SEMICOLON COLON QMARK
%token IF THEN ELSE WHILE FOR RETURN 
%token AND ARROW FUN IN LET REC TYPE
%token EOF

%right ELSE

%start start
%type <Miniml.prog> start

%%

start: prog { $1 }
;

prog:
  typedef main_exp end_marker_opt EOF
  {Prog ($1, $2) }
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

main_exp
  : let_in_exp { $1 }
  | FUN IDENTIFIER func_body { Fn($2, $3) }
  | IF main_exp THEN main_exp ELSE main_exp { Cond($2, $4, $6) }
  | LET REC let_rec_definitions IN main_exp { Fix ($3, $5) }
  | pair_exp { $1 }
;

let_rec_definitions
  : let_rec_binding { [$1] }
  | let_rec_binding AND let_rec_definitions { $1 :: $3 }
;

let_rec_binding
  : IDENTIFIER EQ main_exp { ($1, $3) }
;

/* (let x = e in a) gets translated into ((fun x -> a) e) */
let_in_exp
  : LET IDENTIFIER EQ main_exp IN main_exp { App(Fn($2, $6), $4) }
;

/* allows the syntax fun x y z -> e */
func_body
  : IDENTIFIER func_body { Fn ($1, $2) }
  | ARROW main_exp { $2 }
;;

/* left associative (kinda random choice). allows the absence of parentheses. */
pair_exp
  : pair_exp COMMA or_exp { Pair ($1, $3) }
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
  : plus_exp EQ plus_exp { binary_exp $1 EQ $3 }
  | plus_exp GE plus_exp { binary_exp $1 GE $3 }
  | plus_exp GT plus_exp { binary_exp $1 GT $3 }
  | plus_exp LE plus_exp { binary_exp $1 LE $3 }
  | plus_exp LT plus_exp { binary_exp $1 LT $3 }
  | plus_exp NE plus_exp { binary_exp $1 NE $3 }
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
  | lowest_exp { $1 }
;

/* terminal values and parentheses */
lowest_exp
  : LPAREN main_exp RPAREN { $2 }
  | IDENTIFIER { Var($1) }
  | BCONSTANT { Bool($1) }
  | INTCONSTANT { Int($1) }
  | unary { PrimOp (UnOp($1)) }
  /* OMITTED: string-literal */
;

unary
  : FST { Fst }
  | SND { Snd }
;