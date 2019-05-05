# Projet : Compilateur de Mini-ML vers C

## Terminologie

**CCAM** : le nom que j'ai donné à la machine virtuelle (CAM) que j'ai écrite en C et qui exécute les instructions compilées depuis le code source _mini-ML_

## Organisation globale du Projet

Le projet est divisé en deux grosses parties :
-   **Partie OCaml** : le compilateur code _mini-ML_ --> code C

    Fichiers : tous les fichiers .ml qui se trouvent à la racine du dossier du projet

-   **Partie C** : la machine virtuelle **CCAM**, qui prend le code C généré par la partie OCaml et produit un exécutable que l'on peut directement lancer

    Fichiers : les fichiers .c et .h qui se trouvent dans le dossier ccam/

## Partie OCaml

J'ai remplacé le fichier instrs.ml par les fichiers **encoder.ml**, **flattener.ml** et **codeGenerator.ml** (voir plus bas, **Processus de Compilation**). Ces trois fichiers s'occupent de transformer l'arbre syntaxique parsé par le lexer/parser combo en code source C que l'on peut ensuite compiler avec la **CCAM**.

J'ai ajouté un fichier **simulator.ml** qui contient le code du simulateur d'exécution de la CAM. Il contient par ailleurs une fonction `main`.

Le `Makefile` de la partie Caml (celui qui compile l'exécutable `./comp`, et qui se trouve à la racine du projet) permet de compiler un second exécutable appelé `./simu`, qui prend en paramètre un fichier `mini-ML`, et lance la simulation de **simulator.ml** sur son contenu.

Tout cela a par ailleurs nécessité un petit peu de changement dans le `Makefile` en question.

### Usage

- Pour générer les exécutables **comp** et **simu** :

  ```sh
  $ make comp
  $ make simu
  $ make    # <- pour générer les deux
  ```

- Pour compiler un fichier `mini-ML` vers source `C` :

```sh
$ ./comp input-file.ml output-file.c
```

- Pour lancer la simulation d'exécution OCaml sur un fichier `mini-ML` :

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

  Pour éviter les bugs causés par la limitation de cette implémentation de la récursivité, j'ai fait en sorte que le parser refuse tout programme contenant un let-rec inclus comme sous-expression d'une autre expression plus grande. Ainsi l'unique manière autorisée d'utiliser un let-rec dans ce langage _mini-ML_ est de le mettre en expression principale, au plus haut de la hiérarchie du programme

- valeurs de type `list` ; la syntaxe est la même qu'en ocaml, soit `[]`, `2 :: [1]`, `[3;4;]` (trailing semicolon allowed) ; les deux opérateurs élémentaires `head` et `tail` ont aussi été rajoutés, dans la même veine que `fst` et `snd`

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
