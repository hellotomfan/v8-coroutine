#!/bin/bash

# build funcion
function build() {
	path=$(dirname $0)
	#PYTHONPATH=deps/v8/tools/generate_shim_headers:deps/v8/build:$PYTHONPATH:deps/v8/build/gyp/pylib:$PYTHONPATH
	${path}/build/gyp/gyp --depth=${path}/ -I ${path}/build/common.gypi --generator-output=build $* all.gyp #-Dv8_enable_i18n_support=0
}

build $*
