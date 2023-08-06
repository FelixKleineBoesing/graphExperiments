from distutils.extension import Extension

from setuptools import setup
from Cython.Build import cythonize
from setuptools_rust import RustExtension, Binding


setup(
    name='pygraph',
    version="0.1",
    packages=["pygraph", "pygraph.cycpp", "pygraph.cypy", "pygraph.rust", "pygraph.nativepy"],
    rust_extensions=[RustExtension("pygraph.rust.graphlib", "Cargo.toml", binding=Binding.PyO3)],
    ext_modules=cythonize([
        Extension("pygraph.cycpp.astar", sources=["pygraph/cycpp/astar.pyx"], language="c++", extra_compile_args=["-O3"]),
        Extension("pygraph.cycpp.multidigraph", sources=["pygraph/cycpp/multidigraph.pyx"], language="c++", extra_compile_args=["-O3"]),
        Extension("pygraph.cypy.multidigraph", sources=["pygraph/cypy/multidigraph.pyx"], language="c", extra_compile_args=["-O3"]),
        Extension("pygraph.cypy.astar", sources=["pygraph/cypy/astar.pyx"], language="c", extra_compile_args=["-O3"]),
    ], compiler_directives=dict(
        boundscheck=False,
        wraparound=False,
        initializedcheck=False,
        nonecheck=False,
        overflowcheck=False,
        cdivision=True,
    )),
    zip_safe=False,
)
