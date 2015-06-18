// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:math';
import 'dart:sky';

import 'package:sky/rendering/box.dart';
import 'package:sky/rendering/object.dart';
import 'package:sky/rendering/paragraph.dart';
import 'package:sky/rendering/sky_binding.dart';
import 'package:sky/rendering/stack.dart';
import 'package:sky/theme/colors.dart';

// Material design colors. :p
List<Color> colors = [
  Teal[500],
  Amber[500],
  Purple[500],
  LightBlue[500],
  DeepPurple[500],
  Lime[500],
];

class Dot {
  final Paint _paint;
  double x = 0.0;
  double y = 0.0;
  double radius = 0.0;

  Dot({ Color color }) : _paint = new Paint()..color = color;

  void update(PointerEvent event) {
    x = event.x;
    y = event.y;
    radius = 5 + (95 * event.pressure);
  }

  void paint(RenderObjectDisplayList canvas) {
    canvas.drawCircle(x, y, radius, _paint);
  }
}

class RenderTouchDemo extends RenderBox {
  Map<int, Dot> dots = new Map();

  RenderTouchDemo();

  void handleEvent(Event event, BoxHitTestEntry entry) {
    switch (event.type) {
      case 'pointerdown':
        Color color = colors[event.pointer.remainder(colors.length)];
        dots[event.pointer] = new Dot(color: color)..update(event);
        break;
      case 'pointerup':
        dots.remove(event.pointer);
        break;
      case 'pointercancel':
        dots = new Map();
        break;
      case 'pointermove':
        dots[event.pointer].update(event);
        break;
    }
    markNeedsPaint();
  }

  void performLayout() {
    size = constraints.constrain(Size.infinite);
  }

  void paint(RenderObjectDisplayList canvas) {
    Paint white = new Paint()..color = const Color(0xFFFFFFFF);
    canvas.drawRect(new Rect.fromSize(size), white);
    for (Dot dot in dots.values)
      dot.paint(canvas);
  }
}

void main() {
  var paragraph = new RenderParagraph(new InlineText("Touch me!"));
  var stack = new RenderStack(children: [
    new RenderTouchDemo(),
    paragraph,
  ]);
  // Prevent the RenderParagraph from filling the whole screen so
  // that it doesn't eat events.
  paragraph.parentData..top = 40.0
                      ..left = 20.0;
  new SkyBinding(root: stack);
}
