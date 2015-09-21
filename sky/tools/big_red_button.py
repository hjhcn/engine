#!/usr/bin/env python
# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# See https://github.com/domokit/sky_engine/wiki/Release-process

import argparse
import os
import subprocess
import sys
import distutils.util


CONFIRM_MESSAGE = """This tool is destructive and will revert your current branch to
upstream/master among other things.  Are you sure you wish to continue?"""
DRY_RUN = False


def run(cwd, args):
    print 'RUNNING:', ' '.join(args), 'IN:', cwd
    if DRY_RUN:
        return
    subprocess.check_call(args, cwd=cwd)


def confirm(prompt):
    user_input = raw_input("%s (y/N) " % prompt)
    try:
        return distutils.util.strtobool(user_input) == 1
    except ValueError:
        return False


def git_revision(cwd):
    return subprocess.check_output([
        'git', 'rev-parse', 'HEAD',
    ], cwd=cwd).strip()


class Artifact(object):
    def __init__(self, category, name):
        self.category = category
        self.name = name


GS_URL = 'gs://mojo/sky/%(category)s/%(config)s/%(commit_hash)s/%(name)s'


ARTIFACTS = {
    'android-arm': [
        Artifact('shell', 'SkyShell.apk'),
        Artifact('viewer', 'sky_viewer.mojo'),
        Artifact('viewer', 'libsky_viewer_library.so'),
    ],
    'linux-x64': [
        Artifact('shell', 'icudtl.dat'),
        Artifact('shell', 'sky_shell'),
        Artifact('shell', 'sky_snapshot'),
        Artifact('viewer', 'sky_viewer.mojo'),
        Artifact('viewer', 'libsky_viewer_library.so'),
    ]
}


def upload_artifacts(dist_root, config, commit_hash):
    for artifact in ARTIFACTS[config]:
        src = os.path.join(artifact.category, artifact.name)
        dst = GS_URL % {
            'category': artifact.category,
            'config': config,
            'commit_hash': commit_hash,
            'name': artifact.name,
        }
        z = ','.join([ 'mojo', 'dat' ])
        run(dist_root, ['gsutil', 'cp', '-z', z, src, dst])


def main():
    parser = argparse.ArgumentParser(description='Deploy!')
    parser.add_argument('sky_engine_root', help='Path to sky_engine/src')
    parser.add_argument('--stage-two', action='store_true', default=False,
        help='Publish the sky package')
    parser.add_argument('--dry-run', action='store_true', default=False,
        help='Just print commands w/o executing.')
    parser.add_argument('--no-pub-publish', dest='publish',
        action='store_false', default=True, help='Skip pub publish step.')
    args = parser.parse_args()

    global DRY_RUN
    DRY_RUN = args.dry_run

    if not args.dry_run and not confirm(CONFIRM_MESSAGE):
        print "Aborted."
        return 1

    sky_engine_root = os.path.abspath(os.path.expanduser(args.sky_engine_root))

    # Derived paths:
    dart_sdk_root = os.path.join(sky_engine_root, 'third_party/dart-sdk/dart-sdk')
    pub_path = os.path.join(dart_sdk_root, 'bin/pub')
    android_dist_root = os.path.join(sky_engine_root, 'out/android_Release/dist')
    linux_dist_root = os.path.join(sky_engine_root, 'out/Release/dist')
    sky_package_root = os.path.join(sky_engine_root, 'sky/packages/sky')
    sky_engine_package_root = os.path.join(android_dist_root, 'packages/sky_engine/sky_engine')
    sky_services_package_root = os.path.join(android_dist_root, 'packages/sky_services/sky_services')
    sky_engine_revision_file = os.path.join(sky_engine_package_root, 'lib', 'REVISION')

    run(sky_engine_root, ['git', 'fetch', 'upstream'])
    run(sky_engine_root, ['git', 'reset', 'upstream/master', '--hard'])

    def stage_one():
        run(sky_engine_root, ['gclient', 'sync'])

        commit_hash = git_revision(sky_engine_root)

        run(sky_engine_root, ['sky/tools/gn', '--android', '--release'])
        run(sky_engine_root, ['ninja', '-C', 'out/android_Release', ':dist'])

        run(sky_engine_root, ['sky/tools/gn', '--release'])
        run(sky_engine_root, ['ninja', '-C', 'out/Release', ':dist'])

        with open(sky_engine_revision_file, 'w') as stream:
            stream.write(commit_hash)

        upload_artifacts(android_dist_root, 'android-arm', commit_hash)
        upload_artifacts(linux_dist_root, 'linux-x64', commit_hash)

        if args.publish:
            run(sky_engine_package_root, [pub_path, 'publish', '--force'])
            run(sky_services_package_root, [pub_path, 'publish', '--force'])

    def stage_two():
        if args.publish:
            run(sky_package_root, [pub_path, 'publish', '--force'])

    if args.stage_two:
        stage_two()
    else:
        stage_one()


if __name__ == '__main__':
    sys.exit(main())
