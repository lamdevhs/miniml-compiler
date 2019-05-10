
# Projet : Compilation d'un langage fonctionnel vers C

## Terminologie

**Mini-ML** : Mini-langage fonctionnel similaire en syntaxe à OCaml, mais non typé et restreint en fonctionnalité, utilisé comme langage source initial pour ce projet de compilation. Extension de fichier : `.ml`.

**CAM** : Acronyme pour _Categorical Abstract Machine_. Modèle abstrait d'exécution d'un langage fonctionnel de type _ML_ sous la forme d'une suite d'instructions atomiques. Les instructions modifient l'état de la machine virtuelle, qui est constitué d'un triplet de trois valeurs : un **terme** représentant le registre principal d'opération, une **pile** qui fait office de pile d'exécution, et enfin, le **code** lui-même restant à exécuter. _Référence_ : Cousineau, Curien, Mauny: _The Categorical Abstract Machine_, Science of Computer Programming 8 (1987), pp. 173-202.

**CCAM** : l'implémentation de la CAM que j'ai écrite en C et qui fait office de machine virtuelle pour les instructions compilées en C depuis le code source _mini-ML_.


## Résumé du projet

Le projet est composé d'un compilateur appelé **comp**, écrit en _OCaml_ à l'aide de _Lex_ et _Yacc_, dont le rôle est de parser et de compiler un fichier source _mini-ML_, produisant alors un fichier intermédiaire en _C_, qui peut alors être à son tour compilé avec `gcc` et les sources de la **CCAM** (écrite en _C_) pour obtenir finalement un fichier exécutable, dont l'exécution correspond à l'évaluation du fichier _mini-ML_ d'origine.
Le projet est aussi composé d'un simulateur **simu** écrit en _OCaml_ dont le rôle est de parser, compiler et interpréter un fichier source _mini-ML_, toujours selon le modèle de la CAM.


## Organisation globale concrète du projet

Le projet est divisé en deux grosses parties :

- **Partie OCaml** : sous-dossier **ocaml/**. Contient les sources pour le compilateur **comp** (_mini-ML_ --> _C_) et le simulateur **simu**. Un Makefile **ocaml/Makefile** permet de les générer.

- **Partie C** : sous-dossier **ccam/**. Contient les sources pour la machine virtuelle **CCAM**. Un Makefile **ccam/Makefile** permet de générer un exécutable à partir des ces sources et du code généré par **comp** à partir du fichier _mini-ML_ source. Contient aussi des tests unitaires.

Autres fichiers du projet :

- **Jeu de tests** : sous-dossier **demo**. Contient divers programmes écrits en _Mini-ML_.
- **generate-all.sh** : script qui permet de générer des exécutables pour chacun des programmes dans **demo**, en les compilant, d'abord avec **comp**, puis avec **ccam/Makefile**.
- **USAGE.txt** : informations sur comment utiliser le projet.


## Usage des fichiers du projet

Lire **USAGE.txt**. Ce fichier contient toutes les informations d'usage pour :
- **ocaml/Makefile** et **ccam/Makefile**
- **comp** et **simu**
- le script **generate-all.sh**


## Mini démonstration

Juste pour briser la glace :
```sh
$
```


## Organisation du dossier **ocaml/**

Sources du compilateur **comp** `Mini-ML -> C` et du simulateur **simu**, tous deux écrits en _OCaml_. Ce qui suit n'est qu'une brève description du contenu de chaque fichier dans ce dossier ; de plus amples explications suivent dans le reste de ce document.

- **Makefile** permet de compiler les exécutables **comp** et **simu**. _c.f._ **USAGE.txt**.
- **lexer.mly** et **parser.mly** contiennent le code _Lex_ et _Yacc_ du lexer et du parseur _Mini-ML_, respectivement.
- **interf.ml** contient la fonction `parse` qui fait office d'interface entre le parser _Yacc_ et le reste du code _OCaml_.
- **miniml.ml** définit le `type mlexp`, dont le but est de représenter l'arbre syntaxique d'un programme _Mini-ML_. Un _pretty-printer_ `pp_prog/pp_exp` est aussi contenu dans ce fichier, mais il n'est plus très utile, c'était un outil de débuggage durant les premières tentatives de modification du parser.
- **encoder.ml** contient les fonctions permettant de transformer un arbre syntaxique de `type mlexp` en suite d'instructions `type instr`, selon le modèle d'instructions de la CAM vu en cours.
- **simulator.ml** contient la fonction `main` et le reste du code principal de l'exécutable **simu**, qui réalise une simulation _OCaml_ d'exécution de `instr list`, toujours selon le modéle vu en cours.
- **flattener.ml** contient les fonctions permettant d'"aplatir" le code généré par **encoder.ml** en un nouveau `type flat_code`, beaucoup plus proche du modèle de données que j'ai utilisé en _C_ dans la **CCAM**.
- **codeGenerator.ml** est responsable de la traduction du `flat_code` en code _C_ (sous la forme d'une `string`).
- **tools.ml** contient des fonctions utilitaires importées par plusieurs modules, comme par exemple la fonction _zip_.

## _Mini-ML_ : description du langage

Fichiers sources du lexer/parser : **lexer.mll**, **parser.mly**, **miniml.ml**

Le langage est similaire à _OCaml_ en syntaxe. Il n'est pas typé. Le programme **demo/language.ml** expose tous les éléments de syntaxe et de sémantique valides en _Mini-ML_ :

```OCaml
(* language.ml *)
TODO: include language.ml here
```

Quelques notes :
- _let bindings_ : `let x = e in t` est traduit en `(fun x -> t) e`
- opérations booléennes : `(&&)`, `(||)`, et `not`, sont traduits en branchements conditionnels `if..then..else`
- _let-rec bindings_ : `let rec ... and ... in ...`

  Pour éviter les bugs causés par la limitation de cette implémentation de la récursivité, j'ai fait en sorte que le parser refuse tout programme contenant un let-rec inclus comme sous-expression d'une autre expression plus grande. Ainsi l'unique manière autorisée d'utiliser un let-rec dans ce langage _mini-ML_ est de le mettre en expression principale, au plus haut de la hiérarchie du programme.

- sucre syntactique :

  - `fun x y z ->` devient `fun x -> fun y -> fun z ->`.
  - `let f x y = ... in ... ` devient `let f = fun x -> fun y -> ... in ...` ; même sucre autorisé dans le cas de _let-rec bindings_.

- valeurs de type `list` : la syntaxe est la même qu'en _OCaml_, c'est-à-dire, `[]`, `2 :: [1]`, `[3;4;]` (trailing semicolon allowed).

  Deux opérateurs élémentaires `head` et `tail` ont aussi été rajoutés, dans la même veine que `fst` et `snd`. Un opérateur supplémentaire de test (`is_empty`) permet de vérifier si une valeur est une liste vide. Note : ne provoque pas d'erreur si la valeur n'est pas une liste.

Pour écrire la grammaire dans **parser.mly**, et donc en particulier choisir l'associativité et la précédence de chacun des éléments syntaxiques, je me suis basé sur la documentation d'_OCaml_ trouvé au lien suivant : https://caml.inria.fr/pub/docs/manual-ocaml/expr.html


## Phase d'encodage

Fichiers : **miniml.ml**, **encoder.ml**

Note : le fichier **encoder.ml** est grosso modo un remplacement du fichier **instrs.ml**, que j'ai supprimé.

Une fois le fichier source _Mini-ML_ parsé, l'on se retrouve avec un programme sous forme d'arbre syntaxique (**miniml.ml**: `type mlexp`). Cet arbre est alors traduit en une liste d'instructions (**encoder.ml**: `type instr`). Cette traduction suit assez fidèlement les règles de compilation vues en cours.

Quelques ajouts et modifications du `type instr`, tout de même :
- `Halt` : pour indiquer explicitement à la machine virtuelle qu'elle doit s'arrêter, en fin de programme.
- `QuoteBool`, `QuoteInt` : remplace le constructeur `Quote of value` qui n'était pas assez _type-safe_ pour la suite de la compilation.
- `Call`, `AddDefs`, `RmDefs` : pour exécuter des expressions de type `let-rec`.
- `QuoteEmptyList` : pour implémenter la valeur `[]`, _i.e._ la liste vide.
- `MakeList` : joue le rôle de l'opérateur `::` dans la construction de listes. Calquée sur le modèle de l'instruction `Cons` qui construit des paires à partir du **terme** et de l'élément de tête de la **pile**.

Cette suite d'instructions constitue par ailleurs la valeur d'entrée du simulateur écrit en _OCaml_.


## Simulateur _OCaml_

Fichiers : **simulator.ml**, **simu** (exécutable)

Le simulateur dépend de l'encoder et bien sûr du parser et du lexer. Il exécute les instructions de `type instr` selon le modèle de la CAM vu en cours. **simulator.ml** contient par ailleurs une fonction `main`, ce qui permet de générer un exécutable **simu**, qui prend en paramètre un fichier **Mini-ML** et lance la simulation d'exécution sur son contenu. Il y a aussi une option `verbose` (_c.f._ **USAGE.txt**) que l'on peut passer à **simu**, permettant d'afficher une trace du processus d'exécution de chaque instruction et de l'évolution de l'état de la CAM.

Je décris ci-dessous les principaux types utilisés dans le code de la simulation. Le double intérêt de cette description est que la deuxième implémentation de la CAM, celle que j'ai réalisée en _C_ (**CCAM**), est vraiment très proche de cette implémentation-ci en _OCaml_ (aussi proche que les différences de paradigme et d'abstraction entre _C_ et _OCaml_ me l'ont permis).

- `type value` : valeurs manipulées par la CAM.
- `type code = instr list` : les instructions dont on simule l'exécution.
- `type stack` : pile d'exécution de la CAM (permet de stocker valeurs et code).
- `type defstack` : pile de définitions, utilisée pour implémenter la récursivité autorisée par les _let-rec bindings_. C'est une liste d'association `var * code` (`var` étant un alias pour `string` utilisé pour représenter le nom d'une variable).
- `type machine_state = (value * code * stack * defstack)` : ce quadruplet représente l'état de la CAM à tout instant durant la simulation, c'est l'état sur lequel les instructions opèrent.

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
-  `main` appelle `run_simulation` avec les options de verbosité et le `code` compilé par l'encoder.
- `run_simulation` construit l'état initial de la machine à l'aide de `blank_state` et du `code` récupéré. Puis elle lance la simulation à l'aide de `run_machine`, qui prend en paramètre un `machine_state` et une option de verbosité.
- `run_machine` est une fonction récursive. Ses paramètres sont `verbose` (indicateur de verbosité), et `ms` (de type `machine_state`, l'état de la machine).
  Si l'option `verbose` est activée, elle commence par afficher l'état de la machine : valeur du **terme**, valeur de sommet de **pile**, prochaine instruction à exécuter.
  Une fois cela fait, elle appelle `execute_next_instruction` avec `ms` en paramètre.
- `execute_next_instruction` correspond au cœur du travail de la simulation : elle lit la prochaine instruction à exécuter, et retourne une valeur de `type status`. Selon les cas :
    - si l'instruction s'avère être `Halt` : la valeur de retour est alors `Stopped (Halted term)` où `term` correspond à la valeur finale calculée par la machine
    - sinon, si l'instruction ne provoque pas d'erreurs : la valeur de retour est alors `AllOk new_ms`, où `new_ms` est le nouvel état de la machine après exécution de l'instruction en question
    - sinon, une erreur s'est produite (par exemple, tentative d'évaluer `fst 4`) : la valeur est alors `Stopped (Crashed error_msg)`.
- de retour dans `run_machine` : on vérifie le `status` récupéré. Si `AllOk`, on réitère `run_machine` avec le nouvel état de la machine. Si `Stopped`, on retourne le `final_status`.
- de retour dans `run_simulation` : selon que le constructeur de `final_status` soit :

    - `Halted`, on affiche la valeur finale calculée par la machine, et on termine la simulation
    - `Crashed`, on affiche le message d'erreur et on termine la simulation

    dans les deux cas, on termine la simulation.

## Représentation des instructions dans la **CCAM**

En _C_, la manière la plus simple de représenter une suite d'instructions est de l'enregistrer sous forme d'un tableau. Comme les instructions du modèle de la CAM contiennent parfois des paramètres (par exemple l'instruction `QuoteBool` contient une valeur littérale booléenne), le type des éléments du tableau doit être suffisamment flexible pour pouvoir contenir au besoin :

- une constante représentant une instruction, comme `QuoteBool` ou `Swap`
- une valeur littérale (entier ou booléen)
- une constante représentant une opération particulière (`Plus`, `Fst`, `Eq`, etc)
- un pointeur vers un autre morceau de code, c'est-à-dire vers un autre tableau d'instruction

Ce quatrième type de cellule est nécessaire pour le cas d'instructions qui sont censées "contenir" du code, comme l'instruction `Branch`, qui, en tant que constructeur pour le `type instr` en _OCaml_, est censé lui-même contenir les instructions pour les branches `if` et `else`.

C'est cette stratégie que j'ai employé pour encoder les instructions en _C_ dans la **CCAM**. Le type correspondant à une cellule de tableau de code s'appelle `CodeT`. Il est déclaré dans le fichier d'en-tête **ccam.h** :
```C
  union CodeT; typedef union CodeT {
    int instruction;
    int operation;
    union CodeT *reference;
    long data;
  } CodeT;
```
Note : le `union CodeT;` placé juste avant le `typedef` est juste là pour permettre une définition récursive : la ligne `union CodeT *reference;` nécessite que `union CodeT` soit déclaré _a priori_.

Les unions en _C_ s'utilisent assez similairement à un type somme ; en pseudo-code _OCaml_ la même structure serait définie par :
```OCaml
type code_t = Instruction of int | Operation of int
  | Code of code_t pointer | Data of long ;;
```
Par similarité de concept, j'appellerai "constructeur" les différents membres d'une `union` -- ici `.instruction`, `.operation`, `.reference` et `.data`.

En _C_ une valeur de type union peut se définir de la sorte :
```C
CodeT foo = { .operation = 3 } ;
// altenative :
CodeT bar; bar.reference = &foo;
```
Ici on a choisit le constructeur `.operation` pour `foo`, et le constructeur `.reference` pour `bar`. Une valeur de type `union` n'est pas censé initialiser plus d'un constructeur (sous peine de comportements imprévisibles).

Pour revenir à la représentation des instructions de la CAM dans l'implémentation **CCAM** : l'exemple qui suit sera peut-être plus parlant.
```OCaml
[QuoteInt(3); Swap; PrimInstr (UnOp (Fst)); Branch([Push], [App])] : instr list
```
Il s'agit d'une liste d'instructions arbitraire, telle que celles produites par **encoder.ml** et consommées par **simulator.ml** -- mis à part le fait qu'elle n'ait pas le moindre sens.

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

Les instructions comme `QuoteInt` ou `Unary` ou `Branch`, qui contiennent des paramètres, sont codées sur plusieurs cellules du tableau. Le constructeur choisit pour chaque chaque cellule est adapté à son contenu.

Le tableau `main_code` est spécial dans le sens que c'est le point de départ du programme exécuté par la **CCAM**. Il est d'ailleurs directement utilisé dans la fonction `main()` de **runtime.c**.

Tous les autres morceaux de code qui peuvent avoir été générés, comme ici `else_branch1` et `if_branch0`, proviennent d'instructions comme `Branch` ou `Call` qui sont censées modifier la liste d'instructions que la machine virtuelle exécute. Ces autres tableaux "périphériques" sont reliées les uns aux autres et au tableau `main_code` par le biais des cellules de `.reference`.

## Phase de _flattening_

Fichier : **flattener.ml**

Pour passer d'instructions du `type instr` (avec des constructeurs comme `Branch of instr list * instr list`), à une représentation en _C_ "plate" (sans structure directement récursive), on a besoin de passer par un format d'instructions intermédiaire qui soit débarrassé de cette récursion structurelle.

C'est le rôle de `type flat_instr` et `type flat_code = flat_instr list` définis dans **flattener.ml**. Essentiellement, on remplace chaque constructeur souffrant de récursion structurelle par un nouveau constructeur "plat" ne contenant plus que des _placeholders_ (de type `string`). Ces chaînes de caractère correspondent dans la représentation finale en _C_ aux noms des tableaux périphériques référencés : `"if_branch0"` par exemple.

Le code _OCaml_ du fichier **flattener.ml** est particulièrement compliqué/pénible, avec beaucoup de récursion, de récursion mutuelles, et une foule de variables difficiles à nommer. La raison en est que l'opération qui permet de transformer une `instr list` en `flat_code` est une opération particulièrement _stateful_. En particulier, il faut conserver et mettre à jour un compteur (que j'ai généralement appelé `n` ou `nextN`, etc) pour s'assurer que les _placeholders_ que l'on utilise sont uniques (puisqu'ils correspondent à des noms de tableaux de `CodeT` en _C_).

Un point qui mérite peut-être quelques explications : le traitement des instructions `Call`, `AddDefs` et `RmDefs` durant la traduction de `type instr` vers `flat_code`. Par soucis de simplicité et d'efficacité, toutes les définitions contenues dans toutes les instructions `AddDefs` (à travers tout le programme) sont automatiquement transcrites en tableaux périphériques uniques (comme pour les branches `if` et `else` de `Branch`).

À partir de là, on n'a plus vraiment besoin des instructions `AddDefs` et `RmDefs` dans le type `flat_instr`, ni, par extension, dans le modèle d'exécution de la **CCAM**. Il suffit de faire en sorte que `FlatCall` contienne le _placeholder_ correspondant au bon _let-rec binding_, et de définir le comportement de cette instruction comme étant identique à un `Branch` non conditionnel (un seul chemin qui est forcément emprunté).
Il faut de plus s'assurer que le code du _binding_ termine bien par l'instruction `Return` (comme pour le cas des branchements conditionnels), chose qui n'était pas prévue dans le modèle d'origine (celui du simulateur _OCaml_).

Je n'en dirai pas plus, l'opération de _code flattening_ est de loin la plus compliquée à mettre en mots. Le résultat final de cette phase de compilation est une `fragment list`, avec `type fragment = string * flat_code`. En d'autres termes, on récupère une liste d'association de _placeholders_ et de `flat_instr list` qui sont prêts à être traduits en _C_.

## Phase de génération de code _C_

Fichier : **codeGenerator.ml**

Une fois qu'on a récupéré la `fragment list` de la phase précédente, il ne reste plus grand chose à faire : simplement à construire la chaîne de caractère qui sera le contenu du fichier _C_ généré par **comp**. L'opération n'a rien de très compliqué ni rien de très intéressant : c'est essentiallement un _printer_ un peu spécial.

## Organisation du dossier **ccam/**

Ce dossier contient les sources de mon implémentation de la CAM en C (**CCAM**). Fichiers notables :

- **Makefile** est le fichier `make` pour la **CCAM**. Il permet de compiler un fichier généré avec le compilateur **comp** en un exécutable. _c.f._ **USAGE.txt**.
- **ccam.h** est le fichier d'en-tête principal. Il définit les `struct`, `union` et `enum` utilisés par la **CCAM**, et déclare aussi toutes les fonctions définies dans les fichiers **enums.c**, **value.c**, **stack.c**, **machine.c**.
- **enums.c** regroupe diverses fonctions utilitaires qui concernent les divers `enum` utilisés par la **CCAM**.
- **value.c** contient les divers constructeurs et fonctions de pattern-matching pour le type `ValueT`, qui correspond au `type value` du simulateur OCaml.
- **stack.c** contient les divers constructeurs et fonctions de pattern-matching pour le type `StackT`, qui correspond au `type stack` du simulateur OCaml.
- **machine.c** contient les constructeurs pour le type `MachineStateT`, ainsi que toutes les fonctions d'exécution individuelles des diverses instructions -- _e.g_ `exec_Apply()` ou `exec_Branch()`.
- **runtime.c** contient la fonction `main()` de la **CCAM**, et fait directement appel au pointeur/tableau `main_code`, ce qui permet de faire le lien avec le code généré par **comp**.
- **unit-tests.c** contient des tests unitaires pour vérifier le comportement de chaque instruction et des divers constructeurs de `ValueT`.
- **testing.h** et **testing.c** contiennent des fonctions uniquement nécessaires pour les tests unitaires.

## Lien entre **CCAM** et **simu**

Autant que possible, j'ai aussi essayé de conserver les mêmes noms, entre la **CCAM** et le simulateur _OCaml_ (**simulator.ml**), pour des types remplissant des rôles équivalents dans les deux implémentations :
- `type value` --> `ValueT`
- constructeur `PairV()` --> `PairT`, etc
- constructeur `ListConsV()` --> `ListConsT`
- `type stack` --> `StackT`
- `type machine_state` --> `MachineStateT`
- `type status` --> `enum Status`
- constructeur `Halted/AllOk` --> constante (_enum value_) `Halted/AllOk`

Note : le type `MachineStateT` est cependant un triplet, au lieu d'un quadruplet comme `machine_state`, puisqu'on s'est débarrassé du besoin du paramètre `defstack` lors de la phase de _code flattening_.

Même les fonctions gardent les mêmes noms autant que possible : `run_machine()`, `execute_next_instruction()`, `blank_state()` remplissent tous le même rôle dans la **CCAM** que leurs homologues _OCaml_ dans **simulator.ml**.

Le flôt d'exécution est également extrêmement proche.

## Flot d'exécution de la **CCAM**

La fonction `main()` se trouve dans **runtime.c**.

À partir du pointeur `main_code`, un état initial pour la machine virtuelle est créé (**machine.c**: `MachineStateT *blank_state(CodeT *code)`). Puis la fonction `run_machine(MachineStateT *ms, int verbose)` est lancée.

Cette fonction se trouve dans **machine.c**. Elle réalise une boucle `while` qui ne prend fin que lorsque le `status` de la machine virtuelle passe de `AllOk` à une autre valeur (`Halted` si l'instruction `Halt` est atteinte, une valeur d'erreur dans tout autre cas).

Dans le corps de cette boucle, la fonction `execute_next_instruction(MachineStateT *ms);` est appelée. Elle est définie dans **enum.c**. Elle lit la prochaine cellule de code enregistrée dans l'état (`ms`) de la machine virtuelle. Cette cellule (`ms->code[0]`), de type `CodeT`, doit normalement contenir une des constantes du type `enum instruction`. Un `switch` réalise le choix de la bonne fonction d'exécution à appeler. Par exemple si `ms->code[0] == Apply`, alors c'est `exec_Apply(ms)` qui sera appelée. Toutes les fonctions de la forme `exec_...` se trouvent dans **machine.c**.

La fonction d'exécution choisie (par exemple `exec_Apply()`) modifie alors l'état de la machine virtuelle de la manière attendue pour l'instruction en question. Elle peut :
- modifier le **terme** (`ms->term`), qui est de type `ValueT *`
- modifier le pointeur **code** (`ms->code`), qui est de type `CodeT *`
- modifier la **pile** (`ms->stack`), qui est de type `StackT *`

Par exemple :
- l'instruction `Push` copie le **terme** et l'ajoute à la **pile**, et incrémente le pointeur **code** de 1 (car l'instruction `Push` est encodée avec une seule cellule de type `CodeT`).
- L'instruction `Branch` vérifie que le **terme** est un booléen, ajoute à la **pile** le pointeur vers l'instruction qui suit le `Branch` (qui correspond à `ms->code + 3` car l'instruction `Branch` est encodée sur trois cellules de type `CodeT`), et remplace le pointeur **code** par la cellule `ms->code[1]` si le **terme** vaut `true`, et `ms->code[2]` si le **terme** vaut `false` -- correspondant å choisir la branche _if_ ou la branche _else_.

Et de même pour toutes les autres instruction.

À la fin de l'exécution, on affiche un message d'erreur en cas d'erreur, dans le cas contraire on affiche la valeur (de type `ValueT`) finale du **terme** principal de l'état de la machine virtuelle (_i.e._, `ms->term`).

## Modèle de données du code _C_

Fichiers : **ccam.h**

Le modèle de données de la **CCAM** est fortement inspiré du modèle de données d'un langage fonctionnel. On utilise un tag au lieu de constructeurs, et le pattern-matching est fait par des fonctions individuelles comme `match_value_with_pair()` (**value.c**), mais cela mis à part, j'ai essayé de garder le même esprit.

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

Bien sûr, la construction de nouvelles valeurs est une tâche répétitive, qui est donc rendue automatique par les fonctions de construction suivantes, toutes définies dans **value.c** :

```C
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

Par exemple `ValueT *d = PairValue(b, c);` équivaut aux trois dernières lignes de l'exemple plus haut (sauf que le résultat de `PairValue` est un pointeur au lieu d'être une valeur locale).

Tout ce que je viens de dire pour `ValueT` s'applique tout aussi bien au type `StackT` :
```C
typedef struct StackT {
  enum StackTag tag;
  union {
    ValueOnStackT with_value;
    CodeOnStackT with_code;
  } as;
} StackT;
```
 qui correspond au type _OCaml_ :
```OCaml
type stackelem = Val of value | Cod of code and stack = stackelem list
```
Sauf que les objets alloués de type `StackT` ne sont pas gérés par _reference counting_ (on n'en a pas besoin).
Utilisation de `StackT`, pour continuer l'exemple précédent :
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
Fonctions de construction pour `StackT` (dans **stack.c**) :
```C
StackT *EmptyStack();
StackT *ValueOnStack(ValueT *value, StackT *old_stack);
StackT *CodeOnStack(CodeT *code, StackT *old_stack);
```
Réécriture de l'exemple précédent pour en faire usage :
```C
StackT *u = CodeOnStack(foo, ValueOnStack(IntValue(3), EmptyStack()));
```

## Pattern-matching

Une fois que l'on a instancié une `ValueT` ou une `StackT`, on veut pouvoir effectuer un pattern-matching, pour :
- vérifier que le type d'objet (une liste non vide, une valeur nulle, une pile avec du code (pointeur vers `CodeT`) comme élément de tête, etc) correspond bien à ce que l'on veut
- si c'est le cas, récupérer le contenu de la valeur ; par exemple, si c'est une paire, récupérer les deux champs `first` et `second` de la structure `PairT` contenue dans l'union `as` de la valeur en question ; si c'est une pile avec une valeur au-dessus, récupérer les champs `top` (`ValueT *`) et `bottom` (`StackT *`) de la structure `ValueOnStackT`, correspondant respectivement à la valeur au dessus de pile et au reste de la pile.
- une fois cela fait, on doit alors libérer la mémoire (si nécessaire) de l'objet que l'on a déconstruit par pattern-matching pour éviter les fuites de mémoire.

C'est le travail des fonctions suivantes :
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
  // transféré à pattern.first (contient IntValue(3)) et pattern.second
  // (contient BoolValue(True)).
}
else {
  // matching failure: cela implique que x est intact, n'a pas été
  // libéré en mémoire, et pattern n'a pas été modifié, donc n'est
  // toujours pas initialisé
}
```
Toutes les fonctions de matching fonctionnent selon ce modèle.

## Gestion de la mémoire

Les objets de type `StackT` sont libérés au fur et à mesure par pattern-matching. Ils n'ont jamais besoin d'être copiés, donc on n'a pas besoin de les gérer avec _reference counting_.

À l'inverse, les objets de type `ValueT` sont susceptibles d'être copiés fréquemment, du fait de l'existence de l'instruction `Push`, qui place une copie du **terme** principal sur la **pile**.

Au lieu de réellement copier les `ValueT` récursivement, il est plus économe de simplement incrémenter un `copy_count` (récursivement là encore), et de modifier le système de libération de mémoire pour que les objets de type `ValueT` ne soient libérés pour de vrai que lorsque leur `copy_count` tombe à zéro.

Les fonctions assurant ce travail de _mini garbage collecting_ (**value.c**):
```C
ValueT *malloc_value();
  // ^ initialise le copy_count
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

Les variables globales suivantes sont uniquement utilisées pour vérifier le comportement du _reference counting_ :
```C
int mallocated_values_count;
int freed_values_real_count;
int freed_values_fake_count;
void memory_value_report();
  // ^ affiche un court rapport d'information
  // se basant sur les trois variables globales précédentes
```
