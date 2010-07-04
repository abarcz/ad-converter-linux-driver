#/bin/sh
# $Id: init_host.sh,v 1.2 2006/03/05 19:39:31 gaufille Exp $

/etc/init.d/cupsys stop
modprobe -r lp
modprobe ppdev

TOOLSDIR=`dirname \`pwd\``/tools

#$TOOLSDIR/bin/odyssey init
chmod a+r /sys/kernel/debug/usbmon/2t

echo Initialization done.

