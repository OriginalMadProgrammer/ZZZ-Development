#! /bin/bash
# isbare--check if current Linux host is running on bare metal and not virtual.
# copyright 2016 Gilbert Healton
# License: GNU Public License 3.0, or any later of your choice.
#	https://www.gnu.org/licenses/
#
#   RESTRICTION: for Linux systems only
#   BEWARE: still under development. Documentation may not yet match code.
#	Gilbert reserves the right to change this code or 
#	documentation at any time without notice. Gilbert
#	reserves the right to change, modify, or even attack,
#	his previous ideas and beliefs without notice on the
#	receipt of new information.
#
# Can be used as standalone script or Borne shell library function.
#
#   WARRANTY (from GPL):
#     THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY
#     APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT
#     HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY
#     OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
#     THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#     PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM
#     IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF
#     ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
#
# COMMAND:
#	isbare.sh [--quiet | -q] [--shell | --export | --xml | --yaml]
#	
#	Prints nothing to standard out if bare metal or trouble. Else 
#	the "best guess" VM type is printed to standard out. This
#	can include details specific to a particular release of the VM.
#
#	  --quiet - does not print name of VM to standard out.
#	  --shell|--xml|--yaml: publishes symbolic names for
#		    numeric exit codes. If you make decision based
#		    on the specific type of VM then using the 
#		    name/value pair shown in one of these formats is 
#		    HIGHLY recommended as this code is still under
#		    development (e.g., numbers subject to change 
#		    without notice).
#	  --export  Like --shell, but prefixes "export" to each definition.
#		    eval "$(isbare.sh --export)";  #gather ISBARE_X_* variables
#
# 	Exit codes: 0 for bare metal host. Non-zero if VM or trouble 
#	(see following FUNCTION call returns)
#
#	Note: local admin might install without .sh suffix.
#
# FUNCTION CALL
#	source $(which isbare.sh)   #read in as function
#	 . . .
#	string="$( isbare )";	#check if bare or not
#	isbare_exit=$?;		#acquire exit code
#	
#	Numeric exit code is best identification of VM type. 
#       Text string provides additional information that may be useful
#       but using it alone may lead to ambiguity as some virtualizers 
#       are based on other virtualizer and this code tries to sort out
#	the highest level virtualizers. Example: AWS uses XEN.
#
#   exit/return codes. See the ISBARE_X_.... definitions for name/value maps.
#     0:    is bare metal host (the simple case)
#     1-2   internal trouble values... should never return these as 
#	    they should map to 201 and 202 on return.
#     3-199 indicates virtual hosts
#	  # The returned text reflects the VM type for logging. Those 
#           who like to live exciting lives may test for values
#	    within the text to note variations on a virtualizer. 
#	    This text is printed to stdout by both the command and function.
#	  # Safest bet is to look for return of 0 (bare metal), 3-199 (VM),
#	    and 200+ (trouble), considering the stdout a useful 
#	    decoration for logging but don't bet your logic on it unless
#	    you really need, and have tested, VM-vendor specific behavior.
#     127 # Reserved: callers attempting to run isbare.sh typically see
#	    this if isbare.sh is not installed and the shell returned 
#	    a "file not found" error. 127 is not normally not
#           returned by isbare.
#     200-255 indicates trouble
#
#  RESTRICTIONS ON USING TEXT VALUES:
#    # Where possible text output needs to be treated more as an
#      interesting log tidbit than serious value to be used.
#      The basic VM type should work, with care, but use the 
#      numeric exit code for decisions.
#    # Text may provide additional information that may be useful
#      and often contain additional identifying information on the VM.
#    # Examining only text may lead to ambiguity as some virtualizers 
#      are embedded in other virtualizers and isbare tries to identify
#      the highest level virtualizer. Example: AWS uses XEN and both
#      may be found in AWS text strings.
#    # If you DO examine text:
#        # Do not be case sensitive.
#        # Order the tests in the numeric order of the return codes to 
#	   best catch embedded VM situations.
#	 # Do something sane if the text does not match anything
#	   you expect. Changes across time have been observed.
#        # Beware that different versions and combinations of the 
#          virtualizer, kernel, and distributions can return 
#          wildly different text for the same virtualizer type.
#          Assume text values will be inconsistent over time.
#        # It's a bug in your code, not mine, if your code fails due
#	   to a sudden change to text values if the exit value 
#	   remains unchanged.
#
#  EXAMPLES:
#    # Remembering if VM or not and logging type
#	isbare.sh >>$LOGFILE && isbare=true || isbare="false $?";
#	isbare.sh >>$LOGFILE && isvm=false  || isvm="true $?";
#	  case "$isexit" in	#real programmers check for errors
#	   ($ISBARE_X_TROUBLE_WC) echo 2>&1 "ISBARE TROUBLE: ${isvm#*2}2";;
#	  esac
#    # Remembering type of VM for later use. Also grabs 
#      numeric VM types.
#	 eval "$( isbare.sh --export )";	#get numeric exit values
#	 isbare.sh -q;  
#	 vmtype=$?;
#	 if [ $vmtype -ge $ISBARE_X_TROUBLE ]; then
#	     echo >&2 "TROUBLE: $vmtype";
#	     exit $vmtype;
#	 fi
#	 case $vmtype; in
#	  ($ISBARE_X_BARE)   echo "Bare Metal";
#	  ($ISBARE_X_VMBOX)  echo "Virtual Box";;
#	  ($ISBARE_X_VMWARE) echo "VMWare";;
#	  ($ISBARE_X_GENERIC) echo "Generic";;
#	  (*)                echo "Type we don't track";;
#	 esac;
#
#  DISCLAIMER
#    # This is a hodgepodge of tests gathered over many years.
#      It had organic, messy, growth that should be replaced by 
#      something neater. The upside of it is it seems to work. 
#
# ALSO DISCOVERED:
#   #  Not proud.... have incorporated tests for VM's I have not used
#      from various sources.
#   #  http://unix.stackexchange.com/questions/89714/easy-way-to-determine-virtualization-technology
#   #  http://unix.stackexchange.com/questions/3685/find-out-if-the-os-is-running-in-a-virtual-environment
#   #  http://www.dmo.ca/blog/detecting-virtualization-on-linux/
#
#  NOTES:
#    # Started life under the name "isvirtual", but I soon found
#      that exit codes of "0" on virtual do not leave much room
#      to identify the system type to those callers that require it.
#    # Doesn't really consider containers yet, such as Docker. 
#      Anyone with ideas from experience may pipe in.
#      

is_virtual_private=".not set.";	#sticky memory
				#private variable NOT for external use.
				#may have sneaky values in it
is_exit_private=256;		#becomes numeric exit (256 is invalid value)

# exit/return codes in no intentional order.
ISBARE_X_BARE=0;	#bare metal host
ISBARE_X_OOPS1=1;	#reserved: normal commands use this on trouble
ISBARE_X_OOPS2=2;	#reserved: another trouble, often from grep
	# the OOPS# values should map to TROUBLE# values before return.

  # 3 through 41 reserved for lesser virtualizers. Perhaps dockers.
  # As code still under development numbers can change without notice,
  # so scripts using return values should use the following names.
  # Users of commands should see --export, --shell, --xml, or --yaml options.
ISBARE_X_VMWARE=42;	#VMWare  (the first this author used!)
ISBARE_X_VBOX=43;	#VirtualBox from Oracle (previously Sun)
ISBARE_X_MSVPC=44;	#Microsoft Virtual PC
ISBARE_X_QEMU=46;	#QEMU
ISBARE_X_KVM=47;	#KVM
ISBARE_X_XEN_AWS=48;	#XEN embedded in AMAZON
ISBARE_X_XEN=49;	#XEN
ISBARE_X_VIRTUOZZO=50;	#Virtuozzo

ISBARE_X_OOPS127=127;	#RESERVED: typical shell "command not found" error
			# (don't expect this, but caller may get it)

ISBARE_X_GENERIC=199	#generic VM if specific not known
ISBARE_X_TROUBLE=200	#general trouble: anything -ge this value is trouble
ISBARE_X_SYSERR=255	#unspecific high-level system troubles
ISBARE_X_TROUBLE1=$(( ISBARE_X_OOPS1 + ISBARE_X_TROUBLE ))
ISBARE_X_TROUBLE2=$(( ISBARE_X_OOPS2 + ISBARE_X_TROUBLE ))
ISBARE_X_TROUBLE127=$(( ISBARE_X_OOPS127 + ISBARE_X_TROUBLE ))

# expressions callers may find useful for generic error detection
ISBARE_X_TROUBLE_RE='2[0-9][0-9]' # regular expression to notice trouble
ISBARE_X_TROUBLE_WC='2[0-9][0-9]' # shell wild card version for use in `case`



#
#   isbare--function to test if local host is a virtual machine or not
#     # no arguments.
#     # returns:
#	# if bare metal machine
#	   # stdout: nothing
#	   # return: 0 (success)
#	# if virtual machine
#	   # stdout: best specific text identify of virtualizer.
#	   # return: non-zero
#	# trouble: 
#	   # stderr often has error message.
#	   # returns values 200 or higher.
#        
#     # a lot of tests are made herein as a simple fact of life is
#       that there is no single way to detect VMs. Worse, the different
#       vintages of a particular VM can require different detection 
#       techniques over time.
#     # for many of the tests, a "VM positive" can be considered solid.
#       others tests, not so much. slippery conditions produces fears 
#       over false positive indications of "bare metal" systems. 
#	This results in balancing weaker tests against stronger tests 
#	and stopping further tests on high-value results.
#
function isbare 
{
    while [ $is_exit_private -eq 256 ] &&
          [ "$is_virtual_private" = ".not set." ]; do
    {   #as tests are expensive in time, only do once
        # (logically this single-iteration while is an "if", but use
	#  of "while" allows embedded breaks to exit early on
	#  solid test results)
    	is_virtual_private="";	#suppose not virtual
				# (guarantees "while" can't iterate again)

	typeset dmesg="$( dmesg | sed -e 's/^\[[:. 0-9][:. 0-9]*\]  *//' -e 's/"/@/g')";

	[ "$is_virtual_private" = "" ] &&       #AWS uses Xen... make special test for it
	    is_virtual_private="$( isbare_dmesg | grep '^DMI: Xen .*amazon')" && break;


	[ "${is_virtual_private#.bare.}" = "" ] && 	#Linux kernel identified virtualizer
	    is_virtual_private="$( isbare_dmesg | sed -ne '/Hypervisor detected:/s/.*: *//p' | grep . )" && break;


	if [ "$is_virtual_private" = "" ] &&
	    typeset para="$( isbare_dmesg | grep -i '^Booting paravirtualized kernel' )"; then
	    # emitted on newer kernels natively supporting VMs
	    if [[ "$para" == *"bare hardware" ]]; then
	    	is_virtual_private='.bare.';	#likely bare: but do more tests
	    else
	    	is_virtual_private="${para#"Booting paravirtualized kernel on "}";
		break;		#solid known VM
	    fi
	fi

	[ "$is_virtual_private" = "" ] && 	#Linux bare hardware test
	    is_virtual_private="$( isbare_dmesg | grep -qi '^Booting paravirtualized kernel on bare hardware' && echo ".bare." )";

	if [ $EUID -eq 0 ]; then
	{   #high-quality checks only root can do
	    if dmidecode="$(which dmidecode 2>/dev/null)" &&
	       [ -x $dmidecode ]; then
	    {
		typeset dmisystem="$(dmidecode -t system | 
			    egrep '^[[:space:]]*(Manufacturer|Product Name)')";
		typeset manufacturer="$(  echo "$dmisystem" | 
			    		sed -ne 's/.*Manufacturer: *//p' )";
		typeset productName="$(  echo "$dmisystem" | 
			    		sed -ne 's/.*Product Name: *//p' )";
		typeset vendor="$(  echo "$dmisystem" | 
			    		sed -ne 's/.*Vendor: *//p' )";
		typeset dmi="dmi: ";	#prefix to put before DMI deduced names
		case "${productName:-}" in
		  ("Microsoft"*)    case "${manufacturer:-}" in
			               ("Virtual Machine"*) is_virtual_private="${dmi}MS VirtualPC";
				       			    break 3;;
			               ("Virtual"*)         is_virtual_private="${dmi}$productName";
				       			    break 3;;
			            esac;
			            ;;
		  ("Standard PC"*)  is_virtual_private="${dmi}QEMU";
		  		    break 2;;
		  (*"VMware"*)      is_virtual_private="${dmi}VMware";
		  		    break 2;;
		  (*"VirtualBox"*)  is_virtual_private="${dmi}VirtualBox";
		  		    break 2;;
		  (*HVM*domU*)      is_virtual_private="${dmi}Xen";
		  		    break 2;;
		  (*)	case "${manufacturer:-}" in
			      # product name did not pick: try manufacturer
			      (*innotek*) is_virtual_private="${dmi}VirtualBox";
			      		  break 3;;
			      ("QEMU")    is_virtual_private="${dmi}QEMU";
			      		  break 3;;
					 
			      (*)	  #Prod and Manuf empty... 
			         	  case "$vendor" in
			      		    (*QEMU*) is_virtual_private="${dmi}QEMU";
			      		             break 4;;
					  esac;;
			    esac;
		esac;
	    } fi
	} fi

	[ "$is_virtual_private" = "" ] && 
	    is_virtual_private="$( isbare_dmesg | egrep -qi '\<vmware\>' && echo "VMware" )" && break
	[ "$is_virtual_private" = "" ] && 
	    is_virtual_private="$( isbare_dmesg | egrep -qi "\<vbox\>' - .*ACPI:" && echo "VirtualBox" )" && break;
	[ "$is_virtual_private" = "" ] && 
	    is_virtual_private="$( isbare_dmesg | egrep -qi 'hd[a-z]: Virtual HD' && echo "VirtualPC" )" && break;
	[ "$is_virtual_private" = "" ] && 
	    is_virtual_private="$( isbare_dmesg | egrep -qi 'Xen virtual' && echo "Xen" )" && break;
	isbare_dmesg --empty &&
	    [ $( wc -l /var/log/dmesg | awk '{print $1}' ) -eq 0 ] &&
	    [ -e /proc/vz ] && ls -al /proc/vz | grep -q veinfo &&
	    is_virtual_private="Virtuozzo" &&
	    break;

	### QEMU [ "$is_virtual_private" = "" ] && 
	###    is_virtual_private="$( isbare_dmesg | grep -qi "vbox - .*ACPI:" && echo "VirtualBox" )";

	# corse grained information, but rarely fails, test
	[ "${is_virtual_private#.bare.}" = "" ] && [ -e /proc/cpuinfo ] &&
	    grep -E '^flags[[:space:]]*:.*\<hypervisor\>' /proc/cpuinfo &&
	    is_virtual_private='.hypervisor.';

        break;		#no detection: get out without iterating while
    } done

    if [ $is_exit_private -eq 256 ]; then
    {	#first use sets static numeric value: map text to return code
	typeset -l ibp="${is_virtual_private}";
	case "$ibp" in
	   ("" | ".bare.")
	   		is_exit_private=$ISBARE_X_BARE;;
	      # Order of following can be important.
	      # The tests are done in ISBARE_X_* numeric order.
	      # Reorder numeric value if test order changes.
	   (*amazon*)	is_exit_private=$ISBARE_X_XEN_AWS;;
	   (*vmware*)	is_exit_private=$ISBARE_X_VMWARE;;
	   (*v*box*)	is_exit_private=$ISBARE_X_VBOX;;
	   (*v*pc*)	is_exit_private=$ISBARE_X_MSVPC;;
	   (*qemu*)  	is_exit_private=$ISBARE_X_QEMU;;
	   (*kvm*)  	is_exit_private=$ISBARE_X_KVM;;
	   (*xen*)  	is_exit_private=$ISBARE_X_XEN;;
	   (*v*ozzo*)  	is_exit_private=$ISBARE_X_VIRTUOZZO;;

	   (*)  	is_exit_private=$ISBARE_X_GENERIC;;	#not a clue
	esac
	[ $is_exit_private != $ISBARE_X_BARE ] && echo "$is_virtual_private"; 
    } fi

    return $is_exit_private;
}

# print $dmesg variable to standard out without pesky traces under -x debugging.
# --empty option just tests if text string is empty or not.
function isbare_dmesg
{
 (			#isolate +x to subshell
   set +x;		#assure -x trace is OFF for this monster text
   if [ "${1:--}" = "--empty" ]; then
   	test -z "$dmesg"
   else
	echo "$dmesg" 
   fi
 )
}


#
#   main program - if executed from command line
#
if [[ "/${0##*/}" == /isbare* ]]; then
{   #command usage
    name0="$(basename "$0")";	#properly get execution name
    name00="${name0%.*sh}";	#without any .*sh suffix

    opt_echo=true;		#suppose will echo any VM type
    opt_show=false;		#true if printing numbers ($1 is format)

    xit=$ISBARE_X_BARE;		#exit code (also "successful exit")map to 201 and 202 on exit.

    #process command line "-*" options
    while [ $# -gt 0 ] && [[ "${1:-}" == "-"* ]]; do
    {   
	opt="$1"; shift;
	case "$opt" in
	    ("--quiet"|"-q")  opt_echo=false;;
	    ("--shell"|"--export"|"--xml"|"--yaml") opt_show="true $opt";;
	    (--)	      break;;
	    (*)		      echo >&2 "$name0: Unknown option: $opt";
			      xit=$ISBARE_X_TROUBLE;;
	esac
    } done
    if [ $# -ge 1 ]; then
	echo >&2 "$name0: Extra options ($#): $*";
	xit=$ISBARE_X_TROUBLE;
    fi

    # determine if showing numeric values or running command
    if $opt_show; then
    {   # the show must go on
	set | grep '^ISBARE_X_' | sort -t= -k2,2n | {
	    case "$opt_show" in
	      (*"--shell")	sed -e 's/$/;/';;
	      (*"--export")	sed -e 's/^/export /' -e "s/\$/;/";;
	      (*"--xml")	awk -F= '{print "<"$1">"$2"</"$1">"}';;
	      (*"--yaml")	sed -e 's/=/: /';;
	      (*)		echo >&2 "$name00 BUG: unexpected opt_show='$opt_show'";
				xit=$ISBARE_X_TROOUBLE;;
	    esac
	} 
    }
    else
    {   # normal operations
	if [ $xit -eq $ISBARE_X_BARE ]; then 
	{   # all fine so far... now do the actual detective work
	    is="$(isbare)" || xit=$?;
	    $opt_echo && [ "$is" != "" ] && echo "$is"
	} fi
    } fi

    case $xit in	#in case of odd command exit, including grep.
      # have tried hard to avoid these conditions, but let's be sure
      ($ISBARE_X_OOPS1) xit=$ISBARE_X_TROUBLE1;;
      ($ISBARE_X_OOPS2) xit=$ISBARE_X_TROUBLE2;;
      ($ISBARE_X_OOPS127) xit=$ISBARE_X_TROUBLE127;;
    esac

    exit $xit;		#explicit command bye-bye
} fi

#end: isbare
