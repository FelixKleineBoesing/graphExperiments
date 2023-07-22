from setuptools import setup
from Cython.Build import cythonize

setup(
    name='Graph',
    ext_modules=cythonize(["pygraph/cython/astar.pyx", "pygraph/cython/multidigraph.pyx", "pygraph/nativepy/multidigraph.py"]),
    zip_safe=False,
)
