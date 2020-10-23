#! /usr/bin/env python
# -*- coding: utf-8 -*-

# This file is part of Miscellaneous Container Templates.
#
#             https://launchpad.net/libmct/
#
# Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012, 2013 Paul Pogonyshev.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.


import os

from urlparse import urlunsplit


def read_file (*filename):
    file = open (os.path.join (*filename))
    try:
        return file.read ()
    finally:
        file.close ()


def pick_provider (providers, requested, cxx_type, description):
    for provider, namespace, header in providers:
        if requested == 'auto' or requested == provider:
            if configuration.CheckType ('%s::%s' % (namespace, cxx_type),
                                        includes = '#include <%s>' % header,
                                        language = 'CXX'):
                return header, namespace
            elif requested == provider:
                raise Exit ("*** error ***: %s %s requested, but header <%s> is not found"
                            % (provider, description, header))

    return '', ''


def check_cxx0x_support (context):
    context.Display ("Checking for C++0x support... ")
    supported = context.TryCompile (read_file ('aux' ,'check-cxx0x.cpp'), '.cpp')
    context.Result (supported)

    return supported


def check_boost_interprocess (context):
    # Note: see 'misc/interprocess-test.cpp' for explanation.
    context.Display ("Checking for Boost.Interprocess 1.48 or later... ")
    success = context.TryCompile (read_file ('aux' ,'check-boost-interprocess-1-48.cpp'), '.cpp')
    context.Result (success)

    return success


def create_substitutions (variables):
    return dict (('@{%s}' % variable, str (value if not isinstance (value, bool) else int (value)))
                 for variable, value in variables.items ())

def main_configuration_substitutions (configuration, environment):
    variables = { 'version_string': mct_version,
                  'major_version':  int (mct_version.split ('.') [0]),
                  'minor_version':  int (mct_version.split ('.') [1]),
                  'patch_version':  int (mct_version.split ('.') [2]) }

    variables['hash_header'], variables['hash_namespace'] = pick_provider (
        providers   = [('std',   'std',      'functional'),
                       ('tr1',   'std::tr1', 'tr1/functional'),
                       ('boost', 'boost',    'boost/functional/hash.hpp')],
        requested   = environment['hash_provider'],
        cxx_type    = 'hash <int>',
        description = 'hashing')

    if not variables['hash_header']:
        raise Exit ("*** error ***: no known hash function provider found")

    variables['type_traits_header'], variables['type_traits_namespace'] = pick_provider (
        providers   = [('std',   'std',      'type_traits'),
                       ('tr1',   'std::tr1', 'tr1/type_traits'),
                       ('boost', 'boost',    'boost/type_traits.hpp')],
        requested   = environment['type_traits_provider'],
        cxx_type    = 'is_pod <int>',
        description = 'type traits')

    variables['have_type_traits'] = (variables['type_traits_header'] is not '')

    configuration.AddTest ('CheckCxx0xSupport', check_cxx0x_support)
    variables['cxx0x_supported'] = configuration.CheckCxx0xSupport ()

    variables['have_long_long'] = configuration.CheckType ('long long', language = 'CXX')

    return create_substitutions (variables)

have_boost_interprocess = False
def tests_configuration_substitutions (configuration, environment):
    global have_boost_interprocess

    if not configuration.CheckLibWithHeader ('boost_unit_test_framework',
                                             'boost/test/unit_test.hpp',
                                             language = 'CXX'):
        # Check if it is possible to use old non-dynamic mode.
        del environment['CPPDEFINES']['BOOST_TEST_DYN_LINK']
        environment    ['CPPDEFINES']['BOOST_AUTO_TEST_MAIN'] = ''

        if not configuration.CheckLibWithHeader ('boost_unit_test_framework',
                                                 'boost/test/auto_unit_test.hpp',
                                                 language = 'CXX'):
            return False

        # The real test program will define the macro itself where needed.
        del environment['CPPDEFINES']['BOOST_AUTO_TEST_MAIN']

    if not configuration.CheckCXXHeader ('boost/mpl/vector.hpp'):
        return False

    variables = { 'hash_namespace_enter': ' '.join ('namespace %s {' % namespace
                                                    for namespace in hash_namespace.split ('::')),
                  'hash_namespace_leave': ' '.join (('}',) * len (hash_namespace.split ('::'))) }

    for concept in ('set', 'map'):
        any_found = False
        for variable, header in (('have_unordered_%s',       'unordered_%s'),
                                 ('have_tr1_unordered_%s',   'tr1/unordered_%s'),
                                 ('have_boost_unordered_%s', 'boost/unordered_%s.hpp')):
            variables[variable % concept] = (not any_found
                                             and configuration.CheckCXXHeader (header % concept))
            any_found                     = (any_found or variables[variable % concept])

        if not any_found:
            return False

    variables['have_long_long_hash_specialization'] \
        = configuration.CheckType ('%s::hash <unsigned long long>' % hash_namespace,
                                   includes = '#include <%s>' % hash_header,
                                   language = 'CXX')

    configuration.AddTest ('CheckBoostInterprocess', check_boost_interprocess)
    have_boost_interprocess = configuration.CheckBoostInterprocess ()
    variables['have_boost_interprocess'] = have_boost_interprocess

    if have_boost_interprocess:
        if not configuration.CheckFunc ('shm_unlink'):
            configuration.CheckLibWithHeader ('rt', 'sys/mman.h', language = 'C',
                                            call = 'shm_unlink (0);')

        if not configuration.CheckFunc ('pthread_join'):
            configuration.CheckLibWithHeader ('pthread', 'pthread.h', language = 'C',
                                              call = 'pthread_join (0, 0);')

    variables['have_boost_serialization'] \
        = configuration.CheckLibWithHeader ('boost_serialization',
                                            'boost/serialization/serialization.hpp',
                                            language = 'CXX')

    return create_substitutions (variables)

def benchmark_configuration_substitutions (configuration, environment):
    return create_substitutions (
        { 'have_unordered_set':         configuration.CheckCXXHeader ('unordered_set'),
          'have_tr1_unordered_set':     configuration.CheckCXXHeader ('tr1/unordered_set'),
          'have_boost_unordered_set':   configuration.CheckCXXHeader ('boost/unordered_set.hpp'),
          'have_google_dense_hash_set': configuration.CheckCXXHeader ('google/dense_hash_set') })


def inject_cxx_flags (environment, flags, inhibits = None, gcc_only = False):
    if gcc_only:
        # Splitting is supposed to catch things like 'ccache g++' etc.
        for name_part in Split (environment['CXX']):
            # While Clang is certainly not GCC, it is compatible enough.
            if (name_part   .startswith ('g++')
                or name_part.startswith ('gcc')
                or name_part.startswith ('clang')):
                break
        else:
            return

    if not (inhibits and any (True for flag in environment['CXXFLAGS']
                              if flag.startswith (inhibits))):
        environment.Append (CXXFLAGS = Split (flags))


mct_version = read_file ('version').strip ()


variables = Variables ('variables')
variables.AddVariables (
    PathVariable ('prefix', "Where to install MCT to", '/usr/local'),
    EnumVariable ('hash_provider',
                  "Default hash function to use: 'std', 'tr1', 'boost' or 'auto' (the default)",
                  'auto', allowed_values = ('std', 'tr1', 'boost', 'auto')),
    EnumVariable ('type_traits_provider',
                  ("Default type traits to use: 'std', 'tr1', 'boost', 'none' or 'auto' "
                   "(the default)"),
                  'auto', allowed_values = ('std', 'tr1', 'boost', 'auto', 'none')),
    ('only_test_module', "Compile only one test module, to speed up frequent recompilations"),
    BoolVariable ('benchmark_debugging', "Whether to enable debug output in benchmarks", False))


def build_documentation (target, source, env):
    if len (source) != 1 or len (target) != 1:
        raise ValueError

    try:
        from docutils.core import publish_cmdline
    except ImportError:
        raise Exit ("*** error ***: docutils not found; get them from "
                    "http://docutils.sourceforge.net/")

    # FIXME: Rewrite to get rid of command line usage.
    argv = ['--stylesheet-path=%s' % File ('doc/reST.css'),
            '--embed-stylesheet',
            '--no-source-link',
            '--strip-comments',
            '--language=en',
            '--input-encoding=utf-8:strict',
            '--output-encoding=utf-8:strict',
            '--no-file-insertion',
            '--field-name-limit=25',
            str (source[0]), str (target[0])]

    publish_cmdline (writer_name = 'html',
                     # Workaround a bug in certain docutils versions that cannot accept
                     # 'str' objects for some options.
                     argv        = [unicode (string) for string in argv])

    print ("Documentation has been generated as file '%s'" % str (target[0]))
    print ("Point your browser to %s"
           % urlunsplit (('file', None, os.path.abspath (str (target[0])), None, None)))


base_env = Environment (tools     = ['default', 'textfile'],
                        variables = variables,
                        CPPPATH   = ['.'],
                        BUILDERS  = { 'Documentation': Builder (action     = build_documentation,
                                                                suffix     = '.html',
                                                                src_suffix = '.txt') })

unknown = variables.UnknownVariables ()
if unknown:
    raise Exit ("*** error ***: one or more unknown build variables: %s"
                % ', '.join (unknown.keys ()))

# SCons doesn't propagates environment settings by itself.
for setting in ['CC', 'CPP', 'CXX']:
    value = os.environ.get (setting)
    if value is not None:
        base_env[setting] = value

for setting in ['CFLAGS', 'CXXFLAGS', 'LINKFLAGS']:
    value = os.environ.get (setting)
    if value is not None:
        base_env.Append (**{ setting: value.split (' ') })

inject_cxx_flags (base_env,
                  ' -pedantic -Wno-long-long -Wall -Wunused-parameter -Wno-unused-local-typedefs',
                  '-W',
                  gcc_only = True)

cleaning = bool (base_env.GetOption ('clean'))


if not cleaning:
    configuration = base_env.Configure ()
    substitutions = main_configuration_substitutions (configuration, base_env)
    base_env      = configuration.Finish ()

    base_env.Substfile ('mct/config.hpp.in', SUBST_DICT = substitutions)

    # Needed later.
    hash_header    = substitutions['@{hash_header}']
    hash_namespace = substitutions['@{hash_namespace}']

    print ("Further checks are performed for test and benchmarking programs only")


base_env.Install ('${prefix}/include/mct',      Glob ('mct/*.hpp'))
base_env.Install ('${prefix}/include/mct/impl', Glob ('mct/impl/*.hpp'))
base_env.Alias ('install', '${prefix}')


test_env = base_env.Clone ()
test_env.Append (CPPDEFINES = { 'BOOST_TEST_DYN_LINK': 1 },
                 CXXFLAGS   = ['-g'])


if not cleaning:
    configuration = test_env.Configure ()
    substitutions = tests_configuration_substitutions (configuration, test_env)
    test_env      = configuration.Finish ()

    enable_tests = bool (substitutions)
    if enable_tests:
        test_env.Substfile ('tests/config.hpp.in', SUBST_DICT = substitutions)
    else:
        print ("* note *: testing functionality is disabled as requirements are not met")

if cleaning or enable_tests:
    test_sources = [node for node in Glob ('tests/*.cpp') if node.name != 'external-validator.cpp']
    if not cleaning and test_env.get ('only_test_module'):
        additional_cpp = test_env['only_test_module']
        if not additional_cpp.endswith ('.cpp'):
            additional_cpp += '.cpp'
        test_sources = [node for node in test_sources
                        if node.name in ('main.cpp', additional_cpp)]

    test_program = test_env.Program ('run-tests', test_sources)
    test         = test_env.Command ('test', None, './run-tests --show_progress')

    test_env.Depends (Dir ('tests'), test_program)
    test_env.Depends (test, 'tests')

    if cleaning or have_boost_interprocess:
        external_validator = test_env.Program ('tests/external-validator',
                                               'tests/external-validator.cpp')

    test_env.AlwaysBuild (test)


benchmark_env = base_env.Clone ()

# Optimize heavily unless requested otherwise in environment.
inject_cxx_flags (benchmark_env, '-O2', '-O')

if benchmark_env['benchmark_debugging']:
    benchmark_env.Append (CPPDEFINES = { 'MCT_ENABLE_DEBUGGING': 1 })

if not cleaning:
    configuration = benchmark_env.Configure ()
    substitutions = benchmark_configuration_substitutions (configuration, benchmark_env)
    benchmark_env = configuration.Finish ()

    benchmark_env.Substfile ('benchmark/config.hpp.in', SUBST_DICT = substitutions)

benchmark_env.Program ('benchmark/set', 'benchmark/set.cpp')


base_env.Documentation ('doc/mct')
base_env.Depends ('doc/mct.html', 'doc/reST.css')


package_directories = ['mct', 'mct/impl', 'tests', 'benchmark', 'doc', 'misc', 'aux']
package_files       = [File (name) for name in ('AUTHORS', 'COPYING', 'INSTALL', 'NEWS', 'README',
                                                'SConstruct', 'variables.template', 'Makefile',
                                                'version', 'doc/mct.html')]

for directory in package_directories:
    package_files += Glob ('%s/*.cpp' % directory)
    package_files += Glob ('%s/*.hpp' % directory)
    package_files += Glob ('%s/*.in' % directory)
    package_files += Glob ('%s/*.txt' % directory)
    package_files += Glob ('%s/*.css' % directory)

package_files = [node for node in package_files
                 if (node.name != 'config.hpp' and not node.name.startswith ('autogenerated_'))]


# Can't believe there's no easier way to do it with SCons.
def distribution_package (environment, suffix, command):
    dist_name = 'mct-%s' % mct_version
    command   = environment.Command ('%s.%s' % (dist_name, suffix), None,
                                     [Mkdir (dist_name)]
                                     + [Mkdir (os.path.join (dist_name, directory))
                                        for directory in package_directories]
                                     + [Copy (os.path.join (dist_name,
                                                            os.path.dirname (file.path)),
                                              file)
                                        for file in package_files]
                                     + [command % dist_name]
                                     + [Delete (dist_name)])
    environment.AlwaysBuild (command)

    environment.Depends (command, 'doc')

    return command


dist_gz  = distribution_package (base_env, 'tar.gz',  'tar -c -z -f $TARGET %s')
dist_bz2 = distribution_package (base_env, 'tar.bz2', 'tar -c -j -f $TARGET %s')
dist_zip = distribution_package (base_env, 'zip',     'zip -r $TARGET %s')

base_env.Alias ('dist-gz',  dist_gz)
base_env.Alias ('dist-bz2', dist_bz2)
base_env.Alias ('dist-zip', dist_zip)
base_env.Alias ('dist',     dist_gz)


help = base_env.Command ('help', None, """
@echo "  scons install -- install to ${prefix}"
@echo "  scons install prefix=... -- install to given location"
@echo "  scons test -- build and run all available tests"
@echo "  scons benchmark -- build all available benchmarks"
@echo "  CXX=... CXXFLAGS=... scons ... -- you can change typical parameters"
@echo "  scons doc -- generate documentation"
@echo "  scons dist -- create a source tarball"
""")

if not cleaning:
    Default ('help')


# Local variables:
# mode: python
# python-indent: 4
# indent-tabs-mode: nil
# fill-column: 90
# End:
