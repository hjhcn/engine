// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:sky' as sky;

import 'package:sky/animation.dart';
import 'package:sky/rendering.dart';
import 'package:sky/widgets.dart';

import '../rendering/solid_color_box.dart';

// Solid colour, RenderObject version
void addFlexChildSolidColor(RenderFlex parent, sky.Color backgroundColor, { int flex: 0 }) {
  RenderSolidColorBox child = new RenderSolidColorBox(backgroundColor);
  parent.add(child);
  child.parentData.flex = flex;
}

// Solid colour, Widget version
class Rectangle extends Component {
  Rectangle(this.color, { Key key }) : super(key: key);
  final Color color;
  Widget build() {
    return new Flexible(
      child: new Container(
        decoration: new BoxDecoration(backgroundColor: color)
      )
    );
  }
}

Widget builder() {
  return new Container(
    height: 300.0,
    child: new Column([
        new Rectangle(const Color(0xFF00FFFF)),
        new Container(
          padding: new EdgeDims.all(10.0),
          margin: new EdgeDims.all(10.0),
          decoration: new BoxDecoration(backgroundColor: const Color(0xFFCCCCCC)),
          child: new RaisedButton(
            child: new Row([
              new NetworkImage(src: "https://www.dartlang.org/logos/dart-logo.png"),
              new Text('PRESS ME'),
            ]),
            onPressed: () => print("Hello World")
          )
        ),
        new Rectangle(const Color(0xFFFFFF00)),
      ],
      justifyContent: FlexJustifyContent.spaceBetween
    )
  );
}

Duration timeBase;
RenderTransform transformBox;

void rotate(Duration timeStamp) {
  if (timeBase == null)
    timeBase = timeStamp;
  double delta = (timeStamp - timeBase).inMicroseconds.toDouble() / Duration.MICROSECONDS_PER_SECOND; // radians

  transformBox.setIdentity();
  transformBox.translate(transformBox.size.width / 2.0, transformBox.size.height / 2.0);
  transformBox.rotateZ(delta);
  transformBox.translate(-transformBox.size.width / 2.0, -transformBox.size.height / 2.0);
}

void main() {
  // Because we're going to use Widgets, we want to initialise its
  // FlutterBinding, not use the default one. We don't really need to do
  // this, because RenderBoxToWidgetAdapter does it for us, but
  // it's good practice in case we happen to not have a
  // RenderBoxToWidgetAdapter in our tree at startup, or in case we
  // want a renderViewOverride.
  WidgetFlutterBinding.initWidgetFlutterBinding();

  RenderProxyBox proxy = new RenderProxyBox();
  new RenderBoxToWidgetAdapter(proxy, builder); // adds itself to proxy

  RenderFlex flexRoot = new RenderFlex(direction: FlexDirection.vertical);
  addFlexChildSolidColor(flexRoot, const sky.Color(0xFFFF00FF), flex: 1);
  flexRoot.add(proxy);
  addFlexChildSolidColor(flexRoot, const sky.Color(0xFF0000FF), flex: 1);

  transformBox = new RenderTransform(child: flexRoot, transform: new Matrix4.identity());
  RenderPadding root = new RenderPadding(padding: new EdgeDims.all(80.0), child: transformBox);

  FlutterBinding.instance.renderView.child = root;
  scheduler.addPersistentFrameCallback(rotate);
}
