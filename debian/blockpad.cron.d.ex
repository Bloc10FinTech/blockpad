#
# Regular cron jobs for the blockpad package
#
0 4	* * *	root	[ -x /usr/bin/blockpad_maintenance ] && /usr/bin/blockpad_maintenance
