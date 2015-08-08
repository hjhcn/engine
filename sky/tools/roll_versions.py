#!/usr/bin/env python
# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import os
import subprocess
import yaml
import xml.etree.ElementTree as ET


PUBSPECS = [
    'sky/packages/sky/pubspec.yaml',
    'sky/packages/sky_engine/pubspec.yaml',
    'sky/packages/sky_services/pubspec.yaml',
]

MANIFESTS = [
    'examples/demo_launcher/apk/AndroidManifest.xml',
]

MANIFEST_PREFACE = '''<?xml version="1.0" encoding="utf-8"?>
<!-- Copyright 2015 The Chromium Authors. All rights reserved.
     Use of this source code is governed by a BSD-style license that can be
     found in the LICENSE file.
 -->
'''


def increment_version(version):
    pieces = version.split('.')
    pieces[-1] = str(int(pieces[-1]) + 1)
    return '.'.join(pieces)


def sort_dict(unsorted):
    sorted_dict = collections.OrderedDict()
    for key in sorted(unsorted.keys()):
        sorted_dict[key] = unsorted[key]
    return sorted_dict


def count_commits(start, end):
    return subprocess.check_output([
        'git', 'rev-list', '%s...%s' % (start, end)]).count('\n')


def last_commit_to(file_path):
    return subprocess.check_output(['git', 'log', '-1', '--format=%h', file_path]).strip()


def update_pubspec(pubspec):
    # TODO(eseidel): This does not prserve any manual sort-order of the yaml.
    with open(pubspec, 'r') as stream:
        spec = yaml.load(stream)
        old_version = spec['version']
        spec['version'] = increment_version(old_version)
        print "%20s  %6s => %6s" % (spec['name'], old_version, spec['version'])

    with open(pubspec, 'w') as stream:
        yaml.dump(spec, stream=stream, default_flow_style=False)
    return spec['version']


def update_changelog(changelog, pubspec, version):
    old = last_commit_to(pubspec)
    new = last_commit_to('.')
    url = "https://github.com/domokit/mojo/compare/%s...%s" % (old, new)
    count = count_commits(old, new)
    message = """## %s

  - %s changes: %s

""" % (version, count, url)
    prepend_to_file(message, changelog)


def prepend_to_file(to_prepend, filepath):
    with open(filepath, 'r+') as f:
        content = f.read()
        f.seek(0, 0)
        f.write(to_prepend + content)


def update_manifest(manifest):
    VERSION_CODE = '{http://schemas.android.com/apk/res/android}versionCode'
    VERSION_NAME = '{http://schemas.android.com/apk/res/android}versionName'
    tree = ET.parse(manifest)
    root = tree.getroot()
    package_name = root.get('package')
    old_code = root.get(VERSION_CODE)
    old_name = root.get(VERSION_NAME)
    root.set(VERSION_CODE, increment_version(old_code))
    root.set(VERSION_NAME, increment_version(old_name))
    print "%20s  %6s (%s) => %6s (%s)" % (package_name, old_name, old_code,
        root.get(VERSION_NAME), root.get(VERSION_CODE))
    # TODO(eseidel): This isn't smart enough to wrap/intent multi-attribute
    # elements like <manifest> as is the typical AndroidManifiest.xml style
    # we could write our own custom prettyprinter to do that?
    tree.write(manifest)
    prepend_to_file(MANIFEST_PREFACE, manifest)


def main():
    # Should chdir to the root directory.

    print 'Pub packages:'
    for pubspec in PUBSPECS:
        new_version = update_pubspec(pubspec)
        changelog = os.path.join(os.path.dirname(pubspec), 'CHANGELOG.md')
        update_changelog(changelog, pubspec, new_version)

    # TODO(eseidel): Without this ET uses 'ns0' for 'android' which is wrong.
    ET.register_namespace('android', 'http://schemas.android.com/apk/res/android')

    print 'APKs:'
    for manifest in MANIFESTS:
        update_manifest(manifest)


if __name__ == '__main__':
    main()
