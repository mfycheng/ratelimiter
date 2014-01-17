env = Environment(CCFLAGS=['-std=c++11', '-Wc++11-extensions'])
env.Append(CPPPATH=['src'])
env.Append(LIBPATH=['src'])
env.Append(LIBS='rate_limiter.o')
env.VariantDir('build', 'src', duplicate=0)

# Library
src_files = Glob('src/*.cpp')
env.Object(src_files)

# Testing
test_files = Glob('test/*.cpp')
env.Program('test.out', test_files)
