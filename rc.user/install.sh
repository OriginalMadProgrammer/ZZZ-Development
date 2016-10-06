#/bin/sh 
#install.rc -- finish install of these files
#   use minimal shell commands for maximum portability
#   Usage: make install
# SCOPE: rc.user ZZZZ
#
# Copyright 2007-2008,2016 by Gilbert Healton
# This version of the software is released under the perl Artistic
# license (visit http://www.perl.com/pub/a/language/misc/Artistic.html for
#   details) dated August 15, 1997, or any subsequent license of your choice.
#

rcDIR=`dirname "$0"`;
rcDIR=`cd "$rcDIR" && pwd`;

die () {
    echo >&2 "DIE: $*";
    exit 1;
}

rcHOME=`dirname "$rcDIR"`;
rcDIR_HOME=`echo "$rcDIR" | sed -e "s|^$HOME\>|\\\$HOME|"`;


cd $rcDIR || die "CAN NOT cd $rcDIR";

# assure .vim/colors is set up to point to vim color schemes delivered with
# this file (from http://vimcolors.com).
echo "setting ~/.vim/colors";
[ ! -d "$HOME/.vim" ] && mkdir "$HOME/.vim"
[ -L "$HOME/.vim/colors" ] && rm "$HOME/.vim/colors";  #remove any old symlink
vim_color_dir=false;		#assume $HOME/.vim/colors is real directory
[ -d "$HOME/.vim/colors" ] && vim_color_dir=true; #yes
$vim_color_dir || ln -s "$rcDIR/vimcolors" "$HOME/.vim/colors"; #build symlink
[ -L "$HOME/.vim/colors" ] && echo "  ~/.vim/colors is symlink to $rcDIR/vimcolors (good!)" ||
			      echo "  ~/.vim/colors is hard dir we copy files into";

for file in `find * -type f`; do
    if expr "$file" : 'vimcolors/' >/dev/null; then
	$vim_color_dir && echo "   ~/.vim/colors/$f" && cp -p "$f" "$HOME/.vim/colors/.";  #copy file if not link
    elif [ -x "$file" ]; then
    # fix #! scripts to point to proper program
	type=`file $file | sed -e 's/^.*: *//'`;       #("file -b" the hard way)
	if expr ."$type" : ".* script" >/dev/null ||   #new `file` is smart
	   expr ."$type" : ".* text" >/dev/null; then  #old `file` is dumb
	    # ZZZZ rework to allow for "/usr/bin/eval xxxx" style notation
	    #  typeset -a command=( ..... );
	    command=`head -n1 "$file" | grep '^# *!' | sed -e 's/^# *! *//'`;
	    if [ -x "${command:-$0}" ]; then true; else
	        #command in #! does NOT exist (note above true;else)
		basename=`basename "$command"`;	#get base name of command
		which=`which "$basename" 2>/dev/null`;	#get true path
		if [ $? -eq 0  -a  ."$which" != . ]; then
		    rm -f "$file~";
		    echo "  redirecting $which $file";
		    echo "#! $which" | 
			cat - "$file" |
				# discard all but opening #! lines
			awk '
				BEGIN	{ printing = 0; first = 1; }
				/^# *!/ { #only print 1st #!, from echo
					  if ( printing || first ) 
					      print; 
					  first = 0;
					  next; 
				        }
					{ printing = 1; first = 0; print; }
			    ' > "$file~"
			 mv -f "$file~" "$file" || 
				die "CAN NOT RENAME $file~ -> $file";
		  	 chmod a+x "$file" ||
			 	die "CAN NOT CHMOD A+X $file";
		  fi
	      fi
 	  fi
      fi
  done

echo "  patching \$rcDIR=$rcDIR_HOME rc.sh"
sed <rc.sh -e "s|typeset rcDIR=.*|typeset rcDIR=\"\\\${rcDIR:-$rcDIR_HOME}\";|" >rc.sh~
mv rc.sh~ rc.sh ||
	die "CAN NOT RENAME $rcDIR/rc.sh~ -> $rcDIR/rc.sh";


echo "  assuring rc.sh files NOT executable"
find * -name 'rc.sh' -type f | xargs chmod a-x

patched=false;
for this_rc in bashrc kshrc; do
{
        echo "  assuring .$this_rc invokes rc.user/rc.sh"
	if ! grep -q "^[ 	]*\.[ 	].*rc\.user/rc.sh\>" $rcHOME/.$this_rc \
		; then 
	{
	    cat <<RC_END | sed -e 's/^[ 	]*://' >>$rcHOME/.$this_rc 
	    	:
		:#invoke rc.user/rc.sh
		:#  (patch added by $0 on $( date ))
		:if [ -f \$HOME/rc.user/rc.sh ]; then
		:      . \$HOME/rc.user/rc.sh
		:  fi
RC_END
	    echo "       patched"
	}  fi
        patched=true;		#either patched now, or in the past
} done


echo "successful install"

$pathed || echo 'NOTE: no shell "*rc" files patched'

#end
