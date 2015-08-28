// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'package:sky/widgets.dart';

import '../resources/display_list.dart';

class FiddleApp extends App {

  bool arbitrarySetting = true;

  void toggle() {
    setState(() {
      arbitrarySetting = !arbitrarySetting;
    });
  }

  Widget buildFlex1() {
    return new Row([
      new Flexible(child: new Container(
        decoration: new BoxDecoration(
          backgroundColor: const Color(0xFF00FFFF)
        )
      )),
      new RaisedButton(child: new Text('TAP ME TO CHANGE THE BACKGROUND COLOUR'), onPressed: toggle)
    ]);
  }

  Widget buildFlex2() {
    return new Row([
      new Flexible(child: new Container(
        key: const ValueKey<String>('something-else'),
        decoration: new BoxDecoration(
          backgroundColor: const Color(0xEFFF9F00)
        )
      )),
      new RaisedButton(child: new Text('PRESS ME TO CHANGE IT BACK'), onPressed: toggle)
    ]);
  }

  Widget buildStack1() {
    return new Stack([
      new Positioned(child: new Container(
        decoration: new BoxDecoration(
          backgroundColor: const Color(0xFF00FFFF)
        )
      )),
      new RaisedButton(child: new Text('TAP ME TO CHANGE THE BACKGROUND COLOUR'), onPressed: toggle)
    ]);
  }

  Widget buildStack2() {
    return new Stack([
      new Positioned(child: new Container(
        key: const ValueKey<String>('something-else'),
        decoration: new BoxDecoration(
          backgroundColor: const Color(0xEFFF9F00)
        )
      )),
      new RaisedButton(child: new Text('PRESS ME TO CHANGE IT BACK'), onPressed: toggle)
    ]);
  }

  Widget build() {
    return new Column([
        new SizedBox(
          key: const ValueKey<String>('flex-example'),
          height: 250.0,
          child: arbitrarySetting ? buildFlex1() : buildFlex2()
        ),
        new SizedBox(
          key: const ValueKey<String>('stack-example'),
          height: 250.0,
          child: arbitrarySetting ? buildStack1() : buildStack2()
        )
      ],
      alignItems: FlexAlignItems.stretch
    );
  }
}

main() async {
  TestRenderView renderViewOverride = new TestRenderView();
  FiddleApp app = new FiddleApp();
  runApp(app, renderViewOverride: renderViewOverride);
  await renderViewOverride.checkFrame();
  app.toggle();
  await renderViewOverride.checkFrame();
  app.toggle();
  await renderViewOverride.checkFrame();
  renderViewOverride.endTest();
}
