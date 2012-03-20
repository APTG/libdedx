from distutils.core import setup, Extension

module1 = Extension('libdedx',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = ['/usr/local/include'],
                    libraries = ['dedx'],
                    library_dirs = ['/usr/local/lib'],
                    sources = ['libdedx.c'])

setup (name = 'libdedx',
       version = '1.0',
       description = 'Wrapper for stopping power library libdedx',
       author = 'Jakob Toftegaard',
       author_email = 'jakob.toftegaard@gmail.com',
       url = 'http://libdedx.sourceforge.net/',
       long_description = '',
       ext_modules = [module1])



