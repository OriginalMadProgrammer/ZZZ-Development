# Korn-specific configurations
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
    set -o vi
    set -o monitor
    set -o ignoreeof

      # NOT all ksh's are created equal: BSD vs rest of world
    if expr :"$rcGLOBAL": : ".*:$rcWORKDIR:" >/dev/null; then true; else
    	# have NOT seen prior bash operations
	if rc_isxterm && true; then 

	    # xterms supportfalse the wonderful banner operations we can show status in,
	    # but ksh doesn't provide a neat way to throw it out to the world


	    PS1=']0;${USER}@${rcU_HOSTNAME0}:${PWD}/ksh#!$ '
	    # see: http://tldp.org/HOWTO/Xterm-Title-4.html
	  else
	    # not an xterm, or simulator, or device with a banner
	    PS1='$LOGNAME@\h $PWD ';  #use a shortned, basename, for dir
	  fi
      fi
  fi


#######################################################################
#
#  ONE-TIME KSH DEFINITIONS
#
if expr :"$rcLOCAL": : ".*:$rcWORKDIR:" >/dev/null; then true; else
    rcLOCAL="$rcLOCAL:$rcWORKDIR";

fi
