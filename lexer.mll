{
  open Lexing
  open Parser
  open Miniml
  exception Lexerror

  let pos lexbuf = (lexeme_start lexbuf, lexeme_end lexbuf)

  let advance_line_pos pos =
    { pos with pos_lnum = pos.pos_lnum + 1; pos_bol = pos.pos_cnum; }

  let advance_line lexbuf =
    lexbuf.lex_curr_p <- advance_line_pos lexbuf.lex_curr_p

}

let alph =           ['a'-'z''A'-'Z']
let num  =           ['0'-'9'] 
let decimal	=	'0'|(['1'-'9']['0'-'9']*)
let comment = '(' '*' [^'*']* ('*' (('*'*)|([^'*'')'][^'*']*)))* '*' ')'

rule token = parse
 [' ' '\t']
    { token lexbuf }    (* white space: recursive call of lexer *)
|'\n'
    {advance_line lexbuf; token lexbuf }    (* white space: recursive call of lexer *)
| comment
    { token lexbuf }    (* comment --> ignore *)
| decimal  as i	  { INTCONSTANT (int_of_string i)}
| '('  { LPAREN }
| ')'  { RPAREN }
| '{'  { LBRACE }
| '}'  { RBRACE }
| ','  { COMMA }
| ';'  { SEMICOLON }
| ':'  { COLON }
| '?'  { QMARK }

| "true"       {BCONSTANT true}
| "false"      {BCONSTANT false}
| "fst"        {FST}
| "snd"        {SND}
| "if"         {IF}
| "then"       {THEN}
| "else"       {ELSE}
| "return"     {RETURN}

| "and"        {AND}
| "->"         {ARROW}
| "fun"        {FUN}
| "in"         {IN}
| "let"        {LET}
| "rec"        {REC}
| "type"       {TYPE}

| '+'          { ADD }
| '-'          { SUB }
| '*'          { MUL }
| '/'          { DIV }
| "mod"        { MOD }

| '='          {EQ}
| ">="         {GE}
| '>'          {GT}
| "<="         {LE}
| '<'          {LT}
| "<>"         {NE}

| "&&"         {BLAND}
| "||"         {BLOR}

| eof          {EOF}

| alph(alph|num)* as i  {IDENTIFIER i}

| _  {Printf.printf "ERROR: unrecogized symbol '%s'\n" (Lexing.lexeme lexbuf);
      raise Lexerror }

and
    ruleTail acc = parse
| eof { acc }
| _* as str { ruleTail (acc ^ str) lexbuf }
