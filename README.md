# Projet : Compilation d'un langage fonctionnel vers C

/!\ This file is a work in progress right now. But soon it'll be done.

## Ingrédients de base

Projet testé et développé sous Linux (x86_64, _i.e._ architecture 64 bits) avec les outils suivants :
- `ocaml` : The OCaml toplevel, version 4.07.1
- `make` : GNU Make 4.2.1, Built for x86_64-pc-linux-gnu
- `gcc` : gcc (GCC) 8.3.0
- `bash` : GNU bash, version 5.0.3(1)-release (x86_64-pc-linux-gnu)

J'ai aussi testé brièvement le projet sur un ordinateur de l'université. Il semble fonctionner avec les versions suivantes :
- `ocaml` : version 4.02.3
- `make` : GNU Make 4.1, Construit pour x86_64-pc-linux-gnu
- `gcc` : gcc version 6.3.0 20170516 (Debian 6.3.0-18+deb9u1)
- `bash` : GNU bash, version 4.4.12(1)-release (x86_64-pc-linux-gnu)

Le projet pourrait ne pas fonctionner avec d'autres versions, en particulier plus anciennes. Par exemple, les développeurs de Ocaml ont tendance à ajouter des fonctions à leurs librairies standards assez régulièrement. Et les différentes versions de `bash` ne sont pas toujours compatibles.

Quant au code C de ce projet, je n'ai aucune idée s'il compile et fonctionne correctement sur une architecture 32 bits (même si j'ai essayé de ne pas faire de suppositions sur la taille des pointeurs et des entiers).


## Organisation de ce fichier

Je n'ai pas présenté pas les choses ici dans un ordre synthétique, mais plutôt dans un ordre didactique, afin de profiter au maximum des liens à faire entre les différentes parties du projet. Dans l'ensemble ce document a donc été écrit pour être lu de haut en bas de manière linéaire.

## Terminologie

**Mini-ML** : Mini-langage fonctionnel similaire en syntaxe à OCaml, mais non typé et restreint en fonctionnalité, utilisé comme langage source initial pour ce projet de compilation. Extension de fichier : `.ml`, comme pour OCaml, puisque la syntaxe est virtuellement identique.

**CAM** : Acronyme pour _Categorical Abstract Machine_. Modèle abstrait d'exécution d'un langage fonctionnel de type _ML_ représenté sous la forme d'une suite d'instructions atomiques. Les instructions modifient l'état de la machine virtuelle, qui est constitué d'un triplet de trois valeurs : un **terme** représentant le registre principal d'opération, une **pile** qui fait office de pile d'exécution, et enfin, le **code** restant à exécuter.

[_Référence_ : Cousineau, Curien, Mauny: _The Categorical Abstract Machine_, Science of Computer Programming 8 (1987), pp. 173-202]

**CCAM** : l'implémentation de la CAM que j'ai écrite en C et qui fait office de machine virtuelle pour les instructions compilées en C depuis le code source _Mini-ML_.


## Résumé du projet

Le projet est composé d'un compilateur appelé **comp**, écrit en _OCaml_ à l'aide de _Lex_ et _Yacc_, dont le rôle est de parser et de compiler un fichier source _mini-ML_, produisant alors un fichier intermédiaire en _C_.

Ce fichier intermédiaire peut alors être à son tour compilé avec `gcc` et les sources _C_ de la **CCAM** pour obtenir un fichier exécutable final, dont l'exécution correspond à l'évaluation du fichier _mini-ML_ d'origine.

Le projet contient aussi un simulateur **simu** écrit en _OCaml_ dont le rôle est de parser, compiler et interpréter un fichier source _mini-ML_, toujours selon le modèle de la CAM.


## Entrée en matière

Petit exemple d'utilisation du projet.

```
[miniml-compiler] $ cat -n demo/map.ml
     1	let rec map f xs =
     2	  if is_empty xs
     3	    then []
     4	    else f (head xs) :: map f (tail xs) in
     5	let f x = x + 100 in
     6	map f [1;2;3;4;5;6;] ;;
```
```
[miniml-compiler] $ cd ocaml/
[ocaml] $ make
ocamlc -c miniml.ml
ocamlyacc -v parser.mly
ocamlc -c parser.mli
ocamlc -c parser.ml
ocamllex lexer.mll
103 states, 4561 transitions, table size 18862 bytes
ocamlc -c lexer.ml
ocamlc -c tools.ml
ocamlc -c encoder.ml
ocamlc -c flattener.ml
ocamlc -c codeGenerator.ml
ocamlc -c interf.ml
ocamlc -c comp.ml
ocamlc -o comp miniml.cmo parser.cmo lexer.cmo tools.cmo encoder.cmo flattener.cmo codeGenerator.cmo interf.cmo comp.cmo
ocamlc -c simulator.ml
ocamlc -o simu miniml.cmo parser.cmo lexer.cmo tools.cmo encoder.cmo interf.cmo simulator.cmo
```
```
[ocaml] $ ./simu ../demo/map.ml
[101; 102; 103; 104; 105; 106]
```
```
[ocaml] $ ./comp ../demo/map.ml ../ccam/map.c
Finished compiling miniml to C.
```
```
[ocaml] $ cd ../ccam/
```
```
[ccam] $ make build in=map.c out=map.out
gcc -Wall   \
 -o map.out map.c runtime.c stack.c machine.c value.c enums.c ccam.h
```
```
[ccam] $ ./map.out
[101; 102; 103; 104; 105; 106]
```


## Organisation globale concrète du projet

Le projet est divisé en deux grosses parties :

- _Partie OCaml_ : sous-dossier **ocaml/**. Contient les sources pour le compilateur **comp** et le simulateur **simu**. Un Makefile **ocaml/Makefile** permet de les générer.

- _Partie C_ : sous-dossier **ccam/**. Contient les sources pour la machine virtuelle **CCAM**. Un Makefile **ccam/Makefile** permet de générer un exécutable à partir des ces sources et du code généré par **comp** à partir d'un fichier _mini-ML_ source. Contient aussi des tests unitaires.

Autres fichiers du projet :

- _Jeu de tests_ : sous-dossier **demo/**. Contient un petit nombre de programmes écrits en _Mini-ML_, représentatif des capacités du langage.
- Un script **generate-all.sh** qui permet de générer d'un seul coup un exécutable pour chacun des programmes dans **demo/**, en les compilant, d'abord avec **comp**, puis avec les sources de la **CCAM**.
- **USAGE.txt** : informations d'usages pour tout le projet.


## Usage des fichiers du projet

Lire **USAGE.txt**. Ce fichier contient toutes les informations d'usage pour :
- **ocaml/Makefile** et **ccam/Makefile**
- **comp** et **simu**
- le script **generate-all.sh**


## Organisation du dossier **ocaml/**

Ce qui suit n'est qu'une brève description du contenu de chaque fichier dans ce dossier ; de plus amples explications suivent dans le reste de ce document.

- **Makefile** permet de compiler les exécutables **comp** et **simu**.
- **lexer.mll** et **parser.mly** contiennent le code _Lex_ et _Yacc_ du _lexer_ et du _parser_ _Mini-ML_, respectivement.
- **interf.ml** contient la fonction `parse` qui fait office d'interface entre le _parser_ et le reste du code _OCaml_.
- **miniml.ml** définit le `type mlexp`, dont le but est de représenter l'arbre syntaxique d'un programme _Mini-ML_. Un _pretty-printer_ `pp_prog/pp_exp` est aussi contenu dans ce fichier, mais il n'est plus très utile, c'était un outil de _debugging_ durant les premières tentatives de modification du _parser_ pour visualiser l'arbre syntaxique.
- **encoder.ml** contient les fonctions permettant de transformer un arbre syntaxique de `type mlexp` en suite d'instructions de `type instr`, selon le modèle d'instructions de la CAM vu en cours. Note : ce fichier est grosso modo un remplacement du fichier **instrs.ml**, que j'ai supprimé.
- **simulator.ml** contient la fonction `main` pour l'exécutable **simu**, et le reste du code principal de ce simulateur _OCaml_ d'exécution de la CAM.
- **flattener.ml** contient les fonctions permettant d'´aplatir´ les instructions généré par **encoder.ml** en un nouveau `type flat_instr`, très proche du modèle de données que j'ai utilisé en _C_ dans la **CCAM**.
- **codeGenerator.ml** est responsable de la traduction du `flat_code` en source _C_.
- **comp.ml** contient la fonction `main` pour l'exécutable **comp**.
- **tools.ml** contient des fonctions utilitaires génériques importées par plusieurs modules, comme par exemple la fonction _zip_.


## _Mini-ML_ : description du langage

Fichiers : **lexer.mll**, **parser.mly**, **miniml.ml**

Le langage est similaire à _OCaml_ en syntaxe, mais il n'est pas typé du tout. Ci-dessous le contenu du fichier **demo/language.ml**, qui expose tous les éléments de syntaxe et de sémantique valides en _Mini-ML_ :

```OCaml
(* language.ml *)
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
```

Quelques notes :
- les _let bindings_ `let x = e in t` sont traduits en `(fun x -> t) e`
- les opérations booléennes `(&&)`, `(||)`, et `not`, sont traduites en branchements conditionnels `if..then..else`
- _let-rec bindings_ : `let rec ... and ... in ...`

  Pour éviter les bugs causés par la limitation de cette implémentation de la récursivité, j'ai fait en sorte que le _parser_ refuse tout programme contenant un let-rec inclus comme sous-expression d'une autre expression plus grande. Ainsi l'unique manière autorisée d'utiliser un let-rec dans ce langage _mini-ML_ est de le mettre en expression principale, au plus haut de la hiérarchie du programme.

- valeurs de type `list` : la syntaxe est la même qu'en _OCaml_, c'est-à-dire, `[]`, `2 :: [1]`, `[3;4;]` (trailing semicolon allowed).
- sucre syntactique :

  - `fun x y -> e` devient `fun x -> fun y -> e`.
  - `let f x y = a in b ` devient `let f = fun x -> fun y -> a in b`
  - `let rec f x y = a in b ` devient `let rec f = fun x -> fun y -> a in b`

  Deux opérateurs élémentaires `head` et `tail` ont aussi été rajoutés, dans la même veine que `fst` et `snd`. Un opérateur supplémentaire de test (`is_empty`) permet de vérifier si une valeur est une liste vide. Note : ne provoque pas d'erreur si la valeur n'est pas une liste.

Pour déterminer l'associativité et la précédence de chacun des éléments syntaxiques, je me suis basé sur la documentation d'_OCaml_ trouvé au lien suivant : https://caml.inria.fr/pub/docs/manual-ocaml/expr.html


## Phase d'encodage

Fichiers : **miniml.ml**, **encoder.ml**

Le résultat du _parsing_ d'un fichier source _Mini-ML_ est un arbre syntaxique de `type mlexp` (**miniml.ml**) représentant le programme d'origine. Cet arbre est alors traduit en une liste d'instructions de `type instr` (**encoder.ml**). Cette traduction suit assez fidèlement les règles de compilation vues en cours.

Quelques ajouts et modifications du `type instr` à noter :
- `Halt` : instruction pour indiquer explicitement à la CAM que le programme est terminé, pour qu'elle s'arrête.
- `QuoteBool`, `QuoteInt` : remplace le constructeur `Quote of value` qui n'était pas assez _type-safe_ pour les phases suivantes de la compilation.
- `QuoteEmptyList` : pour implémenter la valeur `[]`, _i.e._ la liste vide.
- `MakeList` : joue le rôle de l'opérateur `::` dans la construction de listes. Calquée sur le modèle de l'instruction `Cons` qui construit des paires à partir du **terme** et de l'élément de tête de la **pile**.


## Simulateur _OCaml_

Fichiers : **simulator.ml**, **simu** (exécutable)

Le code de **simulator.ml** exécute les instructions de `type instr` générées par l'encoder selon le modèle de la CAM vu en cours. L'exécutable **simu** prend en paramètre un fichier **Mini-ML** et lance la simulation d'exécution sur son contenu.

Il y a aussi une option `verbose` (_c.f._ **USAGE.txt**) que l'on peut passer à **simu**, permettant d'afficher une trace de l'évolution de l'état de la CAM durant le processus d'exécution.

Je décris ci-dessous les principaux types utilisés dans le simulateur. Le double intérêt de cette description est que la deuxième implémentation de la CAM, celle que j'ai réalisée en _C_ (**CCAM**), est vraiment très proche de cette implémentation-ci en _OCaml_ (aussi proche que les différences de paradigme et d'abstraction entre _C_ et _OCaml_ me l'ont permis).

- `type value` : valeurs manipulées par la CAM.
- `type code = instr list` : les instructions dont on simule l'exécution.
- `type stack` : pile d'exécution de la CAM (permet de stocker valeurs intermédiaires et code).
- `type defstack` : pile de définitions, utilisée pour implémenter la récursivité autorisée par les _let-rec bindings_. C'est une liste d'association `(var * code) list` qui associe des noms de variables récursives à leur code.
- `type machine_state = (value * code * stack * defstack)` : ce quadruplet représente l'état de la CAM à tout instant durant la simulation, c'est l'état sur lequel les instructions opèrent. Le premier élément est appelé le **terme**, et fait office de registre principal.

- `type status` : utilisé pour savoir quand s'arrêter (à la fin du programme ou bien lorsqu'on tombe sur une erreur).

    ```OCaml
    type status
      = AllOk of machine_state
      | Stopped of final_status
    and final_status
      = Halted of value
      | Crashed of string
    ;;
    ```
Le flot d'exécution du simulateur est lui aussi similaire à celui de la **CCAM**. En voici une courte description :
-  `main` appelle `run_simulation` avec l'option `verbose` et le `code` compilé par l'encoder.
- `run_simulation` construit l'état initial de la machine à l'aide de `blank_state` et du `code` récupéré. Puis elle lance la simulation à l'aide de `run_machine`, qui prend en paramètre `ms : machine_state` et l'option `verbose`.
- `run_machine` est une fonction récursive. Si l'option `verbose` est activée, elle commence par afficher l'état de la machine : valeur du **terme**, valeur de sommet de **pile**, prochaine instruction à exécuter. Une fois cela fait, elle appelle `execute_next_instruction` avec `ms` en paramètre.
- `execute_next_instruction` correspond au cœur du travail de la simulation : elle lit la prochaine instruction à exécuter, modifie `ms` en conséquence, et retourne une valeur de `type status`. Selon les cas :

    - si l'instruction s'avère être `Halt` : la valeur de retour est alors `Stopped (Halted term)` où `term` correspond à la valeur finale calculée par la machine
    - sinon, et si l'instruction ne provoque pas d'erreurs : la valeur de retour est alors `AllOk new_ms`, où `new_ms` est le nouvel état de la machine calculé
    - sinon, une erreur s'est produite (_e.g._ `fst 4`) : la valeur de retour est alors `Stopped (Crashed error_msg)`.

- de retour dans `run_machine` : on _match_ sur le `status` récupéré. Si `AllOk`, on rappelle récursivement `run_machine` avec le nouvel état de la machine récupéré. Si `Stopped`, on retourne le `final_status`.
- de retour dans `run_simulation` : selon que le constructeur de `final_status` soit :

    - `Halted`, on affiche la valeur finale calculée par la machine
    - `Crashed`, on affiche le message d'erreur

    puis, dans les deux cas, on termine la simulation.


## Représentation des instructions dans la **CCAM**

En _C_, la manière la plus simple de représenter une suite d'instructions est de l'enregistrer sous la forme d'un tableau. Comme les instructions du modèle de la CAM contiennent parfois des paramètres (par exemple l'instruction `QuoteBool` contient une valeur littérale booléenne), le type des éléments du tableau doit être suffisamment flexible pour pouvoir contenir au besoin :

- une constante représentant une instruction, comme `QuoteBool` ou `Swap`
- une valeur littérale (entier ou booléen)
- une constante représentant une opération particulière (`Plus`, `Fst`, `Eq`, etc)
- un pointeur vers un autre morceau de code, c'est-à-dire vers un autre tableau d'instruction

Ce quatrième type de cellule est nécessaire pour le cas d'instructions qui sont censées "contenir" du code, comme l'instruction `Branch`, qui, en tant que constructeur pour le `type instr` en _OCaml_, est censé lui-même contenir les instructions pour les branches `if` et `else`.

C'est cette stratégie de tableau à contenu variable que j'ai employé pour encoder les instructions en _C_ dans la **CCAM**. Le type d'une cellule de tableau de code s'appelle `CodeT` (déclaré dans le fichier d'en-tête **ccam.h**) :
```C
/* ccam.h */
union CodeT; typedef union CodeT {
  int instruction;
  int operation;
  union CodeT *reference;
  long data;
} CodeT;
```
Pour info, le "`union CodeT;`" placé juste avant le `typedef` est nécessaire pour permettre une définition récursive : la ligne `union CodeT *reference;` nécessite que `union CodeT` soit déclaré _a priori_.

Les unions en _C_ s'utilisent assez similairement à un type somme ; en pseudo-code _OCaml_ la même structure pourrait être définie ainsi :
```OCaml
type code_t = Instruction of int | Operation of int
  | Code of code_t pointer | Data of long ;;
```
Par similarité de concept, j'appellerai "constructeur" les différents membres possibles d'une `union` -- ici `.instruction`, `.operation`, `.reference` et `.data`.

En _C_ une valeur de type union se définit de la sorte :
```C
CodeT foo = { .operation = 3 } ;
// alternative :
CodeT bar; bar.reference = &foo;
```
Ici on a choisit le constructeur `.operation` pour `foo`, et le constructeur `.reference` pour `bar`. Une valeur de type `union` n'est pas censée être utilisée avec plus d'un constructeur (sous peine de comportements imprévisibles).

Pour revenir à la représentation des instructions de la CAM dans l'implémentation **CCAM** : l'exemple qui suit sera peut-être plus parlant.
```OCaml
let (main_code : instr list) = [
  QuoteInt(3);
  Swap;
  PrimInstr (UnOp (Fst));
  Branch([Push], [App])
] ;;
```
Il s'agit d'une liste d'instructions quelconque, du même type que celles produites par **encoder.ml** et consommées par **simulator.ml**.

Voici maintenant sa représentation en _C_ pour la **CCAM**  après avoir subi tous les traitements effectués par **comp** :
```C
#include "ccam.h"

CodeT if_branch0[];
CodeT else_branch1[];
CodeT main_code[];

CodeT if_branch0[] =
{
    {.instruction = Push},
};
CodeT else_branch1[] =
{
    {.instruction = Apply},
};
CodeT main_code[] =
{
    {.instruction = QuoteInt},{.data = 3L},
    {.instruction = Swap},
    {.instruction = Unary},{.operation = Fst},
    {.instruction = Branch},{.reference = if_branch0},{.reference = else_branch1},
};
```

Les instructions comme `QuoteInt` ou `Unary` ou `Branch`, qui contiennent des paramètres, sont codées sur plusieurs cellules du tableau. Le constructeur de l'`union CodeT` choisi pour chaque chaque cellule est adapté à son contenu.

Le tableau `main_code` dans le code _C_ précédent est spécial dans le sens que c'est le point de départ du programme exécuté par la **CCAM**. Il est d'ailleurs directement utilisé dans la fonction `main()` de **runtime.c**.

Tous les autres morceaux de code qui peuvent avoir été générés, comme ici `else_branch1` et `if_branch0`, proviennent d'instructions comme `Branch` ou `Call`. Ces autres tableaux "périphériques" sont reliées les uns aux autres et au tableau `main_code` par le biais des cellules de `.reference`, comme ici `{.reference = if_branch0}`.


## Phase de _code flattening_

Fichier : **flattener.ml**

Pour passer d'instructions du `type instr` (avec des constructeurs comme `Branch of instr list * instr list`), à une représentation en _C_ "plate" (sans structure directement récursive), on a besoin de passer par un format d'instructions intermédiaire qui soit débarrassé de cette récursion structurelle.

C'est le rôle de `type flat_instr` et `type flat_code = flat_instr list`, définis dans **flattener.ml**. Essentiellement, on remplace chaque constructeur souffrant de récursion structurelle par un nouveau constructeur "plat" ne contenant plus que des _placeholders_ (de type `string`). Ces chaînes de caractère correspondent dans la représentation finale en _C_ aux noms des tableaux périphériques référencés : `"if_branch0"` par exemple.

Le code _OCaml_ du fichier **flattener.ml** est particulièrement compliqué/pénible, avec beaucoup de récursion, de récursion mutuelles, et une foule de variables difficiles à nommer. La raison en est que l'opération qui permet de transformer une `instr list` en `flat_code` est une opération particulièrement _stateful_. En particulier, il faut conserver et mettre à jour un compteur de type `int` (que j'ai généralement appelé `n` ou `nextN`, etc) pour s'assurer que les _placeholders_ que l'on utilise sont uniques (puisqu'ils correspondent à des noms de tableaux de `CodeT` en _C_).

Un point qui mérite peut-être quelques explications : le traitement des instructions `Call`, `AddDefs` et `RmDefs` durant la traduction de `type instr` vers `flat_code`. Par soucis de simplicité et d'efficacité, toutes les définitions contenues dans toutes les instructions `AddDefs` (à travers tout le programme) sont automatiquement transcrites en tableaux périphériques uniques et isolés (comme pour les branches `if` et `else` de `Branch`).

À partir de là, on n'a plus vraiment besoin des instructions `AddDefs` et `RmDefs` dans le type `flat_instr`, ni, par extension, dans le modèle d'exécution de la **CCAM**. Il suffit de faire en sorte que `FlatCall` (la version "plate" du constructeur `Call`) contienne le _placeholder_ correspondant au bon _let-rec binding_, et de définir dans la **CCAM** le comportement de cette instruction comme étant identique à un `Branch` non conditionnel : un seul chemin qui est forcément emprunté.

Il faut de plus s'assurer que le code du _binding_ termine bien par l'instruction `Return` (comme pour le cas des branchements conditionnels), chose qui n'était pas prévue dans le modèle d'origine (celui du simulateur _OCaml_).

Je n'en dirai pas plus, l'opération de _code flattening_ est de loin la plus compliquée à mettre en mots. Le résultat final de cette phase de compilation est une `fragment list`, avec `type fragment = string * flat_code`. En d'autres termes, on récupère une liste d'association de _placeholders_ et de `flat_instr list` qui sont prêts à être traduits en _C_ en tableaux de `CodeT`.


## Phase de génération de code _C_

Fichier : **codeGenerator.ml**

Une fois qu'on a récupéré la `fragment list` de la phase précédente, il ne reste plus grand chose à faire : simplement à construire la chaîne de caractère qui sera le contenu du fichier _C_ généré par **comp**. L'opération n'a rien de très compliqué ni rien de très intéressant : c'est essentiellement juste un _printer_.


## Lien entre **CCAM** et **simu**

Autant que possible, j'ai essayé de conserver les mêmes noms, entre la **CCAM** et le simulateur _OCaml_ (**simulator.ml**), pour les types et les fonctions qui remplissent des rôles équivalents dans les deux implémentations :
- `type value ---> struct ValueT`
- `PairV ---> struct PairT`, `ListConsV ---> struct ListConsT`, etc.
- `type stack ---> struct StackT`
- `type machine_state ---> struct MachineStateT`
- `type status ---> enum Status`
- `Halted/AllOk`
- `run_machine()`, `execute_next_instruction()`, `blank_state()`

Note : le type `MachineStateT` est un triplet, au lieu d'un quadruplet comme `machine_state`, ceci car on s'est débarrassé du besoin du paramètre `defstack` lors de la phase de _code flattening_.
```C
/* ccam.h */
typedef struct MachineStateT {
  ValueT *term;
  CodeT *code;
  StackT *stack;
} MachineStateT;
```
Le champ `code` correspond à un pointeur vers le début de la prochaine instruction à exécuter.

Le flot d'exécution de la **CCAM** est également volontairement très similaire à celui du simulateur ; _c.f._ la section là-dessus un peu plus bas.


## Organisation du dossier **ccam/**

Ce dossier contient les sources de mon implémentation de la CAM en C. Fichiers notables :

- **Makefile** est le fichier `make` pour la **CCAM** (_c.f._ **USAGE.txt**).
- **ccam.h** est le fichier d'en-tête principal. Il définit les `struct`, `union` et `enum` utilisés par la **CCAM**, et déclare aussi toutes les fonctions définies dans les fichiers **enums.c**, **value.c**, **stack.c** et **machine.c**.
- **enums.c** regroupe diverses fonctions utilitaires qui concernent les divers `enum` utilisés dans la **CCAM**.
- **value.c** contient les fonctions de construction et de _pattern-matching_ pour le type `ValueT`.
- **stack.c** contient les divers constructeurs et fonctions de _pattern-matching_ pour le type `StackT`.
- **machine.c** contient les constructeurs pour le type `MachineStateT`, ainsi que toutes les fonctions d'exécution individuelles des diverses instructions : `exec_Apply()`, `exec_Branch()`, etc.
- **runtime.c** contient la fonction `main()` de la **CCAM**.
- **unit-tests.c** contient des tests unitaires pour vérifier le comportement de chaque instruction et des divers constructeurs de `ValueT` (_c.f._ **USAGE.txt** pour comment lancer les tests).
- **testing.h** et **testing.c** contiennent du code nécessaire uniquement pour les tests unitaires.


## Flot d'exécution de la **CCAM**

La fonction `main()` se trouve dans **runtime.c**. À partir du pointeur `main_code` (définit dans le code généré par **comp**), un état initial pour la machine virtuelle est créé, via la fonction
```C
MachineStateT *blank_state(CodeT *code) /* machine.c */
```
Puis on lance la fonction :
```C
enum Status run_machine(MachineStateT *ms, int verbose) /* machine.c */
```
Elle réalise une boucle `while` qui ne prend fin que lorsque le `status` de la machine virtuelle passe de `AllOk` à une autre valeur (`Halted` si l'instruction `Halt` est atteinte, une valeur d'erreur dans tout autre cas).

Dans le corps de cette boucle `while`, on appelle alors la fonction
```C
enum Status execute_next_instruction(MachineStateT *ms) /* enum.c */
```
Elle lit la prochaine cellule de code depuis l'état (`ms`) de la machine virtuelle. :
```C
/* enum.c: execute_next_instruction() */
int instruction = ms->code[0].instruction;
```
Cet entier doit normalement contenir une constante de `enum instruction`.

Un `switch(instruction)` réalise alors le choix de la bonne fonction d'exécution à appeler. Par exemple :
```C
/* enum.c: execute_next_instruction() */
case Halt: status = exec_Halt(ms); break;
case Branch: status = exec_Branch(ms); break;
// etc.
```
Toutes les fonctions de la forme `exec_Zzzz(ms)` sont définies dans le fichier **machine.c**.

La fonction d'exécution choisie par le `switch` (par exemple `exec_Apply()`) modifie alors l'état de la machine virtuelle d'une certaine manière. Elle peut :
- modifier le **terme** `ms->term`, qui est une `ValueT`,
- modifier le pointeur **code** `ms->code`, qui est du type `CodeT *`,
- modifier la **pile** `ms->stack`, qui est une `StackT`

En fait, sauf dans le cas d'instructions comme `Branch` ou `Call`, qui changent radicalement la valeur de `ms->code`, toutes les autres instructions incrémentent `ms->code` du bon nombre de cellules pour pointer à l'instruction suivante.

Par exemple :
- l'instruction `Push` copie le **terme** et l'ajoute au dessus de la **pile**, puis incrémente le pointeur **code** de 1 (car l'instruction `Push` est encodée sur une seule cellule de type `CodeT`).
- L'instruction `Branch` :
  - vérifie que le **terme** est un booléen,
  - ajoute à la **pile** le pointeur vers l'instruction qui suit, c'est-à-dire `ms->code + 3` puisque l'instruction `Branch` est encodée sur trois cellules,
  - remplace le pointeur **code** par la cellule :
    - `ms->code[1]` si le **terme** vaut `true`, ce qui correspond à choisir la branche _if_, _i.e._ le premier paramètre de `Branch`,
    - `ms->code[2]` si le **terme** vaut `false` (branche _else_).

Et ainsi de suite pour toutes les autres instruction.

À la fin de l'exécution, on affiche un message d'erreur en cas d'erreur, dans le cas contraire on affiche la valeur finale du **terme** principal de l'état de la machine virtuelle (_i.e._, `ms->term`), à l'aide de
```C
void print_value(ValueT *value) /* value.c */
```


## Modèle de données du code _C_

Fichiers : **ccam.h**

Le modèle de données de la **CCAM** est fortement inspiré du modèle de données d'un langage fonctionnel. On utilise un tag au lieu de constructeurs, et le _pattern-matching_ est fait par des fonctions individuelles comme `match_value_with_pair()` (**value.c**), mais cela mis à part, j'ai essayé de garder le même esprit.

Puisqu'une valeur (`ValueT`) manipulée par la **CCAM** peut être selon les cas un booléen, un entier, une valeur nulle (`NullV()`), une paire, etc, la solution que j'ai choisie a été de définir `ValueT` comme une structure comprenant trois champ distincts : `copy_count` pour gérer la mémoire, `tag` pour identifier le type de valeur en question (fait office de constructeur, au sens qu'il permet de faire un _pattern-match_ sur une valeur), et `as` (comme le mot anglais qui signifie ici "considéré en tant que") pour contenir le vrai contenu de la valeur :

```C
typedef struct ValueT {
  int copy_count;
  enum ValueTag tag;
  union {
    long integer;
    long boolean;
    PairT pair;
    ClosureT closure;
    ListConsT listcons;
  } as;
} ValueT;
```

Le membre `as` est donc une union. Au besoin, on pourra définir donc définir une `ValueT` de différentes manières. Les commentaires dans l'exemple suivant font le lien avec la syntaxe _OCaml_.

```C
ValueT a, b, c, d; CodeT *x = NULL;
a.tag = ValueIsInt;
  a.as.integer = 3; // let a = IntV(3) ;;
b.tag = ValueIsBool;
  b.as.boolean = True; // let b = BoolV(true) ;;
c.tag = ValueIsClosure;
  c.as.closure.code = x;
  c.as.closure.value = a; // let c = ClosureV([], a) ;;
d.tag = ValueIsPair;
  d.as.pair.first = b;
  d.as.pair.second = c; // let d = PairV(b, c) ;;
```
Et ainsi de suite.

Bien sûr, la construction de nouvelles valeurs est une tâche répétitive. Les fonctions de construction suivantes simplifie le processus :

```C
//| value.c
ValueT *PairValue(ValueT *first, ValueT *second);
ValueT *ClosureValue(CodeT *code, ValueT *closure_value);
ValueT *BoolValue(long b);
ValueT *IntValue(long x);
ValueT *NullValue();
ValueT *EmptyListValue();
ValueT *ListConsValue(ValueT *head, ValueT *tail);
```

Réécriture de l'exemple précédent avec ces fonctions de constructions (et en travaillant avec des pointeurs) :
```C
ValueT *d = PairValue(BoolValue(True), ClosureValue(NULL, IntValue(3)));
```


Tout ce qui vient d'être dit pour `ValueT` s'applique aussi au type `StackT` :
```C
/* ccam.h */
typedef struct StackT {
  enum StackTag tag;
  union {
    ValueOnStackT with_value;
    CodeOnStackT with_code;
  } as;
} StackT;
```
 qui correspond aux types _OCaml_ :
```OCaml
type stackelem = Val of value | Cod of code and stack = stackelem list
```

Une différence tout de même entre `StackT` et `ValueT` : les objets alloués de type `StackT` ne sont pas gérés par _reference counting_ (on n'en a pas besoin).

Exemple d'utilisation de `StackT` :
```C
StackT s, t, u;
s.tag = StackIsEmpty; // let s = [] ;;
t.tag = StackTopIsValue;
  t.as.with_value.top = IntValue(3);
  t.as.with_value.bottom = s; // let t = Val(IntV(3)) :: s ;;
u.tag = StackTopIsCode;
  u.as.with_code.top = foo;
  u.as.with_code.bottom = t; // let u = Cod(foo) :: t ;;
```
Et les fonctions de construction pour `StackT` qui automatisent ce processus :
```C
//| stack.c
StackT *EmptyStack();
StackT *ValueOnStack(ValueT *value, StackT *old_stack);
StackT *CodeOnStack(CodeT *code, StackT *old_stack);
```
Réécriture de l'exemple précédent pour en faire usage :
```C
StackT *u = CodeOnStack(foo, ValueOnStack(IntValue(3), EmptyStack()));
```


## Pattern-matching

Une fois que l'on a instancié une `ValueT` ou une `StackT`, on veut pouvoir effectuer un _pattern-matching_, pour :
- vérifier que le type d'objet correspond bien à ce que l'on veut. Par exemple, un entier, une liste non vide, une valeur nulle, une pile avec du code (pointeur vers `CodeT`) comme élément de tête, une pile avec une `ValueT` comme élément de tête, etc.
- si c'est bien le cas, récupérer le contenu de la valeur. Par exemple :
  - si c'est une paire, récupérer les deux champs `first` et `second` de la structure `PairT` contenue dans le champ `as` de la valeur en question
  - si c'est une pile avec une `ValueT` au-dessus, récupérer les champs `top` (`ValueT *`) et `bottom` (`StackT *`) de la structure `ValueOnStackT`, correspondant respectivement à la `ValueT` qui constitue le dessus de pile, et au reste de la pile
- une fois cela fait, pour éviter les fuites de mémoire, on doit alors libérer la mémoire (si nécessaire) de l'objet que l'on vient de déconstruit par _pattern-matching_.

C'est le travail qu'effectuent les fonctions suivantes :
```C
//| value.c
enum result match_value_with_pair(ValueT *value, PairT *output);
enum result match_value_with_closure(ValueT *value, ClosureT *output);
enum result match_value_with_boolean(ValueT *value, long *output);
enum result match_value_with_integer(ValueT *value, long *output);
enum result match_value_with_list_cons(ValueT *value, ListConsT *output);
enum result match_value_with_empty_list(ValueT *value);
//| stack.c
enum result match_stacktop_with_value(StackT *stack, ValueOnStackT *output);
enum result match_stacktop_with_code(StackT *stack, CodeOnStackT *output);
```

Exemple d'utilisation :
```C
ValueT *x = PairValue(IntValue(3), BoolValue(True));

PairT pattern; // <- pattern à remplir par le matching plus bas

// les fonctions de matching retournent une des deux valeurs
// `Success` ou `Failure` de `enum result`
if (match_value_with_pair(x, &pattern) == Success) {
  // success: cela implique que x a été libéré (sauf s'il existe
  // à d'autres endroits en mémoire), et que son contenu a été
  // transféré dans pattern.first (qui contient maintenant IntValue(3))
  // et pattern.second (qui contient maintenant BoolValue(True))
}
else {
  // matching failure: cela implique que `x` est intact, n'a pas été
  // libéré en mémoire, et `pattern` n'a pas été modifié, donc
  // en particulier n'est toujours pas initialisé
}
```
Toutes les fonctions de _pattern-matching_ fonctionnent selon ce modèle.


## Gestion de la mémoire

Les objets de type `StackT` sont libérés au fur et à mesure par _pattern-matching_. Ils n'ont jamais besoin d'être copiés, donc on n'a pas besoin de les gérer par _reference counting_.

À l'inverse, les objets de type `ValueT` sont susceptibles d'être copiés fréquemment, du fait de l'existence de l'instruction `Push`, qui place une copie du **terme** principal sur la **pile**.

Au lieu de réellement copier les `ValueT` récursivement, il est plus économe de simplement incrémenter un champ interne `copy_count` (récursivement là encore), et de modifier le système de libération de mémoire pour que les objets de type `ValueT` ne soient libérés pour de vrai que lorsque leur `copy_count` tombe à zéro.

Les fonctions assurant ce travail de _mini garbage collecting_ (**value.c**):
```C
ValueT *malloc_value();
  // ^ alloue une nouvelle ValueT et initialise son copy_count
void free_value(ValueT *value);
  // ^ décrémente le copy_count, et éventuellement libère la ValueT
void deepfree_value(ValueT *value);
  // ^ version récursive de free_value(), qui fait appel à free_value()
void deepincrement_copy_count(ValueT *value);
  // ^ incrémente de 1 le copy_count d'une valeur, et récursivement
  // de toutes les valeurs contenues dans cette valeur (si par exemple
  // il s'agit d'une paire)
ValueT *deepcopy_value(ValueT *value);
  // ^ fait semblant de copier une valeur,
  // mais ne fait qu'appeler deep_increment_copy_count()
  // avant de renvoyer son argument d'entrée tel quel
```

Les variables globales suivantes sont uniquement utilisées pour vérifier (durant le _debugging_) le comportement du _reference counting_ :
```C
int mallocated_values_count;
int freed_values_real_count;
int freed_values_fake_count;
void memory_value_report();
  // ^ affiche un court rapport d'information
  // se basant sur les trois variables globales précédentes
```
