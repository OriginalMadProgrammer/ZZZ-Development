# Bourne-again shell
# SCOPE: rc.user
#
# Copyright 2007-2008 by Gilbert Healton
# This version of the software is released under the perl Artistic
# license (visit http://www.perl.com/pub/a/language/misc/Artistic.html for
#   details) dated August 15, 1997, or any subsequent license of your choice.
#

#######################################################################
#
#  PER-SHELL DEFINITIONS
#	# RESTRICTION: must NOT start any subshells herein
#	  (only on-time definitions may do so)
#

if [[ ."$-" = *i* ]]; then
    set -o vi;
    set -o notify;
    set -o ignoreeof;

    command_oriented_history=1

    # avoid use of \h in PS1 so earlier scripts can change rcU_HOSTNAME0
    if expr :"$rcGLOBAL": : ".*:$rcWORKDIR:" >/dev/null; then true; else
    	# have NOT seen prior bash operations
	if rc_isxterm; then
	    # see: http://tldp.org/HOWTO/Xterm-Title-4.html

	    # xterms support the wonderful banner operations we can show status in
	    PS1='\[\e]0;\u@\h: \w\a\]bash\#$ '

	    # prompt command puts information into banner
	    #: PROMPT_COMMAND="echo -en \"\033]2;\u@$rcU_HOSTNAME0:\w\"";
	    #: export PROMPT_COMMAND;
	  else
	    # not an xterm, or simiular, device with a banner
	    PS1='bash$ '
	  fi

	PUSHD_s=" ";		#disable any -s silent mode
    fi
  fi



#######################################################################
#
#  ONE-TIME SHELL DEFINITIONS
#
if expr :"$rcLOCAL": : ".*:$rcWORKDIR:" >/dev/null; then true; else
    rcLOCAL="$rcLOCAL:$rcWORKDIR";

fi

