VariantDir('bin/main', 'src/', duplicate=False)
VariantDir('bin/test', 'test/', duplicate=False)

# Environment Setup
env = Environment(CCFLAGS=['-std=c++0x'])
env.Append(CPPPATH=['src/'])

# Library
lib_objects = env.Object('bin/main/rate_limiter', Glob('bin/main/*.cpp'))

sources = Glob('bin/test/*.cpp')
sources += Glob('bin/main/rate_limiter.o')

# Testing
env.Program('bin/test/test.out', sources)
