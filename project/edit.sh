sed -i 's/replace.c \\/&\n\tmy_malloc.c \\/' ./gawk-3.1.8/Makefile
sed -i 's/replace.\$/&\(OBJEXT\) my_malloc.\$/' ./gawk-3.1.8/Makefile
sed -i 's/replace.Po/&\ninclude .\/\$\(DEPDIR\)\/my_malloc.Po/' ./gawk-3.1.8/Makefile
