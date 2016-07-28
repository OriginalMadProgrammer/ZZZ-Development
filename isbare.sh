#! /bin/bash
# isbare--check if current Linux host is running on bare metal and not virtual.
# copyright 2016 Gilbert Healton
# License: GNU Public License 2.0, or any later of your choice.
#
#   RESTRICTION: for Linux systems only
#
# Can be used as standalone script or Borne shell library function.
#
# COMMAND:
#	isbare.sh [--quiet | -q]
#	
#	Prints nothing if bare metal or trouble. Else "best guess" 
#	VM type is printed to standard out.
#
# 	Exit codes: 0 for bare metals. Non-zero if VM or trouble 
#	(see following FUNCTION call returns)
#
#	Note: local admin might install without .sh suffix.
#
# FUNCTION CALL
#	string="$( isbare )";	#check if bare or not
#	
#	String is empty if bare metal or trouble. Else VM type is
#	put into string.
#
#   exit/return codes
#     0: is bare metal host (the simple case... variations are  non-zero)
#     1-199 indicates virtual hosts
#	  # The returned text string also reflects the VM type and
#           may, perhaps,  provide further details on different versions,
#	    but only if well documented tests exist.  This string is 
#	    printed to stdout by the command.
#	  # Safest bet is to look for return of 0 (bare metal), 1-199 (VM),
#	    and 200+ (trouble), considering the string a useful 
#	    decoration for logging but don't bet your logic on it unless
#	    you really need, and have tested, VM-vendor specific behavior.
#     200-255 indicates trouble
#
#  NOTES:
#    # Started life under the name "isvirtual", but I soon found
#      that exit codes of "0" on virtual do not leave much room
#      to identify the system type to those callers that require it.
#    # Doesn't really consider containers yet, such as Docker. 
#      Maybe exits 190+.
#    # Where possible string needs to be treated more as an
#      interesting log tidbit than serious value to be used.
#      The basic VM type should be OK, but use the exit code more
#      than anything else.
#      

is_virtual_private=".not set.";	#sticky memory
				#private variable NOT for external use.
				#may have sneaky values in it
is_exit_private=256;		#becomes numeric exit (256 is invalid value)

# exit/return codes in no intentional order.
ISBARE_X_BARE=0;	#bare metal host
ISBARE_X_VMWARE=1;	#VMWare 
ISBARE_X_VBOX=2;	#VirtualBox from Oracle (previouslly Sun)
ISBARE_X_MSVPC=3;	#Microsoft Virtual PC
ISBARE_X_QEUM=4;	#QEMU
ISBARE_X_KVM=5;		#KVM
ISBARE_X_XEN=6;		#XEN
ISBARE_X_VIRTUOZZO=7;	#Virtuozzo

ISBARE_X_GENERIC=199	#generic VM if specific not known
ISBARE_X_TROUBLE=200	#general trouble: anything -ge this value is trouble
ISBARE_X_SYSERR=255	#specific high-level system troubles


	        #  http://unix.stackexchange.com/questions/89714/easy-way-to-determine-virtualization-technology
		#  http://unix.stackexchange.com/questions/3685/find-out-if-the-os-is-running-in-a-virtual-environment
		#  http://www.dmo.ca/blog/detecting-virtualization-on-linux/
	
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
#	# trouble: returns values 100 or higher
#     # a lot of tests are made herein as a simple fact of life is
#       that there is no single way to detect VMs. Worse, the different
#       vintages of a particular VM can require different detection 
#       techniques over time.
#     # for many of the tests, a "VM positive" can be considered solid.
#       others tests, not so much. this slippery produces fears over 
#       false positive indications of "bare metal" systems. This results 
#       in balancing weaker tests against stronger tests and stopping 
#       further tests on high-value results.
#
#  DISCLAIMER
#    # This is a hodgepodge of tests gathered over many years.
#      It's organic, messy, growth should be replaced by something
#      neater. The upside of it is it seems to work. 
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

	typeset dmesg="$( dmesg --notime | sed -e 's/"/@/g')";

	[ "${is_virtual_private#.bare.}" = "" ] && 	#Linux found virtualizer
	    is_virtual_private="$( echo "$dmesg" | sed -ne '/Hypervisor detected:/s/.*: *//p' && grep .)" && break;


	if [ "$is_virtual_private" = "" ] &&
	    typeset para="$( echo "$dmesg" | grep -i '^Booting paravirtualized kernel' )"; then
	    # emitted on newer kernels natively supporting VMs
	    if [[ "$para" == *"bare hardware" ]]; then
	    	is_virtual_private='.bare.';	#likely bare: but do more tests
	    else
	    	is_virtual_private="${para#"Booting paravirtualized kernel on "}";
		break;		#solid known VM
	    fi
	fi
	[ "$is_virtual_private" = "" ] && 	#Linux bare hardware test
	    is_virtual_private="$( echo "$dmesg" | grep -qi '^Booting paravirtualized kernel on bare hardware' && echo ".bare." )";

	if [ $EUID -eq 0 ]; then
	{   #high-quality checks only root can do
	    dmidecode="$(which dmidecode 2>/dev/null || true)" 
	    if [ -x $dmidecode ]; then
	    {
		typeset dmisystem="$(dmidecode -t system | 
			    egrep '^[[:space:]]*(Manufacturer|Product Name)')";
		typeset manufacturer="$(  echo "$dmisystem" | 
			    		sed -ne 's/.*Manufacturer: *//p' )";
		typeset productName="$(  echo "$dmisystem" | 
			    		sed -ne 's/.*Product Name: *//p' )";
		typeset vendor="$(  echo "$dmisystem" | 
			    		sed -ne 's/.*Vendor: *//p' )";
		case "${productName:-}" in
		  ("Microsoft"*)    case "${manufacturer:-}" in
			               ("Virtual Machine"*) is_virtual_private="MS VirtualPC";
				       			    break 3;;
			               ("Virtual"*)         is_virtual_private="$productName";
				       			    break 3;;
			            esac;
			            ;;
		  ("Standard PC"*)  is_virtual_private="QEMU";
		  		    break 2;;
		  (*"VMware"*)      is_virtual_private="VMware";
		  		    break 2;;
		  (*"VirtualBox"*)  is_virtual_private="VirtualBox";
		  		    break 2;;
		  (*HVM*domU*)      is_virtual_private="Xen";
		  		    break 2;;
		  (*)	case "${manufacturer:-}" in
			      # product name did not pick: try manufacturer
			      (*innotek*) is_virtual_private="VirtualBox";
			      		  break 3;;
			      ("QEMU")    is_virtual_private="QEMU";
			      		  break 3;;
					 
			      (*)	  #Prod and Manuf empty... 
			         	  case "$vendor" in
			      		    (*QEMU*) is_virtual_private="QEMU";
			      		             break 4;;
					  esac;;
			    esac;
		esac;
	    } fi
	} fi

	[ "$is_virtual_private" = "" ] && 
	    is_virtual_private="$( echo "$dmesg" | egrep -qi '\<vmware\>' && echo "VMware" )" && break
	[ "$is_virtual_private" = "" ] && 
	    is_virtual_private="$( echo "$dmesg" | egrep -qi "\<vbox\>' - .*ACPI:" && echo "VirtualBox" )" && break;
	[ "$is_virtual_private" = "" ] && 
	    is_virtual_private="$( echo "$dmesg" | egrep -qi 'hd[a-z]: Virtual HD' && echo "VirtualPC" )" && break;
	[ "$is_virtual_private" = "" ] && 
	    is_virtual_private="$( echo "$dmesg" | egrep -qi 'Xen virtual' && echo "Xen" )" && break;
	[ "$dmesg" == "" ] && 
	    [ $( wc -l /var/log/dmesg | awk '{print $1}' ) -eq 0 ] &&
	    [ -e /proc/vz ] && ls -al /proc/vz | grep -q veinfo &&
	    is_virtual_private="Virtuozzo" &&
	    break;

	### QEMU [ "$is_virtual_private" = "" ] && 
	###    is_virtual_private="$( echo "$dmesg" | grep -qi "vbox - .*ACPI:" && echo "VirtualBox" )";

	# corse grained information, but rarely fails, test
	[ "${is_virtual_private#.bare.}" = "" ] && [ -e /proc/cpuinfo ] &&
	    grep -E '^flags[[:space:]]*:.*\<hypervisor\>' /proc/cpuinfo &&
	    is_virtual_private='.hypervisor.';

        break;		#no detection: get out without iterating while
    } done

    if [ $is_exit_private -eq 256 ]; then
    {	#first use sets static numeric value: resolve string to return code
	typeset -l ibp="${is_virtual_private}";
	case "$ibp" in
	   ("" | ".bare")
	   		is_exit_private=$ISBARE_X_BARE;;
	   (*vmware*)	is_exit_private=$ISBARE_X_VMWARE;;
	   (*v*box*)	is_exit_private=$ISBARE_X_VBOX;;
	   (*v*pc*)	is_exit_private-$ISBARE_X_MSVPC;;
	   (*qemu*)  	is_exit_private=$ISBARE_X_QEMU;;
	   (*kvm*)  	is_exit_private=$ISBARE_X_KVM;;
	   (*xen*)  	is_exit_private=$ISBARE_X_XEN;;
	   (*v*ozzo*)  	is_exit_private=$ISBARE_X_VIRTUOZZO;;

	   (*)  	is_exit_private=$ISBARE_X_GENERIC;;	#not a clue
	esac
	[ $is_exit_private != $ISBARE_X_VMWARE ] && echo "$is_virtual_private"; 
    } fi

    return $is_exit_private;
}


#
#   main program - if executed from command line
#
if [[ "/${0##*/}" == /isbare* ]]; then
    name0="$(basename "$0")";	#properly get execution name
    name00="${name0%.*sh}";	#without any .*sh suffix

    opt_echo=true;		#suppose will echo any VM type

    xit=$ISBARE_X_BARE;		#exit code (also "successful exit")

    while [ $# -gt 0 ] && [[ "${1:-}" == "-*" ]]; do
	opt="$1"; shift;
	case "$opt" in
	    ("--quiet"|"-q")  opt_echo=false;;
	    (--)	      break;;
	    (*)		      echo >&2 "$name0: Unknown option: $opt";
	    		      xit=$ISBARE_X_TROUBLE;;
	esac
    done
    if [ $# -ge 1 ]; then
	echo >&2 "$name0: Extra options ($#): $*";
	xit=$ISBARE_X_TROUBLE;
    fi

    if [ $xit -eq $ISBARE_X_BARE ]; then 
    	# all fine so far... now do the actual work
	is="$(isbare)" || xit=$?;
	$opt_echo && [ "$is" != "" ] && echo "$is"
    fi
    exit $xit;
fi

#end: isbare
