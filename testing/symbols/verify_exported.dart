import 'dart:convert';
import 'dart:io';

import 'package:path/path.dart' as p;

// This script verifies that the release binaries only export the expected
// symbols.
//
// Android binaries (libflutter.so) should only export one symbol "JNI_OnLoad"
// of type "T".
//
// iOS binaries (Flutter.framework/Flutter) should only export Objective-C
// Symbols from the Flutter namespace. These are either of type
// "(__DATA,__common)" or "(__DATA,__objc_data)".

/// Takes the path to the out directory as argument.
void main(List<String> arguments) {
  assert(arguments.length == 1);
  final String outPath = arguments.first;
  String platform;
  if (Platform.isLinux) {
    platform = 'linux-x64';
  } else if (Platform.isMacOS) {
    platform = 'mac-x64';
  } else {
    throw new UnimplementedError('Script only support running on Linux or MacOS.');
  }
  final String nmPath = p.join(p.dirname(outPath), 'buildtools', platform, 'clang', 'bin', 'llvm-nm');
  assert(new Directory(outPath).existsSync());

  final Iterable<String> releaseBuilds = new Directory(outPath).listSync()
      .where((FileSystemEntity entity) => entity is Directory)
      .map((FileSystemEntity dir) => p.basename(dir.path))
      .where((String s) => s.contains('_release'));

  final Iterable<String> iosReleaseBuilds = releaseBuilds
      .where((String s) => s.startsWith('ios_'));
  final Iterable<String> androidReleaseBuilds = releaseBuilds
      .where((String s) => s.startsWith('android_'));

  int failures = 0;
  failures += _checkIos(outPath, nmPath, iosReleaseBuilds);
  failures += _checkAndroid(outPath, nmPath, androidReleaseBuilds);
  // TODO(goderbauer): Return failing exit code on failure once it works on bots.
  print('Failures: $failures'); // exit(failures);
}

int _checkIos(String outPath, String nmPath, Iterable<String> builds) {
  int failures = 0;
  for (String build in builds) {
    final String libFlutter = p.join(outPath, build, 'Flutter.framework', 'Flutter');
    final String stdout = Process.runSync(nmPath, <String>['-gUm', libFlutter]).stdout;
    print('+++ DEBUG: stdout of nm +++');
    print(stdout);
    print('+++ END: stdout of nm +++');
    final Iterable<NmEntry> unexpectedEntries = NmEntry.parse(stdout).where((NmEntry entry) {
      return !((entry.type == '(__DATA,__common)' && entry.name.startsWith('_Flutter'))
          || (entry.type == '(__DATA,__objc_data)'
              && (entry.name.startsWith('_OBJC_METACLASS_\$_Flutter') || entry.name.startsWith('_OBJC_CLASS_\$_Flutter'))));
    });
    if (unexpectedEntries.isNotEmpty) {
      print('ERROR: $libFlutter exports unexpected symbols:');
      print(unexpectedEntries.fold<String>('', (String previous, NmEntry entry) {
        return '${previous == '' ? '' : '$previous\n'}     ${entry.type} ${entry.name}';
      }));
      failures++;
    } else {
      print('OK: $libFlutter');
    }
  }
  return failures;
}

int _checkAndroid(String outPath, String nmPath, Iterable<String> builds) {
  int failures = 0;
  for (String build in builds) {
    final String libFlutter = p.join(outPath, build, 'libflutter.so');
    final String stdout = Process.runSync(nmPath, <String>['-gU', libFlutter]).stdout;
    print('+++ DEBUG: stdout of nm +++');
    print(stdout);
    print('+++ END: stdout of nm +++');
    final Iterable<NmEntry> entries = NmEntry.parse(stdout);
    if (entries.isEmpty) {
      print('ERROR: $libFlutter exports no symbol');
      print(' Expected exactly one symbol "JNI_OnLoad" of type "T", but got none.');
      failures++;
    } else if (entries.length > 1 || entries.first.type != 'T' || entries.first.name != 'JNI_OnLoad') {
      print('ERROR: $libFlutter exports unexpected symbols.');
      print('  Expected exactly one symbol "JNI_OnLoad" of type "T", but got instead:');
      print(entries.fold<String>('', (String previous, NmEntry entry) {
        return '${previous == '' ? '' : '$previous\n'}     ${entry.type} ${entry.name}';
      }));
      failures++;
    } else {
      print('OK: $libFlutter');
    }
  }
  return failures;
}

class NmEntry {
  NmEntry._(this.address, this.type, this.name);

  final String address;
  final String type;
  final String name;

  static Iterable<NmEntry> parse(String stdout) {
    return LineSplitter.split(stdout).map((String line) {
      final List<String> parts = line.split(' ');
      return new NmEntry._(parts[0], parts[1], parts.last);
    });
  }
}
