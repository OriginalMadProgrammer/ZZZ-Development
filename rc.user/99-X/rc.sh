# X windows start
# SCOPE: rc.user

# Copyright 2007-2008, 2013 by Gilbert Healton
# # This script has two purposes in life, depending on how it is started:
#    1) For logins from actual consoles: automatically start X windows on 
#       login if it is not already running.
#        # Such is done on systems that do not go to run-level 5 upon 
#          boot (e.g. start X, with it's graphicl login screen).
#        # only console or hard-tty-1 trigger this. Switching to another
#	   login termainal allows users to bypass these autostarts.
#    2) On logins from remote systems: set the DISPLAY environment variable 
#	to allow any X-window clients on the local POSIX/UNIX system to
#	send the grapical display to the remote X-server system.
#	  # clienthost: this host, the one a remote user is logging in on,
#	    and is running this script.
#	  # remotehost: the host issuing the login request. "remote" in
#	    the sense it is NOT this process and typically not on this host. 
#	    It may be ajacent or on the other side of the world. Or, these 
#	    days, even in outer space.
#         # The remotehost will need to run an X Windows server. 
#             # For remote Microsoft Windows systems this is an application 
#               such as XMING. 
#                # Likely you will need to add the UNIX/POSIX host names to 
#                  XMING's X0.hosts file on the remotehost Windows system.
#			localhost	<already present... keep>
#			clienthost.example.com
#	      # For POSIX/UNIX remotehost systems the xhost() command may 
#		need to be run on that system to allow incoming X commections from 
#		this client.
#			xhost +clienthost.example.com	#run on remotehost
#         # Test by running xclock from the command line on clienthost once
#	    the login is complete (and DISPLAY= is set)
#               $ xclock -update 1
#
# This version of the software is released under the perl Artistic
# license (visit http://www.perl.com/pub/a/language/misc/Artistic.html for
#   details) dated August 15, 1997, or any subsequent license of your choice.
#


#######################################################################
#
#  ONE-TIME DEFINITIONS
#
if expr ."$rcLOCAL": : ".*:$rcWORKDIR:" >/dev/null; then true; else

    rcLOCAL="$rcLOCAL:$rcWORKDIR";

    	# expect ttys to start with and optional /dev/, followed
	# by the letters tty1 for the main tty. "console" also works.
#set -x
    if [ ."${DISPLAY:-}" = . ] && $rcLOGIN && 
          [ ."$LOGNAME" != ."root" ]; then
       # only logins get this stuff
       # root does not get this stuff... likely does not want... even dangerous
       case ."${rcTTY#/dev/}" in
         .tty1 | .console ) 
	   if which startx >/dev/null 2>&1; then	#assure installed
	       startx ||		# start X windows for login
	           echo X WINDOW START FAILED
	       sleep 5;		#give users chance to see screen on
	       			##exits with error messages.
	       exit 0;		#log out of shell
	     fi
	   ;;

	    # if this is the original login from remote host then set
	    # the DISPLAY environment variable to point to that remote host.  
         .pts/[0-9]* | pty[0-9]* )
	   typeset -l ip="$(last -i "$LOGNAME" | grep " ${rcTTY#/dev/} " | 
		    awk '{print $3; exit(0);}' )";
	     # use IP address rather than name as names can be long enough
	     # to be truncated by "last" commands. I fear for IPv6 addresses,
	     # but so far I have not been bit.
	   if expr x"$ip" : 'x[1-9][0-9]*\.[.0-9]*[0-9]$' >/dev/null; then
	       export DISPLAY="$ip:0.0";	#ipV4 address
	       echo "    IPv4: DISPLAY=$DISPLAY"
	   elif expr x"$ip" : 'x[0-9a-f][0-9a-f]*:[:0-9a-f]*[0-9a-f]$' >/dev/null; then
	       export DISPLAY="$ip:0.0";	#ipV6 address
	       echo "    IPv6: DISPLAY=$DISPLAY"
	   else
	       echo "    IP trouble: '$ip'"
	   fi
	   unset ip
	   ;;

         esac
      fi
#set +x
fi

#end: 99-X/rc.sh
