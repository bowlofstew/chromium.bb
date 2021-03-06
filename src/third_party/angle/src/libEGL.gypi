# Copyright (c) 2013 The ANGLE Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
    'conditions':
    [
        ['OS=="win"',
        {
            'targets':
            [
                {
                    'target_name': 'libEGL_static',
                    'type': 'static_library',
                    'dependencies': [ 'commit_id' ],
                    'include_dirs':
                    [
                        '.',
                        '../include',
                        'libGLESv2',
                        '<(SHARED_INTERMEDIATE_DIR)',
                    ],
                    'sources':
                    [
                        '<!@(python <(angle_build_scripts_path)/enumerate_files.py \
                             -dirs common libEGL ../include \
                             -exclude common/version.h libEGL/dllmain.cpp \
                             -types *.cpp *.h)',
                    ],
                    'msvs_disabled_warnings': [ 4267 ],
                    'link_settings':
                    {
                        'libraries':
                        [
                            '-ld3d9.lib',
                        ],
                    },
                    'configurations':
                    {
                        'Debug':
                        {
                            'defines':
                            [
                                'ANGLE_ENABLE_PERF',
                            ],
                        },
                    },
                },
                {
                    'target_name': 'libEGL',
                    'type': 'loadable_module',
                    'dependencies': [ 'libGLESv2_shared', 'libEGL_static' ],
                    'include_dirs':
                    [
                        '.',
                        '../include',
                    ],
                    'sources':
                    [
                        'common/version.h',
                        'libEGL/dllmain.cpp',
                        'libEGL/libEGL.def',
                        'libEGL/libEGL.rc',
                    ],
                },
            ],
        },
        ],
    ],
}
