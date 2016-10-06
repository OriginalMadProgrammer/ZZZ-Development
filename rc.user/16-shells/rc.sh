# settings common to all (Bourne family) shells
# SCOPE: ZZZZ

#######################################################################
#
#  PER-SHELL DEFINITIONS
#	# RESTRICTION: must NOT start any subshells herein
#	  (only on-time definitions may do so)
#
if [[ ."$-" = *i* ]]; then

    stty erase '^?';	#aZ

fi


#######################################################################
#
#  ONE-TIME SHELL DEFINITIONS
#
if expr :"$rcLOCAL": : ".*:$rcWORKDIR:" >/dev/null; then true; else
    rcLOCAL="$rcLOCAL:$rcWORKDIR";

fi

#end

