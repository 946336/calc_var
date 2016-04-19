#!/usr/bin/env bash

rm -f solution.tmp solution.c

cat \
    *.h \
    *.c \
    >> solution.tmp

mv solution.tmp solution.c

# sed -i 's/#include ".*/\/\/\0/g' solution.c

make solution

