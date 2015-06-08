// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:sky';
import 'package:sky/framework/rendering/box.dart';

class RenderSolidColorBox extends RenderDecoratedBox {
  final Size desiredSize;
  final Color backgroundColor;

  RenderSolidColorBox(Color backgroundColor, { this.desiredSize: Size.infinite })
      : backgroundColor = backgroundColor,
        super(decoration: new BoxDecoration(backgroundColor: backgroundColor));

  double getMinIntrinsicWidth(BoxConstraints constraints) {
    return constraints.constrainWidth(desiredSize.width);
  }

  double getMaxIntrinsicWidth(BoxConstraints constraints) {
    return constraints.constrainWidth(desiredSize.width);
  }

  double getMinIntrinsicHeight(BoxConstraints constraints) {
    return constraints.constrainHeight(desiredSize.height);
  }

  double getMaxIntrinsicHeight(BoxConstraints constraints) {
    return constraints.constrainHeight(desiredSize.height);
  }

  void performLayout() {
    size = constraints.constrain(desiredSize);
  }

  void handlePointer(PointerEvent event) {
    if (event.type == 'pointerdown')
      decoration = new BoxDecoration(backgroundColor: const Color(0xFFFF0000));
    else if (event.type == 'pointerup')
      decoration = new BoxDecoration(backgroundColor: backgroundColor);
  }
}