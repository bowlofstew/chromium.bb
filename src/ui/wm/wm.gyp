# Copyright 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'wm_public',
      'type': 'static_library',
      'dependencies': [
        '../../skia/skia.gyp:skia',
        '../aura/aura.gyp:aura',
        '../gfx/gfx.gyp:gfx_geometry',
      ],
      'sources': [
        'public/window_types.h',
      ],
    },
    {
      'target_name': 'wm_core',
      'type': '<(component)',
      'dependencies': [
        '../../base/base.gyp:base',
        '../../skia/skia.gyp:skia',
        '../aura/aura.gyp:aura',
        '../compositor/compositor.gyp:compositor',
        '../events/events.gyp:events_base',
        '../events/events.gyp:events',
        '../gfx/gfx.gyp:gfx_geometry',
        '../gfx/gfx.gyp:gfx',
        '../resources/ui_resources.gyp:ui_resources',
        '../base/ui_base.gyp:ui_base',
      ],
      'defines': [
        'WM_CORE_IMPLEMENTATION',
      ],
      'sources': [
        'core/base_focus_rules.cc',
        'core/base_focus_rules.h',
        'core/capture_controller.cc',
        'core/capture_controller.h',
        'core/compound_event_filter.cc',
        'core/compound_event_filter.h',
        'core/cursor_manager.cc',
        'core/cursor_manager.h',
        'core/easy_resize_window_targeter.cc',
        'core/easy_resize_window_targeter.h',
        'core/focus_controller.cc',
        'core/focus_controller.h',
        'core/focus_rules.h',
        'core/image_grid.cc',
        'core/image_grid.h',
        'core/input_method_event_filter.cc',
        'core/input_method_event_filter.h',
        'core/masked_window_targeter.cc',
        'core/masked_window_targeter.h',
        'core/native_cursor_manager.h',
        'core/native_cursor_manager_delegate.h',
        'core/shadow.cc',
        'core/shadow.h',
        'core/shadow_controller.cc',
        'core/shadow_controller.h',
        'core/shadow_types.cc',
        'core/shadow_types.h',
        'core/transient_window_controller.cc',
        'core/transient_window_controller.h',
        'core/transient_window_manager.cc',
        'core/transient_window_manager.h',
        'core/transient_window_observer.h',
        'core/transient_window_stacking_client.cc',
        'core/transient_window_stacking_client.h',
        'core/visibility_controller.cc',
        'core/visibility_controller.h',
        'core/window_animations.cc',
        'core/window_animations.h',
        'core/window_modality_controller.cc',
        'core/window_modality_controller.h',
        'core/window_util.cc',
        'core/window_util.h',
        'core/wm_core_export.h',
        'core/wm_core_switches.cc',
        'core/wm_core_switches.h',
        'core/wm_state.cc',
        'core/wm_state.h',
      ],
    },
    {
      'target_name': 'wm_test_support',
      'type': 'static_library',
      'dependencies': [
        '../../skia/skia.gyp:skia',
        '../aura/aura.gyp:aura',
      ],
      'sources': [
        'test/wm_test_helper.cc',
        'test/wm_test_helper.h',
      ],
    },
    {
      'target_name': 'wm_core_unittests',
      'type': 'executable',
      'dependencies': [
        '../../base/base.gyp:base',
        '../../skia/skia.gyp:skia',
        '../aura/aura.gyp:aura',
        '../compositor/compositor.gyp:compositor',
        '../events/events.gyp:events_base',
        '../events/events.gyp:events',
        '../gfx/gfx.gyp:gfx_geometry',
        '../gfx/gfx.gyp:gfx',
        '../base/ui_base.gyp:ui_base',
        'wm_core',
        'wm_test_support',
      ],
      'sources': [
        'test/run_all_unittests.cc',
        'core/compound_event_filter_unittest.cc',
        'core/cursor_manager_unittest.cc',
        'core/focus_controller_unittest.cc',
        'core/input_method_event_filter_unittest.cc',
        'core/image_grid_unittest.cc',
        'core/shadow_controller_unittest.cc',
        'core/transient_window_manager_unittest.cc',
        'core/transient_window_stacking_client_unittest.cc',
        'core/visibility_controller_unittest.cc',
        'core/window_animations_unittest.cc',
        'core/window_util_unittest.cc',
      ],
    },
  ],
}