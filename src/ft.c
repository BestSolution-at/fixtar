/* cc  -o ft ft.c -O2 -s */
/************************************************************************
 *
 *	ft - copy valid tar data from damaged tarfiles
 *	     (moved blocks are found & copied by "ft" too)
 *	
 *	USAGE:
 *		ft < damaged.tar > repaired.tar
 *		ft < damaged.tar | tee repaired.tar | tar -tf -
 *		ft < damaged.tar | tar -xvf -
 *	
 *	(see below for more)
 *	
 ***********************************************************************/
 /*
  * symbolic code:
  * 	do_until_EOF
  * 	- find valid tar hdr
  * 		- if usual hdr: write hdr and according data
  * 		- if ext. hdr : write hdr and according data
  * 			- get according usual hdr -> write hdr and according data
  * 		- if error occurs while reading data: fill up with newline 
  *
  * released under the terms of the GNU Public License V2
  *
  * (c) Thomas Graf
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* strict ANSI compilation don't know inline */
#ifndef inline
#	define inline
#endif

static char rcsid[] = "$Id: ft.c,v 1.21.1.5 2008/04/09 12:23:03 tom Exp $";

/* this prototype should avoid warning for: gcc -pedantic ... */
int snprintf(char *str, size_t size, const char *format, ...);

typedef unsigned long ulong;
typedef struct data {
	char *dptr;
	ulong dsize, pos;
} Data;

/* but only 8 chars used:
 * - C appends to the string a \0 and does not claim that's the array is
 *   to short 
 * - C++ find the too short array
 */
static char magicString[3][9] = {
	"\0\0\0\0\0\0\0\0",   /* old tar */
	"ustar\x20\x20\x00", /* GNU tar */
	"ustar\x00\x30\x30" /* ustar/POSIX */
};

static inline ulong roundTo512(size_t size){
	return size % 512 ? size + 512 - (size % 512) : size;
}


int checksum(const char *buffer){
	unsigned i, s, t, u;

	i = t = 0;
	s = 8 * ' '; /* signed checksum */
	u =  0xff & (8 * ' '); /* unsigned */
	sscanf(buffer + 148, "%8o", &t);
	
	for(i = 0; i < 148; ++i){
		s += buffer[i];
		u += 0xff & buffer[i];
	}
	/* now skip ckecksum field, beacuse of init it */
	for(i = 156; i < 512; ++i){
		s += buffer[i];
		u += 0xff & buffer[i];
	}

	return t == u || t == s;
}

int main(int a, char *v[]){
	char b[1024], *zero = b+512, *headerFlag = b+156;
	int c, i;
	ulong size, fillbytes;
	const char *help[] = {
			"ft - copy valid tar data from damaged tarfiles",/*{{{*/
			"     (moved blocks are found & copied by \"ft\" too)",
			"\nUSAGE:",
			"\tft < damaged.tar > repaired.tar",
			"\tft < damaged.tar | tee repaired.tar | tar -tf - | tee list.txt",
			"\tft < damaged.tar | tar --backup=numbered -xvf -",
			"",
			"*** I do my best to handle posix or vendor tar headers",
			"*** but I'm not an expert.",
			"",
			"\nNOTE:",
			"\t- no options, no diagnostic messages",
			"\t- a truncated file is filled with newlines and",
			"\t  you can see an error message",
			"\t- it's not guaranteed that a tar extention is",
			"\t  situated to the according data. avoiding trouble with",
			"\t  double names, you should check the file list of",
			"\t  resulting archive and use tar in this way:",
			"\t  $ tar --backup=numbered -xvf repaired.tar",
			"\t  or use \"-k\" option while extracting",
			"\t- vendor or posix extension have auxiliary names like:",
			"\t  ./@LongLink, ./@LongName or */PaxHeader/*, which are",
			"\t  not listed by tar, but by cpio or pax - remember this",
			"\t  if you got an error message with such name",
			"\t- keep in mind it's hard to find and handle all possible",
			"\t  cases to repair a tar archive; so I do some simple",
			"\t  assumptions and guessing around ... it's just a simple",
			"\t  tool",
			"",
			0/*}}}*/
	};

	/* print help screen, if
	 * - we have program arguments
	 * - STDIN or STDOUT is a terminal 
	 */
	if(a != 1 || isatty(0) || isatty(1)){
		for(i = 0; help[i]; ++i) {
			puts(help[i]);
		}
		exit(0);
	}

	memset(b, 0, sizeof(b));
	while(EOF != (c = getchar())){
		size = 0;
		memmove(b, b+1, 511);
		b[511] = c;
		/* qick and dirty hdr heuristic test:
		 * - valid hdr should not start with \0 
		 * - 1st pos of magic str should be a 'u' or \0
		 * - last char of hdr is \0
		 */
		if(!(b[0] && (b[257] == 'u' || b[257] == 0) && b[511] == 0)) continue;
		if(!checksum(b)) continue; /* wrong checksum: start again */
		
		/* if no one match, it's not a tar hdr -> continue */
		if(0 != memcmp(b+257, magicString[0], 8) && \
			0 != memcmp(b+257, magicString[1], 8) && \
			0 != memcmp(b+257, magicString[2], 8) ) {
			continue;
		}

		/* some headers are considered for the following file,
		 * but if the record of these file are bad, the headers
		 * effect to the wrong file, e.g. 
		 * we have a very long filename: very_long_filename is
		 * save in L-type header, when the next header is
		 * wrong, then the next file will be renamed to
		 * very_long_filename. solution we save auxilllary data
		 * and check the next record ...
		 */

		/* '0' - '7' and 0x0 for different filetypes 
		 * a - z for posix extensions
		 * A - Z for vendor extensions
		 */
		/* usual filetypes or volume header or global archive
		 * header, they should not influence the next file 
		 */
		if( !(*headerFlag == 0x0 || 
				(*headerFlag >= '0' && *headerFlag <= '7') || 
				*headerFlag == 'V' ||
				*headerFlag == 'g') ){
			Data data;
			unsigned i;

			sscanf(b+124, "%12lo", &data.dsize); /* get size */
			data.dsize += 512; /* the hdr is included */ 
			data.dsize = roundTo512(data.dsize);
			if(!(data.dptr = (char*)malloc(data.dsize))){
				perror("memory");
				exit(1);
			}
			/* 1) read until cur. data
			 * 2) read next hdr
			 * 3) if hdr is valid write out aux data
			 */
			/* assumming that's the aux hdr and
			 * corresponding record has the correct distance
			 */
			memcpy(b, data.dptr, 512);
			size = data.dsize - 512;
			i = 511;
			/* read according full data to spec hdr */
			while(size--){
				if(EOF != (c = getchar())){
					data.dptr[++i] = c;
					continue;
				}
				fprintf(stderr, "ERROR: unexpected end of file (we are in a spec tar hdr\n");
				free(data.dptr);
				goto END;
			}

			/* read next hdr */
			/* ASSUMING: WE HAVE ONLY ONE EXT. HDR */
			memset(b, 0, 512);
			size = 512;
			while(size--){
				if(EOF != (c = getchar())){ /* ext hdr without according data */
					memmove(b, b+1, 511);
					b[511] = c;
				} else {
					free(data.dptr);
					goto END;
				}
			}
			if(!checksum(b)){
				free(data.dptr);
				memset(b, 0, 512);
				size = 0;
				continue;
			}
			size = 0;
			(void)fwrite(data.dptr, data.dsize, 1, stdout);
			free(data.dptr);
		}

		sscanf(b+124, "%12lo", &size); /* get size */
		(void)fwrite(b, 512, 1, stdout); /* output hdr */
		if((fillbytes = size%512)) fillbytes = 512 - fillbytes; /* calc fill bytes */
		while(size--){ /* output file*/
			if(EOF == (c = getchar())){ /* unexpected EOF*/
				char fullname[100+155+1+1]; /* 257 incl \0 */
				
				/* POSIX/ustar format: save long names in the prefix field */
				memset(fullname, 0, sizeof fullname);
				if(b[345]) snprintf(fullname, 156, "%s/", b+345); /* prefix field is used */
				strncat(fullname, b, 100);
				fprintf(stderr, "ERROR: unexpected end of file at \"%s\","
						" fillup with newlines\n", fullname);
				++size;
				while(size--) (void)putchar('\n');/* fill up with newlines */
				break;
			}
			(void)putchar(c);
		}
		
		/* write fill-bytes */
		(void) fwrite(zero, fillbytes, 1, stdout);
		fflush(stdout);
		memset(b, 0, 512);
	}

	END:
	/* make tartools happy: write 2 EOT blocks */
	memset(b, 0, 1024);
	(void)fwrite(b, 1024, 1, stdout);
	fflush(stdout);
	return 0;
}

