# setup all appropriate "rc.user" profiles for current system.
# SCOPE: rc.user
#
# # profile to be "."ed from "per-shell" profile, such as ".bashrc".
#     # . $HOME/rc.user/rc.sh
#     # source $HOME/rc.user/rc.sh
# # individual rc.sh files in this tree make test for login or not
#   using the "rcLOCAL" environment variable, which accumulates which
#   "rc.sh" scripts have been run to avoid running login code 
#   multiple times. The way this is done should work, even if special
#   tricks are being done by users.
# # this script performs the following:
#   # all [digits]-* directories are tested for a rc.sh script, and if
#     found, it is sourced.
#   # [digits].* (note dot, not hyphen) are run in the same way, but 
#     only if a local name matches. the following local names are checked:
#	# shell: name of user's login shell (e.g., bash, ksh, sh).
#	# rcU_HOSTNAME0: top level host name (aaa of aaa.bbb.com).
#	# rcU_HOSTNAMEx: host cluster (host name w/o any final digit, 
#         aaa if aaa3)
#	# rcU_DOMAINNAME: domain name, as found in any /etc/resolv.conf.
#	# rcU_OS: Kernel name, as returned by uname -s
#	# LOGNAME: user's login name
#     The rcU_... code names are names of variables defined when the scripts 
#     run. Example: the following will run if `uname -s` returns "Linux":
#     		15.Linux/rc.sh
#   # A [digits]--* or [digits]..* directories are processed in the same
#     maner BUT they are considered local directories that will be 
#     ignored by the .gitignore file. Actually, any *--* or *..* files
#     are ignored.
#   # the order the files are determined by the leading digits.
#       # The names are sorted in order allowing the "." and "-" 
#	  numerics to run in sequence.
#	# General groupings author finds handy:
#	    00: super early, very basic, configurations that even
#	        "15" level, or earlier, can depend on. Try hard to avoid.
#	    15: normal early conifigurations.
#	    55: normal configurations... put here when possible.
#	    98: Final settings for normal operations. Some host-specific
#		configurations can be found here.
#	    99: Reserved for conditionally starting applications 
#		during the login process.
#		  # All other configurations should have been made by now.
#		  # One example is 99-X/rc.sh to start X windows, 
#		    if appropriate. This is for systems where 
#		    administrators want users to start X-windows after
#		    login rather than the more traditional 
#		    system boot time.
#   # When individual rc.sh files are run, the following variables
#     are available (simple variables, not environment variables,
#     despite their capital names)
#	# rcWORKDIR: directory name (e.g., digits-name)
#	# rcWORKPATH: full path
#	# rcLOCAL: : delimited list built by */*.rc files to accumulate dir names
#	  that have been processed.  
#	   # first ":" entry becomes numeric nest level where ":1:", or
#	     lack of any number at all, is a good indication that
#	     this is a login shell.  See notes where first set in this file.
#	   # local to current shell to allow "su starting a nested
#	     shell, etc., to reset settings not inherited from parent.
#	   # beware that ENVIRONMENT variables are inherited and some must
#	     not be set twice.
#	# rcGLOBAL: global copy of rcLOCAL to catch nested things
#	# rcLOGIN: true if likely login shell. false if not.
#
# Copyright 2007-2008, 2010-2013, 2016 by Gilbert Healton
# This version of the software is released under the perl Artistic
# license (visit http://www.perl.com/pub/a/language/misc/Artistic.html for
#   details) dated August 15, 1997, or any subsequent license of your choice.
#

# this checks not so much if the terminal is a geniuine xterm, but
# if the terminal supports window banners that status information
# can be loaded into.
rc_isxterm ()
{
    if tty -s >/dev/null 2>&1 && expr ."$-" : ".*i" >/dev/null; then
        #is a true tty and is interactive....
	case /"$TERM" in
	   */xterm*) return 0;;
	esac
      fi
    return 1;
}

# die on fatal error... needs to be something so awful it is worth
# aborting an entire login for. 
#   # Has a short sleep to assure users see message if logoff follows
#     (of high use on xterm where window closes immediately after logoff).
#   # Control-C during sleep may get users in anyway... maybe. 
#     Best bet for debugging.
#   # When changing files used by loging should have alternate spare
#     window open to allow fixing of problems if test logins in other
#     windows fail.
rc_die ()
{
    typeset x=$?;
    echo >&2 "DIE: $*";
    if [ $x -eq 0 ]; then
        x=1;
    fi;
    sleep 5;
    exit $x;
}


# clean ":" delimited lists
# arguments provide a series of ":" delimited strings that are
# to be consolidated into a single string, without any duplicates.
#   VAR="$( rc_cleaner -- "$new_value" "$VAR:$altNewValue" )";
# Restriction: strings may not contain critical metacharacters that
#   confuse shells.
rc_cleaner ()
{
    # allow for -- POSIX style command line options (none now, but 
    #  allow for the future)
    typeset opts=true;
    while $opts && [ ."$1" = .--* ]; do
	typeset opt="$1"; shift;
        case "$opt" in
 	   "--" ) opts=false;;
	   * ) error "Unknown rc_cleaner option $opt";
	       opts=false;;
	  # --delim=: ) delimiter character
	  # --drop=mark  ) drop values between :/mark: and :/mark/: 
	esac
    done

    typeset new="";		#new value to return

    while [ $# -ge 1 ]; do
	typeset val="$1:"; shift;	#string to add
	while [ -n "$val" ]; do
	    typeset v="${val%%:*}";
	    val="${val#*:}";
	    #have a unique value
	    if [ -n "$v" ] && [[ ":$new:" != *":$v:"* ]]; then
		new="${new#:}:$v";
	    fi
	done
    done

    if [ -n "$new" ]; then
        echo "$new"
    fi

    return 0;
}



rc_main ()
{
    trap 'erx=$?; echo "ERROR EXIT $errx AT LINE $LINENO"; sleep 13; exit $erx;' ERR

    typeset rcDELIM=':';	#PATH delimiter 
				#   newer def: old code may not use yet.

    # a leading :number: is shell nest level (note lack of suffix)
    #   1: is typically login level shell, but may not be if a user that has
    #      not used this in a profile ever manually runs rc.sh or executes
    #      a "set uid" script.
    rcLOCAL="${rcLOCAL:-${rcGLOBAL:-}}";  #assure defined for fussy shells
    if expr "$rcLOCAL" : ':[0-9][0-9]*:' >/dev/null; then
	rcLOCAL="${rcLOCAL#:}";	#temporary value without any leading :
	rcLOCAL=":$(( ${rcLOCAL%%:*} + 1 )):${rcLOCAL#*:}";
    else
        rcLOCAL=":1$rcLOCAL";
		#special case ":1" indicates no rc profiles run
    fi

    # find home directory to rc.user scripts in case a user other
    # than the owner uses them. In particular if owner accesses them
    # from another account on the same system.
 case "$SHELL" in
  (*/ksh)
    typeset n0x=".sh.file";;	#best variable to fetch 
  (*)
    typeset n0x="0";;
 esac
 eval "n0=\"\${$n0x}\"";
 true "0=$0; SHELL=$SHELL; $n0x=$n0;";

    typeset rcDIR0="${rcDIR}";	#directory current script found
    if [ -e ."$rcDIR0/rc.sh"  ]; then
        true;		#caller has provided
    else
	# hunt down directory
	case "$0" in	#possible shell-specific directories (try avoiding)
	   *xxksh) rcDIR0="$(dirname "$n0")";;
	   *) true;;
	esac
	if [ ."$rcDIR0" = . ] || ! [ -d "$rcDIR0" ]; then
	    #hunt for directories
	    for rcDIR0 in \
		    $HOME/rc.user		\
		    ~ghealton/rc.user	\
		    ~gilbert/rc.user	\
		    "////NOT FOUND"		\
	    ; do
		if [ -e "$rcDIR0/rc.sh" ]; then
		    break;
		fi
	    done
	fi;

	if [ ."$rcDIR0" = ."////NOT FOUND" ]; then
	    # can't go on if we are homeless
	    echo >&2 "Not found: rcDIR0=$rcDIR0";
	    return 1;
	fi
    fi

    rcLOGIN=false;	#suppose NOT login
    if [[ ".$-" = *i* ]]; then
	# may need to play with this over time. 2013-09 edition.
        if [[ "$0" = -* ]] || [[ "$rcLOCAL" = :1:* ]] ; then
	    rcLOGIN=true;
        fi
    fi
    export rcLOGIN;

    # declare local variables
      # full path to the "rc.user/" directory. allow caller to provide
    typeset rcDIR="${rcDIR:-$HOME/rc.user}";
    unset rcDIR0;		#clear temporary variable
    rcARCH="$( $rcDIR/makefile.sh --arch )";	#PATH friendly os and arch

      # top level writable directory for user (typically $HOME, but
      # some systems are strange). Highest dir in home tree users 
      # CAN store profiles in. Typically at same level as rc.user/.
    typeset rcHOME=`dirname "$rcDIR"`;

      #working directory name (not path) under consideration
    typeset rcWORKDIR;

      # dir path to rc.sh file under consideration. alias for $rcDIR/$rcWORKDIR.
    typeset rcWORKPATH;

      # host name, maybe fully qualified host name, as returned by hostname
    rcU_HOSTNAME=`uname -n`; 		export rcU_HOSTNAME;

      # short (unqualified) host name
    rcU_HOSTNAME0="${rcU_HOSTNAME%%.*}"; export rcU_HOSTNAME0;

    # names ending in digit indicate potential family of hosts
    typeset rcU_HOSTNAMEx="${rcU_HOSTNAME0%[0-9]}";
            rcU_HOSTNAMEx="${rcU_HOSTNAMEx%[0-9]}";

      # obtain any available domain name
    typeset rcU_DOMAINNAME;
    if [ -f /etc/resolv.conf ]; then
	rcU_DOMAINNAME=`grep '^domain' /etc/resolv.conf | 
	                awk '{print $2}' | 
			grep '^[a-z0-9]'    |
			head -n1`;
    fi

      # os name
    rcU_OS=`uname -s`;			export rcU_OS;

      # tty name in variable to avoid wandering ttys
    typeset rcTTY=` tty -s && tty`;	#empty if not a tty

    # find all nested "rc.sh" files appropriate for current system
    #	##-name    used for generic configurations for all systems
    #   ##.name    used for specific conditions where name 
    #              indicates conditoin.
    # if a combination of these are required, or new values of
    # uname options, consider placing the extensions within
    # existing files rather than adding new iterations to
    # the following loop.
    for rcWORKDIR in $( 
	    cd $rcDIR && ( 
	    	# unconditional finds
		find [0-9][0-9][-.]* -type d '(' \
		    -name "[0-9][0-9].$( basename "${0#-}" )"  -o \
		    -name "[0-9][0-9].$rcU_HOSTNAME0"  -o \
		    -name "[0-9][0-9].$rcU_OS"  -o \
		    -name "[0-9][0-9].$LOGNAME"  -o \
		    -name "[0-9][0-9]-*" \
		  ')' -prune; 

		# conditional finds
		if [ ."$rcU_HOSTNAMEx" != ."$rcU_HOSTNAME0" ]; then
		    #look for famlies of hosts sharing common configs
		    find * -type d -name "[0-9][0-9].$rcU_HOSTNAMEx" -prune;
		fi

		if [ ."${rcU_DOMAINNAME:-}" != . ]; then
		    #names ending in a digit indicate potential family of hosts
		    find * -type d -name "[0-9][0-9].$rcU_DOMAINNAME" -prune;
		fi

		  ) |
	      sort -u
	   ); do
	${rcDEBUG:-false} && echo "DEBUG: rcWORKDIR=$rcWORKDIR"
	rcWORKPATH="$rcDIR/$rcWORKDIR";	#build path that may be used by rc.sh
	if [ -f "$rcWORKPATH/rc.sh" ]; then
	     .  "$rcWORKPATH/rc.sh";

	     # ./scripts/ : contents get copied to ../../bin/.
	     if [ -d "$rcWORKPATH/scripts" ]; then
		for f in $( find "$rcWORKPATH/scripts" -type f ); do
		    cp "$f" bin/. &&
		    chmod a+rx,go-w "bin/$(basename "$f")" ||
		    echo "$f: failed to copy and chmod"
		done
	     fi

	     # ./bin: put on PATH
	     for d in bin "bin/$rcARCH"; do
		 if [ -d "$rcWORKPATH/$d" ]; then
		     PATH="$PATH$rcDELIM$rcWORKPATH/$d"
		 fi
	     done
	  fi
      done;

    unset rcWORKDIR rcWORKPATH

    PATH="${PATH}:${rcDIR}/bin:${rcDIR}/bin/$rcARCH"

    rcGLOBAL="$rcLOCAL";
    export rcGLOBAL;
    unset  rcLOCAL;		#assure local goes away (typedefs are
    #				# not honored when "source"d file
    #				# redefines the variable)

    trap - ERR;
}

rc_main "$@";

unset -f rc_main;
unset -f rc_misc;
unset -f rc_isxterm;
unset -f rc_die;


#end
