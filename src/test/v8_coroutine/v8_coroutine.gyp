{
	'targets': [
	{
		'target_name': 'test_v8',
		'type': 'executable',
		'dependencies': [
			#'../../../src/common/network/network.gyp:network',
			#'../../../deps/v8/tools/gyp/v8.gyp:v8',
			#'../../../deps/v8/tools/gyp/v8.gyp:v8_libplatform',
			'../../../src/common/v8coroutine/v8coroutine.gyp:v8_coroutine',
		],
		'include_dirs': [
			'../../../src/common',
			'../../../deps/v8/include',
			'../../../deps/v8/',
		],
		'sources' : [
			'test.cc',
		],
		'cflags' : [
		],
	},]
}
