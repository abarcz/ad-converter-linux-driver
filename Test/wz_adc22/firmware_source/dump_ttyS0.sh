#!/bin/bash
# $Id: dump_ttyS0.sh,v 1.2 2006/03/05 19:39:31 gaufille Exp $

stty 115200 -cstopb cs8 < /dev/ttyS0
cat /dev/ttyS0

