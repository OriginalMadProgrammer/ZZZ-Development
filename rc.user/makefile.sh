#!/bin/bash
######
# SCOPE: rc.user scripts
#  internal script used within our very own makefiles: not intended for typical user

pwd0="$(pwd)";			#original current directory (typically "src")
name0="$(basename "$0")";	#base name of this script
dir0="$( cd "$(dirname "$0")" && pwd )";  #absolute path to directory script is in

rsync_common()
{
    typeset direction="$1"; shift;

    cd $HOME || exit $?;
    typeset a="-a --exclude='.*.swp'";
    shift;
    while [[ "$1" = -* ]]; do
	a="$a $1";
	shift;
    done

    typeset verbosw_sw=false;
    if [[ ." $a" = *" --dry-run" ]]; then
	verbose_sw=true;
    fi
    if [[ ." $a " = *" -v " ]]; then
	verbose_sw=true;
    fi

    if [ ."${1:-}" = . ]; then
    	echo 1>&2 "Host name missing";
	exit 1;
    fi

    case ".$direction" in
        ( ".to" )   
		$verbose_sw && echo "rsync $a $HOME/rc.user '${1%rc.user}:.'";
		rsync $a "$HOME/rc.user" "${1%rc.user}:.";;

	( ".from" ) 
		$verbose_sw && echo "rsync $a '${1%rc.user}' $HOME/.";
		rsync $a "${1%rc.user}" $HOME/.;;

        (*) echo >&2 "BUG: Invalid rsync_common code in arg1"; exit 1;;
    esac
}

# creates a "makefile" from "Makefile" when the local "make" does not 
# support include statements.
# BIG RESTRICTIONS:
#   # MUST only be used within the lower "src" directory.
#   # IF the local directories are insensitive to case of
#     file names THEN make supporting includes, such as 
#     GNU make, must be used.
#	# Cloud sync systems, such as Dropbox, do not allow
#	  file names differing only in their case. The
#	  GNU make must be used in such environments.
makefile_gen ()
{

    if [ "${1:-}" = --makeforce ] || ! make make_test >/dev/null 2>&1; then

	mf=Makefile; 
	mf_new="${mf%Makefile}makefile"; 
    	echo "#non-gnu make: generating $mf_new";
	echo "build $mf -> $mf_new"; 

        set -o pipefail;
      ( 
	echo "## Because the local make does not support include statements"; 
	echo "## this '${mf_new}' has been generated with the included text."; 
	echo "## It works because make prefers 'makefile' over 'Makefile'.";
	date "+##   generated %Y-%m-%d %H:%M:%S"; 
	echo "##"; 

	awk '/^include/ { print "## BEGIN  "$0 " {"; 
			  x = system("cat "$2); 
			  if ( x != 0 ) exit x;
			  print "## END "$0 " }"; 
			  next;} 
	       {print}' "$mf" || exit $?;
      ) >"$mf_new" || exit $?;
      if [ ! -f "$mf_new" ]; then
	  # if it does not exist despite the successful exit code then
	  # it is likely the local directory does not tollerate two names
	  # differing only in case. Cloud sync services, such as Drop box,
	  # can force the issue.
	  echo >&2 "FATAL: $mf_new not found:"
	  echo >&2 "   Are you running in a directory without full-case support?";
	  exit 1;
      fi
    else
    	echo "#good make: No generated 'makefile' required";
    fi

}

export UNAME_ARCH="arch-$( uname -s ),$( uname -m )";
   		# NOTE: important to use pattern that makes it easy to
		# keep linked files out of source archives. Examples:
		#   svn propset svn:ignore -R "arch-*,*" .
		#   .gitignore:  arch-*,*
		# SAMPLES:
		#  arch-Linux,i686  
		#  arch-Linux,x86_64  
		#  arch-Linux,sun4u
		#  arch-SunOS,sun4u

case "${1:-.}" in

   (--arch)  echo "$UNAME_ARCH";;

   (--makefile | --makeforce) 
	    makefile_gen "$1";
	    ;;

   (--mkdirs) 
	    arch="$($dir0/$name0 --arch)";
	    for d in "$dir0/bin" "$pwd0"; do
	        arch_dir="$d/$arch";
	        if [ ! -d "$arch_dir" ]; then
		    echo \
   	     	     mkdir -p "$arch_dir"; 
   	     	    mkdir -p "$arch_dir" || exit $?
		fi
	    done
	    ;;

   (--rsync-from) 
	rsync_common from "$@";
	;;

   (--rsync-to) 
	rsync_common to "$@";
	;;


   (--tar)
	t="rc.user.$( hostname | sed -e 's/\..*//').tgz";
      (
        cd "$( dirname "$PWD" )" &&
	tar -czf $t \
		--exclude='*~' \
		--exclude='arch-*' \
		--exclude=install \
		--exclude=.git \
		--exclude-vcs \
		rc.user/*
	ls -l $t
      )
	;;


   (--tarxxxx)
	t="rc.user.$( hostname | sed -e 's/\..*//').tgz";
      (
        cd "$( dirname "$PWD" )" &&
	tar -czf $t --exclude=.git --exclude='arch-*' --exclude-vcs --exclude='*~' $( 
	    cd "$pwd0" || exit $?;
	    excludes="$( 
		for f in $( find * -type f ); do 
		   [ -x "$f" ] && file "$f" | grep -q " linked" && echo " -a @NOT -name ${f##*/}"
		done 
	      )";
	    ( cd "$(dirname "$PWD")" && 
	       x="${excludes:+"'\(' "}$( echo "$excludes" | sed -e 's/^ -a //' -e 's/@NOT/!/g' )${excludes:+" '\)'"}" &&
		  eval find rc.user -type f $x | sort -f |
		      egrep -v '[~\.swp|\.tmp]$' | egrep -v '~|/cat1/' ) 
	) || exit $?;
	ls -l $t
      )
	;;

   (-*)
   	echo "FATAL: invalid $name0 option $1";
	exit 1;
	;;

   (*) 
	for m in $( find [0-9][0-9][-,]* -name makefile | 
			egrep 'bin|src' | sort ); do
	    d="$(dirname "$m")";
	    ( cd "$d" && make "$1" )
	done
	;;
esac
