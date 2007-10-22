#!/bin/sh


# the source dir where this script is
SCRIPT_DIR=`echo $0 | sed 's|\(.*\)/.*$|\1|'`
. $SCRIPT_DIR/auto-build-common

# the name of this script
SCRIPT=`echo $0| sed 's|.*/\(.*\)|\1|g'`

for logfile in `ls -1 /home/pd/logs/${DATE}_*_mingw*.txt`; do
	 completion_test=`tail -1 ${logfile}`
	 if [ "x${completion_test}" != "xSUCCESS" ]; then
		  SUBJECT="autobuild: $distro $HOSTNAME $DATE $TIME"
#		  tail -20 ${logfile} | mail -s "autobuild: $logfile" ${RECIPIENT}
		  tail -20 ${logfile} | mail -s "autobuild: $logfile" hans@eds.org
	 fi
done
