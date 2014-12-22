{
	'targets': [
	{
		'target_name': 'v8_coroutine',
		'type': 'static_library',
		'dependencies': [
			'../../../deps/v8/tools/gyp/v8.gyp:v8',
			'../../../deps/v8/tools/gyp/v8.gyp:v8_libplatform',
		],
		'include_dirs': [
			'.',
			'../',
			'../../../deps/v8/include',
			'../../../deps/v8/',
		],
		'direct_dependent_settings': {
			'include_dirs': [
			'.', 
		],},
		'sources' : [
			'v8_coroutine.cc',
		],
		'cflags' : [
			#'--std=c++0x',
		],
	},]
}
