#!/usr/bin/python
from distutils.core import setup, Extension

module1 = Extension('libdedx',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '2')],
                    include_dirs = ['/home/bassler/Projects/libdedx/trunk/libdedx'],
                    libraries = ['dedx'],
                    library_dirs = ['/home/bassler/Projects/libdedx/trunk/libdedx'],
                    sources = ['libdedx.c'])
setup (name = 'libdedx',
       version = '1.2.0-svn215',
       description = 'Wrapper for stopping power library libdedx',
       author = 'Jakob Toftegaard',
       author_email = 'jakob.toftegaard@gmail.com',
       url = 'http://libdedx.sourceforge.net/',
       long_description = '',
       ext_modules = [module1])

