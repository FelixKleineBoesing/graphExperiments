from setuptools import setup
from Cython.Build import cythonize

setup(
    name='Graph',
    ext_modules=cythonize(["src/cython/astar.pyx", "src/cython/multidigraph.pyx", "src/nativepy/multidigraph.py"]),
    zip_safe=False,
)
