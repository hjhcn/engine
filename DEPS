# This file is automatically processed to create .DEPS.git which is the file
# that gclient uses under git.
#
# See http://code.google.com/p/chromium/wiki/UsingGit
#
# To test manually, run:
#   python tools/deps2git/deps2git.py -o .DEPS.git -w <gclientdir>
# where <gcliendir> is the absolute path to the directory containing the
# .gclient file (the parent of 'src').
#
# Then commit .DEPS.git locally (gclient doesn't like dirty trees) and run
#   gclient sync
# Verify the thing happened you wanted. Then revert your .DEPS.git change
# DO NOT CHECK IN CHANGES TO .DEPS.git upstream. It will be automatically
# updated by a bot when you modify this one.
#
# When adding a new dependency, please update the top-level .gitignore file
# to list the dependency's destination directory.

vars = {
  'chromium_git': 'https://chromium.googlesource.com',
  'fuchsia_git': 'https://fuchsia.googlesource.com',
  'skia_git': 'https://skia.googlesource.com',
  'github_git': 'https://github.com',
  'skia_revision': '16d8ec66cdce2f30ce89b87066d3ac7a244c460d',

  # When updating the Dart revision, ensure that all entries that are
  # dependencies of Dart are also updated to match the entries in the
  # Dart SDK's DEPS file for that revision of Dart. The DEPS file for
  # Dart is: https://github.com/dart-lang/sdk/blob/master/DEPS.
  # You can use //tools/dart/create_updated_fluter_deps.py to produce
  # updated revision list of existing dependencies.
  'dart_revision': '1d384ca3ff4aa03bb83471c1c9591a77f1ccd530',

  'dart_args_tag': '0.13.7',
  'dart_async_tag': 'daf66909019d2aaec1721fc39d94ea648a9fdc1d',
  'dart_boringssl_gen_rev': '753224969dbe43dad29343146529727b5066c0f3',
  'dart_boringssl_rev': 'd519bf6be0b447fb80fbc539d4bff4479b5482a2',
  'dart_charcode_tag': 'v1.1.1',
  'dart_collection_tag': '1.13.0',
  'dart_convert_tag': '2.0.1',
  'dart_crypto_tag': '2.0.1',
  'dart_csslib_tag': '0.13.3+1',
  'dart_dart2js_info_tag': '0.5.4+2',
  'dart_glob_tag': '1.1.3',
  'dart_html_tag': '0.13.1',
  'dart_isolate_tag': '1.0.0',
  'dart_logging_tag': '0.11.3+1',
  'dart_observatory_pub_packages_rev': '26aad88f1c1915d39bbcbff3cad589e2402fdcf1',
  'dart_package_config_tag': '1.0.0',
  'dart_path_tag': '1.4.1',
  'dart_plugin_tag': '0.2.0',
  'dart_pub_semver_tag': '1.3.2',
  'dart_root_certificates_rev': 'a4c7c6f23a664a37bc1b6f15a819e3f2a292791a',
  'dart_source_span_tag': '1.4.0',
  'dart_string_scanner_tag': '1.0.1',
  'dart_typed_data_tag': '1.1.3',
  'dart_utf_tag': '0.9.0+3',
  'dart_watcher_tag': '0.9.7+3',
  'dart_yaml_tag': '2.1.12',

  # Build bot tooling for iOS
  'ios_tools_revision': '69b7c1b160e7107a6a98d948363772dc9caea46f',

  'buildtools_revision': 'c8db819853bcf8ce1635a8b7a395820f39b5a9fc',
}

# Only these hosts are allowed for dependencies in this DEPS file.
# If you need to add a new host, contact chrome infrastructure team.
allowed_hosts = [
  'chromium.googlesource.com',
  'fuchsia.googlesource.com',
  'github.com',
  'skia.googlesource.com',
]

deps = {
  'src': 'https://github.com/flutter/buildroot.git' + '@' + 'b0b836db790abff26695a944224a4934480f2f48',

   # Fuchsia compatibility
   #
   # The dependencies in this section should match the layout in the Fuchsia gn
   # build. Eventually, we'll manage these dependencies together with Fuchsia
   # and not have to specific specific hashes.

  'src/lib/ftl':
   Var('fuchsia_git') + '/ftl' + '@' + 'bd6e605513008bc074d0e8022446cea8a06a3ce7',

  'src/lib/tonic':
   Var('fuchsia_git') + '/tonic' + '@' + '32e37b1478ea334ee215bd909cd35b85a6197c65',

  'src/lib/zip':
   Var('fuchsia_git') + '/zip' + '@' + '92dc87ca645fe8e9f5151ef6dac86d8311a7222f',

  'src/third_party/gtest':
   Var('fuchsia_git') + '/third_party/gtest' + '@' + 'c00f82917331efbbd27124b537e4ccc915a02b72',

  'src/third_party/rapidjson':
   Var('fuchsia_git') + '/third_party/rapidjson' + '@' + '9defbb0209a534ffeb3a2b79d5ee440a77407292',

   # Chromium-style
   #
   # As part of integrating with Fuchsia, we should eventually remove all these
   # Chromium-style dependencies.

  'src/buildtools':
   Var('fuchsia_git') + '/buildtools' + '@' +  Var('buildtools_revision'),

  'src/ios_tools':
   Var('chromium_git') + '/chromium/src/ios.git' + '@' + Var('ios_tools_revision'),

  'src/third_party/icu':
   Var('chromium_git') + '/chromium/deps/icu.git' + '@' + 'c3f79166089e5360c09e3053fce50e6e296c3204',

  'src/dart':
   Var('chromium_git') + '/external/github.com/dart-lang/sdk.git' + '@' + Var('dart_revision'),

  'src/third_party/boringssl':
   Var('github_git') + '/dart-lang/boringssl_gen.git' + '@' + Var('dart_boringssl_gen_rev'),

  'src/third_party/boringssl/src':
   'https://boringssl.googlesource.com/boringssl.git' + '@' + Var('dart_boringssl_rev'),

  'src/dart/third_party/observatory_pub_packages':
   Var('chromium_git') + '/external/github.com/dart-lang/observatory_pub_packages' + '@' + Var('dart_observatory_pub_packages_rev'),

  'src/dart/third_party/pkg/args':
   Var('chromium_git') + '/external/github.com/dart-lang/args' + '@' + Var('dart_args_tag'),

  'src/dart/third_party/pkg/async':
   Var('chromium_git') + '/external/github.com/dart-lang/async' + '@' +   Var('dart_async_tag'),

  'src/dart/third_party/pkg/charcode':
   Var('chromium_git') + '/external/github.com/dart-lang/charcode' + '@' + Var('dart_charcode_tag'),

  'src/dart/third_party/pkg/collection':
   Var('chromium_git') + '/external/github.com/dart-lang/collection' + '@' + Var('dart_collection_tag'),

  'src/dart/third_party/pkg/convert':
   Var('chromium_git') + '/external/github.com/dart-lang/convert' + '@' + Var('dart_convert_tag'),

  'src/dart/third_party/pkg/crypto':
   Var('chromium_git') + '/external/github.com/dart-lang/crypto' + '@' + Var('dart_crypto_tag'),

  'src/dart/third_party/pkg/csslib':
   Var('chromium_git') + '/external/github.com/dart-lang/csslib' + '@' + Var('dart_csslib_tag'),

  'src/dart/third_party/pkg/dart2js_info':
   Var('chromium_git') + '/external/github.com/dart-lang/dart2js_info' + '@' + Var('dart_dart2js_info_tag'),

  'src/dart/third_party/pkg/isolate':
   Var('chromium_git') + '/external/github.com/dart-lang/isolate' + '@' + Var('dart_isolate_tag'),

  'src/dart/third_party/pkg/glob':
   Var('chromium_git') + '/external/github.com/dart-lang/glob' + '@' + Var('dart_glob_tag'),

  'src/dart/third_party/pkg/html':
   Var('chromium_git') + '/external/github.com/dart-lang/html' + '@' + Var('dart_html_tag'),

  'src/dart/third_party/pkg/logging':
   Var('chromium_git') + '/external/github.com/dart-lang/logging' + '@' + Var('dart_logging_tag'),

  'src/dart/third_party/pkg_tested/package_config':
   Var('chromium_git') + '/external/github.com/dart-lang/package_config' + '@' + Var('dart_package_config_tag'),

  'src/dart/third_party/pkg/path':
   Var('chromium_git') + '/external/github.com/dart-lang/path' + '@' + Var('dart_path_tag'),

  'src/dart/third_party/pkg/plugin':
   Var('chromium_git') + '/external/github.com/dart-lang/plugin' + '@' + Var('dart_plugin_tag'),

  'src/dart/third_party/pkg/pub_semver':
   Var('chromium_git') + '/external/github.com/dart-lang/pub_semver' + '@' + Var('dart_pub_semver_tag'),

  'src/dart/third_party/pkg/source_span':
   Var('chromium_git') + '/external/github.com/dart-lang/source_span' + '@' + Var('dart_source_span_tag'),

  'src/dart/third_party/pkg/string_scanner':
   Var('chromium_git') + '/external/github.com/dart-lang/string_scanner' + '@' + Var('dart_string_scanner_tag'),

  'src/dart/third_party/pkg/typed_data':
   Var('chromium_git') + '/external/github.com/dart-lang/typed_data' + '@' + Var('dart_typed_data_tag'),

  'src/dart/third_party/pkg/utf':
   Var('chromium_git') + '/external/github.com/dart-lang/utf' + '@' + Var('dart_utf_tag'),

  'src/dart/third_party/pkg/watcher':
   Var('chromium_git') + '/external/github.com/dart-lang/watcher' + '@' + Var('dart_watcher_tag'),

  'src/dart/third_party/pkg/yaml':
   Var('chromium_git') + '/external/github.com/dart-lang/yaml' + '@' + Var('dart_yaml_tag'),

  'src/third_party/root_certificates':
   Var('chromium_git') + '/external/github.com/dart-lang/root_certificates' + '@' + Var('dart_root_certificates_rev'),

  'src/third_party/skia':
   Var('skia_git') + '/skia.git' + '@' +  Var('skia_revision'),

  'src/third_party/libjpeg-turbo':
   Var('skia_git') + '/third_party/libjpeg-turbo.git' + '@' + 'debddedc75850bcdeb8a57258572f48b802a4bb3',

  'src/third_party/gyp':
   Var('chromium_git') + '/external/gyp.git' + '@' + '4801a5331ae62da9769a327f11c4213d32fb0dad',

   # Headers for Vulkan 1.0
   'src/third_party/vulkan':
   Var('github_git') + '/KhronosGroup/Vulkan-Docs.git' + '@' + 'e29c2489e238509c41aeb8c7bce9d669a496344b',
}

recursedeps = [
  'src/buildtools',
]

deps_os = {
  'android': {
    'src/third_party/colorama/src':
     Var('chromium_git') + '/external/colorama.git' + '@' + '799604a1041e9b3bc5d2789ecbd7e8db2e18e6b8',

    'src/third_party/freetype2':
       Var('fuchsia_git') + '/third_party/freetype2' + '@' + 'e23a030e9b43c648249477fdf7bf5305d2cc8f59',
  },
}


hooks = [
  {
    # This clobbers when necessary (based on get_landmines.py). It must be the
    # first hook so that other things that get/generate into the output
    # directory will not subsequently be clobbered.
    'name': 'landmines',
    'pattern': '.',
    'action': [
        'python',
        'src/build/landmines.py',
    ],
  },
  {
    # Update the Windows toolchain if necessary.
    'name': 'win_toolchain',
    'pattern': '.',
    'action': ['python', 'src/build/vs_toolchain.py', 'update'],
  },
  {
    'name': 'download_android_tools',
    'pattern': '.',
    'action': [
        'python',
        'src/tools/android/download_android_tools.py',
    ],
  },
  {
    'name': 'buildtools',
    'pattern': '.',
    'action': [
      'python',
      'src/tools/buildtools/update.py',
      '--ninja',
      '--gn',
      '--toolchain'
    ],
  },
  {
    # Pull dart sdk if needed
    'name': 'dart',
    'pattern': '.',
    'action': ['python', 'src/tools/dart/update.py'],
  },
  {
    # Ensure that we don't accidentally reference any .pyc files whose
    # corresponding .py files have already been deleted.
    'name': 'remove_stale_pyc_files',
    'pattern': 'src/tools/.*\\.py',
    'action': [
        'python',
        'src/tools/remove_stale_pyc_files.py',
        'src/tools',
    ],
  },
]
