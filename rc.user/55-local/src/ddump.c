/* ddump.c - old style hex/octal dump for data files with Ascii notation */
  /* ddump -h for help     -- see help_usage() in this source */
  /* ddump -L for license  -- see license_text[] in this source */
/* SCOPE:  rc.user */

/*
 *   000000000000000000000000000000000000000000000000000000000000000
 *   111111111111111111111111111111111111111111111111111111111111111
 */

static char const copyright_hex[] = __FILE__ " \
Copyright 1993 to 2012, 2013 by Gilbert Healton \n\
   All rights reserved. Use `ddump -L` to view license.";

/* Files are dumped in hex, octal, or decimal, with corresponding ascii
 * characters, in a dense format.
 *
 * Rather than repeating blocks of lines in a file that differ only
 * in the starting address repeating lines are abbreviated. While
 * the threshold of repeating lines defaults to 3 this threshold can
 * be changed by a command line option.
 *
 * Use the -h command line option to get the help text for 
 * this program.
 *
 * Extracts of a dump follow. Quite a few rows have been cut.
 * Note the repeats near the end.
 *
 *--- ddump.c ---
 *0000:	2f2a 2064  6475 6d70  2e63 202d  2d20 6f6c    /\* ddump.c -- ol/
 *0010:	6420 7374  796c 6520  6865 7820  6475 6d70    /d style hex dump/
 *0020:	2066 6f72  2064 6174  6120 6669  6c65 7320    / for data files /
 *  . . . cut cut cut . . .
 *03b0:	2e2e 780a  0a2e 2e2e  2e2c 2e2e  2e2e 312e    /..x~~....,....1./
 *03c0:	2e2e 2e78  0a2e 2e2e  2e2c 2e2e  2e2e 312e    /...x~....,....1./
 *03d0-03ff: 3 repeats
 *0400:	2e2e 2e78  0a0a 2e2e  2e2e 2c2e  2e2e 2e31    /...x~~....,....1/
 *0410:	2e2e 2e2e  780a 2e2e  2e2e 2c2e  2e2e 2e31    /....x~....,....1/
 *0420-045f: 4 repeats
 *0460:	2e2e 2e2e  780a 0a2e  2e2e 2e2c  2e2e 2e2e    /....x~~....,..../
 *0470:	312e 2e2e  2e78 0a2e  2e2e 2e2c  2e2e 2e2e    /1....x~....,..../
 *0480-04cf: 5 repeats
 *04d0:	312e 2e2e  2e78 0a0a  2a2f 0a0a  2364 6566    /1....x~~*\~~#def/
 *04e0:	696e 6520  4845 5844  554d 505f  5645 5253    /ine DDUMP_VERS/
 *04f0:	494f 4e20  2020 2035  2020 2020  2020 2020    /ION    5        /
 *  . . . cut cut cut . . .
 *4f80:	293b 0a7d  7d0a 0a2f  2a20 656e  6420 6464    /);~}}~~\* end dd/
 *4f90:	756d 702e  6320 2a2f  0a2f 2f33  3435 360a    /ump.c *\~//3456~/
 *4fa0:	2f2f 2e2e  2c2e 2e2e  2e31 2e2e  2e2e 780a    ///..,....1....x~/
 *4fb0-500f: 6 repeats
 *
 *
 * Rambling: on
 *   The first version of ddump was written so long ago it hurts to
 *   think about it. The reason was I was annoyed at the native "od"
 *   dump program of UNIX. That first ddump was simple enough to 
 *   quickly rewrite at each new site I wanted to use it it.
 *
 *   Once I rewrote it under my own Linux system I kept the source, 
 *   adding to it over the years in an organic growth.  Adding the 
 *   decimal option changed the code a lot and kind of makes me want 
 *   to apologize for the ugliness herein. The saving grace is that 
 *   ddump works. 
 *
 *   Some time ago I renamed this to hexdump, but that eventually 
 *   clashed with others.
 *
 *   With one exception this program is strongly intended to compile 
 *   on C99 systems so it is not using some nifty GNU extensions. The 
 *   only exception is the use of getopt(3), an origianl UNIX call 
 *   inherited by POSIX. 
 *     * POSIX getopt() is available on a lot of C compilers despite 
 *       it's absence from ISO C standards. 
 *     * You may need to hunt down your compiler's POSIX options 
 *       to compile getopt() calls if you don't want to change this
 *       source (likly worth trying). See -std=gnu99 below.
 *     * If you just can't find getopt() free source is available
 *       from many places, including:
 *        * GNU:     http://svn.savannah.gnu.org/svn/acct/trunk/lib/
 *        * FreeBSD: http://svnweb.freebsd.org/base/head/lib/libc/stdlib/
 *     * If you need the source look for corresponding .h files 
 *       as well.
 *     * CONFORMING TO in most Linux man pages can provide helpful 
 *       background details to those locally compiling getopt().
 *
 *   Under GNU compilers I build with:
 *     gcc -std=gnu99 -Wall -pedantic -o ddump ddump.c
 *       # note: -std=gnu99 expects getopt() in <unistd.h>, which is
 *       #  where a lot of other POSIX/UNIX providers put it.
 *       #  Elsewhere #include <getopt.h> may work. Some older 
 *       #  compilers incorrectly drop getopt() prototypes into 
 *       #  <stdio.h>.
 */


/*   test data
....,....1....x
....,....1....x
....,....1....x

....,....1....x
....,....1....x
....,....1....x
....,....1....x

....,....1....x
....,....1....x
....,....1....x
....,....1....x
....,....1....x

....,....1....x
....,....1....x
....,....1....x
....,....1....x
....,....1....x
....,....1....x

....,....1....x
....,....1....x
....,....1....x
....,....1....x
....,....1....x
....,....1....x
....,....1....x

*/

#define DDUMP_VERSION    6            /* major version.. minor is rev */
static char const ddump_revision[] = "$Rev: 37 $";
static char const ddump_Id[]      = "@(#) $Id: ddump.c 37 2013-06-20 11:01:44Z ghealton $";

static char       version_ddump[12];      /* will build v.rev here */

static char const self_hex[] = 
                "gil" /* @ */  "bert"
                "@"             /* hide address from evil crawlers */
                "heal" 
                "ton"
                "."
                "ne"
		"t";

static char const license_ddump[] = "\
   Redistribution and use in source and binary forms, with or  \n\
   without modification, are permitted provided that the following  \n\
   conditions are met: \n\
 \n\
       * Redistribution of source code must retain the above  \n\
         copyright notice, this list of conditions and the  \n\
         following disclaimer. \n\
       * Redistribution in binary form must reproduce the above  \n\
         copyright notice, this list of conditions and the  \n\
         following disclaimer in the documentation and/or other \n\
         materials provided with the distribution. \n\
       * Neither the name of Gilbert Healton nor the names of  \n\
         other contributors may be used to endorse or promote  \n\
         products derived from this software without specific  \n\
         prior written permission. \n\
        \n\
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND  \n\
   CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES,  \n\
   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF  \n\
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  \n\
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR  \n\
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  \n\
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  \n\
   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR  \n\
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS  \n\
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,  \n\
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  \n\
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  \n\
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY \n\
   OF SUCH DAMAGE. \n\
   \n\
   \n\
 (FYI reference: http://www.opensource.org/licenses/BSD-3-Clause)\n\
";

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>		/* POSIX API (for getopt()) */
	    /* See "Rambling" above if unistd.h does not work for you */

#include <limits.h>
#include <stdint.h>	/* standard integer types from C99
     *	http://pubs.opengroup.org/onlinepubs/009695399/basedefs/stdint.h.html
     *  http://en.wikibooks.org/wiki/C_Programming/C_Reference/stdint.h
     */
#  define	SIZEOF_INT8	(sizeof(int8_t))
#  define	SIZEOF_INT16	(sizeof(int16_t))
#  define	SIZEOF_INT32	(sizeof(int32_t))
#  define	SIZEOF_INT64	(sizeof(int64_t))

typedef int64_t  buf_t;		/* buffer type (worst alignment) */
typedef uint64_t ubuf_t;	/* buffer type (worst alignment) */

static char *name0;             /* file name of program */

static struct options_t
{
   int     opt_ascii;		/* ascii dump control:
   				 *  0: take radix dependent default
				 *  1: print
				 *  -1: no print */
   int     opt_debug;		/* TRUE for debugging output */
   int     opt_dupmin;		/* duplicate count */
   int     opt_endian;		/* Endianness of dump
   		** NOT DOCUMENTED! NOT SUPPORTED! SUBJECT TO CHANGE! **
				 *   0: native for local machine
				 *   1: swap of local machine (one)
				 *   b or B: print big endian
				 *   l or L: print little endian
				 *   n or N: print network byte order endian */
   int     opt_form_feed;       /* TRUE to print \f form feed at end */
   int     opt_heads;           /* TRUE to print headers */
   int     opt_col_mode;        /* column mode: 0 for 16 bytes, 1 for 32. */
   int     opt_radix;		/* radix (8, 10, or 16) */
   int	   opt_text_only;	/* -T raw text only mode dump */
   char   *opt_title;		/* title to print at start of dump */
   int     opt_unsigned;	/* true if unsigned radix 10 values */
   int     opt_wordsize;        /* sizeof integers to dump */
   int     opt_format_char;	/* raw format character */
} options;

/*
 *   Define the various formats we can dump
 */
enum dump_enum { 	    /* what is being dumped */
    dm_head_addrs = 0, 	    /* heading: address row */
    dm_head_stars,	    /* heading: "***" row */
    dm_data,		    /* regular data row */
    dm_addr	    	    /* dump address (last dm_... value) */
     };
typedef struct fmts_struct
{
    int    fm_format_char;	/* getopt option ('\0' ends array) */
    short  fm_radix;		/* radix */
    short  fm_field_count;	/* default field count */
             /* { begin values ordered by dump_enum (change fm_ to dm_) */
    char  *fm_head_addrs; 	/* : heading: address row */
    char  *fm_head_stars;	/* : heading: "***" row */
    char  *fm_data;		/* : regular data row */
    char  *fm_addr;	    	/* : dump address (last fm_... value) */
             /* } end values ordered by dump_enum (see fm_array) */
} fmts_t;

static fmts_t   fmts[] = 
             {
		{
		  'x',			/* -x hexadecimal */
		  16,			/* fm_radix */
		  16,			/* 16 * char */
		  "%-2x",		/* fm_head_addrs */
		  "**",			/* fm_head_stars */
		  "%02x",		/* fm_data */
		  "%04llx:\t"		/* fm_addr: dumping address */
		},
		{
		  'o',			/* -o octal */
		  8,			/* fm_radix */
		  16,			/* 16 * char */
		  "%-3o",		/* fm_head_addrs */
		  " **",		/* fm_head_stars */
		  "%03o",		/* fm_data */
		  "%06llo:\t"		/* fm_addr: dumping address */
		},
		{
		  'b',			/* -b byte int8_t */
		  10,			/* fm_radix */
		  16,			/* 16 * int8_t */
		  "%4d",		/* fm_head_addrs */
		  " ***",		/* fm_head_stars */
		  "%4d",		/* fm_data */
		  "%llu:\t"		/* fm_addr: dumping address */
		},
		{
		  's',			/* -s short int16_t */
		  10,			/* fm_radix */
		  10,			/* 16 * int16_t */
		  "%6d",		/* fm_head_addrs */
		  " *****",		/* fm_head_stars */
		  "%6d",		/* fm_data */
		  "%llu:\t"		/* fm_addr: dumping address */
		},
		{
		  'i',			/* -i integer int32_t */
		  10,			/* fm_radix */
		  5,			/* 5 * int32_t */
		  "%11ld",		/* fm_head_addrs */
		  " **********",	/* fm_head_stars */
		  "%11ld",		/* fm_data */
		  "%llu:\t"		/* fm_addr: dumping address */
		},
		{
		  'l',			/* -l long long int64_t */
		  10,			/* fm_radix */
		  3,			/* 3 * int64_t */
		  "%21lld",		/* fm_head_addrs */
		  " ********************",	/* fm_head_stars */
		  "%21lld",		/* fm_data */
		  "%llu:\t"		/* fm_addr: dumping address */
		},
		{
		  '\0'
		}
             };

   /* to find raw width's, without '-' sign:
    *
      perl -e 'foreach $n ( 8, 16, 32, 64 ) { $x = sprintf("%d", (1<<($n-1))-1); printf( "%d: %s\n", length($x), $x);}' */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * 
 *   help text
 *
 */
static void help_usage(int exit_code)
{{
        /* RESERVED options for future ideas
	"    -sx  Dump uint16_t hex words subject to -e. Also -o and -bsil\n"
        "    -Ux  Unicode/UTF formats (8, 16, 32, BOM)\n"
        "    -wn  Field count per row\n"
         */
	/* HIDDEN and UNSUPPORTED... not happy with current state and I
	 *   expect it to change before publishing.
        "    -ex  Endian byte swapping for -s -i -l\n"
	"          0: native for local machine (zero)\n"
	"          1: swap of local machine (one)\n"
	"          b or B: print big endian\n"
	"          l or L: print little endian\n"
	"          n or N: print network byte order endian\n"
         */
    fprintf( exit_code == EXIT_SUCCESS ? stdout : stderr,
        "%s [-oxbsiluUaA] [-fhHLW] [-d#] file list\n"
        "  Dump files in hex with Ascii interpretation in a compact format\n"
        "  based on mainframe dumps of old.\n"
	"    -o   print in octal\n"
	"    -x   print in hexadecimal (default)\n"
	"\n"
        "    -b   print decimal int8_t dumps (char 8-bit byte: default)\n"
        "    -s   print decimal int16_t dumps (short 16-bit: see stdint.h)\n"
        "    -i   print decimal int32_t dumps (int 32-bit)\n"
        "    -l   print decimal int64_t dumps (long long 64-bit)\n"
        "    -u   unsigned modifier to above decimal\n"
        "    -U   signed modifier to above decimal (cancel -u)\n"
	"\n"
	"    -a   Print Ascii text on the right (default for -o -x)\n"
	"    -A   Do not print Ascii text (default for -b -s -i -l)\n"
        "    -d n shorten duplicate row dumps. n=threshold (3). 0=never\n"
        "    -f   each dump ends with a form-feed\n"
        "    -h   show help text\n"
        "    -H   print column headers\n"
        "    -L   print license information\n"
        "    -W   Wide mode dump of 32 bytes vs default of 16 bytes\n"
	"    -z   Show debugging information, including local Endianness\n"
        "    file list  file names to dump. One may be \"-\" for standard in.\n"
        "  %s\n"
        "  Version %s\n"
        "  %s\n",
         name0, copyright_hex, version_ddump, self_hex );
    exit( exit_code );
}}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *   local, and safe, strerror()
 *     * For the rare non-C99 systems where strerror() returns 
 *       NULL on some unknown errors.
 */
static char *my_strerror( int my_errno )
{
    char       *err_text = strerror(my_errno);

    if ( ! err_text || ! *err_text )
    {   /* fail-safe return of well defined text */
        char static unknown_text[32];
        snprintf( unknown_text, sizeof unknown_text, 
                "Unknown error %d\n", my_errno );
        err_text = unknown_text;
    }

    return err_text;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *   define functions for each type of dump
 *     * can't use C++ inheritance so do it the hard way
 */
#define WORDDUMP_DEF2(ty,n) /* function fetching values of the right size & sign */ \
static buf_t ty##n##word(buf_t *buf_in,int i) \
  {  \
     return (buf_t)*((ty##n##_t *)(buf_in)+i); \
  }

#define WORDDUMP_DEF(n)     /* define unsigned/signed pair of functions */ \
	WORDDUMP_DEF2(uint,n) \
	WORDDUMP_DEF2(int,n)

	/*  instantiate the functions */ \
  WORDDUMP_DEF(8)
  WORDDUMP_DEF(16)
  WORDDUMP_DEF(32)
  WORDDUMP_DEF(64)

#define WORDSET_CASE(n)   /* case statement to set reference to proper function */ \
  case n: o->wordget = options.opt_unsigned ? uint##n##word : int##n##word; break;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *  fake object - kind of - to hold data we pass around a lot
 */
typedef struct object_struct
{
    int            bytes_per_row;   /* bytes in a (hex) row */
    int		   field_count;     /* fields to print per row */
    int		   field_width;     /* printable width, with sign */
    int		   endian_host;	    /* endian of our system:
				     *   -1=little;  +1:big  */
    int            endian_swap;	    /* true to swap local endianness */

    fmts_t	   d_fmts;          /* printf formats to use */
    char	  *spacer_even;	    /* spacers for even bytes */
    char	  *spacer_odd; 	    /* spacers for odd bytes */
    int	           bb_end;	    /* number of words in a line */

    int            widest;	    /* widest text dump seen */
    buf_t      (*wordget)(buf_t *buf, int i); /* pointer to getter */
    int		   text_size;	    /* current textdump() wordsize */
} object_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *  format individual row of data. can include headings
 *	* returns number of characters in address field (to \t tab) that
 *	  we must ignore on duplicate row checks.
 */
#define ROWDUMP_OFFSET        1		/* non-signifcant chars at end of address */
#define HEADER_ASCII_CHAR	'|'	/* char to quote ascii text */

static int rowdump ( 
	    object_t	      *o,		/* this */
	    char              *printbuf,        /* target buffer */
            size_t             printbuf_sizeof, /* sizeof target */
	    unsigned char     *asciibuf,        /* intermediate ascii temp*/
	    buf_t             *buff_in,       	/* data being dumped */
	    int                bb,	        /* number of bytes to dump*/

	    size_t	      *addr_ptr,        /* pointer to offset */
	    enum dump_enum     dump_mode )      /* what is being dumped */
{{
    int		return_value;
    int         printindex = 0;		/* offset into print line */
    int         chars      = 0;		/* characters formatted in line */
    int         i;
    int    	scnt;
    int		spacer_modulo;		/* modulo to find where we space */

    spacer_modulo = 10 == options.opt_radix ? 5 : 2;

    /* first put out appropriate address column */
    switch( dump_mode )
    {
      case dm_head_addrs:
	printindex = snprintf( printbuf, printbuf_sizeof, 
			       "ADDR\t" );
	break;

      case dm_head_stars:
	printindex = snprintf( printbuf, printbuf_sizeof, 
			       "******\t" );
	break;

      case dm_data:
	printindex = snprintf( printbuf, printbuf_sizeof, 
			        o->d_fmts.fm_addr,
				(long long)*addr_ptr/options.opt_wordsize );
	break;

      default:
	fprintf( stderr, "BUG: UNKNOWN dump_mode %d at line %d\n",
			    (int)dump_mode, __LINE__ );
	exit(EXIT_FAILURE);
    }
    return_value = printindex - ROWDUMP_OFFSET;


    /* iterate over the fields in this row */
    char  **fm_array  = &o->d_fmts.fm_head_addrs;
    			/* ugly trick that works in C: 
			 *  will treat sequential (char *) as
			 *  members of an array of (char *). */
    char   *dump_format = fm_array[dump_mode]; /* EZ access pointer */

    int work_count;
    buf_t  work_mask;	/* signed values: ~0 to preserve all bits.
                         * unsigned: mask to keep only significant bits.
			 * why? %x and %o go wacko on negative values */


	/* 0 if little endian systems, else extra padding bytes in buf_t */
    int  bendian = ( o->endian_host > 0 ) ?	/* 0 for little endian */
		   sizeof( buf_t ) -  options.opt_wordsize :
		   0 ;

    work_count = 	/* o->field_count: may adjust down in last row */
    	 ( bb + options.opt_wordsize - 1 ) / options.opt_wordsize;
    work_mask = ~0LL;		/* suppose signed */
    if ( options.opt_unsigned )
    {   /* unsigned */
        work_mask = 	/* set on highest bit w/o ever overflowing */
		 1LL << ( (options.opt_wordsize * CHAR_BIT ) - 1 );
	work_mask |= work_mask - 1;	/* or in all the other bits */
	work_mask |= 1;			/* survive 1's complement machines
					 * (are there any these days?) */
    }

    for ( i = 0; i < work_count; i++ )
    {
	buf_t  bc;		/* current value being dumped */

	bc = (*o->wordget)( buff_in, i ) & work_mask;

	if ( o->endian_swap )
	{   /* byteswap signifcant bytes of logical word size */
	    int     bb;
	    unsigned char   *bcchar = (unsigned char *)&bc;
	    buf_t            bc_new = 0;
	    unsigned char   *bc_new_char = (unsigned char *)&bc_new;

	    for ( bb = 0; bb < options.opt_wordsize; bb++ )
	    {
		int            bx  = options.opt_wordsize - 1 - bb ; 
		unsigned char  bbc = bcchar[bx];   /* get a char */
		bc_new_char[bb]     = bbc;		   /* install */
	    }

	    bc = bc_new;	/* use swapped value */
	}

  
	scnt = snprintf( printbuf + printindex, 
			    printbuf_sizeof - printindex,
				dump_format, bc );
	chars += scnt;
	printindex += scnt;

	/** put out extra spaces between approprate columns **/
	char spacer[4];
	spacer[0] = '\0';
	if ( spacer_modulo == 1 + i % spacer_modulo )
	{   /* odd value (think hex) */
	    strcpy( spacer, o->spacer_odd );
	    if ( spacer_modulo == 1 + (i/2) % spacer_modulo )
	    {   /* double space every other even */
		strcat( spacer, o->spacer_odd );
	    }
	}
	else
	{   /* even value */
	    strcpy( spacer, o->spacer_even );
	}
	if ( spacer[0] )
	{
	    scnt = snprintf( printbuf + printindex, 
				(printbuf_sizeof) - printindex,
			      "%s", spacer );
	    chars += scnt;
	    printindex += scnt;
	}

	/** now move out each byte of the word as Ascii text **/
	if ( options.opt_ascii >= 0 )
	{   /* doing Ascii dump */
	    int   bb;
	    int   ii = i * options.opt_wordsize;
	    char *bbascii = (char *)&bc;

	    for ( bb = 0; bb < options.opt_wordsize; bb++ )
	    {
	        short  bx = bendian + bb;

		unsigned char  bbc = bbascii[bx];   /* get a char */
		asciibuf[ii+bb] = ( isspace( bbc ) && bbc != ' ' )  ?
				      '~'  /* control white space */ :
				      ( isprint( bbc ) ?
					    bbc        :	/* printable */
					    '.' );		/* odd-guy out */
	    }
	}

    }		/* end: for ( i = 0; i < work_count; i++ ) */

    /* pad the last line as wide as needed to keep text aligned */
    /*	(NOTE: can NEVER iterate on first loop when o->widest == 0) */
    while ( o->widest > chars )
    {
	printbuf[printindex++] = ' ';
	chars++;
    }

    /* ASCII data */
    if ( options.opt_ascii >= 0 )
    {
	char    const static hac[] = { HEADER_ASCII_CHAR, '\0' };
	scnt = 0;

	switch( dump_mode )
	{
	 case dm_head_addrs:
	  {
	    scnt = snprintf( printbuf + printindex, 
				printbuf_sizeof - printindex,
			      "  %s%-*.*s%s", 
			      	hac, bb, bb, "ASCII...", hac  );
		    /* format "ASCII..." into heading */

	    break;
	  }
	 case dm_head_stars:
	  {
	    scnt = snprintf( printbuf + printindex,
				printbuf_sizeof - printindex,
			      "  %s%-*.*s%s", hac, bb, bb, "", hac);
		    /* format "........" into heading */

	    /* replace spaces within / /s */
	    char *hp = strstr( printbuf + printindex, hac );
	    while ( *++hp && *hp == ' ' )
		*hp = '-';
	    
	    break;
	  }

	 case dm_data:
	  { 
	    if ( o->widest == 0 )
	    {   /* remember how wide our first line is BEFORE /ascii/ */
		o->widest = chars;
	    }

	    int   ii = i * options.opt_wordsize;
	    asciibuf[ii] = '\0';	/* install \0 terminator */
	    scnt = snprintf( printbuf + printindex, 
				    printbuf_sizeof - printindex,
				  "  |%s|", asciibuf );
	  }

	  default:		/* anything else does not print */
	    break;
	}

	chars += scnt;		/* adjust character details */
	printindex += scnt;

    }

    /* assure final \n termination */
    strncpy( printbuf + printindex, "\n", printbuf_sizeof - printindex );
    if ( dm_data == dump_mode )
    {   /* run up byte count on data dump */
	*addr_ptr += bb;	/* allow for bytes we read */
    }

    /*
     *   All done
     */
    return return_value;
}}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *  text dump data. can include headings
 *	* returns number of characters in address field (to \t tab) that
 *	  we must ignore on duplicate row checks.
 */
static int textdump ( 
	    object_t	      *o,		/* this */
	    char              *printbuf,        /* target buffer */
            size_t             printbuf_sizeof, /* sizeof target */
	    unsigned char     *asciibuf,        /* intermediate ascii temp*/
	    buf_t             *buff_in,       	/* data being dumped */
	    int                bb,	        /* number of bytes to dump*/

	    size_t	      *addr_ptr,        /* pointer to offset */
	    enum dump_enum     dump_mode )      /* what is being dumped */
{{
    int		return_value;
    int		printindex;
    int    	scnt;

    /* first put out appropriate address column */
    switch( dump_mode )
    {
      case dm_head_addrs:
	printindex = snprintf( printbuf, printbuf_sizeof, 
			       "ADDR\t" );
	break;

      case dm_head_stars:
	break;

      case dm_data:
	printindex = snprintf( printbuf, printbuf_sizeof, 
			        o->d_fmts.fm_addr,
				(long long)*addr_ptr/options.opt_wordsize );
	break;

      default:
	fprintf( stderr, "BUG: UNKNOWN dump_mode %d at line %d\n",
			    (int)dump_mode, __LINE__ );
	exit(EXIT_FAILURE);
    }
    return_value = printindex - ROWDUMP_OFFSET;

    /* iterate over the fields in this row */
    char  **fm_array  = &o->d_fmts.fm_head_addrs;
    			/* ugly trick that works in C: 
			 *  will treat sequential (char *) as
			 *  members of an array of (char *). */
    char   *dump_format0 = fm_array[dump_mode]; /* EZ access pointer */
   
    char    dump_format[32];	/* dump_format0 with leading \ */


    int work_count;
    buf_t  work_mask;	/* signed values: ~0 to preserve all bits.
                         * unsigned: mask to keep only significant bits.
			 * why? %x and %o go wacko on negative values */


	/* 0 if little endian systems, else extra padding bytes in buf_t */
    int  bendian = ( o->endian_host > 0 ) ?	/* 0 for little endian */
		   sizeof( buf_t ) -  options.opt_wordsize :
		   0 ;


    /* take regular dump_format and stick \\ escape in front */
    snprintf( dump_format, sizeof dump_format, "\\%s", dump_format0 );

    work_count = 	/* o->field_count: may adjust down in last row */
    	 ( bb + options.opt_wordsize - 1 ) / options.opt_wordsize;
    work_mask = ~0LL;		/* suppose signed */
    if ( options.opt_unsigned )
    {   /* unsigned */
        work_mask = 	/* set on highest bit w/o ever overflowing */
		 1LL << ( (options.opt_wordsize * CHAR_BIT ) - 1 );
	work_mask |= work_mask - 1;	/* or in all the other bits */
	work_mask |= 1;			/* survive 1's complement machines
					 * (are there any these days?) */
    }

    o->text_size = 8;		 /* default to plain 8-bit chars */

    int     i;
    int     chars;
    chars = 0;
    for ( i = 0; i < work_count; i++ )
    {
	buf_t  bc;		/* current value being dumped */

	bc = (*o->wordget)( buff_in, i ) & work_mask;

	if ( o->endian_swap )
	{   /* byteswap signifcant bytes of logical word size */
	    int     bb;
	    unsigned char   *bcchar = (unsigned char *)&bc;
	    buf_t            bc_new = 0;
	    unsigned char   *bc_new_char = (unsigned char *)&bc_new;

	    for ( bb = 0; bb < options.opt_wordsize; bb++ )
	    {
		int            bx  = options.opt_wordsize - 1 - bb ; 
		unsigned char  bbc = bcchar[bx];   /* get a char */
		bc_new_char[bb]     = bbc;		   /* install */
	    }

	    bc = bc_new;	/* use swapped value */
	}

	/*
	 *   beware of wordsizes
	 */
	if ( o->text_size != options.opt_wordsize )
	{   /* wordsize has changed */
	    fprintf( stderr, "UNSUPPORTED AT %d\n", __LINE__ );
	    exit(EXIT_FAILURE);
	      /* need to eat bytes differently: 
	       *  allow for all sorts of different sizes
	       *  and alignments. */
	  /*
	   *  emit \W1 \W2 \W4 to signal wordsize, in bytes.
	   */
	}

	if ( isspace( bc ) || ( isprint( bc ) ) && bc != '\\' )
	{   /* space or printable character */
	    static const char  esc_list[] = "\r" "\n" "\t" "\f" "\v";
	    static const char  esc_code[] = "r"  "n"  "t"  "f"  "v";
	    /* need to beware of word size */
	    


	    char const *esc_char = strchr( esc_list, bc );
	    if ( esc_char )
	    {   /* escape codes */
		scnt = snprintf( printbuf + printindex, 
				    printbuf_sizeof - printindex,
		    "\\%c", esc_code[esc_char-esc_list] );
	    }
	    else
	    {   /* printable character */
		scnt = snprintf( printbuf + printindex, 
				    printbuf_sizeof - printindex,
		    "%c", (int)bc );
	    }
	}
	else
	{   /* non-printable character, or \ escape code */

	    scnt = snprintf( printbuf + printindex, 
			    printbuf_sizeof - printindex,
				dump_format, bc );
	}
	chars      += scnt;
	printindex += scnt;
    }		/* end: for ( i = 0; i < work_count; i++ ) */

    /* pad the last line as wide as needed to keep text aligned */
    /*	(NOTE: can NEVER iterate on first loop when o->widest == 0) */
    while ( o->widest > chars )
    {
	printbuf[printindex++] = ' ';
	chars++;
    }

    /* ASCII data */
    if ( options.opt_ascii >= 0 )
    {
	char    const static hac[] = { HEADER_ASCII_CHAR, '\0' };
	scnt = 0;

	switch( dump_mode )
	{
	 case dm_head_addrs:
	  {
	    scnt = snprintf( printbuf + printindex, 
				printbuf_sizeof - printindex,
			      "  %s%-*.*s%s", 
			      	hac, bb, bb, "ASCII...", hac  );
		    /* format "ASCII..." into heading */

	    break;
	  }
	 case dm_head_stars:
	  {
	    scnt = snprintf( printbuf + printindex,
				printbuf_sizeof - printindex,
			      "  %s%-*.*s%s", hac, bb, bb, "", hac);
		    /* format "........" into heading */

	    /* replace spaces within / /s */
	    char *hp = strstr( printbuf + printindex, hac );
	    while ( *++hp && *hp == ' ' )
		*hp = '-';
	    
	    break;
	  }

	 case dm_data:
	  { 
	    if ( o->widest == 0 )
	    {   /* remember how wide our first line is BEFORE /ascii/ */
		o->widest = chars;
	    }

	    int   ii = i * options.opt_wordsize;
	    asciibuf[ii] = '\0';	/* install \0 terminator */
	    scnt = snprintf( printbuf + printindex, 
				    printbuf_sizeof - printindex,
				  "  |%s|", asciibuf );
	  }

	  default:		/* anything else does not print */
	    break;
	}

	chars += scnt;		/* adjust character details */
	printindex += scnt;

    }

    /* assure final \n termination */
    strncpy( printbuf + printindex, "\n", printbuf_sizeof - printindex );
    if ( dm_data == dump_mode )
    {   /* run up byte count on data dump */
	*addr_ptr += bb;	/* allow for bytes we read */
    }

    /*
     *   All done
     */
    return return_value;
}}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *   print any duplicate lines that have yet to be printed
 *
 */
static void dupdump( 
	object_t *o,
	size_t	 dupaddr,	/* address duplicates started at */
	char    *dupbuf, 	/* buffer duplicates saved in */
	int	 aa,    	/* offset to \t in last formatted address */
	int	 dupcnt		/* number of duplicates */
 		   )
{{
    int        dupprint =  	/* duplicate lines to actually print */
                         ( options.opt_dupmin > 0 &&
                           dupcnt + 1 > options.opt_dupmin ) ?
                          0	 			         :
                          dupcnt                             ;
    size_t     ooaddr = dupaddr;    /* working address */
    int        dd;
    char       debug_buf[128];          /* for holding DEBUG message */

    if ( options.opt_debug )
        snprintf( debug_buf, sizeof debug_buf,
           "DEBUG: dupaddr=%04llx; aa=%d; dupcnt=%d; dupprint=%d",
             (long long)dupaddr,     aa,     dupcnt,  dupprint );

    /*
     *   print the proper number of regular dump rows
     */
    for ( dd = 0; dd < dupprint; dd++ )
    {
        char       addrbuf[20];

        int  aa_addr = 
               snprintf( addrbuf, sizeof addrbuf,
                      o->d_fmts.fm_addr,
			 (long long)ooaddr / options.opt_wordsize );

        int  dupoffset = aa - ( aa_addr - ROWDUMP_OFFSET );
                    /* notice if the width of this lower address offset
                     * is narrower than the address width in aa */

	  /* in case address width changes in middle of repeat 
	   * recalculate offset based on observed values */
	char *aa_ptr = strchr( dupbuf, ':' );
	if ( aa_ptr )
	   dupoffset = aa_ptr - dupbuf + 1 
				- ( aa_addr - ROWDUMP_OFFSET );

        memcpy( dupbuf + dupoffset, addrbuf, aa_addr - 1 );

        if ( options.opt_debug )
        {
            dupbuf[aa-dupoffset-1] = '-';  /* overlay ':' to show */
            if ( dd == 0 )
                sprintf( strchr( debug_buf, '\0' ), 
                           "; offset=%lld", (long long)dupoffset );
        }

        fputs( dupbuf + dupoffset, stdout );

        ooaddr += o->bytes_per_row;

        dupcnt--;
    }

    /*
     *   Print any "repeats" notice
     */
    if ( dupcnt > 0 )
    {
        int bb = sprintf( dupbuf, 
                   o->d_fmts.fm_addr, (long long)ooaddr );
        char  *dptr = strchr( dupbuf, ':' );
        if ( ! dptr )	/* fail-safe test */
            dptr = strchr( dupbuf, '\0' );
        *dptr++ = '-';

        bb = sprintf( dptr,
                   o->d_fmts.fm_addr, 
                   (long long)ooaddr + dupcnt * o->bytes_per_row - 1 );
        sprintf(dptr+bb-1, " %d repeats\n", dupcnt );

        fputs( dupbuf, stdout );
    }

    /*
     *   print debug trace for repeat summary, if appropriate
     */
    if ( options.opt_debug )
        printf( "%s\n", debug_buf );
}}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *    dump a file in hex, octal, or decimal
 */
static void ddump ( char const * const file_name )
{{
    		/* default set up for hex dumps */

    int	              nn;
    FILE	     *StreamIn;


    object_t	      object;		/* common data */
    object_t	     *o;

    memset( &object, 0, sizeof object );
    o = &object;

   {   /* find the proper formats for the dump type -bsil */
    int   f;
    for ( f = 0; f < fmts[f].fm_format_char; f++ )
    {
	if ( options.opt_format_char == fmts[f].fm_format_char )
	{
	    o->d_fmts = *(fmts + f);	/* copy to working value */
	    break;
	}
    }
   }
    if ( ! o->d_fmts.fm_format_char )
    {   /* did NOT find in our list */
	char   str[12];
	str[0] = options.opt_format_char;
	str[1] = '\0';
	if ( !isprint(options.opt_format_char) )
	{
	    snprintf( str, sizeof str, "%03o", options.opt_format_char );
	}

	fprintf( stderr, "BUG: "
		"Unknown options.opt_format_char %s: line %d\n",
			str, __LINE__ );
	exit( EXIT_FAILURE );
    }

    /* variables that stay in scope across dump of file.
     * typically used when we need to modify default values that
     * are defined string constants. Valuse copied here then modified. */
    char  alt_head_addrs[20];	
    char  alt_data[20];

    o->field_count   = o->d_fmts.fm_field_count; /* default field count */
    o->field_width = strlen( o->d_fmts.fm_head_stars );

    o->spacer_even = "";		/* assume hex/octal dumping */
    o->spacer_odd  = " ";

    o->bytes_per_row = 			/* bytes in a row */
    		o->field_count * options.opt_wordsize;
			/* (it all derives from field_count) */

  {   /* puzzle out endianness of local hardware properly unpack ascii */
    union   ee_union
    {
       int16_t  ee_16;			/* binary value */
       char     ee_char[SIZEOF_INT16];	/* corresponding char storage */
    }  ee;

    ee.ee_16 = 1;		/* set binary value subject to endianness */
    o->endian_host = ee.ee_char[0] ? -1 : +1;  /* now look at char's view */
		    /* Big Endian: number ends at largest byte address.
		     * Little Endian: number ends at smallest byte address. */
		    /* Nitpick: other swapping patterns are possible
		     * that can show up in int and long long values.
		     * If you enounter such you will need to roll your 
		     * own byte swapping. */

    if ( options.opt_debug )
        printf( "DEBUG: host endianness: %s\n", o->endian_host < 0 ? "Little" : "Big" );

    if ( options.opt_wordsize >= 2 )
    {   /* byte swapping is approprate */
	switch ( options.opt_endian )
	{
	   case 0:
	   case 1:
	       o->endian_swap = options.opt_endian;	/* relative to native */
	       break;
	   case 'B':
	       o->endian_swap = o->endian_host < 0;
	       break;
	   case 'L':
	       o->endian_swap = o->endian_host > 0;
	       break;
	}
    }

    if ( options.opt_debug && o->endian_swap )
	printf( "DEBUG: byte swapping on wordsize of %d\n",
			    (int)options.opt_wordsize );

  }

    switch ( options.opt_radix )
    {
	case 8: 
	    break;

	case 10: 
	  {  /* decimal dump: word size is important here */

	     o->spacer_even = "";	/* known decimal dumping */
	     o->spacer_odd  = " ";

	     if ( options.opt_unsigned )
	     {  /* unsigned wanted */
	        char	*s;

		/* copy to writable storage that lives as long as program */
		strncpy( alt_head_addrs, o->d_fmts.fm_head_addrs,
					sizeof alt_head_addrs );
		s = strstr( alt_head_addrs, "d" );
		if ( s )		/* (fail-safe if) */
		{
		    *s = 'u';	/* modify our _writable_ copy */
		    o->d_fmts.fm_head_addrs = alt_head_addrs;
		}

		strncpy( alt_data, o->d_fmts.fm_data, sizeof alt_data );
		s = strstr( alt_data, "d" );
		if ( s )
		{
		    *s = 'u';
		    o->d_fmts.fm_data = alt_data;
		}
	     }
	     break;
	  }

	default: 	/* 16 */
	    break;
    }

    if ( options.opt_col_mode )
    {   /* double on -W request */
	o->bytes_per_row *= 2;		
	o->field_count   *= 2;
    }
    if ( options.opt_text_only )
    {   /* double on -T request */
	o->bytes_per_row *= 2;		
	o->field_count   *= 2;
    }

    if ( !options.opt_unsigned && strpbrk( o->d_fmts.fm_data, "oxu" ) )
    {   /* unsigned in truth */
	options.opt_unsigned = 2;	/* truly unsigned */
    }

    /* open the file or select stdin */
    if ( 0 == strcmp( file_name, "-" ) )
        StreamIn = stdin;
    else
        StreamIn = fopen( file_name, "rb" );
    if ( !StreamIn )
    {   /* did not open file */
        (void)fprintf( stderr, "COULD NOT OPEN FILE \"%s\": %s\n",
                file_name, my_strerror(errno) );
        return;
    }

    (void)printf( "\n--- %s --- %s\n",    /* print file name */
                    0 == strcmp( file_name, "-" )  ?
                    "-stdin"                  :
                    file_name,
		    options.opt_title ? options.opt_title : "" );  


  {     /* main dump logic */
    /* size_t            asciibuf_sizeof = o->bytes_per_row+1; */
    size_t            asciibuf_sizeof = o->field_count * options.opt_wordsize + 1;
    size_t            printbuf_sizeof = 20+o->field_width*(2*o->field_count)+(2*asciibuf_sizeof+4);
    size_t	      dupbuf_sizeof   = printbuf_sizeof;

    char              printbuf[printbuf_sizeof];    /* format here */
    int               aa;			    /* bytes dumped */
    unsigned char     asciibuf[asciibuf_sizeof];    /* ascii built here */
    size_t            addr;			    /* dump offset */
    buf_t             buffer[o->field_count+1];	    /* plenty big */
    char              dupbuf[dupbuf_sizeof]; /*duplicate row memory*/

    int               dupcnt;           /* duplicate lines NOT printed */
    size_t	      dupaddr;		/* address dup started at */
    int               dupbuf_aa;	/* aa for dupbuf */

    addr = 0L;		/* starting at address 0 */
    memset( buffer, '\0', sizeof buffer );  /* assure all bytes zeroed */

    memset( dupbuf, '\0', dupbuf_sizeof );
    dupaddr = addr;
    dupcnt = 0;
    dupbuf_aa = 0 ;

    o->widest = 0;		/* have not seen any line */

    if ( o->bytes_per_row > sizeof buffer )
    {   /* assertion failed - show programmer friendly error */
	fprintf( stderr, "BUG: o->bytes_per_row %ld > sizeof buffer %ld "
				"at line %d \n",
			(long)o->bytes_per_row, (long)(sizeof buffer),
			__LINE__ );
	exit(EXIT_FAILURE);
    }
    if ( o->bytes_per_row % options.opt_wordsize )	/* assert proper word size*/
    {   /* assertion failed... write programmer friendly message */
	fprintf( stderr,
	    "BUG: bytes in row %d fails to align with word size %d: "
		    "line %d\n",
		  o->bytes_per_row, options.opt_wordsize, __LINE__ );
	exit(EXIT_FAILURE);
    }

    /* point to proper getter */
    switch( options.opt_wordsize * CHAR_BIT )
    {
	WORDSET_CASE(64)
	WORDSET_CASE(32)
	WORDSET_CASE(16)
	WORDSET_CASE(8)

	default:
	{
	    fprintf( stderr,
		    "BUG: bitsize %d (wordsize %d) NOT 64, 32, ..., 8: "
		       " at line %d\n",
			    options.opt_wordsize * CHAR_BIT, 
			    options.opt_wordsize, 
			    __LINE__ );
	    exit(EXIT_FAILURE);
	}
    }

    while ( memset( buffer, '\0', o->bytes_per_row ),
            ( nn = fread( buffer, 1, o->bytes_per_row, StreamIn ) ) > 0 )
    {
	size_t        addr0 = addr;	/* address BEFORE formatting */
	int           dup_sw;		/* TRUE if duplicate row */

	if ( options.opt_text_only )
	{   /* text-only gets different style of dump */
	    aa = textdump( o, 
	              printbuf, printbuf_sizeof, asciibuf, 
		      buffer, nn, &addr, dm_data );
	}
	else
	{   /* format current data row */
	    aa = rowdump( o, 
	              printbuf, printbuf_sizeof, asciibuf, 
		      buffer, nn, &addr, dm_data );
	}

        /* check if this row duplicates prior (heads already printed) */
	dup_sw = strcmp( dupbuf + dupbuf_aa, printbuf + aa );

	if ( 0 == dup_sw )
	{   /* matches... do not print: never happens in first row */

	    strncpy( dupbuf, printbuf, dupbuf_sizeof );
	    dupbuf_aa = aa;	/* save just in case next row has
				 *  wider address */
	    dupcnt++;
	    continue;
	}

	/* not duplicate... watch for any prior duplicates */
	if ( dupcnt > 0 )
	{
	    dupdump( o, dupaddr, dupbuf, aa, dupcnt );
	    dupcnt = 0;
	}

        dupaddr = addr;		/*  assume next is duplicate */

	/* save non-duplicate row for next dup check */
	strncpy( dupbuf, printbuf, dupbuf_sizeof );
	dupbuf_aa = aa;		/* save just in case next row has
			 	 *  wider address */

	/* format any heading rows */
	if ( addr0 == 0 && options.opt_heads )
	{   /* first row dump */
	    char         headbuf[printbuf_sizeof];  /* formatted hdr */
	    int          hh;
	
	    /* populate with values to format into heading */
	    if ( options.opt_radix == 10  &&  
	    		options.opt_wordsize != SIZEOF_INT8 )
	    {
		switch( options.opt_wordsize )
		{
		  case SIZEOF_INT16:
		  {
		    int16_t   *buffer16 = (int16_t *)buffer;
		    for ( hh = 0; hh < o->field_count; hh++ )
		 	buffer16[hh] = hh;
		    break;
		  }

		  case SIZEOF_INT32:
		  {
		    int32_t   *buffer32 = (int32_t *)buffer;
		    for ( hh = 0; hh < o->field_count; hh++ )
		 	buffer32[hh] = hh;
		    break;
		  }

		  case SIZEOF_INT64:
		  {
		    int64_t   *buffer64 = (int64_t *)buffer;
		    for ( hh = 0; hh < o->field_count; hh++ )
		 	buffer64[hh] = hh;
		    break;
		  }
		}
	    }
	    else
	    {   /* hex, octal, or single-byte decimal */
		char    *buffer_char = (char *)buffer;
		for ( hh = 0; hh < o->field_count; hh++ )
		    buffer_char[hh] = hh;
	    }

	    rowdump( o, headbuf, sizeof headbuf, asciibuf, 
		     buffer, nn, 
		     &addr, dm_head_addrs );
	    fputs( headbuf, stdout );  /* put out built-up string */

	    rowdump( o, headbuf, sizeof headbuf, asciibuf,
		     buffer, nn, 
		     &addr, dm_head_stars );
	    fputs( headbuf, stdout );  /* put out built-up string */
	}

	/* put out buffer */
	fputs( printbuf, stdout );  /* put up built-up string */
    }		/* end: while ( ( nn = fread( buffer, ... */

   {                                    /* limit scope of errno_save */
    int         errno_save = errno;

    /* put out dangling duplicate lines */
    if ( dupcnt > 0 )           /* dump any dangling repeats */
	dupdump( o, dupaddr, dupbuf, aa, dupcnt );

    /* check if clean or trouble finish */
    if ( nn <= 0 && ferror(StreamIn) )
    {
        (void)fprintf( stderr, "DATA ERROR WHILE READING %s: %s\n",
                file_name, my_strerror(errno_save) );
    }
   }
  }


    /* close any input file */
    if ( 0 == strcmp( file_name, "-" ) )
        StreamIn = NULL;
    else
        (void)fclose( StreamIn );

    /* finishing touches */
    if ( options.opt_form_feed )
        (void)printf( "\f" );	/* put out form-feed */

    (void)fflush( stdout );         /* assure flushed in all ways */

    return;

}}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 *   main control ... gather arguments then process files
 */
int main ( int argc, char **argv )
{{
    extern int optind;

    int    c;

    snprintf( version_ddump, sizeof version_ddump,
                "%d.%d", 
                 DDUMP_VERSION,
                    atoi( strpbrk( 1+ddump_revision, "0123456789$" ) ) );

    /* capture our true name */
    if ( argc >= 1 && argv[0] )
    {   /* typical system - '/' (UNIX) or '\' (Windows) is not universal  */
	const char known_terms[] = "/\\]:";	/* known directory terms */
	char *n0 = NULL;
	name0 = NULL;		/* have yet to find normal name */
	/* as there is no ISO-C strrpbrk(), find last match the hard way */
	for ( n0 = argv[0]; 
	      n0 && *n0 && ( n0 = strpbrk( name0 = n0, known_terms ) );
	      n0++		/* candidate for file name */
	    )
	  ;
	if ( ! name0 || ! *name0 )	/* seen any dir chars at all? */
	    name0 = argv[0];		/* no: use just argv[0] */
    }
    if ( !name0 || !*name0 )
    {   /* small system or assorted troubles */
        name0 = "ddump";
    }

    
  { /* capture command line options */
    int    fatals = 0;

    /* initialize options */
    options.opt_dupmin = 3;
    options.opt_radix  = 16;
    options.opt_wordsize = SIZEOF_INT8;
    options.opt_format_char = 'x';

    while ( -1 != ( c = getopt( argc, argv, "aAbd:e:fhiloHLst:TuUWxz" ) ) )
    {
        switch( c )
        {
	   case 'a':
	   {
		options.opt_ascii = 1;
		break;
	   }
	   case 'A':
	   {
		options.opt_ascii = -1;
		break;
	   }

	   case 'b':
	   {
		options.opt_format_char = c;
	   	options.opt_wordsize = SIZEOF_INT8;
		options.opt_radix = 10;
		break;
	   }

	   case 'd':
	    {
		if ( optarg && *optarg )
		{
		    long ii = atol( optarg );
		    options.opt_dupmin = ii;
		}
		break;
	    }

	   case 'e':
	    {
		if ( optarg && *optarg )
		{
		    switch( optarg[0] )
		    {
		       case '0':
		       case '1':
			{   /* 0 is native. 1 is opposite of native. */
			    options.opt_endian = optarg[0] - '0';	
			    break;
			}

		       case 'b':
		       case 'B':
			{   /* big endian */
			    options.opt_endian = 'B';
			    break;
			}

		       case 'l':
		       case 'L':
			{   /* little endian 
			     *  ('L' to avoid visual confusion with '1') */
			    options.opt_endian = 'L';
			    break;
			}

		       case 'n':
		       case 'N':
			{   /* network byte order */
			    options.opt_endian = 'B';	/* network is big */
			    break;
			}

		       default:
		        {
             		  fprintf( stderr, "UNKNOWN ENDIANNESS -e%c\n", optarg[0] );
             		  fatals++;
             		  break;
			}
		    }
		}

		if ( optarg[1] )
		{  /* endianness dump is single character - found multiple */
		   fprintf( stderr, "INVALID ENDIANNESS -e%s\n", optarg );
		   fatals++;
		}

		break;
	    }

           case 'f':
             options.opt_form_feed = !options.opt_form_feed;
             break;

           case 'h':
             help_usage(EXIT_SUCCESS);
             exit(EXIT_SUCCESS);

           case 'H':
             options.opt_heads = !options.opt_heads;
             break;

	   case 'i':
	   {
		options.opt_format_char = c;
	   	options.opt_wordsize = SIZEOF_INT32;
		options.opt_radix = 10;
		break;
	   }

	   case 'l':
	   {
		options.opt_format_char = c;
	   	options.opt_wordsize = SIZEOF_INT64;
		options.opt_radix = 10;
		break;
	   }

           case 'L':
             fputs( copyright_hex, stdout );
             fputs( "\n",          stdout );
             fputs( license_ddump,   stdout );
             printf( "\n%s\n",    ddump_Id );
             exit(EXIT_SUCCESS);

           case 'o':
	      options.opt_format_char = c;
	      options.opt_radix = 8;
	      options.opt_wordsize = SIZEOF_INT8;
	      break;

	   case 's':
	   {
	      options.opt_format_char = c;
	      options.opt_wordsize = SIZEOF_INT16;
	      options.opt_radix = 10;
	      break;
	   }

	   case 't':
	    {
		if ( optarg && *optarg )
		   options.opt_title = optarg;
		break;
	    }

	   case 'T':
	    {
	        options.opt_text_only++;
		options.opt_ascii = 1;
                options.opt_col_mode = 1;
		break;
	    }

           case 'W':
             options.opt_col_mode = !options.opt_col_mode;
             break;

           case 'u':
	      options.opt_unsigned = 1;
	      break;;

           case 'U':
	      options.opt_unsigned = 0;
	      break;

           case 'x':
	      options.opt_format_char = c;
	      options.opt_radix = 16;
	      options.opt_wordsize = SIZEOF_INT8;
	      break;

           case 'z':
	      options.opt_debug++;
	      break;

           case ':':
             fprintf( stderr, "OPTION REQUIRED: -%c\n", optopt );
             fatals++;
             break;

           case '?':
             fprintf( stderr, "UNKNOWN DDUMP OPTION: -%c\n", optopt );
             fatals++;
             break;

           default:
	     	/* getopt() supports option though switch() does not */
             fprintf( stderr, "GETOPT PHASE ERROR: -%c\n", optopt );
             fatals++;
             break;
        }
    }

    if ( optind >= argc )
    {
        fprintf( stderr, "TOO FEW ARGUMENTS\n" );
        fatals++;
    }

    if ( fatals++ )
    {
        help_usage( EXIT_FAILURE );
        exit(EXIT_FAILURE);
    }

    /* decimal inhibits ascii by default */
    if ( 0 == options.opt_ascii /* (at default) */ &&  
         10 == options.opt_radix )
        options.opt_ascii = -1;
  }

  {
    int   ac;

    for ( ac = optind; ac < argc; ac++ )
        ddump( argv[ac] );
  }

    exit( EXIT_SUCCESS );
}}

/* end ddump.c */
/*3456
 *..,....1....x
 *..,....1....x
 *..,....1....x
 *..,....1....x
 *..,....1....x
 *..,....1....x
 *..,....1....x*/
