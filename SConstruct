env = Environment(CCFLAGS=['-std=c++0x'])
env.Append(LIBS='rate_limiter.o')
env.Append(CPPPATH=['src'])
env.Append(LIBPATH=['src'])

# Library
src_files = Glob('src/*.cpp')
env.Object(src_files)

# Testing
test_files = Glob('test/*.cpp')
env.Program('test.out', test_files)
