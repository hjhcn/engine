// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:async';

import 'package:sky/widgets/widget.dart';

import '../../packages/sky/example/widgets/tabs.dart';
import '../resources/display_list.dart';

main() async {
  TestRenderView testRenderView = new TestRenderView();
  TabbedNavigatorApp app = new TabbedNavigatorApp();
  runApp(app, renderViewOverride: testRenderView);
  await testRenderView.checkFrame();
  testRenderView.endTest();
}
