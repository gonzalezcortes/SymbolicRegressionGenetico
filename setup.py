from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import subprocess
import setuptools

__version__ = '0.0.8'

# Custom build_ext subclass to build CUDA files
class build_ext_subclass(build_ext):
    def finalize_options(self):
        super().finalize_options()
        import pybind11
        self.include_dirs.append(pybind11.get_include())

    def run(self):
        # compile the CUDA part
        self.compile_cuda('kernel.cu', 'kernel.o')
        # build extensions as usual
        super().run()

    @staticmethod
    def compile_cuda(source, output):
        command = [
            'nvcc',
            '-c',
            '-o', output,
            '-arch=sm_35',
            '--compiler-options', '-fPIC',
            source
        ]
        print("Running:", " ".join(command))
        subprocess.check_call(command)

ext_modules = [
    # ... your existing modules
    Extension(
        'kernel',
        ['kernel.cpp', 'kernel.o'],
        include_dirs=[],
        language='c++',
        extra_compile_args=["-std=c++11"],
        extra_link_args=['-shared'],
    ),
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
    cmdclass={'build_ext': build_ext_subclass},
    zip_safe=False,
)
