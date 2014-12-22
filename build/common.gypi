{
  'variables': {
  },
  'target_defaults': {
    'variables': {
	#'target_arch': 'x86',
	#'v8_target_arch': 'x86',
	#'v8_enable_i18n_support': 0,
    },
    'link_settings': {
    },
    'default_configuration': 'debug',
    'configurations': {
      'common': {
        'cflags': [
        ],
        'ldflags': [
        ],
        'defines': [
        ],
      },
      'debug': {
        'inherit_from': ['common',],
        'defines': [
          'DEBUG',
          #'_DEBUG',
        ],
        'cflags': [
          '-g',
        ],
        'ldflags': [
          '-g',
        ],
      },
      'release': {
        'inherit_from': ['common',],
        'defines': [
          'NDEBUG',
        ],
        'cflags': [
          '-O2',
        ],
        'ldflags': [
        ],
      },
    },
  },
}
