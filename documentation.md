# malloc

## fonctions autorisées :
- mmap(2)
- munmap(2)
- getpagesize under OSX or sysconf(_SC_PAGESIZE) under linux
- getrlimit(2)
- The authorized functions within your libft (write(2) for example)
- The functions from libpthread

## Recherches préliminaires

- mmap -> sert à mapper la mémoire

- munmap -> sert à démapper la mémoire
	> à voir si le mapping agit comme allocation

- getpagesize - sysconf font plus ou moins la même chose... De ce que je comprends pour l'instant,
	ça sert à tester si il y a assez d'espaces dans la heap.
	J'aimerais utiliser les deux en même temps pour pouvoir coder à la fois sur mon mac et sur les linux de l'école. J'imagine une structure de données dans mon header avec ses informations pratiques + le système d'exploitation utilisé par l'utilisateur. Ce serait un bool isLinux, si c'est le cas on utilise sysconf, sinon getpagesize. L'un renvoit un int et l'autre un long. Ça reste des entiers, j'espère que ça ne posera pas de soucis.

- apparemment free tient une liste des zones à libérés ([source ici](https://stackoverflow.com/questions/1119134/how-do-malloc-and-free-work)).
	Si c'est vrai, je pense que la variable globale autorisée par le sujet parle d'un tableau de pointeur.

#### de ce que j'ai compris
mmap demande au système d'exploitation une page de mémoire. Généralement 4096 sera donnée pour plus de simplicité ? Je ne sais pas mais voici ce que dit le sujet par rapport à ça:

>The size of these zones must be a multiple of getpagesize() under macOS or sysconf(_SC_PAGESIZ
under Linux.


Pour malloc, on utilise mmap(), on doit checker aussi si la size qu'on donne en argument est correct. <br>
La plupart des projets malloc contiennent une structure `chunk` pour pouvoir travailler plus facilement les chunks de mémoires. Quand l'utilisateur demande un bloc de mémoire de taille 1 par exemple, notre malloc va demander une page de mémoire de 4096 octets et le système va faire avancer le break (brk) du programme. 4096 est beaucoup trop grand pour une taille de 1 donc on va venir split le chunk. C'est là qu'intervient la structure `chunk` qui ressemble généralement à ceci:

	struct s_chunk {
		void *data;
		size_t size;
		int free;
		struct s_chunk *next;
		struct s_chunk *prev;
	}

Je pense avoir tout mis. Donc `void *data` sera ce qui est renvoyé à l'utilisateur. `int free` est utile seulement pour nous, pour savoir si le chunk actuel est libre ou non (si non on passe on noeud suivant au lieu de renvoyer le chunk occupé). `size_t size` est simplement la taille du chunk actuel. Les deux dernières variables sont ce qui fait la liste doublement chainées. Donc oui c'est ça la liste chaînée de free dont je parlais avant.

Donc avec mon exemple où l'utilisateur demande un chunk de 1, on a notre chunk de taille 4096 qui est tout seul dans la liste et on vient le split pour avoir un chunk de taille demandé. Avec ça, on a 2 élément dans la liste et si tout va bien, on marque le chunk comme étant utilisé, on lui donne sa taille on renvoit `void *data` depuis le `malloc`.

Si l'utilisateur refait une demande d'allocation de mémoire, par exemple de 100 cette fois ci, alors au lieu de refaire une demande au système d'exploitation, on check les éléments de la liste 1 à 1 pour voir si de la mémoire est inutilisée.

Encore dans notre exemple:

	-----------       -----------
	|         |       |         |
	|    1    |<----->|   4095  |
	|    1    |       |    0    |
	-----------       -----------

On a un chunk de taille 1 utilisé et un chunk de taille 4095 non-utilisé. Donc on prend le chunk le plus grand pour ensuite le split et ça devient ceci:

	-----------       -----------       -----------
	|         |       |         |       |         |
	|    1    |<----->|   100   |<----->|   3995  |
	|    1    |       |    1    |       |    0    |
	-----------       -----------       -----------

C'est efficace parce qu'on évite de faire un appel système mais en contre-partie on doit itérer dans toutes la liste. Ce n'est que de complexité O(n) mais si on à 1 million d'éléments ça devient un peu compliqué.

Pour free, on prend juste le `chunk` que l'utilisateur demande de free et on le met en disponible. si par contre on a un chunk de 1, comme dans notre exemple, et que l'utilisateur demande un chunk de 3996, au lieu de re demander au système de donner une page de mémoire, on peut simplement fusionner les deux chunks. On le fait en amont avec free, qui merge les blocs non utilisé. C'est le soucis de fragmentation et de défragmentation.
<br>
Ça correspond bien à ce que le sujet demande:
 > With performance in mind, you must limit the number of calls to mmap(), but also
to munmap(). You have to “pre-allocate” some memory zones to store your “small”
and “medium” malloc.

<br>

`realloc` est un problème pour plus tard. D'abord je commence à faire `malloc` et `free` et ensuite on verra bien.

Je ne comprends pas quand utilisé `getpagesize` ou `sysconf`, ni même les threads. Je ne trouve pas les ressources nécessaires.

Dans ce [forum](https://stackoverflow.com/questions/35101016/performing-malloc-in-threads), ils disent que malloc n'est que thread safe. Je pensais qu'il utilisait des threads pour allouer de la mémoire mais je me suis trompé. Il ne fait qu'utiliser des mutexs donc ?
[Ici](https://man7.org/linux/man-pages/man3/malloc.3.html_) (le man) ils montrent ceci :

       ┌──────────────────────────────────────┬───────────────┬─────────┐
       │ Interface                            │ Attribute     │ Value   │
       ├──────────────────────────────────────┼───────────────┼─────────┤
       │ malloc(), free(), calloc(),          │ Thread safety │ MT-Safe │
       │ realloc()                            │               │         │
       └──────────────────────────────────────┴───────────────┴─────────┘

où `MT-Safe` :

	MT-Safe
              MT-Safe or Thread-Safe functions are safe to call in the
              presence of other threads.  MT, in MT-Safe, stands for
              Multi Thread.

              Being MT-Safe does not imply a function is atomic, nor that
              it uses any of the memory synchronization mechanisms POSIX
              exposes to users.  It is even possible that calling MT-Safe
              functions in sequence does not yield an MT-Safe
              combination.  For example, having a thread call two MT-Safe
              functions one right after the other does not guarantee
              behavior equivalent to atomic execution of a combination of
              both functions, since concurrent calls in other threads may
              interfere in a destructive way.

              Whole-program optimizations that could inline functions
              across library interfaces may expose unsafe reordering, and
              so performing inlining across the GNU C Library interface
              is not recommended.  The documented MT-Safety status is not
              guaranteed under whole-program optimization.  However,
              functions defined in user-visible headers are designed to
              be safe for inlining.


Donc j'imagine que ça parle bien de mutex. Je ne dois pas oublier de tester dans un programme multi thread du coup.

On a droit à deux variables globales pour le programme. Je pense que l'une est la liste chaînée et que l'autre sera un mutex simple, un lock.

<br>

Le sujet parle de zone aussi mais je ne comprends pas encore ce que c'est. Voici ce que le sujet dit:

	• Each zone must contain at least 100 allocations.
		◦ “TINY” mallocs, from 1 to n bytes, will be stored in N bytes big zones.
		◦ “SMALL” mallocs, from (n+1) to m bytes, will be stored in M bytes big zones.
		◦ “LARGE” mallocs, from (m + 1) bytes and above, will be allocated outside the standard memory zones. This means they will be handled using mmap(), placing them in their own separate memory zone.
	• It’s up to you to define the size of n, m, N and M so that you find a good compromise between speed (saving on system recall) and saving memory.

<br>

Le sujet met un gros `warning`:
>You must properly align the memory returned by your malloc

Je dois creuser l'allignement de la mémoire.

---

Pour linker un fichier qui test mon malloc, je dois faire ceci `-L. -l (chemin_de_la_lib)`. Dans mon Makefile, j'ai la règle run qui est écrite comme tel:

	@cc main.c -o test -L. -l${LNNAME}

Donc je compile main.c en nommant l'exécutable en test, je donne le dossier actuel pour donner le chemin de la librairie avec `-L` et je donne le nom de ma lib avec `-l`.
<br>
Si on essaie d'exécuter `test` directement, on ressort avec cet output:

	./test: error while loading shared libraries: libft_malloc.so: cannot open shared object file: No such file or directory

qui dit en gros qu'il ne trouve pas la `shared librarie` correspondante... Le linker `ld` utilise la variable d'environnement `LD_LIBRARY_PATH` pour connaître la position des fichiers de la lib. Donc il faut exécuter `test` comme ceci:

	LD_LIBRARY_PATH=. ./test

Super, ça fonctionne. Sauf que j'ai mis des `printf`'s un peu partout dans mon programme et cette fonction utilise `malloc`. Sauf que mon `malloc` ne fonctionne pas ! Donc segfault directement.
Si j'export cette variable d'environnement, les programmes qui utilisent `malloc` ne fonctionneront plus, comme gdb par exemple qui est super utile pour vérifier ce que je fais. Je sais qu'il y a un moyen d'export une variable localement dans gdb pour faire fonctionner un programme, mais je dois trouver comment.

	bash	> gdb --tui test
	break nom_de_fonction pour avoir un breakpoint
	break file.c:line pour break à une ligne spécifique
	info break pour avoir les infos
	disable num_breakpoint désactive un breakpoint
	enable num_breakpoint active un breakpoint
	clear function_name/file.c:line efface un breakpoint spécifique
	delete num_breakpoint
	run pour lancer le programme
	next pour continuer le programme
	step pour aller dans la fonction
	continue pour continuer jusqu'au prochain breakpoint
	kill pour terminer le programme
	print var_name pour voir la valeur d'une variable
	SUPER IMPORTANT !!!!
	set exec-wrapper env "LD_LIBRARY_PATH=/test/."
