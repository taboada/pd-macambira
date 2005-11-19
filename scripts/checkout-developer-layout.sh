#!/bin/sh

# this script automatically generates a directory with all of the Pd code out
# of CVS in the standard developer's layout.  <hans@at.or.at>

# Usage: 
#	 - with no arguments, it will check out the code using anonymous CVS.
#	 - to check out using your SourceForge ID, add that as the argument

# Be aware that SourceForge's anonymous CVS server is generally 24 hours
# behind the authenticated CVS.

function print_usage ()
{
	 echo " "
	 echo "Usage: $0 [sourceforge ID]"
	 echo "   if no ID is given, it will check out anonymously"
	 echo " "
	 exit
}

if [ $# -eq 0 ]; then
	 echo "Checking out anonymously. Give your SourceForge ID as an argument otherwise."
	 CVSROOT=":pserver:anonymous@cvs.sourceforge.net:/cvsroot/pure-data"
elif [ "$1" == "--help" ]; then
	 print_usage
elif [ "$1" == "-h" ]; then
	 print_usage
elif [ $# -eq 1 ]; then
	 CVSROOT=":ext:${1}@cvs.sourceforge.net:/cvsroot/pure-data"
else
	 print_usage
fi

mkdir pure-data && cd pure-data
for section in abstractions doc extensions externals pd scripts; do
	 cvs checkout $section
done

