# Projet : Compilateur de Mini-ML vers C

## Terminologie

**CCAM** : le nom que j'ai donné à la machine virtuelle (CAM) que j'ai écrite en C et qui exécute les instructions compilées depuis le code source _mini-ML_

## Organisation globale du Projet

Le projet est divisé en deux grosses parties :
- **Partie OCaml** : le compilateur code _mini-ML_ --> code C

  Fichiers : tous les fichiers .ml qui se trouvent à la racine du dossier du projet

- **Partie C** : la machine virtuelle **CCAM**, qui prend le code C généré par la partie OCaml et produit un exécutable que l'on peut directement lancer

  Fichiers : les fichiers .c et .h qui se trouvent dans le dossier ccam/

- **generate-all.sh**: ............... ................ ...........

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

J'ai remplacé le fichier instrs.ml par les fichiers **encoder.ml**, **flattener.ml** et **codeGenerator.ml** (voir plus bas, **Processus de Compilation**). Ces trois fichiers s'occupent de transformer l'arbre syntaxique parsé par le lexer/parser combo en code source C que l'on peut ensuite compiler avec la **CCAM**.

J'ai ajouté un fichier **simulator.ml** qui contient le code du simulateur d'exécution de la CAM. Il contient par ailleurs une fonction `main`.

Le `Makefile` de la partie Caml (celui qui compile l'exécutable `./comp`, et qui se trouve à la racine du projet) permet de compiler un second exécutable appelé `./simu`, qui prend en paramètre un fichier `mini-ML`, et lance la simulation de **simulator.ml** sur son contenu.

Tout cela a par ailleurs nécessité un petit peu de changement dans le `Makefile` en question.

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

Fichiers : **lexer.mll**, **parser.mly**

Les éléments suivants ont été implémentés :

- tous les opérateurs et constructeurs prévus au départ pour le type `mlexp` : `+`, `==`, `fst`, `(,)`, `if then else`, `fun ... -> ... `, etc
- _let-binding_ : `let x = e in t` qui est traduit en `(fun x -> t) e`
- opérations booléennes `&&` et `||`, qui se traduisent en branchements `if..then..else` pour obtenir l'effet de court-circuit attendu
- let-rec bindings `let rec ... and ... in ...`

  Pour éviter les bugs causés par la limitation de cette implémentation de la récursivité, j'ai fait en sorte que le parser refuse tout programme contenant un let-rec inclus comme sous-expression d'une autre expression plus grande. Ainsi l'unique manière autorisée d'utiliser un let-rec dans ce langage _mini-ML_ est de le mettre en expression principale, au plus haut de la hiérarchie du programme.

- _syntactic sugar_ :

  - `fun x y z ->` qui devient `fun x -> fun y -> fun z ->`
  - `let x y = ... in ... ` qui devient `let x = fun y -> ... in ...` ; idem pour les _let-rec bindings_

- valeurs de type `list` ; la syntaxe est la même qu'en ocaml, c'est-à-dire, `[]`, `2 :: [1]`, `[3;4;]` (trailing semicolon allowed) ; les deux opérateurs élémentaires `head` et `tail` ont aussi été rajoutés, dans la même veine que `fst` et `snd`

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

Le type `instr` a été augmenté des instructions suivantes :
- `Halt` : pour indiquer explicitement à la machine virtuelle qu'elle doit s'arrêter, en fin de programme.
- `QuoteBool`, `QuoteInt` : le constructeur d'origine (`Quote of value`) n'était pas assez _type-safe_ pour la suite de la compilation.
- `Call`, `AddDefs`, `RmDefs` : tout ce qu'il faut pour exécuter des expressions récursives.
- `QuoteEmptyList` : pour implémenter la valeur `[]`, _i.e._ la liste vide.
- `MakeList` : joue le rôle de l'opérateur `::` dans la construction de listes. Calqué sur le modèle de l'instruction `Cons` qui elle construit des paires à partir du **terme** et de l'élément de tête de la **pile**.

### Simulator

Fichier : **simulateur.ml**, **simu**

Contient le code qui permet de simuler l'exécution de la CAM en OCaml, selon le modèle vu dans les transparents du cours. La seule différence notable est l'existence de l'instruction `Halt`, qui arrête explicitement la simulation et affiche en sortie standard la dernière valeur prise par le **terme**.

###
