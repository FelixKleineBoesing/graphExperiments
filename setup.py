from setuptools import setup
from Cython.Build import cythonize
from setuptools_rust import RustExtension, Binding

setup(
    name='pygraph',
    version="0.1",
    packages=["pygraph"],
    rust_extensions=[RustExtension("pygraph.rust.graphlib", "Cargo.toml", binding=Binding.RustCPython)],
    ext_modules=cythonize(["pygraph/cython/astar.pyx", "pygraph/cython/multidigraph.pyx", "pygraph/nativepy/multidigraph.py"]),
    zip_safe=False,
)
