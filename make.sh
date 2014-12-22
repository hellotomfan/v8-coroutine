#!/bin/bash

cmd=""

function make_cmd() {
  for c in $@; do
    if [ $c != "debug" -a $c != "release" ]; then
      cmd+=" $c"
    else
      cmd+=" BUILDTYPE=$c"
    fi
  done
}

# parse cmd
function parse_cmd() {
	case $1 in
    debug)
      make_cmd $*
      ;;
    release)
      make_cmd $*
      ;;
    clean)
      rm build/out -rf
      exit;
      ;;
    *)
      cmd+=$*
      ;;	
	esac
}

# run function
function run() {
	parse_cmd $*
	(cd build && make $cmd)
}

run $*
