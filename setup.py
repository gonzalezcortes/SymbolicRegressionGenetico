# setup.py
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import setuptools

__version__ = '0.0.1'


def add_init_pybind11(cls):
    class build_ext_subclass(cls):
        def finalize_options(self):
            super().finalize_options()
            __builtins__.__NUMPY_SETUP__ = False
            import pybind11
            self.include_dirs.append(pybind11.get_include())
    return build_ext_subclass

ext_modules = [
    Extension(
        'example_1',
        ['src/example_1.cpp'],
        include_dirs=[],
        language='c++'
    )
]

setup(
    name='SymbolicRegression',
    version=__version__,
    author='Daniel Gonzalez Cortes',
    author_email='',
    url='',
    description='',
    long_description='',
    ext_modules=ext_modules,
    install_requires=['pybind11>=2.5.0'],
    cmdclass={'build_ext': add_init_pybind11(build_ext)},
    zip_safe=False,
)
