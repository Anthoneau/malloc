# malloc

## Recherches préliminaires

- mmap -> sert à mapper la mémoire

- munmap -> sert à démapper la mémoire
	> à voir si le mapping agit comme allocation

- getpagesize - sysconf font plus ou moins la même chose... De ce que je comprends pour l'instant,
	ça sert à tester si il y a assez d'espaces dans la heap.
	J'aimerais utiliser les deux en même temps pour pouvoir coder à la fois sur mon mac et sur les linux de l'école. J'imagine une structure de données dans mon header avec ses informations pratiques + le système d'exploitation utilisé par l'utilisateur. Ce serait un bool isLinux, si c'est le cas on utilise sysconf, sinon getpagesize. L'un renvoit un int et l'autre un long. Ça reste des entiers, j'espère que ça ne posera pas de soucis.

- apparemment free tient une liste des zones à libérés ([source ici](https://stackoverflow.com/questions/1119134/how-do-malloc-and-free-work)).
	Si c'est vrai, je pense que la variable globale autorisée par le sujet parle d'un tableau de pointeur.

- Évidemment malloc
