#!/bin/sh
export LD_LIBRARY_PATH="/usr/share/blockpad/lib":$LD_LIBRARY_PATH
export QT_QPA_PLATFORM_PLUGIN_PATH="/usr/share/blockpad/plugins"
/usr/share/blockpad/bin/blockpad "$@"
