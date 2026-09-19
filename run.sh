#!/bin/sh
if [ "$(uname)" = "Darwin" ]; then
	export DYLD_LIBRARY_PATH=.
	export DYLD_INSERT_LIBRARIES=./libft_malloc.so
	export DYLD_FORCE_FLAT_NAMESPACE=1
else
	export LD_LIBRARY_PATH=.
	export LD_PRELOAD=./libft_malloc.so
fi
exec "$@"