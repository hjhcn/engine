// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:sky';
import 'dart:math' as math;
import 'package:sky/framework/app.dart';
import 'package:sky/framework/rendering/box.dart';
import 'package:sky/framework/rendering/block.dart';
import 'package:sky/framework/rendering/flex.dart';
import 'package:sky/framework/rendering/object.dart';
import 'package:sky/framework/rendering/paragraph.dart';
import '../lib/solid_color_box.dart';

AppView app;

// Attempts to draw
// http://www.w3.org/TR/2015/WD-css-flexbox-1-20150514/images/flex-pack.svg
void main() {
  var table = new RenderFlex(direction: FlexDirection.vertical);

  void addRow(FlexJustifyContent justify) {
    RenderParagraph paragraph = new RenderParagraph(text: "${justify}");
    table.add(new RenderPadding(child: paragraph, padding: new EdgeDims.only(top: 20.0)));
    var row = new RenderFlex(direction: FlexDirection.horizontal);
    row.add(new RenderSolidColorBox(const Color(0xFFFFCCCC), desiredSize: new Size(80.0, 60.0)));
    row.add(new RenderSolidColorBox(const Color(0xFFCCFFCC), desiredSize: new Size(64.0, 60.0)));
    row.add(new RenderSolidColorBox(const Color(0xFFCCCCFF), desiredSize: new Size(160.0, 60.0)));
    row.justifyContent = justify;
    table.add(row);
    row.parentData.flex = 1;
  }

  addRow(FlexJustifyContent.flexStart);
  addRow(FlexJustifyContent.flexEnd);
  addRow(FlexJustifyContent.center);
  addRow(FlexJustifyContent.spaceBetween);
  addRow(FlexJustifyContent.spaceAround);

  RenderDecoratedBox root = new RenderDecoratedBox(
    decoration: new BoxDecoration(backgroundColor: const Color(0xFFFFFFFF)),
    child: new RenderPadding(child: table, padding: new EdgeDims.symmetric(vertical: 50.0))
  );

  app = new AppView(root);
}
