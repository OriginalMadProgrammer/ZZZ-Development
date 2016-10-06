# Configurations frequently customized for local user
# SCOPE: rc.user
#
# Copyright 2007-2008 by Gilbert Healton
# This version of the software is released under the perl Artistic
# license (visit http://www.perl.com/pub/a/language/misc/Artistic.html for
#   details) dated August 15, 1997, or any subsequent license of your choice.
#

if expr ."$-" : ".*i" >/dev/null; then
    # only interactive shells get these values: early setup
    true;
  fi

#
# also called called per-shell start, so assure key definitions only occur once.
#
if expr :"$rcLOCAL": : ".*:$rcWORKDIR:" >/dev/null; then true; else
    rcLOCAL="$rcLOCAL:$rcWORKDIR";

    # put our bin directory on PATH
    PATH=$rcWORKPATH/bin:"$PATH";

    # select appropriate pager program, favoring "less"
      if which less 2>&1 | grep '^no less in' >/dev/null; then
	  #(note: some `which` commands return 0 exit codes on failure)
	PAGER='more -is';
      else
	PAGER='less -is';
      fi
    export PAGER

    # set vi defaults WITHOUT using ~/.exrc, or any other local file...
    EXINIT="set nowrapscan | set shiftwidth=4 | set autoindent ";
	export EXINIT

  fi

#end: ~/rc.user/30-local/rc.sh
