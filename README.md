# Architectures Parallèles
liste des différents devoirs effetuées durant ce cours avec Adrien Cambillau.
[lien vers le site du cours](http://www.uqac.ca/flemieux/INF856/index.html)

## Devoir 1
Calcul de la suite de Fibonacci en langage C avec et sans threads, utilisation de la librairie "pthread".
pour compiler: 
```bash
Fibonacci.c -o Fibonacci -lpthread
```

## Devoir 2
Calculs de multiplication matricielle, de tri par fusion et d'opérations sur des tableaux en langage C avec la librairie OpenMP.
pour compiler:
```bash
./compileAll.sh
```
pour lancer l'analyse du tri par fusion:
```bash
./script_d2.sh
```

## Devoir 3
tri d'un tableau de taille n en utilisant le cluster dim_linuxmpi de l'UQAC
pour compiler sur toutes les machines:
```bash
./compile_dim_linuxmpi.sh
```

## Devoir4
implémentation de l'algorithme de Floyd-Warshall pour déterminer les plus courts chemins dans une matrice d'adjacence avec la librairie OpenCL.
pour générer une matrice d'adjacence de taille n:
```bash
./generateMatrix.exe n
```
pour lancer le programme avec la matrice générée ci-dessus:
```bash
./tp4.exe graph
```

## Devoir5
 L'objectif consiste  trouver un cycle hamiltonien de
longueur minimale dans un graph orienté (problme du commis voyageur) en utilisant OpenMP et MPI.

