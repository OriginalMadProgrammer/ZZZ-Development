/* xecho.c -- echo specialized for manipulating x-term banners */ 
    /* bash shell convention:
       export PROMPT_COMMAND='xecho -n "\[\\\$hostname  \\\$USER  \\\$pwd"' */

/*
 *  THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 *  WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  Derived from xt.c dedicated to the Public Domain in 1995
 *   by Gilbert Healton.
 *
 *  Dedicated to the public domain by Gilbert Healton in the hopes this 
 *  program will be useful.  Gilbert Healton. 2004. 2016.
 */

/* Future ideas:
 *  * tests_hex() takes extra arg giving exact byte count.
 *    Negative to run to \0 as currently does.
 *    Would allow tests to use \0 and \u0 tests.
 *  * allow tilde compression of given strings from command line.
 *  * Robust support of CSI - Control Sequence Introducer sequences. 
 *    Come code in place now. 
 *     * ST: 8.3.143    09/12  or  ESC 05/12  [TAB/FF]?
 *       http://www.ecma-international.org/publications/files/ECMA-ST/Ecma-048.pdf 
 */


/*   

 (so I like perl Pod better than troff for simple docs...., despite feature gaps)

 	perldoc xecho.c | less -s

=pod

=head1 NAME

xecho - "X"-window extended echo for easy x-window feature (banner) control 
 
=head1 SYNOPSIS

   xecho [-eEnxz] [-t/dev/tty] message args

=head1 DESCRIPTION

B<xecho> prints meseage args to standard out,
or other devices, 
after handling terminal control sequences,
variables,
and other expansions,
given therein.

Designed to help dynamically set PS1 command prompts of shells and
banners of X windows terminal clients 
B<xecho> has a richer feature set thant traditional methods.

=head1 OPTIONS

The following command-line options modify the behavior of B<xecho>
as follows:

=over 8

=item -c

=item -C

Compress (-c) or do not compress (-C) the home directory portion of various
environment varables, or internal commands, expanded from \${xxx} escapes.
Compression uses tilde "~" using classic Borne Shell conventions.
The default is -c (compress).

Currently HOME and PWD paths are compressed,
providing the substring <B/home/> appears in them.
Rules subject to change.

=item -e

Enable \ escapes (default is to enable).

=item -E

Disable \ escapes.
All message args are treated as plain characters.

=item -n

Inhibit automatic \n at end of message

=item -r

Read response ifrom terminal before exiting then echo to output.
Paring with -t allows reading responses of terminal control sequences that
elect responses from the terminal.
This allows such response to be read out of band from the shell's stdin as
such strings only confuse shells.

Use of C<-r> introduces about a one-second timeout waiting for the
terminal response to complete.


=item -t/dev/tty

Write output to the specified device rather than stdout.
The device C</dev/tty> is typical.
Used to redirect output without
the controlling shell being aware of the redirection,
as is best when using C<-r> to obtain terminal responses.

=item -w I<width>

Sets the maximum width some B<xecho> responses will produce.
Wider responses are truncated with
ellipsis dots showing the point of truncation.

=item -x

Ignored: for compatability with old xt program B<xecho> is based on.

=item -z

Debug: print debugging information at selected times and locations.

=back


=head1 ESCAPE CODES

The following escape codes are suppored by most modern
echo(1) commands,
including B<xecho>.

=over 8

=item \a

alert (bell) code 


=item \b

backspace

=item \c

suppress trailing newline (forces -n)

=item \f

form feed

=item \n

new line

=item \r

carriage return

=item \t

horizontal tab

=item \\

"\" character

=item \nnn

3-digit octal number

=back

The following escape codes are also suppored by B<xecho>.

=over 8

=item \U[+x]I<ddd>

Unicode character. I<ddd> is normally a decimal value of as 
many digits as needed. Place a "x" or "+" after the
\u to use hexadecimal values, of either case.

The number ends at the next non-digit for the radix or
on other "enough" conditions. 
A sure way to end the value is to follow with another
escape code, including the very useful \z.

=item \v

vertical tab

=item \z

No operation.
While also called a "zero width" escape,
\z does does not actually emit any characters. 
\z may be used to safely close various variable length 
escape code fields gathered by B<xecho>,
such as C<\u>.

=item \{ansi control sequence\}

=item \(os control sequence\)

Specifiy terminal control sequences,
without the special magic escape and immediately following 
type character required as B<xecho> will generate those while
doing other useful processing.

A few common uses are given in EXAMPLES.
The full set of control sequences can get quite complex and
is beyond the scope of this man page.

=over +4

=item *

C<{}> contain Ansi control sequences B<xecho> introduces with a "\e[".
Think traditional "glass tty" terminals here.

=item *

C<()> contain OS control sequences B<xecho> introduces with a "\e]".
These tend to be operating-specific sequences
outside of ANSI sequences.
These days some fairly consistent sequences are available for use,
such as setting banners for terminal client windows.

=back

Along with syntax checking escape sequecnes B<xecho> should also
automatically emit any characters required to close 
control sequences regardless of how badly formed I<message args> to
help reduce strange terminal or shell states.
This is really the heart and soul of why B<xecho> was written.

=item \[

=item \]

Emit the the both characters as regular characters.
This is provided to keep B<bash> type shells happy when building 
strings for PS1 environment variables.

Advanced users will note the soft conflict with
Ansi control sequences discussed in the prior C<\{...\}> escape.

=item \${extern}

Expand environment variable, or output of selected commands,
into the output.

The "variable name" may be delimited by {}, [], ' or " quotes to
avoid conflicts with following characters.
If no delimiters are found the name ends at the first non-alphanumeric
character, which may be a C<\z> no-op zero-length string.

It is important the "$" is protected from the shell so $X is not 
expaned by the shell. Single quotes do this.

=back

=over 20

\$PWD \${PWD}

=back

=over 8

Internal definitions:

=over 8 

=item .host 

Name of current host, truncated at first '.' 
(if any). Value obtained from gethostname(3).

=item .cwd

=item .cwd0

=item .pwd0

=item .pwd0

uses getcwd() to obtain the current directory then 
edits it according to ${PWD} rules.

The "0" commands only emit the final directory name of 
the current directory.
Useful to keep PS1 style prompts short while showing the directory
in the prompt when the terminal banner contains the full path.

If the characters in the path are wider than the count given by B<-w\>
then characters are chopped out of the middle to keep the path
no wider than this value.

=back

=back


=back

=head1 EXAMPLES

=head2 References

https://en.wikipedia.org/wiki/ANSI_escape_code#Sequence_elements


=head2 Bash PROMPT_COMMAND external variable:

Bourne shells (bash) supporting PROMPT_COMMAND (e.g., bash)
may use the PROMPT_COMMAND environment variable to rebult the banner
at each command prompt.

   if [ ".${PS1}" != . ]; then
       # interactive shell: rebuild banner at each prompt
       export PS1='$ ';		#fail-safe, should not be used
ZZZZZZZZZZZZZ
       export PROMPT_COMMAND="xecho -n '"'\(0;00\u@$.host:\$.pwd\]\u@.host:\.pwd0$ '"'"
       PROMPT_COMMAND=<<'PC'
       xecho -n '\(0;00\u@$.host:\$.pwd\]\u@.host:\.pwd0$ '
PC
       export PROMPT_COMMAND;
     fi

=head2 Redefining cd function

Bourne shells without PROMPT_COMMAND but supporting "builtin" may 
redefine the "cd" command as follows to set at each "cd" command:

   if [ ".${PS1}" != . ]; then
       # interactive shell: have cd throw up new banner each dir change
       export PS1="....";
       function cd { builtin cd "$@" && 
		    xecho -n "\[2;\$.host  $USER  \$.pwd"; }
     fi

=head2 Configuring C shells:

Not even going to try.... see *Csh Programming Considered Harmful*
by Tom Christiansen: ftp::ftp.perl.com:/pub/perl/versus/csh.whynot.gz
or  http://www.faqs.org/faqs/unix-faq/shell/csh-whynot/
See my pushd.pl program's documentation for longer details about 
csh pain regarding cd.


=head1 ENVIONMENT

HOME, SHELL, USER

=head1 SEE ALSO

echo built into bash, ksh, or other modern Bourne shells.

Linux: console_codes(4)

getcwd(3), getepid(2), geteuid(2), gethostname(2), getpwent(3)

=head1 NOTES

=over 4

=item *

While something like "\\\{${USER}@\\\${hostname}:\\\${pwd}" is more 
traditional, this author finds "\[\\\$hostname  \\\$USER  \\\$pwd" 
easier to read.

=item 4

While setting banners and icon names to the same characters are common,
long names are silly in icons. Thus separate \[long name\]\(short\) 
might be used on systems with icon names you can see.

=cut

 */

#include "stdio.h"

#include "ctype.h"		/* for isalpha and friends */
#include "errno.h"		/* for errno and strerror() */
#include "unistd.h"		/* for getopt && ssize_t */
#include "string.h"		/* for index(), and others */
#include "stdlib.h"		/* for getenv() */
#include "stdarg.h"		/* for vfprintf */
#include "limits.h"		/* for PATH_MAX */

#include "sys/select.h"		/* for select() stuff... */
#include "sys/types.h"		/* for pw stuff... */

#include "pwd.h"		/* .... */
#include "regex.h"		/* POSIX regular expressions */


#define ESCstr "\033"		/* \e escape as string */
#define ESC   ( ESCstr[0] )	/* \e escape as integer */


static char const *name0;	/* name of program */

typedef	 int   int32_ucs;	/* 32-bit integer, signed, for UCS char */

int opt_c = 1;		/* -c to compress home directory to tilde notation */
int opt_e = 1;
int opt_n = 0;
int opt_r = 0;		/* read response from terminal */
int opt_w = 30;		/* width of banner */
char *opt_t = NULL;	/* write to this device. NULL defaults to stdout */
int opt_z = 0;		/* internal debugging */


char const *closure = NULL;		/* closure string required */
			/* moving toward model where multiple closure
			 * characters are accepted. In such cases
			 * closure[0] will hold the default we generate
			 * if approprate. */

    static
    void
die ( char * fmt, ... )
{
    va_list     ap;
    
    va_start( ap, fmt );

    fprintf( stderr, "xecho ABORTED: " );

    vfprintf( stderr, fmt, ap );

    va_end(ap);			/* ensure properly close */

    exit( EXIT_FAILURE );
}


    static
    char  
*malloc_xecho(
	char const *id,		/* string id for this malloc */
	size_t      n	
	 )
{
    int     errno_save;
    short   ecnt;
    char   *value;

    if (  n <= sizeof( int ) )
       die( "malloc %s  FAILED: %ld too small\n", id, (long)n );

    value = NULL;		/* start with nothing */
    for ( ecnt = 0; ecnt < 10 && !value; ecnt++ )
    {   /* best error recovery I've see is to retry a bit */
	value = malloc( n );
	if ( value )
	    break;

	if ( 0 == ecnt )
	    errno_save = errno;	/* save first errno */
        
	sleep(1);		/* give system a bit of time */
    }

    if ( !value )
       die( "malloc %s  FAILED: %s\n", id, strerror( errno_save ) );	

    value[n-1] = '\0';	/* first round of assuring \0 termination */

    return value;
}


/* edit start of string to remove home directory paths */
/*   (right now only the first occrence is compressed) */
    void
tilde_compress(
	char	*value,		/* string to edit */
	size_t   vn )		/* max chars in value */
{

   /*
    *   set up dummy "pw" entry to current entry
    *    (current environment variables win over 
    *     every thing else, as they may resolve
    *     symbolic links in HOME, which the live 
    *     password file may miss)
    */
    struct passwd  pw0;		/* value we populate from environment */
    int            pwctr;	/* how many password entries tested */

    pwctr = 0;			/* assure zero */

  {			/* isolate scope of temporary variables */
    char  *envHOME  = getenv( "HOME" );  /* home, if avail */
    char  *envSHELL = getenv( "SHELL" ); /* shell's name */
    char  *envUSER  = getenv( "USER" );  /* user name */

    memset( (void *)&pw0, '\0', sizeof pw0 );

    if ( envHOME )
        pw0.pw_dir = strdup( envHOME );	

    if ( envSHELL )
        pw0.pw_shell = strdup( envSHELL );	

    if ( envUSER )
        pw0.pw_name = strdup( envUSER );	

    pw0.pw_uid = geteuid();
    pw0.pw_gid = getegid();
  }
   /*
    *  search password file, but use our psuedo-entry first
    */
  {
    struct passwd  *pw_master;	/* current password entry */
    struct passwd   pw;		/* copy we are free to change */

    for (  pw_master = &pw0; pw_master; pw_master = getpwent(), pwctr++ )
    {
	size_t dir_len;		/* length of pw_dir */
	size_t name_len;	/* length of pw_name */
	char   *d;		/* working ptr into pw_dir */
        char   *v;		/* working pointer within value */

	pw = *pw_master;	/* copy system's struct to scratch copy */

	  /* (allow for corrupted entries, especially pw0) */
	if ( ! pw.pw_name || ! pw.pw_name[0]  
	 ||  ! pw.pw_dir  || ! pw.pw_dir[0] )
	    continue;

	   /* wipe any trailing slashes in directory name  */
	while( !!( d = strchr( pw.pw_dir, '\0' ) ) 
		    && d > pw.pw_dir && d[-1] == '/' )
	{   /* ensure no trailing slashes */
	    d[-1] = '\0';	
	}
	dir_len  = strlen( pw.pw_dir );
	name_len = strlen( pw.pw_name );

	if ( dir_len > name_len + 2 &&
	     ! strncmp( value, pw.pw_dir, dir_len ) &&
	     ( value[dir_len] == '/' || value[dir_len] == '\0' ) )
	{   /* found directory longer than user name: SAFE compressing path to */
	    v = value + dir_len; 	/* now at / */
	    	/*
		 *  use cases:
		 *    [1] /home/gilbert		*v == '\0'	=> ~
		 *    [2] /home/gilbert/more	*v == '/'	=> ~/more
		 *    [3] /home/another		*v == '\0'	=> ~another
		 *    [4] /home/another/more	*v == '/'	=> ~another/more
		 */
	    if ( geteuid() != pw.pw_uid )
	    {   /* not OURSELF: need user name */
		int    xx;
		v -= name_len;		/* back up legnth of user name */
		for ( xx = 0; xx < name_len; xx++ )
		{   /* overlay name */
		    v[xx] = pw.pw_name[xx];
		}
	    }
	    v--;		/* back up one char B4 '/' */
	    *v = '~';	/* put tilde char in place */

	    if ( v > value )
	    {   /* need to move value down SAFELY */
		bcopy( v, value, strlen( v ) + 1 );
	    }

	    break;	/* stop looking in password file */
	}

    }		/* end:  for (  pw_master = &pw0; pw_master; ... ) */

    if ( pwctr > 0 )		/* check if our internal pw0 or not... */
        endpwent();   		/*   no: close getpwent() best we can */
  }

}



#define UTF8_MAXBYTES 6		/* maximum bytes in UTF-8 encoding */
#define UTF8_BITS     6		/* maximum payload bits for each UTF8 
	*			   digit - log2( 0xfd - 0xf0 )  */
#define UTF8_MOD      (1 << UTF8_BITS)		/* modulo value */
#define UTF8_BITMASK  ((1 << UTF8_BITS) - 1)	/* bitmask for lower 6 bits */
#define UCS_MASK     0x7FFFFFFF	/* largest acceptable UCS value */

/* convert UCS encodings >= 128 into string */
    static
    char
*ucs2utf8( 
	char 		*utf8, 		/* target string */
	size_t 		 sizeof_utf8, 	/* size of target string, including \0 */
	int32_ucs 	 c )		/* ucs encoding: NEVER negative */
{
  /*
   *    0 0x00000000 - 0x0000007F:
   *        00 3F   0xxxxxxx
   *    
   *    1 0x00000080 - 0x000007FF:
   *        C0 1F   110xxxxx 10xxxxxx
   *           umax  4        5
   *    
   *    2 0x00000800 - 0x0000FFFF:
   *        E0 0F   1110xxxx 10xxxxxx 10xxxxxx
   *                 3        4        5
   *    
   *    3 0x00010000 - 0x001FFFFF:
   *        F0 07   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
   *                 2        3        4        5
   *    
   *    4 0x00200000 - 0x03FFFFFF:
   *        F8 03   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
   *                 1        2        3        4        5
   *
   *    5 0x04000000 - 0x7FFFFFFF:
   *        FC 01   1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
   *         0        1        2        3        4        5
   *
   *	 Surrogates: 0xD800 to 0xDBFF and have no regular code 
   *		       D800-DB74  "High surrogates"
   *		       DB80-DBFF  "High private use surrogates"
   *
   *    http://stackoverflow.com/questions/4607413/c-library-to-convert-unicode-code-points-to-utf8*    if (c<0x80) *b++=c;
   *    else if (c<0x800) *b++=0xC0+c/0x40, *b++=0x80+c%0x40;
   * 	else if (c-0xD800U<0x800) goto error;
   *	else if (c<0x10000) *b++=0xE0+c/0x100, *b++=0x80+c/0x40%0x40, *b++=0x80+c%0x40;
   *	else if (c<0x110000) *b++=0xF0+c/0x40000, *b++=0x80+c/0x1000%0x40, *b++=0x80+c/0x40%0x40, *b++=0x80+c%0x40;
   *	else goto error;
   */

    int    u;
    int    umask;		/* value to add to EVERY utf8-char */
    int    umax;		/* invert of value builds umask of first byte */

    memset( utf8, 0, sizeof_utf8 );	/* for debugging - might go away*/
    u = sizeof_utf8 - 1; 	/* we fill utf8[] from right to left */
    umask = 0x80;
    umax  = 0xFF;		/* start 4 times larger than needed for 2-byte UTF */
    					/* 2* as bias for first iterations reduction */
    					/* 2* to get exta bit of '0' we need in inverse */
    utf8[u] = '\0';		/* assure \0 termination */
    while ( c && u >= 1 )
    {	  /* handles all but the most-signficant byte */
	utf8[--u] = ( c & UTF8_BITMASK ) | umask;  /* keep only six bits */
	c >>= UTF8_BITS;	/* reduce */
	umax >>= 1;		/* maximum value now 1/2 of before */
    }
    umask = ~umax & 0xFF;
    utf8[u] |= umask;

    return utf8 + u;
}


static int spacex = '\0';		/* becomes ' ' after fisrt arg output */

    static
    int
arg ( 
	FILE       *stdterm,	/* stream to write to */
	int         tilde_sw,	/* true to compres home paths using ~ */
	char const *echo	/* character string to echo */
    )
{
    int32_ucs  c;		/* SIGNED 32-bit char. negative to emit \0 */

    if ( spacex ) putc( spacex, stdterm );
    spacex = ' ';			/* any CALL adds a auto-space */

    while( !!( c = *echo ) )
    {
	echo++;				/* point to next char */
	if ( '\\' == c  &&  opt_e )
	{   /* escape code */

	    c = *echo;
	    if ( c ) echo++;


	    switch( c )
	    {
	      case 'a':		/*     alert (bell) */
		c = '\a';
		break;

	      case 'b':		/*     backspace */
		c = '\b';
		break;

	      case 'c':		/*     suppress trailing newline */
		opt_n++;
		continue;

	      case 'e':		/*     an escape character */
		if ( closure )
		{
		    fputc( closure[0], stdterm );
		    closure = NULL;
		}
		c = ESC;
		break;

	      case 'f':		/*     form feed */
		c = '\f';
		break;

	      case 'n':		/*     new line */
		if ( closure ) 
		{
		    fputc( closure[0], stdterm );
		    closure = NULL;
		}
		c = '\n';
		break;

	      case 'r':		/*     carriage return */
		if ( closure ) 
		{
		    fputc( closure[0], stdterm );
		    closure = NULL;
		}
		c = '\r';
		break;

	      case 't':		/*     horizontal tab */
		c = '\t';
		break;

	      case 'v':		/*     vertical tab */
		if ( closure ) 
		{
		    fputc( closure[0], stdterm );
		    closure = NULL;
		}
		c = '\v';
		break;

	      case '\\':	/*     backslash */
		c = '\\';
		break;

	      case '0':
	      case '1':
	      case '2':
	      case '3':
	      case '4':
	      case '5':
	      case '6':
	      case '7':
	       {
		 int    n;
		 c = 0;
		 for ( n = 0; n < 3; n++ )
		 {
		    int   t;
		    t = *echo;
		    if ( ! t  || !isdigit(t) ) break;
		    echo++;
		    c = c * 8 + ( t - '0' );
		 }

		 c &= 0xFF;		/* insist on 8-bits of chars */
		 if ( 0 == c )
		     c = -1;		/* force \0 to be emitted */
		 break;
	       }

	      case 'u':			/* unicode - emit utf-8 */
	       {
		 int    n;		/* current digit position */
		 int    n_max;		/* max digits to accept in value */
		 int    radix;		/* 10 for decimal 16 for hex */

   		{  /* initialize and gather number */
		 radix = 10;
		 n_max = 10;		/* 10-digit value */
	    	 c = tolower(*echo);	/* peek at next character */
		 while ( '+' == c || 'x' == c )
		 {
 		    radix = 16;
		    n_max = 8;		/* 8-digit value */
		    c = *++echo;
		 }

		 c = 0;
		 for ( n = 0; n < n_max; n++ )
		 {  /* carefully consume number ourself for total control */
		    int        t;	/* temporary working char */
    		    int32_ucs  c_next;	

		    t = tolower(*echo);
		    if ( ! t ) break;
		    if ( isdigit(t) )
		       t = t - '0';
		    else if ( radix == 16 && isxdigit(t) )
		       t = t - 'a' + 10;
		    else
		       break;
		    echo++;

		    c_next = c * radix + t;
		    if ( c_next < c )
		        break;		 /* overflow! */
		    c = c_next;
		 }
		}

   		{  /* process number */
		 c &= UCS_MASK;		/* insist in proper range */
		 if ( 0 == c )
		     c = -1;		/* force \0 to be emitted */
		 else if ( c >= 128 )
		 {   /* emit string ourself */
		     char   utf8[UTF8_MAXBYTES+1];   /* where to build utf-8 string */
		     fputs( ucs2utf8( utf8, sizeof utf8, c ), stdterm ); /* write byte-order */
		     continue;
		 }
   		}
		  
		 break;
	       }

	      case 'z':			/* zero-width "no-op" character */
		continue;		/* discard */

	      case '$':
	       {			/* print (possibily edited) env vair */

		  int     ender;	/* becomes ending char, if true */
		  char   *value;	/* value saved locally in ourself */
		  size_t  vn;		/* size of malloc in value */

		  ender = 0;
		  value = NULL;

	        {	/* determine how variable name ends */
		  char      *x;
		  char const starts[] = "{[('\"";
		  char const ends[]   = "}])'\"";

		  if ( ! *echo )	
		     break;		/* print the $ */

		  c = *echo;		/* peek at character after '$' */
		  if ( ( x = index( starts, c ) ) != NULL )
		  {   /* quoted string: remember ending delimiter */
		      ender = ends[ x - starts ];
		      c = *++echo;	/* get following character */
		  }

	        }

	        {	/* gather the "variables" name */
			/*  *echo  next character */

		  char    name[256];	/* name of environment variable with \0 */
		  int     n;		/* offset into name */
		  int     dot;

		  char   *v;		/* working pointer within value */
		  c = *echo;		/* get first char in name */
		  dot = '.';		/* first char may be . */
		  echo++;
		  for ( n = 0; n < sizeof(name)-1; )
		  {
		     if ( ender 
			  ? ( c == ender )
			  : ( !isalnum(c) &&  c != '_' && c != dot ) )
		     {  /* ending character */
			if ( c == ender )
			{   /* explicit termination */
			    c = '\0';		/* discard */
			}
			else
			{   /* not quoted: pop back to the 'c' ending value */
			    echo--;
			}
			break;
		     }
		     dot = '\0';	/* dot nolonger allowd */

		     if ( c )
		         name[n++] = c;	/* save this char */

		     c = *echo;		/* get next character */
		     if ( ! c ) 
		         break;
		     echo++;
		  }
		  name[n] = '\0';	/* terminate */

		  if ( ! strcmp( name, ".cwd" ) || !strcmp( name, ".pwd" ) ||
		       ! strcmp( name, ".cwd0" ) || !strcmp( name, ".pwd0" ) )
		  {   /* ".cwd" is current working directory */
		      value = malloc_xecho( "\\$cwd", vn = PATH_MAX + 10 );
		      if ( ! getcwd( value, vn-1 ) )
		      {   /* getcwd() failure: somehow */
			  die( "getcwd \\$cwd FAILED: %s\n", strerror( errno ) );	
		      }
		      if ( strchr( name, '0' ) )
		      {  /* "0" calls just report last directory */
			 char *vp;
			 vp = strrchr( value, '/' ); /* have multiple dirs? */
			 if ( vp && vp[1] )
			     memmove( value, vp+1, strlen(vp) + 1 ); /*just last*/
		      }
		        

		  }
		  else if ( !strcmp( name, ".host" ) )
		  {   /* "hostname" variable is actually function call */
		      size_t   hmax = 500;
		      value = malloc_xecho( "\\$hostname", vn = hmax ); 
		      value[0] = '\0';
		      gethostname( value, hmax );
		  }
		  else
		  {   /* everything else is "real" variable */
		      v = getenv( name );
		      if ( opt_z >= 2 )
			  fprintf( stderr, "  $%s=%s\n",
				    name, v ? v : "<undefined>" );

		      if ( v )
		      {   /* value defined: watch for editing */
			  value = (char *)malloc_xecho( "\\$env", vn = strlen( v ) + 20 );
			  strncpy( value, v, vn - 1 );
		      }
		  }

		  if ( value && *value )
		  {
		      if ( strstr( value, "/home/" ) 
		       /*  this was the older way.. but I belive the simpler will work
		         ( ! strcasecmp( name, "PWD" ) 
		        || ! strcasecmp( name, "HOME" ) 
		        || ! strcasecmp( name, ".cwd" ) 
		        || ! strcasecmp( name, ".pwd" ) 
		        || ! strcasecmp( name, "XECHO_TEST" ) */
		      )
		      {  /* current directory: look for home directories */
			 if ( tilde_sw && 		  /* ~ compression? */
			      strstr( value, "/home/" ) ) /* likely home? */
			     tilde_compress( value, vn );  /* yes: try compressing */
		      }
		      else if ( ! strcasecmp( name+n, "HOSTNAME" ) )
		      {  /* truncate host name at first '.' (if any) */
			v = strchr( value, '.' );
			if ( v )
			   *v = '\0';
		      }
		      if ( strlen( value ) > opt_w )
		      {   /* too wide.... chop out of the middle */
			  int  head, tail, len, o;
			  static char const splitter[] = "...";
			  char  *dest;
			  o = (sizeof splitter) / 3;
			  head = opt_w / 3 - o;	/* leading gets 1/3 of splitter */
			  tail = opt_w - head;	/* trailing get 2/3 - splitter */
			  len = strlen( value ); /* width of "too much" */
			  strcpy(  value+head, splitter ); /* truncate with ... */
			  dest = strchr( value+head, '\0' );
			  memmove( dest, 
				   value + len - tail - sizeof(splitter) - 1,
				   value + len - dest - 1 );
			  value[vn-1] = '\0'; /* fail-safe */
		      }
		  }
		}

		{	/* echo the value */
		  if ( value )
		  {
		      if ( vn )			/* assure very last char is \0 */
		         value[vn-1] = '\0';	/* NULL (no fputs buffer overflow!) */

		      fputs( value, stdterm );	/* print string, not value */
		      free( (void *)value );
		      value = NULL;
		  }

		}
		  continue;		/* next while( c = *echo ) */
	       }

		/*   https://en.wikipedia.org/wiki/ANSI_escape_code#cite_note-ecma48-13 */
		     /*
		      *   \e number [A-GJKST]]
		      *   \e y;x [Hf]
		      *   \e number;number;... \a
		      *   \e [456] [in]
		      *   \e [su]
		      *   \e ?25[lh]
		      */
	      case '{':		/* CSI - ANSI Control Sequence Introducer */
	      case '(':		/* CSI - OS   Control Sequence Introducer */
	       {
		 static const char xclose[] = "\a\t"; /* accepted closures */
		 typedef struct xecho_regx_struct 
		 {
		   const char  *closure;  /* chars to close CSI. NULL if none. */
		   const char  *str;      /* regular expression */
		   union
		   {			/* for overloading xecho_regx[0] */
		      int          compiled;	/* only valid in [0] */
		      regex_t      reg;		/* compiled regex in str */
		   } u;
		 } xecho_regx_t;
		 static xecho_regx_t *csi;	/* regular expression array */
		 static xecho_regx_t  csi_ansi[] = {
		 			{ "{",   "[" },   /* START code (NOT a regex) */
					{ NULL,   "^[0-9]*[A-GJKST]" },
					{ NULL,   "^[0-9]*(;([0-9])?)?[Hf]" },
					{ NULL,   "^[456]*[in]" },
					{ NULL,   "^[0-9,]*m" },    /* Set Graphic Rend */
					{ NULL,   "^[su]" },
					{ NULL,   "^\\?[0-9]*[lh]" },
					{ NULL,   "" } };
		 static xecho_regx_t  csi_osc[] = {	
		 			{ "(",   "]" },   		/* START code */
					{ xclose, "^2;." },		/* banner */
					{ xclose, "^0;." },		/* banner */
					{ "\a",   "^\\?[,0-9]*[lh]" },	/* for/back color */
					{ "\t",	  "^P" },		/* Linux change palette */
					{ "",     "" } };

		int             n;

		char     const *csi_emit;	/* CSI type code to emit */
		int      compile_sw;

		/* for this, ansi.closure identifies start character */
		if (  csi_ansi[0].closure[0] == c ) 
		    csi = csi_ansi; 	/* ANSI CSI codes */
		else
		    csi = csi_osc;      /* OS device codes outside of ANSI */
		csi_emit = csi[0].str;	  /* save emission string */
		compile_sw = csi[0].u.compiled; /* check if has been compiled */

		if ( ! compile_sw )
		{   /* first time through... compile things */
		    compile_sw = 1;
		    csi[0].u.compiled = 1;	/* set true value */

		    for ( n = 1; csi[n].str[0]; n++ )
		    {
			int err = regcomp( &csi[n].u.reg, csi[n].str, REG_EXTENDED );
			if ( err )
			{
			    char  errbuf[256];
			    compile_sw = -1;
			    regerror( err, &csi[n].u.reg, errbuf, sizeof errbuf );
			    fprintf( stderr, "REGEX ERROR: /%s/: %s\n",
			    			            csi[n].str, errbuf );
			}
		    }
		    if ( compile_sw < 0 )		/* compile failure ? */
			exit(EXIT_FAILURE);
		}
		if ( closure )
		{
		    fputc( closure[0], stdterm );
		    closure = NULL;
		}
		int   p;		/* becomes 0 on regex match */
		p = 1;			/* assume no match */
		for ( n = 1; csi[n].str[0]; n++ )
		{
		    const size_t   nmatch = 1;	/* maximum matches we allow */
		    regmatch_t pmatch[nmatch];	/* match buffer */
		    /*  int regexec(const regex_t *preg, const char *string, 
		    		size_t nmatch, regmatch_t pmatch[], int eflags); */
		    p = regexec( &csi[n].u.reg, echo, nmatch, pmatch, 0 );
		    if ( 0 == p )
		    {
			/* start escape  */
			if ( closure ) fputc( closure[0], stdterm );
			fprintf( stdterm, ESCstr "%s%.*s", 
				                  csi_emit, 
						      pmatch[0].rm_eo, 
						       echo );
			
			echo += pmatch[0].rm_eo;
			closure = csi[n].closure;
			break;
		    }
		}
		if ( p )
		{
		    fprintf( stderr, "%s: NO MATCH: %.10s\n", 
				      name0,  echo - 1 );
		    break;
		}
		else
		{
		    continue;		/* all emissions complete */
		}
	       }

	      case ')':
	      case '}':
	       {
		if ( closure )
		{
		    fputc( closure[0], stdterm );
		    closure = NULL;
		}
		fflush( stdterm );		/* assure stdterm flushed */
		continue;
	       }

	      case '[':
	      case ']':
		printf( "%c%c", echo[-1], c );	/* preserve full bash sequence */
		break;

	      default:			/* silently keep escaped char */
		 break;
	    }
	}

	if ( c )
	{
	    if ( closure )
	        if ( strchr( closure, c ) )
		    closure = NULL;		/* explicit closure */

	    putc( c, stdterm );
	}
    }

    return 0;
}

#define  TESTS_HEX_INC  3	/* bytes used by hex dump for each byte (2 or 3) */

/* format char string as string of hex digits */
    static
    void
tests_hex( char *target, size_t tmax, char const *s )
{
    static const char  tohex[] = "0123456789ABCDEF";
    int	               t;

    for ( t = 0; t + TESTS_HEX_INC - 1 < tmax && *s; t += TESTS_HEX_INC )
    {
        target[t]   = tohex[ ( *s >> 4 ) & 0xF ];
        target[t+1] = tohex[ ( *s++    ) & 0xF ];
        target[t+2] = ' ';
    }
    if ( TESTS_HEX_INC > 2  &&  t >= TESTS_HEX_INC )
       target[t-1] = '\0';	/* wipe padding space */
    target[t] = '\0';
}

typedef struct tests_struct
{
   char   const  *master;
   char   const  *expect;
} tests_t;

    static
    int
tests_run( char *str )
{
     FILE       *stdmem;   		/* for memopen() */
     char	 buffer_stdmem[2048+1];	/* buffer holding string "printed" by arg() */
     char	 buffer_hexmem[TESTS_HEX_INC*sizeof(buffer_stdmem)]; /* hex formatted buffer  */
     size_t      sizeof_buffer;

     int	 return_code;

     return_code = 0;
     sizeof_buffer = sizeof(buffer_stdmem);		/* size of base buffer */
     memset( buffer_stdmem, '\0', sizeof_buffer );

     stdmem =  fmemopen( buffer_stdmem, sizeof_buffer, "ab" );
     if ( !stdmem )
         die( "-Z fmemopen() failure: %s\n", strerror(errno) );

     if ( str && *str )
     {  /* simple test */
	arg( stdmem, 0, str );
	fflush( stdmem );
	tests_hex( buffer_hexmem, TESTS_HEX_INC*sizeof_buffer, buffer_stdmem );
	printf( "%s\n", buffer_hexmem );
     }
     else
     {  /* standard test suite */
	char	env_buffer[2048];	/* for testing \$ */

	tests_t  tests[] =
	   {
		{ "abc\\zdefg",		"61 62 63 64 65 66 67" },
	     /*	{ "abcOOPS",		"61 62 63 64 65 66 67" }, */
		{ "newline\\n",		"6E 65 77 6C 69 6E 65 0A" },
		{ "\\a\\b\\e\\f\\n\\r\\t\\v\\001\\0377\\\\",
					"07 08 1B 0C 0A 0D 09 0B 01 FF 5C" },


		{ "\\$PWD",		env_buffer },

		{ "a\\u10\\u+10",		"61 0A 10" },
		{ "1\\u+80\\u+7ff",		"31 C2 80 DF BF" },
		{ "2\\u+800\\u+ffff",		"32 E0 80 EF BF BF" },
	    	{ "3\\u+10000\\u+1FFFFF",	"33 F0 80 80 F7 BF BF BF" },
	    	{ "4\\u+200000\\u+3FFFFFF",	"34 F8 80 80 80 FB BF BF BF BF" },
		{ "5\\u+4000000\\u+7FFFFFFF",	"35 FC 80 80 80 80 FD BF BF BF BF BF" },

		{ "\\[1;ABCD\\]012",	"1B 5D 31 3B 41 42 43 44 07 30 31 32" },
		{ NULL,			NULL }
	   };

	int      t;
	int      t_failed = 0;
     	for ( t = 0; tests[t].master; t++ )
	{
	    char  const	*buffer_expect = tests[t].expect;	/* formatted as we expect */
	    int		 bytes;			/* bytes actually written */

	    printf( "Test %d: %s\n", t, tests[t].master );
	    fflush( stdmem );			/* assure any dangling chars written */
	    fseek( stdmem, 0, 0 );		/* assure at start */

	    if ( '$' == tests[t].master[1] )
	    {   /* populate env_buffer with appropriate value */
	        char  const *env = tests[t].master + 2;
		char         work_buffer[sizeof env_buffer];

		memset( work_buffer, '\0', sizeof work_buffer );
		if ( ! strcmp( env, ".cwd" ) )
		{
		    getcwd( work_buffer, (sizeof work_buffer) - 1 );
	    	}
		else if ( ! strcmp( env, ".hostname" ) )
		{
		    gethostname( work_buffer, (sizeof work_buffer) - 1 );
	    	}
		else
		{
		    char  *e = getenv( env );
		    if ( e )
		        strncpy( work_buffer, e, (sizeof work_buffer) - 1 );
		    else
		    	die( "FATAL: no environment variable %s\n", env );
		}

		tests_hex( env_buffer, sizeof env_buffer, work_buffer ); /* hexafy */
	    }

	    spacex = '\0';			/* disable arg()'s normal auto spacing */
	    arg( stdmem, 0, tests[t].master );
	    bytes = ftell( stdmem );		/* bytes actually written */
	    fflush( stdmem );			/* assures \0 written at end */
	    buffer_stdmem[bytes] = '\0';	/* assure null termination */

	    tests_hex( buffer_hexmem, sizeof buffer_hexmem, buffer_stdmem );
	    if ( strcmp( buffer_expect, buffer_hexmem ) )
	    {   /* trouble */
		int    b;
		int    diffs;
		t_failed++;
	    	printf( "  FAILED\n" );
		printf( "    expect/%s/\n", buffer_expect );
		printf( "     found/%s/\n", buffer_hexmem );
		printf( "     diffs/" );
		diffs = 0;
#define			DIFF_MAX  11
     		return_code--;
		for ( b = 0; tests[t].expect[b]; b += TESTS_HEX_INC )
		{   /* ugly, but good enough, dump of trouble bytes */
		    int  b1 = b / TESTS_HEX_INC;  /* simple offset 0, 1, 2 */

		    if ( diffs >= DIFF_MAX )
		    {   /* don't get carried away on major troubles */
			if ( diffs == DIFF_MAX )
			    printf( "..." );
		    }
		    else
		    {   /* report difference */
			char    padding[TESTS_HEX_INC+1];
			char    cc;
			memset( padding, ' ', sizeof padding );
			if ( strncmp( buffer_expect + b, buffer_hexmem + b, 2 ) )
			{   /* show only if different */
			    diffs++;
			    cc = buffer_stdmem[b1];	/* output character */
			    if ( ! isgraph(cc) )
				cc = '.';		/* make hidden or control visible */
			    padding[0] = cc;
			}
			padding[TESTS_HEX_INC-1] = '\0';
			printf( "%1d%s", 
				  b1 % 10,
				    padding
				      );
		    }
		}
		printf( "/\n" );
		printf( "  %d bytes different observed\n", diffs );
	    }
	    else
	    {
	    	printf( "  ok\n" );
	    }
	}

	printf(   "\ntests run:    %3d\n", t );
	if ( t_failed )
	    printf( "tests FAILED: %3d\n", t_failed );
	else
	    printf( "no failures observed\n" );

     }

     fclose( stdmem );

     return return_code;
}

    int
main ( int argc, char **argv )
{
    int   fatal = 0;
    FILE *stdterm = NULL; /* default writing to stdout */

    name0 = strrchr( argv[0], '/' );
    if ( name0 )
       name0++;
    else
       name0 = argv[0];

  {
    int   c;

    while( ( c = getopt( argc, argv, "cCeEnrtw::xzZ::" ) ) != -1 )
    {
      switch( c )
      {

	case 'c':
	  opt_c++;
	  break;
	case 'C':
	  opt_c = 0;
	  break;

	case 'e':
	  opt_e++;
	  break;

	case 'E':
	  opt_e = 0;
	  break;

	case 'n':
	  opt_n++;
	  break;

	case 'r':
	  opt_r++;
	  break;

	case 't':
	  opt_t = optarg;
	  break;

        case 'w':
	 {
	   int  width;
	   width = atoi( optarg );
	   if ( width >= 21 )	/* assure reasonable (fear <= 9; <= 16 ugly) */
	       opt_w = width;	
	   break;
	 }

 	case 'x':		/* discard this for historical reasons */
	  break;

	case 'z':
	  opt_z++;
	  break;

	/* undocumented, unsupported, feature, subject to change w/o notice. */
	case 'Z':
	  if ( tests_run( optarg ) < 0 )
	      fatal++;
	  break;

	default:
	  fatal++;
	  break;

      }
    }

    if ( opt_t && *opt_t )
    {
	stdterm = fopen( opt_t, "r+" );
	if ( ! stdterm )
	    die( "CAN NOT OPEN OUTPUT TERMINAL AT %s: %s\n",
	    		opt_t, strerror(errno) );
    }
    else
    {
	stdterm = stdout;
    }

  }
    if ( fatal ) exit( EXIT_FAILURE );

  {
     int   a;

     for ( a = optind; a < argc; a++ )
     {
	arg( stdterm, opt_c, argv[a] );
     }
  }

    if ( closure ) fputc( closure[0], stdterm );	/* close dangling code */

    if ( ! opt_n )
	putc( '\n', stdterm );
    else
	fflush( stdterm );		/* else assure stdterm flushed */

    if ( opt_r )
    {   /* reading response and relaying to stdout */
	struct timeval timeout;
	int     fd = ( stdout == stdterm ) ? fileno(stdin) : fileno(stdterm);

	while ( 1 )
        {   /* character is avaiable */
	    fd_set  readfds;
	    fd_set  exceptfds;
	    int     n;

	    FD_ZERO( &readfds );
	    FD_ZERO( &exceptfds );
	    FD_SET( fd, &readfds );		/* assure only bit we want is set */
	    timeout.tv_sec = 1; 		/* set timeout */
	    timeout.tv_usec = 0; 
	    n = select( 1 + fd, &readfds, NULL, &exceptfds, &timeout );
	    if ( n < 1 )
	        break;			/* no data: exit */

	    if ( FD_ISSET( fd, &readfds ) )	
	    {   /* at least one byte has been promised to us */
		char   buf[2];
		ssize_t bytes = read( fd,  buf, 1 );	/* direct read */
		if ( bytes > 0 )
		    putc( buf[0], stdout );
	    }
	    if ( FD_ISSET( fd, &exceptfds ) )	
		break;
	}
	fflush( stdout );		/* else assure stdout flushed */
    }

    if ( stdterm != stdout )
        fclose( stdterm );

    return( EXIT_SUCCESS );
}


/*
0000000    1b5d    313b    613a    6207    1b5d    323b    613a    6307
         033   ]   1   ;   a   :   b 007 033   ]   2   ;   a   :   c 007
0000020
*/



/* end: xecho.c */
