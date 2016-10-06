# Standard Gilbert Healton sh  "rc" definitions
#
# Copyright 2007-2008 by Gilbert Healton
# This version of the software is released under the perl Artistic
# license (visit http://www.perl.com/pub/a/language/misc/Artistic.html for
#   details) dated August 15, 1997, or any subsequent license of your choice.
#

if expr ."$-" : ".*i" >/dev/null; then
    # only interactive shells get these values: early setup
    dirs()  { pushd.pl +Dirs \-t "${@:-}"; }
    pushd() { eval `pushd.pl +Pd -t${PUSHD_s:-} "${@:-}";`; }
    pd()    { eval `pushd.pl +Pd -rtx${PUSHD_s:-} "${@:-}";`; }
    popd()  { eval `pushd.pl \+Popd \-t${PUSHD_s:-} "${@:-}";`; }
    PUSHD_s="${PUSHD_s:-s}";		#default to silent under assumption
					# each shell shows current dir
					# in prompt.
					#shells that do NOT provide
					# current path in prompt should
					# redefine to "" in their setup
					# (do not change this file) to 
					# have pushd show current path.
					#NOTE: not an environment variable
					# so each shell must reset to 
					# allow changing of shells to 
					# do the right thing.
					#RESTRICTION: may already contain " "
					# to disable -s silent mode, so
					# any use of $PUSHD_s *must* be
					# in a way that allows for this space.

    export OLDPWD="${OLDPWD:-"$HOME"}";	#assure set for full pushd.pl operations
  fi

#
# also called called per-shell, so assure key definitions only occur once.
#
if expr :"$rcLOCAL": : ".*:$rcWORKDIR:" >/dev/null; then true; else
    rcLOCAL="$rcLOCAL:$rcWORKDIR";

    PATH=$rcWORKPATH/bin:"$PATH";

    if expr ."$-" : ".*i" >/dev/null; then
    	# interactive settings to use AFTER $PATH is set up
	PUSHD=" 
		$rcHOME/incoming
		$rcDIR
		${PUSHD:-}";		#always set prior PUSHD last
	export PUSHD

	pushd -sx /tmp;			#clean up stack using -x
	popd  -s;			##
      fi
  fi

#end: ~/rc.user/30-pushd/rc.sh
