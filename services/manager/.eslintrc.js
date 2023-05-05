module.exports = {
  'root'   : true,
  'parser' : '@typescript-eslint/parser',
  'extends': [
    'eslint:recommended',
  ],
  'env': {
    'node'  : true,
    'es2021': true,
  },
  'parserOptions': {
    'ecmaVersion': 'latest',
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
