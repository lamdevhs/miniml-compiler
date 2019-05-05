# Projet : Compilateur de Mini-ML vers C

## Terminologie

**CCAM** : le nom que j'ai donné à la machine virtuelle (CAM) que j'ai écrite en C et qui exécute les instructions compilées depuis le code source `miniml`

## Organisation globale du Projet

Le projet est divisé en deux grosses parties :
-   partie OCaml : le compilateur code miniml --> code C

    fichiers : tous les fichiers .ml qui se trouvent à la racine du dossier du projet

-   partie C : la machine virtuelle **CCAM**, qui prend le code C généré par la partie OCaml et produit un exécutable que l'on peut directement lancer

    fichiers : les fichiers .c qui se trouvent dans le dossier ccam/

## Partie OCaml

### Fichiers et Utilisation

J'ai remplacé le fichier instrs.ml par les fichiers :
- encoder.ml


### Lexer/Parser

Fichiers : lexer.mll, parser.mly

Les éléments suivants ont été implémentés :

- tous les opérateurs et constructeurs prévus au départ pour le type `mlexp`
- opérations booléennes `&&` et `||`, qui se traduisent en branchements `if..then..else` pour obtenir l'effet court-circuit attendu
- let-rec bindings ; pour éviter les bugs causés par la limitation de cette implémentation de la récursivité, j'ai fait en sorte que le parser refuse tout programme contenant un let-rec inclus comme sous-expression d'une autre expression plus grande ; ainsi l'unique manière autorisée d'utiliser un let-rec dans ce langage `miniml` est de le mettre en expression principale, au plus haut de la hiérarchie du programme
- valeurs de type `list` ; la syntaxe est la même qu'en ocaml, soit `[]`, `2 :: [1]`, `[3;4;]` (trailing semicolon allowed) ; les deux opérateurs élémentaires `head` et `tail` ont aussi été rajoutés, dans la même veine que `fst` et `snd`

### Processus de compilation

Fichiers : encoder.ml, flattener.ml, codeGenerator.ml

J'ai remplacé le fichier instrs.ml par ces trois fichiers, qui réalisent dans chacun et dans cet ordre, une partie du processus de compilation :
- encoder.ml : prend une expression de type `mlexp` et produit du `code`, qui peut ensuite ou bien être passé au simulateur Ocaml (c.f. plus bas), ou bien passé à l'étape suivante de la compilation
- flattener.ml : prend du `code` en entrée, et retourne du `flat_code`. L'idée est que pour compiler les instructions en C, on doit se débarrasser de la récursion du type `code` : les instructions `Branch` et `AddDefs` contiennent eux-même du `code`. On fait donc en sorte d'obtenir une liste de code non-récursif (`flat_code`) qui se font référence les uns aux autres, ce qui se traduit en C par des pointeurs d'instructions vers d'autre morceaux de code.
- codeGenerator.ml : prend le `flat_code` et le transforme en code source C sous la forme d'une chaine de caractère

#### Encoder

Fichier : encoder.ml

Ce fichier contient la fonction `encode`, qui prend une expression de type `mlexp` et renvoie une liste de `instr`, c'est-à-dire une valeur de type `code`.

Le type `instr` a été augmenté des instructions suivantes :
- `Halt` : pour indiquer explicitement à la machine virtuelle qu'elle doit s'arrêter, en fin de programme.
- `QuoteBool`, `QuoteInt` : le constructeur d'origine (`Quote of value`) n'était pas assez `type-safe` pour la suite de la compilation.
- `QuoteEmptyList` : pour implémenter la valeur `[]`, _i.e._ la liste vide.
- `Call`, `AddDefs`, `RmDefs` : tout ce qu'il faut pour exécuter des expressions récursives. À noter que `RmDefs` ne prend aucun nombre en paramètre : dans le simulateur OCaml d'exécution de `code`, et à la différence du modèle présenté dans les transparents de cours, les définitions des let-rec bindings ne sont pas toutes stockées dans une même liste (le quatrième champ du quadruplet de la configuration, appelé **fds**), mais sont au contraire conservées séparées dans une liste de listes de définitions, et il suffit alors le moment venu de retirer la tête de cette liste de liste sans avoir besoin de conserver le nombre de définitions ajoutées précédemment.
- `MakeList` : joue le rôle de l'opérateur `::` dans la construction de listes. Calqué sur le modèle de l'instruction `Cons` qui elle construit des paires à partir du **terme** et de l'élément de tête de la **pile**.

#### Simulator

Fichier : simulateur.ml, simu

Ce fichier ne fait pas partie à proprement parler du processus de compilation mais il est logique de le mettre juste après la section concernant l'encoder.
