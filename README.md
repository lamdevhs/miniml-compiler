
# Projet : Compilation d'un langage fonctionnel vers C

## Terminologie

**Mini-ML** : Mini-langage fonctionnel similaire en syntaxe à OCaml, mais non typé et restreint en fonctionnalité, utilisé comme langage source initial pour ce projet de compilation. Extension de fichier : `.ml`.

**CAM** : Acronyme pour _Categorical Abstract Machine_. Modèle abstrait d'exécution d'un langage fonctionnel de type _ML_ sous la forme d'une suite d'instructions atomiques. Les instructions modifient l'état de la machine virtuelle, qui est constitué d'un triplet de trois valeurs : un **terme** représentant le registre principal d'opération, une **pile** qui fait office de pile d'exécution, et enfin, le **code** lui-même restant à exécuter. _Référence_ : Cousineau, Curien, Mauny: _The Categorical Abstract Machine_, Science of Computer Programming 8 (1987), pp. 173-202.

**CCAM** : l'implémentation de la CAM que j'ai écrite en C et qui fait office de machine virtuelle pour les instructions compilées en C depuis le code source _mini-ML_.


## Résumé du projet

Le projet est composé d'un compilateur appelé **comp**, écrit en _OCaml_ à l'aide de _Lex_ et _Yacc_, dont le rôle est de parser et de compiler un fichier source _mini-ML_, produisant alors un fichier intermédiaire en _C_, qui peut alors être à son tour compilé avec `gcc` et les sources de la **CCAM** (écrite en _C_) pour obtenir finalement un fichier exécutable, dont l'exécution correspond à l'évaluation du fichier _mini-ML_ d'origine.
Le projet est aussi composé d'un simulateur **simu** écrit en _OCaml_ dont le rôle est de parser, compiler et interpréter un fichier source _mini-ML_, toujours selon le modèle de la CAM.


## Organisation concrète du projet

Le projet est divisé en deux grosses parties :

- **Partie OCaml** : sous-dossier **./ocaml**. Contient les sources pour le compilateur **comp** (_mini-ML_ --> _C_) et le simulateur **simu**. Un Makefile **./ocaml/Makefile** permet de les générer.

- **Partie C** : sous-dossier **./ccam**. Contient les sources pour la machine virtuelle **CCAM**. Un Makefile **./ccam/Makefile** permet de générer un exécutable à partir des ces sources et du code généré par **comp** à partir du fichier _mini-ML_ source. Contient aussi des tests unitaires.

Autres fichiers du projet :

- **Jeu de tests** : sous-dossier **./demo**. Contient divers programmes écrits en _Mini-ML_.
- **./generate-all.sh** : script qui permet de générer des exécutables pour chacun des programmes dans **./demo**, en les compilant, d'abord avec **comp**, puis avec **./ccam/Makefile**.
- **./USAGE.txt** : informations sur comment utiliser le projet.


## Usage des fichiers du projet

Lire **./USAGE.txt**. Ce fichier contient toutes les informations d'usage pour :
- **./ocaml/Makefile** et **./ccam/Makefile**
- **comp** et **simu**
- le script **./generate-all.sh**


## _Mini-ML_ : description du langage

Fichiers sources du lexer/parser : **lexer.mll**, **parser.mly**, **miniml.ml**

Le langage est similaire à _OCaml_ en syntaxe. Il n'est pas typé. Le programme **./demo/language.ml** expose tous les éléments de syntaxe et de sémantique valides en _Mini-ML_ :

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

Le simulateur dépend de l'encoder et bien sûr du parser et du lexer. Il exécute les instructions de `type instr` selon le modèle de la CAM vu en cours. **simulator.ml** contient par ailleurs une fonction `main`, ce qui permet de générer un exécutable **simu**, qui prend en paramètre un fichier **Mini-ML** et lance la simulation d'exécution sur son contenu. Il y a aussi une option `verbose` (_c.f._ **./USAGE.txt**) que l'on peut passer à **simu**, permettant d'afficher une trace du processus d'exécution de chaque instruction et de l'évolution de l'état de la CAM.

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

En _C_, la manière la plus simple de représenter une suite d'instructions est de la coder sous forme d'un tableau. Comme les instructions contiennent parfois des paramètres (par exemple l'instruction `QuoteBool` contient une valeur littérale booléenne), le type des éléments du tableau doit être suffisamment flexible pour pouvoir contenir au besoin :

- une constante représentant une instruction, comme `QuoteBool` ou `Swap`
- une valeur littérale (entier ou booléen)
- une constante représentant une opération particulière (`Plus`, `Fst`, `Eq`, etc)
- un pointeur vers un autre morceau de code, c'est-à-dire vers un autre tableau d'instruction

Ce quatrième type de cellule est nécessaire pour le cas d'instructions qui sont censées "contenir" du code, comme l'instruction `Branch`, qui, en tant que constructeur pour le `type instr` en _OCaml_, est censé lui-même contenir les instructions pour les branches `if` et `else`.



TODO: continue

On peut alors conserver dans l'état de la machine virtuelle un pointeur vers la prochaine case du tableau d'instructions à exécuter.

TODO: instruction Call, traitement de la récursion


## Phase de _flattening_

Fichier : **flattener.ml**

Le modèle de représentation du code offert par le `type instr` et `type code = list instr` n'est pas compatible avec le format de code que la **CCAM** attend en _C_.

Le code

## Partie C

Dossier: **ccam/**

Implémentation de la CAM en C. Fichiers dans ce sous-projets:

- **Makefile** est le fichier `make` pour la **CCAM**. Il permet de compiler un fichier généré avec le compilateur **comp** en un exécutable.
- **ccam.h** est le fichier d'en-tête principal. Il définit les `struct` et `enum` utilisés par la **CCAM**, et déclare toutes les fonctions des fichiers **enums.c**, **value.c**, **stack.c**, **machine.c**. Il contient aussi la déclaration de la fonction `get_main_code()`, qui est attendue dans le fichier généré par le compilateur **comp**.
- **enums.c** contient divers fonctions utilitaires qui sont fortement concernées par les divers `enum` de la **CCAM**, par exemple une fonction d'affichage d'un message d'erreur à partir d'un `enum Status`.
- **value.c** contient les divers constructeurs et fonctions de pattern-matching pour le type `ValueT`, qui correspond au `type value` du simulateur OCaml.
- **stack.c** contient les divers constructeurs et fonctions de pattern-matching pour le type `StackT`, qui correspond au `type stack` du simulateur OCaml.
- **machine.c** contient les constructeurs pour le type `MachineStateT`, qui correspond au triplet `(value * code * stack)` du simulateur OCaml, c'est-à-dire le type de la configuration de la CAM. Il contient aussi toutes les fonctions d'exécution des instructions, par exemple `exec_Apply()` ou `exec_Branch()`.
- **runtime.c** contient la fonction `main()` de la **CCAM**, et appelle la fonction `get_main_code()` afin de faire le lien avec le code généré par le compilateur OCaml **comp**.
- **unit-tests.c** contient des tests unitaires pour vérifier le comportement de chaque instruction et des divers constructeurs de `ValueT`.
- **testing.h** et **testing.c** contiennent des fonctions uniquement nécessaires pour les tests unitaires.

### Usage de **ccam/Makefile**

En supposant que vous avez compilé un fichier _mini-ML_ en fichier source C à l'aide du compilateur OCaml **comp**, et que le fichier généré s'appelle **generated.c**, la commande :
```sh
$ make build in=generated.c out=generated.out
```
permet de générer l'exécutable **generated.out**. Si vous ne renseignez pas les deux paramètres, la commande :
```sh
$ make build
```
partira du principe par défaut que `in=gen.c` et `out=gen.out`.

Il y a deux options de compilation que vous pouvez choisir d'activer :
- `DBG` pour avoir une trace de toute l'exécution de la **CCAM**, instruction par instruction. Attention, pour certains programmes, l'affichage dans certains terminaux pourrait poser des problèmes à cause de la quantité affichée en sortie.
- `MEM` pour avoir, à la toute fin de l'exécution, après avoir affiché la valeur finale calculée par la **CCAM**, un rapport d'information concernant le _garbage collecting_ des objets de type `ValueT`.

Pour activer une des options, ou bien les deux à la fois :
```sh
$ make build in=foo.c out=bar DBG=y
$ make build in=foo.c out=bar MEM=y
$ make build in=foo.c out=bar DBG=y MEM=y # <-- les deux seront activés
```

Dans le code source, l'option `DBG` correspond à définir la variable de pré-processeur `DEBUGMODE` ; l'option `MEM` correspond elle à la variable `TRACE_MEMORY`.

Pour lancer les tests unitaires, mode non verbeux :
```sh
$ make tests
```
Affiche "All tests passed." si tout s'est bien passé. Mode verbeux :
```sh
$ make tests V=y
```
Affiche une ligne commençant par `[ok]` pour chaque test vérifié avec la description du test en question.

### Encodage des instructions dans la **CCAM**

Le code est représenté par un tableaux dont les cellules sont du type `CodeT`.
Ce type, définit dans **ccam.h**, est une `union` (un type somme), qui peut selon les cas :
- représenter une instruction (`int instruction`).
- représenter une valeur brut (`int data`). Utilisé notamment pour encoder les paramètres des instructions `QuoteInt of int` et `QuoteBool of bool`.
- représenter une opération (`int operation`). Utilisé pour encoder l'instruction `PrimOp`
- représenter une référence vers une autre section de code, c'est-à-dire un pointeur vers `CodeT` (`CodeT *reference`).

Comme exemple concret, voici deux représentations équivalentes de la même suite d'instructions, l'une en OCaml, l'autre en C, telle que générée par le compilateur :
```ocaml
[QuoteInt(3); Swap; PrimInstr (UnOp (Fst)); Branch([Push], [App])] : instr list
```
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

CodeT *get_main_code()
{
    return main_code;
}
```
(La suite d'instruction n'a pas de sens à priori, c'est juste une démonstration visuelle.)

Le code principal de l'exemple (`main_code[]`) est une série de quatre instructions. La première est encodée sur 2 cellules du tableau, une pour spécifier l'instruction (`.instruction = QuoteInt`), l'autre pour la donnée brut contenue (`.data = 3L`).
L'instruction `Branch` est encodée avec 3 cellules, une première pour l'instruction (`.instruction = Branch`), les deux autres pour référencer d'autres fragments de code, ce sont donc bien des pointeurs vers `CodeT` (`.reference = if_branch0`).

### Flot d'exécution de la **CCAM**

La fonction `main()` se trouve dans **runtime.c**.
Le pointeur vers le code principal généré par le compilateur OCaml est récupéré grâce à `get_main_code()` (contenue dans le code généré).

À partir de ce pointeur, un état initial pour la machine virtuelle est créé (**machine.c**: `MachineStateT *blank_state()`). Puis la fonction `run_machine()` est lancée.

Cette fonction se trouve dans **machine.c**. Elle réalise une boucle `while` qui ne prend fin que lorsqu'une erreur est observée par le status de la `CCAM`, ou bien lorsque l'instruction `Halt` est atteinte, après quoi ledit status passe de `AllOk` à `Halted`.

Dans le corps de cette boucle, la fonction `execute_next_instruction(ms)` est appelée. Elle est définie dans **enum.c**. Elle lit la prochaine cellule de code enregistrée dans l'état (`ms`) de la machine virtuelle. Cette cellule (`ms->code[0]`), de type `CodeT`, doit normalement contenir une des constantes du type `enum instruction`. Un `switch` réalise le choix de la bonne fonction d'exécution à appeler. Par exemple si `ms->code[0] == Apply`, alors c'est `exec_Apply(ms)` qui sera appelée. Toutes les fonctions de la forme `exec_...` se trouvent dans **machine.c**.

La fonction d'exécution choisie modifie alors l'état de la machine virtuelle de la manière attendue pour l'instruction en question. Elle peut :
- modifier le **terme** (`ms->term`), de type `ValueT *`
- modifier le pointeur **code** (`ms->code`), de type `CodeT *`
- modifier la **pile** (`ms->stack`), de type `StackT *`

Par exemple :
- l'instruction `Push` copie le **terme** et l'ajoute à la **pile**, et incrémente le pointeur **code** de 1 (car l'instruction `Push` est encodée avec une seule cellule de type `CodeT`).
- L'instruction `Branch` vérifie que le **terme** est un booléen, ajoute à la **pile** le pointeur vers la prochaine instruction (qui correspond à `ms->code + 3` car l'instruction `Branch` est encodée sur trois cellules de type `CodeT`), et remplace le pointeur **code** par la cellule `ms->code[1]` si le **terme** vaut `true`, et `ms->code[2]` si le **terme** vaut `false`.

Et ainsi de suite pour toutes les autres instruction.

### Pattern-matching

Le modèle de données de la **CCAM** est fortement inspiré du modèle de données d'un langage fonctionnel.

............ ................ ............

## Partie OCaml

### Usage

Pour générer les exécutables **comp** et **simu** :
```sh
$ make comp
$ make simu
$ make    # <- pour générer les deux
```
Pour compiler un fichier `mini-ML` vers source `C` (à destination de la **CCAM**) :
```sh
$ ./comp input-file.ml output-file.c
```
Pour lancer la simulation d'exécution OCaml sur un fichier `mini-ML` :
```
$ ./simu input-file.ml
```

### Lexer/Parser

[deleted]

### Processus de compilation

Fichiers : **encoder.ml**, **flattener.ml**, **codeGenerator.ml**

J'ai remplacé le fichier instrs.ml par ces trois fichiers, qui réalisent chacuns et dans cet ordre, une partie du processus de compilation :
- **encoder.ml** : prend une expression de type `mlexp` et produit du `code`, qui peut ensuite ou bien être passé au simulateur Ocaml (c.f. plus bas) **simulator.ml**, ou bien être passé à l'étape suivante de la compilation
- **flattener.ml** : prend du `code` en entrée, et retourne des fragments nommés de `flat_code`, c'est à dire une `fragment list`, avec `type fragment = string * flat_code`, et `type flat_code` = `flat_instr list`.

  L'idée est que pour compiler les instructions en C sous la forme de simples tableaux, on doit se débarrasser de la récursivité du type `code` : les instructions `Branch` et `AddDefs` contiennent eux-même du `code`.

  On fait donc en sorte d'obtenir du code non-récursif (`flat_code`) sous forme de fragments isolés et nommés (`fragment list`) qui se font référence les uns aux autres, ce qui se traduit en C par des pointeurs de morceaux de code vers d'autre morceaux de code.

  Le gros du travail est effectué par flatten_code, qui est un très gros fold_left, avec récursion interne (en plus du folding) et mutuelle avec flatten_defs dont le rôle est d'aplatir les let-rec bindings... Autant dire que c'est assez tentaculaire. Ce qui complique les choses tout particulièrement c'est qu'on a besoin de garder un compteur (de type `int`) que l'on incrémente tout au long du processus, afin de pouvoir créer des noms de fragments uniques.

- **codeGenerator.ml** : génère du code source C (sous la forme d'une chaine de caractère) à partir de ce que renvoie l'étape précédente.

  Plus précisément, prend les fragments nommés `fragment list` et les transforme chacun en tableau de CodeT. Les tableaux se font mutuellement référence, donc on a besoin de tous les déclarer tout au début du fichier généré avant d'écrire leurs définitions.

### Encoder

Fichier : **encoder.ml**

Ce fichier contient la fonction `encode`, qui prend une expression de type `mlexp` et renvoie une liste de `instr`, c'est-à-dire une valeur de type `code`.



### Simulator

Fichier : **simulateur.ml**, **simu**

Contient le code qui permet de simuler l'exécution de la CAM en OCaml, selon le modèle vu dans les transparents du cours. La seule différence notable est l'existence de l'instruction `Halt`, qui arrête explicitement la simulation et affiche en sortie standard la dernière valeur prise par le **terme**.

###
