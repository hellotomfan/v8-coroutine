{
	'targets':[
	{
		'target_name': 'common',
		'type': 'static_library',
		'dependencies': [
		],
		'include_dirs': [
			'.',
		],
		'direct_dependent_settings' : {
			'include_dirs': [
				'.',
			]
		},
		'sources': [
			'common.cc',
		],
	},
	],
}
