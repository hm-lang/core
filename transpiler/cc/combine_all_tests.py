#!/usr/bin/env python3
  
import glob, sys

def is_test_file(filename):
    return filename.endswith('test.h')

def make_test_name(filename):
    return filename[:-2].replace('/', '__')

files = [
    *glob.glob('*.h'),
    *glob.glob('library/*.h'),
    *glob.glob('library/pointers/*.h'),
]

with open('test.h', 'w') as writable:
    writable.write('// AUTOGENERATED BY %s\n'%sys.argv[0])
    writable.write('#ifndef TEST_H\n')
    writable.write('#define TEST_H\n')
    for f in files:
        if is_test_file(f):
            continue
        writable.write('#include "%s"\n'%f)

    writable.write('namespace hm {\n')
    # declare all test functions, though they should be defined somewhere.
    writable.write('#ifndef NDEBUG\n')
    for f in files:
        if is_test_file(f):
            continue
        writable.write('void test_%s();\n'%make_test_name(f))

    writable.write('void run_tests() {\n')
    writable.write('\ttry {\n')
    for f in files:
        if is_test_file(f):
            continue
        writable.write('\t\tTRY(test_%s());\n'%make_test_name(f))
    writable.write('\t} catch (const Error& e) {\n')
    writable.write('\t\tstd::cerr << "Error! " << e.message << "\\n";\n')
    writable.write('\t\tstd::cerr << " at " << e.at << "\\n";\n')
    writable.write('\t\tthrow;\n')
    writable.write('\t}\n')
    writable.write('}\n')

    writable.write('#endif\n') # end #ifndef NDEBUG
    writable.write('}\n') # end namespace hm
    writable.write('#endif\n') # end #ifndef TEST_H
