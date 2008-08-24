from distutils.core import setup, Extension
import sys, os, os.path, string

try:
    import py2exe
except ImportError:
    print "No support for py2exe! Must be non-windows."

def contains_arrayobject_h(path):
    """
    Returns True iff the python path string contains the arrayobject.h
    include file where it is supposed to be.
    """
    f=False
    try:
        s=os.stat(os.path.join(path, 'numpy', 'core', 'include', \
                               'numpy', 'arrayobject.h'))
        f=True
    except OSError:
        pass
    return f

if sys.platform != 'darwin':
    extra_link_args = ['-s']

other_exts = []

valid_paths = filter(contains_arrayobject_h, sys.path)

# The base path is by default the first python path with arrayobject.h in it.
include_numpy_array=valid_paths[0]

if len(valid_paths) > 1:
    print "There are several valid include directories containing numpy/arrayobject.h"
    l=[('%d: %s' % (i + 1, valid_paths[i])) for i in xrange(0, len(valid_paths))]
    s = 1
    print string.join(l, '\n')
    # Prompt the user with a list of selections.
    while not (s >= 1 and s <= len(valid_paths)):
        s = raw_input('Selection [default=1]: ')
        if s == '':
            s = 1
        else:
            s = int(s)
    include_numpy_array=valid_paths[s-1]

# Add the children directory path suffix to the base path.
include_numpy_array=os.path.join(include_numpy_array, 'numpy', 'core', \
                                 'include')


setup(name='cvd',
      version='0.9',
      description='Python CVD',
      author='Damian Eads and Edward Rosten',
      packages=['CVD'],
      scripts=[],
      ext_modules=[Extension('cvd',
                             ['CVD/cvd.cpp'],
                             extra_link_args = extra_link_args,
                             include_dirs=[include_numpy_array])])
