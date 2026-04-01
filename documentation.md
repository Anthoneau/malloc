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

<br>

##### mmap

<br>

mmap demande au système d'exploitation une page de mémoire. Généralement 4096 sera donnée pour plus de simplicité ? Je ne sais pas mais voici ce que dit le sujet par rapport à ça:

>The size of these zones must be a multiple of getpagesize() under macOS or sysconf(_SC_PAGESIZE) under Linux.


Pour malloc, on utilise mmap(), on doit checker aussi si la size qu'on donne en argument est correct. <br>
La plupart des projets malloc contiennent une structure `chunk` pour pouvoir travailler plus facilement les chunks de mémoires. Quand l'utilisateur demande un bloc de mémoire de taille 1 par exemple, notre malloc va demander une page de mémoire de 4096 octets et le système va faire avancer le break (brk) du programme. 4096 est beaucoup trop grand pour une taille de 1 donc on va venir split le chunk. C'est là qu'intervient la structure `chunk` qui ressemble généralement à ceci:

	struct s_chunk {
		void *data;
		size_t size;
		int free;
		struct s_chunk *next;
		struct s_chunk *prev;
	}

Je pense avoir tout mis. Donc `void *data` sera ce qui est renvoyé à l'utilisateur. `int free` est utile seulement pour nous, pour savoir si le chunk actuel est libre ou non (si non on passe le noeud suivant au lieu de renvoyer le chunk occupé). `size_t size` est simplement la taille du chunk actuel. Les deux dernières variables sont ce qui fait la liste doublement chainées. Donc oui c'est ça la liste chaînée de free dont je parlais avant.

Donc avec mon exemple où l'utilisateur demande un chunk de 1, on a notre chunk de taille 4096 qui est tout seul dans la liste et on vient le split pour avoir un chunk de taille demandé. Avec ça, on a 2 éléments dans la liste et si tout va bien, on marque le chunk comme étant utilisé, on lui donne sa taille on renvoit `void *data` depuis le `malloc`.

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

Pour free, on prend juste le `chunk` que l'utilisateur demande de free et on le met en disponible. Si par contre l'utilisateur demande de free le chunk de 1 et qu'il demande ensuite un chunk de 3996, au lieu de re demander au système de donner une page de mémoire, on peut simplement fusionner les deux chunks (1 et 3995). On le fait en amont avec free, qui merge les blocs non utilisé. C'est le soucis de fragmentation et de défragmentation.
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

où `MT-Safe` est défini comme ceci :

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

##### Zones mémoires

<br>

Le sujet parle de zone aussi mais je ne comprends pas encore ce que c'est. Voici ce qui est dit:

	• Each zone must contain at least 100 allocations.
		◦ “TINY” mallocs, from 1 to n bytes, will be stored in N bytes big zones.
		◦ “SMALL” mallocs, from (n+1) to m bytes, will be stored in M bytes big zones.
		◦ “LARGE” mallocs, from (m + 1) bytes and above, will be allocated outside the standard memory zones. This means they will be handled using mmap(), placing them in their own separate memory zone.
	• It’s up to you to define the size of n, m, N and M so that you find a good compromise between speed (saving on system recall) and saving memory.

<br>

##### Alignement

<br>

Le sujet met un gros `warning`:
>You must properly align the memory returned by your malloc

Je dois creuser un peu plus pour comprendre ce que l'allignement de la mémoire est exactement.

---

<br>

##### Link

<br>

Pour linker un fichier qui test mon `malloc`, je dois faire ceci `-L. -l (chemin_de_la_lib)` dans la racine du dossier `malloc`. Dans mon Makefile, j'ai la règle run qui est écrite comme tel:

	@cc main.c -o test -L. -l${LNNAME}

Donc je compile main.c en nommant l'exécutable `test`, je donne le dossier actuel pour donner le chemin de la librairie avec `-L` et je donne le nom de ma lib avec `-l`.
<br>
Si on essaie d'exécuter `test` directement, on ressort avec cet output:

	./test: error while loading shared libraries: libft_malloc.so: cannot open shared object file: No such file or directory

qui dit en gros qu'il ne trouve pas la `shared library` correspondante... Le linker `ld` utilise la variable d'environnement `LD_LIBRARY_PATH` pour connaître la position des fichiers de la lib. Donc il faut exécuter `test` comme ceci:

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
	set exec-wrapper env LD_LIBRARY_PATH=/test/. sera utilisé pour donné un environnement modifié à mon programme


---

<br>

##### Zones de mémoire

<br>

Par rapport aux zones mémoire, le sujet dit ceci (je l'ai dit plus haut):
>The size of these zones must be a multiple of getpagesize() under macOS or sysconf(_SC_PAGESIZE) under Linux.

Donc j'imagine que la zone `TINY` sera de la taille `sysconf(_SC_PAGESIZE)` ou peut être la moitié de ceci ? `SMALL` sera probablement le double ou la valeur normale...

Il faut comprendre déjà une chose : les données envoyées par mmap sont continues. Donc on a un bloc mémoire entier pour nous grâce à la mémoire virtuelle. Mais ça c'est encore autre chose, ce qui nous intéresse nous, c'est ce bloc mémoire.
Quand un utilisateur demande un bloc, on a notre struct t_chunk suivie de la zone envoyée à l'utilisateur. Donc `void *data` ne sert à rien. C'est redondant.

	Notre structure modifié :
	typedef struct s_chunk
	{
		int		used;
		size_t	size;
		struct	s_chunk *next;
		struct	s_chunk *prev;
	} t_chunk;

Notre valeur de retour sera donc `(void *)(chunk + 1)`.
Si notre première adresse mémoire est `0x1000`, alors notre premier t_chunk commencera à `0x1000` jusqu'à `0x1000 + sizeof(t_chunk)`. Notre zone mémoire sera `0x1000 + sizeof(t_chunk)` et terminera à `0x1000 + sizeof(t_chunk) + chunk->size`.

	0x1000                0x1000 + sizeof(t_chunk)
	|_______t_chunk_______|_______données_______|

Et donc la formule pour connaître la taille d'une zone mémoire :

	zone_memoire = 100 * (sizeof(t_chunk) + n);

où :
- zone_memoire = TINY, SMALL ou LARGE
- 100 c'est simplement le sujet qui le demande
	> Each zone must contain at least 100 allocations
- sizeof(t_chunk) = la taille de notre structure. Forcément elle doit être pris en compte dans le calcul, sinon tout sera faussé.
- `n` ou `n + 1` ou `m + 1` = la valeur à trouver

`N` et `M`sont les zones mémoires. `TINY` est en faite juste `N`. `SMALL` est juste `M`.

Le sujet dit ceci pour les zones `LARGE`'s :
> “LARGE” mallocs, from (m + 1) bytes and above, will be allocated outside the standard memory zones. This means they will be handled using mmap(), placing them in their own separate memory zone.

Donc chaque `malloc` de type `LARGE` fera un `mmap()` sans prendre en compte les autres zones. Pour le `free`, on utilisera `munmap()` directement dessus aussi.

Pour le fonctionnement de `free`, on utilisera `munmap()` seulement quand une zone est totalement libre.

<br>

##### Alignement

<br>

Par rapport à l'alignement de mémoire, [cette page wikipedia](https://fr.wikipedia.org/wiki/Alignement_en_m%C3%A9moire) explique assez bien le principe d'alignement.

Dans le wiki, il est dit que les processeurs fonctionnent mieux et plus rapidement quand les données sont alignées sur les addresses mémoires en multiple de 4 (ou de 2 ou 8). Par exemple :

	struct no_align
	{
		char c; //1
		double d; //8
		int i; //4
		char c2[3]; //3
	};

	struct align
	{
		double d; //8
		int i; //4
		char c2[3]; //3
		char c; //1
	};

On a des structures avec exactement les mêmes membres. Mais quand on se met à compter la place que chaque membre prend dans la mémoire, en partant de 0 par exemple, on se rend compte que le processeur va avoir du mal à lire tout ça. Le compilateur intervient généralement ici et met, ce qu'on appelle, du padding.

	struct no_align
	{
		char c; //1
		double d; //8
		int i; //4
		char c2[3]; //3
	};

Donc les deux premiers éléments ici prennent 9 octets et les deux suivants prennent 7 octets. C'est trop aléatoire, certains processeur ARM peuvent crash là dessus, mais comme dit précédemment le compilateur intervient et met du padding. La structure ressemblera donc à ceci :

	struct no_align
	{
		char c; //1
		char __pad1[7]; //7
		double d; //8
		int i; //4
		char c2[3]; //3
		char __pad2; //4
	};

Et maintenant nos membres sont alignés mais la structure prend plus de place. Au lieu de prendre 16 octets (8 + 4 + 1 + (1 * 3)), celle-ci prendra 24 octets.

C'est bien beau tout ça, mais ça n'a pas grand chose à voir avec malloc. C'est juste bon à savoir pour optimiser la place que nos structures prennent en mémoires.
C'est pas si vrai... Grâce à tout ceci, on comprend que le processeur préfère avoir des données à traiter avec des multiples de 2, 4, 8, 16, etc... Donc les données qu'on renvoit avec malloc, doivent aussi être alignées !

Pour ça, on a une macro :

	#define ALIGN(size) (((size) + 15) & ~15)

Cette macro défini l'alignement approprié pour chaque demande d'allocation.
Elle donne la fourchette haute de ce que l'utilisateur demande. Voyons voir un peu ce que ça veut dire concrètement :

Si l'utilisateur écrit `malloc(17);`,
On doit lui renvoyer une allocation réelle de 32 bits au minimum.
17 = `0001 0001` donc il a besoin d'au maximum `0010 0000` (32) et la macro nous renvoit ceci :

	(((17) + 15) & ~15)
	((32) & ~15)
	0010 0000 & 1111 0000
	0010 0000

Si le + 15 n'était pas là, on renverrai 16 :

	((17) & ~15)
	0001 0001 & 1111 0000
	0001 0000

Et on perdrait de l'information + de la capacité de stockage. Donc on donne la fourchette haute.

<br>

##### Pré-allocation

<br>

La préallocation se fera par rapport à la taille de la zone je pense... Par exemple avec `malloc(17)` (j'imagine que c'est `TINY`), on ne peut pas simplement demander à `mmap()` de nous donner 17 octets. La fonction va forcément nous renvoyer une `page` complète. Cette page, on ne l'a jette pas mais on l'a garde pour les prochaines allocations de la même zone.

<br>

##### Re définition des variables globales

<br>

On a le droit a deux variables globale. Pour l'instant on va seulement se pencher sur la variable qui s'occupe de nos allocations.
> You are allowed one global variable to manage your allocations and one for thread-safety.

Notre variable globale pour les allocations, sera en faite une liste vers les structures des zones mémoires.

	typedef struct s_chunk
	{
		int		used;
		// le compilateur va probablement mettre int __pad1; pour que la structure fasse 32 bits
		size_t	size;
		struct	s_chunk *next;
		struct	s_chunk *prev;
	}	t_chunk;

	typedef struct s_zone
	{
		size_t size;
		t_chunk *chunk;
		struct s_zone *next;
	}	t_zone;

	typedef struct s_alloc
	{
		t_zone *tiny;
		t_zone *small;
		t_zone *large;
	}	t_alloc;

	t_alloc g_alloc; //variable globale

`g_alloc` contiendra toutes les zones de mémoires. <br>
Ces zones sont des `t_zone`, avec comme membre :
- `size_t size`, qui est la taille totale de la zone (je pense que ce sera souvent le multiple d'une `pagesize`), sera calculé avec la fameuse formule qu'on a trouvé juste avant : `zone_memoire = 100 * (sizeof(t_chunk) + n);`
- `*chunk`, qui est la liste des chunks à l'intérieur de la zone
- `*next`, qui est la prochaine zone du même type. C'est une liste chaînée, on pourrait la faire en double aussi mais je n'en vois pas encore l'utilité.

D'ailleurs, connaît d'avance la taille de `t_chunk`, donc avec `n` = 0, on peut déduire que la taille d'une zone = 3200. `t_chunk` fait 32 octets, * 100, ça fait bien 3200. C'est plus petit que la moyenne des `pagesizes`, qui est de 4096 donc le `n` et le `m` sera bien utile. Dans tous les cas ce sera un multiple de `pagesize`.

<br>

##### Faisons un flow de A à Z

<br>

Essayons de refaire le flow de a à z.

L'utilisateur utilise malloc, on a une size et on doit checker quel type de malloc c'est.
La size, donnée en paramètre, doit être comparée aux tailles de zones ? À la formule `zone_memoire = 100 * (sizeof(t_chunk) + n);` ?
Parce que du coup, taille de zone = taille max d'une certaine zone. Donc on check, si `size` est plus petit ou égal à `TINY` alors on fout ça dans la liste `TINY`, sinon on check la même chose avec `SMALL` ou `LARGE`.
On demande à `mmap()` un page complète, on prend la valeur renvoyée par la macro `#define ALIGN(size) (((size) + 15) & ~15)` et on envoi à l'utilisateur le bon bloc mémoire avec la bonne taille minimum à l'utilisateur.

Donc concrètement avec `malloc(17);`, on aura avec les structures définies plus haut :

- entrée dans la fonction malloc
- on prend la size donnée, donc ici 17
- on check avec la formule de la taille de zone et on sait que ça va dans `TINY`
- on check si `t_zone *tiny;` existe déjà, ici non donc on le crée
- on crée une instance de t_zone
- on appelle `mmap()` qui nous renvoit une seule page entière
- on fout cette page à côté du chunk, normalement on peut renseigner à `mmap()` l'adresse mémoire à laquelle on veut placer la page donnée
- on initialise tout
- on travaille sur la size que l'utilisateur veut avoir

Là le délire de fragmentation est encore flou... Je ne suis pas sûr de savoir comment déplacer les morceaux voulu à côté des structures t_chunk crées...

En faite non c'est très simple. Déjà, la page donnée par `mmap()` va servir, non seulement, à garder en mémoire les blocs mémoire MAIS EN PLUS les t_zones ! Donc une zone `TINY`, par exemple, aura sa propre page ET ses propres chunks. Tout est séparé !
Donc notre page ressemblera à ceci :

	 t_zone_1 - t_chunk_1 - bloc_1 - t_chunk_2 - bloc_2 - t_chunk_3 - bloc_3
	|---------|-----------|--------|-----------|--------|-----------|--------|

Et quand cette zone n'est plus du tout utilisé, on utilise `munmap()`. Tout prend sens !
Et donc pour la fragmentation, si par exemple le bloc_2 n'est plus utilisé (que l'utilisateur demande à le `free`), notre programme va venir checker les voisins du bloc 2 pour vérifier si il n'y a pas déjà des espaces inutilisés. Si dans notre exemple, le bloc 3 a été `free` juste avant, alors on a deux blocs qui se suivent qui peuvent être fusionnés. La taille du bloc 2 (`t_chunk->size`) sera donc plus grande de `t_chunk_3->size` + `sizeof(t_chunk)`. On ne vient pas nettoyer les données, parce que ça ne sert à rien. C'est pour ça que certains mallocs sont parfois sales.

Par contre que se passe-t-il quand on a 10 blocs et que le bloc 2 et 7 sont free ? On ne défragmente pas ?

<br>

##### Juste une précision

<br>

Je n'étais pas au courant que `pointeur_typé + 1` donnait en gros `sizeof(pointeur_typé) + 1`.
Donc ici :

	t_chunk *current = zone->chunk;
	return (void *)(current + 1);

Donne bien la `FIN` du pointeur. C'est différent de `ptr++`.

---

# source

- [man `mmap()`](https://man7.org/linux/man-pages/man2/mmap.2.html)
- [man `munmap()`](https://man7.org/linux/man-pages/man3/munmap.3p.html)
- [man `malloc`](https://man7.org/linux/man-pages/man3/free.3.html)
- [exemple d'implementation de malloc 1](https://gitlab.com/paulguillier/malloc)
- [exemple d'implementation de malloc 2](https://github.com/jterrazz/42-malloc)
- [exemple d'implementation de malloc 3](https://github.com/st3w4r/42-malloc)
- [discussion de l'alignement sur stackoverflow](https://stackoverflow.com/questions/8752546/how-does-malloc-understand-alignment)
- [memory paging wikipedia](https://en.wikipedia.org/wiki/Memory_paging)
- [memory paging en video](https://www.youtube.com/watch?v=fGP6VHxqkIM)
- [ça m'a un peu aidé mais ça part dans tous les sens](https://gee.cs.oswego.edu/dl/html/malloc.html)
- [nedmalloc, sert à rien](https://www.nedprod.com/programs/portable/nedmalloc/index.html)
- [opérateur en C (pour `~`)](https://learn.microsoft.com/fr-fr/cpp/c-language/c-operators?view=msvc-170)
- [rappel des bitwise-operators](https://www.w3schools.com/c/c_bitwise_operators.php)
- [online compiler](https://www.programiz.com/c-programming/online-compiler/)
- [alignement en mémoire](https://fr.wikipedia.org/wiki/Alignement_en_m%C3%A9moire)
- [playlist youtube de memory management](https://www.youtube.com/watch?v=ObakIXh3mNw&list=PLlqoNzE2bqWulFVmkdoVI9S7Jr0uvPVEP)
- [What if I try to malloc WAY too much memory?](https://www.youtube.com/watch?v=Fq9chEBQMFE)