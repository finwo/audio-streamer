module.exports = {
  'root'   : true,
  'parser' : '@typescript-eslint/parser',
  'extends': [
    'eslint:recommended',
  ],
  'lib': [
    'ES2015',
  ],
  'env': {
    'node'   : true,
    'es2021' : true,
    'browser': true,
  },
  'parserOptions': {
    'ecmaVersion': 'latest',
  },
  'plugins': [
    'svelte3',
  ],
  'overrides': [
    {
      'files'    : ['**/*.svelte'],
      'processor': 'svelte3/svelte3',
    },
  ],
  'globals': {
    'storage_readdir': 'readonly',
    'storage_get'    : 'readonly',
    'storage_set'    : 'readonly',
    'storage_del'    : 'readonly',
    'wv_title_set'   : 'readonly',
  },
  'rules': {
    'semi'        : ['error', 'always'],
    'quotes'      : ['error', 'single'],
    'indent'      : ['error', 2],
    'key-spacing' : ['error', {'align': 'colon'}],
    'comma-dangle': ['error', {
      'arrays'   : 'always-multiline',
      'objects'  : 'always-multiline',
      'imports'  : 'always-multiline',
      'exports'  : 'always-multiline',
      'functions': 'never',
    }],
  },
};
