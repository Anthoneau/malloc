chmod +x run.sh
cc test_align.c   -o test_align
cc test_stress.c  -o test_stress
cc test_errors.c  -o test_errors
cc test_realloc.c -o test_realloc
cc test_show.c    -o test_show -L. -lft_malloc

./run.sh ./test_align
./run.sh ./test_stress
./run.sh ./test_errors
./run.sh ./test_realloc
LD_LIBRARY_PATH=. ./test_show