sed -i 's/version.c \\/&\n\tmalloc.c \\/' ./gawk-3.1.8/Makefile
sed -i 's/version.\$/&\(OBJEXT\) malloc.\$/' ./gawk-3.1.8/Makefile
