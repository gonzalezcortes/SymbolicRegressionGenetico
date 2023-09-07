from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import setuptools

__version__ = '0.0.8'

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
        'genetico.geneticSymbolicRegression',
        ['src/genetic_SR.cpp'],
        include_dirs=[],
        language='c++',
        extra_compile_args=["-std=c++11"],
        extra_link_args=['-shared']
    ),
    Extension(
        'genetico.geneticSymbolicRegressionRN',
        ['src/genetic_SR_RN.cpp'],
        include_dirs=[],
        language='c++',
        extra_compile_args=["-std=c++11"],
        extra_link_args=['-shared']
    ),
    Extension(
        'genetico.metrics',
        ['src/metrics.cpp'],
        include_dirs=[],
        language='c++',
        extra_compile_args=["-std=c++11"],
        extra_link_args=['-shared']
    ),
    Extension(
        'genetico.reverseNotation',
        ['src/reverseNotation.cpp'],
        include_dirs=[],
        language='c++',
        extra_compile_args=["-std=c++11"],
        extra_link_args=['-shared']
    ),
    Extension(
        'genetico.cuda_kernel',
        ['src/kernel.cu'],  # Change this to your actual file path
        include_dirs=[],
        language='c++',
        extra_compile_args={'gcc': ["-std=c++11"],
                            'nvcc': ['-c', '--compiler-options', "'-fPIC'", 
                                     '--shared', '-o', "'kernel.so'", 
                                     "'kernel.cu'", 
                                     "`python3 -m pybind11 --includes`"]},
        extra_link_args=[]
    )
]

setup(
    name='genetico',
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
