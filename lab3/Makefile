COMPILER = gcc
FILESYSTEM_FILES = rawdisk.c ssfs.c fs_support.c
FORMAT_FILES = fs_support.c rawdisk.c format_myfs.c
INFO_FILES = fs_support.c rawdisk.c info_myfs.c
CFLAGS = -D_FILE_OFFSET_BITS=64 -D_DARWIN_USE_64_BIT_INODE
build: $(FILESYSTEM_FILES)
	$(COMPILER) $(CFLAGS) $(FILESYSTEM_FILES) -o ssfs `pkg-config fuse --cflags --libs`
	@echo 'To Mount: ./ssfs -f [mount point]'
	@echo 'For more debug information, run with -d as well.'

tools: $(FORMAT_FILES) $(INFO_FILES)
	$(COMPILER) $(FORMAT_FILES) -o format_myfs
	$(COMPILER) $(INFO_FILES) -o info_myfs

clean:
	rm ssfs format_myfs info_myfs
