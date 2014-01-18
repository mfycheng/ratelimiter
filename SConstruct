from sys import platform as _platform

VariantDir('bin/main', 'src/', duplicate=False)
VariantDir('bin/test', 'test/', duplicate=False)

# Environment Setup
env = Environment(CCFLAGS=['-std=c++0x'])
env.Append(CPPPATH=['src/'])

# If we're on linux, we need to specify pthread
if _platform == "linux" or _platform == "linux2":
    env.Append(LINKFLAGS=['-pthread'])

# Library
lib_objects = env.Object('bin/main/rate_limiter', Glob('bin/main/*.cpp'))

sources = Glob('bin/test/*.cpp')
sources += Glob('bin/main/rate_limiter.o')

# Testing
testProgram = env.Program('bin/test/test.out', sources)
testCommand = Command('test.out', None, 'bin/test/test.out')
AlwaysBuild(testCommand)
