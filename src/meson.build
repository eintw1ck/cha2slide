majorver = '0'
apiver   = '0'
fixver   = '1'

version = majorver + '.' + apiver + '.' + fixver

build_opts = [
  '-D_POSIX_SOURCE',
  '-U__STRICT_ANSI__', '-fvisibility=hidden',

  # Warnings
  '-Wall', '-Wextra', '-Wundef', '-Wmissing-prototypes', '-Wshadow',
  '-Wparentheses', '-Wpointer-arith', '-Wno-pointer-sign',
  '-pendantic',

  # Warnings to treat as errors
  '-Werror=implicit-function-declaration',
]

# Global dependencies
build_deps = [
  dependency('threads'),
  dependency('zlib'),
]

cc = meson.get_compiler('c')

if cc.has_argument('-Wincompatible-pointer-types')
  build_opts += ['-Werror=incompatible-pointer-types']
endif

# clang's version of -Wmissing-braces rejects the common {0} initializers
if cc.get_id() == 'clang'
  build_opts += ['-Wno-missing-braces']
endif

sources = [
  'blake2b.c',
  'crc32.c',
  'chacha20.c',
  'chunk.c',
  'log.c',
]

tests = [
  'test.c',
  'testblake2b.c',
  'testchacha20.c',
  'testchunk.c',
  'testcrc32.c',
]

# Configuration
conf = configuration_data()
conf.set('majorver', majorver)
conf.set('apiver', apiver)
conf.set('fixver', fixver)
conf.set_quoted('version', 'v' + version)

# Update VERSION with `git describe` information if available
git = find_program('git', required: false)
if git.found()
  gitdesc = run_command(git, 'describe')
  if gitdesc.returncode() == 0
    conf.set_quoted('version', gitdesc.stdout().strip())
  endif
endif

lib = static_library('cha2slide', sources,
  install: true,
  dependencies: build_deps,
  soversion: apiver,
)

munit_dep = dependency('munit', fallback: ['munit', 'munit_dep'], required: false)

mytest = executable('mymunit', sources + tests, dependencies: build_deps + [munit_dep])
test('mytest', mytest, is_parallel: false, timeout: 600)

cha2slide = executable('cha2slide', sources + ['cha2slide.c'], dependencies: build_deps)
