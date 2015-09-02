// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'package:sky/src/widgets/basic.dart';
import 'package:sky/src/widgets/dialog.dart';

import '../resources/display_list.dart';

main() async {
  WidgetTester tester = new WidgetTester();

  await tester.test(() {
    return new Dialog(
      title: new Text("I am a title"),
      content: new Text("I am some content\nwith a line break"),
      actions: [new Text("I am some actions")]
    );
  });

  tester.endTest();
}
