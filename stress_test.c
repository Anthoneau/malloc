#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "includes/malloc.h"

#define THREADS             8
#define THREAD_ITERATIONS   50000
#define RANDOM_SLOTS        1000
#define RANDOM_ITERATIONS   500000
#define MASS_ALLOCATIONS    10000

static void fail(const char *msg)
{
    write(2, msg, strlen(msg));
    write(2, "\n", 1);
    exit(1);
}

static void fill_memory(unsigned char *ptr, size_t size,
    unsigned char value)
{
    size_t i = 0;

    while (i < size)
    {
        ptr[i] = value;
        i++;
    }
}

static void check_memory(unsigned char *ptr, size_t size,
    unsigned char value)
{
    size_t i = 0;

    while (i < size)
    {
        if (ptr[i] != value)
            fail("MEMORY CORRUPTION");
        i++;
    }
}

/*
** ============================================================
** TINY
** ============================================================
*/

static void test_mass_tiny(void)
{
    void *ptrs[MASS_ALLOCATIONS];
    int i;

    printf("TEST mass TINY...\n");

    for (i = 0; i < MASS_ALLOCATIONS; i++)
    {
        ptrs[i] = malloc(TINY_SIZE);

        if (!ptrs[i])
            fail("TINY malloc failed");

        fill_memory(ptrs[i], TINY_SIZE,
            (unsigned char)(i % 255));
    }

    for (i = 0; i < MASS_ALLOCATIONS; i++)
        check_memory(ptrs[i], TINY_SIZE,
            (unsigned char)(i % 255));

    for (i = 0; i < MASS_ALLOCATIONS; i++)
        free(ptrs[i]);

    printf("OK\n");
}

/*
** ============================================================
** SMALL
** Utilise exactement SMALL_SIZE = 4096.
** ============================================================
*/

static void test_mass_small(void)
{
    void *ptrs[2000];
    int i;

    printf("TEST mass SMALL...\n");

    for (i = 0; i < 2000; i++)
    {
        ptrs[i] = malloc(SMALL_SIZE);

        if (!ptrs[i])
            fail("SMALL malloc failed");

        fill_memory(ptrs[i], SMALL_SIZE,
            (unsigned char)(i % 255));
    }

    for (i = 0; i < 2000; i++)
        check_memory(ptrs[i], SMALL_SIZE,
            (unsigned char)(i % 255));

    for (i = 0; i < 2000; i++)
        free(ptrs[i]);

    printf("OK\n");
}

/*
** ============================================================
** LARGE
**
** Une allocation LARGE est une allocation > SMALL_SIZE.
**
** SMALL_MAX est la taille approximative d'une ZONE SMALL,
** pas la limite entre SMALL et LARGE.
** ============================================================
*/

static void test_mass_large(void)
{
    void *ptrs[1000];
    size_t sizes[1000];
    int i;

    printf("TEST mass LARGE...\n");

    for (i = 0; i < 1000; i++)
    {
        sizes[i] = SMALL_SIZE + 1 + (i % 10000);

        ptrs[i] = malloc(sizes[i]);

        if (!ptrs[i])
            fail("LARGE malloc failed");

        fill_memory(ptrs[i], sizes[i],
            (unsigned char)(i % 255));
    }

    for (i = 0; i < 1000; i++)
        check_memory(ptrs[i], sizes[i],
            (unsigned char)(i % 255));

    for (i = 0; i < 1000; i++)
        free(ptrs[i]);

    printf("OK\n");
}

/*
** ============================================================
** Frontières TINY / SMALL / LARGE
**
** Très important.
** ============================================================
*/

static void test_boundaries(void)
{
    size_t sizes[] =
    {
        1,
        15,
        16,
        17,

        TINY_SIZE - 2,
        TINY_SIZE - 1,
        TINY_SIZE,
        TINY_SIZE + 1,
        TINY_SIZE + 2,

        SMALL_SIZE - 2,
        SMALL_SIZE - 1,
        SMALL_SIZE,
        SMALL_SIZE + 1,
        SMALL_SIZE + 2,

        SMALL_SIZE * 2,
        SMALL_SIZE * 10,

        SMALL_MAX - 1,
        SMALL_MAX,
        SMALL_MAX + 1
    };

    size_t count;
    size_t i;
    unsigned char *ptr;

    printf("TEST boundaries...\n");

    count = sizeof(sizes) / sizeof(sizes[0]);

    for (i = 0; i < count; i++)
    {
        ptr = malloc(sizes[i]);

        if (!ptr)
            fail("boundary malloc failed");

        fill_memory(ptr, sizes[i], 0x5A);
        check_memory(ptr, sizes[i], 0x5A);

        free(ptr);
    }

    printf("OK\n");
}

/*
** ============================================================
** Balaye toutes les tailles autour de TINY_SIZE.
** ============================================================
*/

static void test_tiny_boundary_stress(void)
{
    int delta;
    int repetition;
    size_t size;
    unsigned char *ptr;

    printf("TEST TINY boundary stress...\n");

    for (repetition = 0; repetition < 10000; repetition++)
    {
        for (delta = -32; delta <= 32; delta++)
        {
            size = TINY_SIZE + delta;

            ptr = malloc(size);

            if (!ptr)
                fail("TINY boundary malloc failed");

            fill_memory(ptr, size,
                (unsigned char)(repetition % 255));

            check_memory(ptr, size,
                (unsigned char)(repetition % 255));

            free(ptr);
        }
    }

    printf("OK\n");
}

/*
** ============================================================
** Même chose autour de SMALL_SIZE.
** ============================================================
*/

static void test_small_boundary_stress(void)
{
    int delta;
    int repetition;
    size_t size;
    unsigned char *ptr;

    printf("TEST SMALL boundary stress...\n");

    for (repetition = 0; repetition < 5000; repetition++)
    {
        for (delta = -32; delta <= 32; delta++)
        {
            size = SMALL_SIZE + delta;

            ptr = malloc(size);

            if (!ptr)
                fail("SMALL boundary malloc failed");

            fill_memory(ptr, size,
                (unsigned char)(repetition % 255));

            check_memory(ptr, size,
                (unsigned char)(repetition % 255));

            free(ptr);
        }
    }

    printf("OK\n");
}

/*
** ============================================================
** Force plusieurs zones TINY.
**
** Comme une zone est dimensionnée pour environ 100 allocations
** maximales, 1000 malloc(TINY_SIZE) doivent largement suffire
** à créer plusieurs zones.
** ============================================================
*/

static void test_many_tiny_zones(void)
{
    void *ptrs[1000];
    int i;

    printf("TEST multiple TINY zones...\n");

    for (i = 0; i < 1000; i++)
    {
        ptrs[i] = malloc(TINY_SIZE);

        if (!ptrs[i])
            fail("multiple TINY zones malloc failed");

        memset(ptrs[i], i % 255, TINY_SIZE);
    }

    for (i = 0; i < 1000; i++)
        check_memory(ptrs[i], TINY_SIZE,
            (unsigned char)(i % 255));

    for (i = 0; i < 1000; i++)
        free(ptrs[i]);

    printf("OK\n");
}

/*
** ============================================================
** Force plusieurs zones SMALL.
** ============================================================
*/

static void test_many_small_zones(void)
{
    void *ptrs[500];
    int i;

    printf("TEST multiple SMALL zones...\n");

    for (i = 0; i < 500; i++)
    {
        ptrs[i] = malloc(SMALL_SIZE);

        if (!ptrs[i])
            fail("multiple SMALL zones malloc failed");

        memset(ptrs[i], i % 255, SMALL_SIZE);
    }

    for (i = 0; i < 500; i++)
        check_memory(ptrs[i], SMALL_SIZE,
            (unsigned char)(i % 255));

    for (i = 0; i < 500; i++)
        free(ptrs[i]);

    printf("OK\n");
}

/*
** ============================================================
** Fragmentation TINY.
** ============================================================
*/

static void test_tiny_fragmentation(void)
{
    void *ptrs[5000];
    int i;

    printf("TEST TINY fragmentation...\n");

    for (i = 0; i < 5000; i++)
    {
        ptrs[i] = malloc(TINY_SIZE);

        if (!ptrs[i])
            fail("TINY fragmentation malloc failed");

        memset(ptrs[i], i % 255, TINY_SIZE);
    }

    /*
    ** Libère un chunk sur deux.
    */
    for (i = 0; i < 5000; i += 2)
    {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }

    /*
    ** Les autres doivent être intacts.
    */
    for (i = 1; i < 5000; i += 2)
        check_memory(ptrs[i], TINY_SIZE,
            (unsigned char)(i % 255));

    /*
    ** Réutilise les trous avec des allocations plus petites.
    */
    for (i = 0; i < 5000; i += 2)
    {
        ptrs[i] = malloc(64);

        if (!ptrs[i])
            fail("TINY reuse failed");

        memset(ptrs[i], 0x42, 64);
    }

    /*
    ** Vérifier que les voisins n'ont pas été corrompus.
    */
    for (i = 1; i < 5000; i += 2)
        check_memory(ptrs[i], TINY_SIZE,
            (unsigned char)(i % 255));

    for (i = 0; i < 5000; i++)
        free(ptrs[i]);

    printf("OK\n");
}

/*
** ============================================================
** Fragmentation SMALL.
** ============================================================
*/

static void test_small_fragmentation(void)
{
    void *ptrs[1000];
    int i;

    printf("TEST SMALL fragmentation...\n");

    for (i = 0; i < 1000; i++)
    {
        ptrs[i] = malloc(SMALL_SIZE);

        if (!ptrs[i])
            fail("SMALL fragmentation malloc failed");

        memset(ptrs[i], i % 255, SMALL_SIZE);
    }

    for (i = 0; i < 1000; i += 2)
    {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }

    for (i = 1; i < 1000; i += 2)
        check_memory(ptrs[i], SMALL_SIZE,
            (unsigned char)(i % 255));

    for (i = 0; i < 1000; i += 2)
    {
        ptrs[i] = malloc(2048);

        if (!ptrs[i])
            fail("SMALL reuse failed");

        memset(ptrs[i], 0x33, 2048);
    }

    for (i = 1; i < 1000; i += 2)
        check_memory(ptrs[i], SMALL_SIZE,
            (unsigned char)(i % 255));

    for (i = 0; i < 1000; i++)
        free(ptrs[i]);

    printf("OK\n");
}

/*
** ============================================================
** Pseudo random déterministe.
** ============================================================
*/

static uint32_t pseudo_random(uint32_t *state)
{
    *state = (*state * 1103515245U + 12345U);
    return *state;
}

/*
** Mélange volontaire :
**
** ~40 % TINY
** ~40 % SMALL
** ~20 % LARGE
*/
static size_t random_size(uint32_t *state)
{
    uint32_t category;

    category = pseudo_random(state) % 10;

    if (category < 4)
        return (pseudo_random(state) % TINY_SIZE) + 1;

    if (category < 8)
        return TINY_SIZE + 1
            + (pseudo_random(state)
            % (SMALL_SIZE - TINY_SIZE));

    return SMALL_SIZE + 1
        + (pseudo_random(state) % (SMALL_SIZE * 10));
}

static void test_random(void)
{
    void *ptr[RANDOM_SLOTS] = {0};
    size_t sizes[RANDOM_SLOTS] = {0};
    unsigned char patterns[RANDOM_SLOTS] = {0};

    uint32_t state = 0x12345678;
    int i;
    int index;
    size_t size;
    unsigned char pattern;

    printf("TEST random mixed allocations...\n");

    for (i = 0; i < RANDOM_ITERATIONS; i++)
    {
        index = pseudo_random(&state) % RANDOM_SLOTS;

        if (ptr[index])
        {
            check_memory(ptr[index],
                sizes[index],
                patterns[index]);

            free(ptr[index]);
            ptr[index] = NULL;
        }
        else
        {
            size = random_size(&state);

            pattern =
                (unsigned char)(pseudo_random(&state) % 255);

            ptr[index] = malloc(size);

            if (!ptr[index])
                fail("random malloc failed");

            sizes[index] = size;
            patterns[index] = pattern;

            fill_memory(ptr[index], size, pattern);
        }
    }

    for (i = 0; i < RANDOM_SLOTS; i++)
    {
        if (ptr[i])
        {
            check_memory(ptr[i],
                sizes[i],
                patterns[i]);

            free(ptr[i]);
        }
    }

    printf("OK\n");
}

/*
** ============================================================
** MULTITHREAD
** ============================================================
*/

typedef struct s_thread_data
{
    int id;
} t_thread_data;

static void *thread_stress(void *arg)
{
    t_thread_data *data;
    void *ptrs[200] = {0};
    size_t sizes[200] = {0};
    unsigned char patterns[200] = {0};

    uint32_t state;
    int i;
    int index;
    size_t size;
    unsigned char pattern;

    data = arg;

    state = 0x12345678U
        + ((uint32_t)data->id * 1234567U);

    for (i = 0; i < THREAD_ITERATIONS; i++)
    {
        index = pseudo_random(&state) % 200;

        if (ptrs[index])
        {
            check_memory(ptrs[index],
                sizes[index],
                patterns[index]);

            free(ptrs[index]);
            ptrs[index] = NULL;
        }
        else
        {
            size = random_size(&state);

            pattern =
                (unsigned char)(
                    (data->id * 31 + index) % 255
                );

            ptrs[index] = malloc(size);

            if (!ptrs[index])
                fail("thread malloc failed");

            sizes[index] = size;
            patterns[index] = pattern;

            fill_memory(ptrs[index],
                size,
                pattern);
        }
    }

    for (i = 0; i < 200; i++)
    {
        if (ptrs[i])
        {
            check_memory(ptrs[i],
                sizes[i],
                patterns[i]);

            free(ptrs[i]);
        }
    }

    return NULL;
}

static void test_threads(void)
{
    pthread_t threads[THREADS];
    t_thread_data data[THREADS];
    int i;

    printf("TEST multithread...\n");

    for (i = 0; i < THREADS; i++)
    {
        data[i].id = i;

        if (pthread_create(
            &threads[i],
            NULL,
            thread_stress,
            &data[i]) != 0)
            fail("pthread_create failed");
    }

    for (i = 0; i < THREADS; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
            fail("pthread_join failed");
    }

    printf("OK\n");
}

int main(void)
{
    test_boundaries();

    test_mass_tiny();
    test_mass_small();
    test_mass_large();

    test_many_tiny_zones();
    test_many_small_zones();

    test_tiny_boundary_stress();
    test_small_boundary_stress();

    test_tiny_fragmentation();
    test_small_fragmentation();

    test_random();

    test_threads();

    printf("\n");
    printf("============================\n");
    printf("ALL STRESS TESTS PASSED\n");
    printf("============================\n");

    return 0;
}
