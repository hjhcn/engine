// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:math' as math;

import 'package:sky/widgets.dart';

class MixedViewportApp extends App {

  MixedViewportLayoutState layoutState = new MixedViewportLayoutState();
  List<double> lengths = <double>[];
  double offset = 0.0;

  static const double kMaxLength = 100.0;

  static math.Random rand = new math.Random();

  void addBox() {
    lengths.add(rand.nextDouble() * kMaxLength);
    updateEnabledState();
  }

  void removeBox(int index) {
    lengths.removeAt(index);
    updateEnabledState();
  }

  void goUp() {
    offset -= 9.9;
    updateEnabledState();
  }

  void goDown() {
    offset += 20.45;
    updateEnabledState();
  }

  bool enabledDown = true;
  bool enabledUp = true;
  bool enabledAdd = true;
  bool enabledRemove = false;
  void updateEnabledState() {
    setState(() {
      enabledUp = offset > -100.0;
      enabledDown = offset < lengths.fold(0.0, (double result, double len) => result + len) + 100.0;
      enabledAdd = true;
      enabledRemove = lengths.length > 0;
    });
  }

  Widget build() {
    return new Theme(
      data: new ThemeData.light(),
      child: new Scaffold(
        toolbar: new ToolBar(
            center: new Text('Block Viewport Demo')),
        body: new Material(
          type: MaterialType.canvas,
          child: new Column([
              new Container(
                padding: new EdgeDims.symmetric(horizontal: 8.0, vertical: 25.0),
                child: new Row([
                    new RaisedButton(
                      enabled: enabledAdd,
                      child: new Text('ADD'),
                      onPressed: addBox
                    ),
                    new RaisedButton(
                      enabled: enabledUp,
                      child: new Text('UP'),
                      onPressed: goUp
                    ),
                    new RaisedButton(
                      enabled: enabledDown,
                      child: new Text('DOWN'),
                      onPressed: goDown
                    )
                  ],
                  justifyContent: FlexJustifyContent.spaceAround
                )
              ),
              new Flexible(
                child: new Container(
                  margin: new EdgeDims.all(8.0),
                  decoration: new BoxDecoration(
                    border: new Border.all(color: new Color(0xFF000000))
                  ),
                  padding: new EdgeDims.all(16.0),
                  child: new MixedViewport(
                    builder: builder,
                    startOffset: offset,
                    token: lengths.length,
                    layoutState: layoutState
                  )
                )
              ),
            ],
            justifyContent: FlexJustifyContent.spaceBetween
          )
        )
      )
    );
  }

  Widget builder(int index) {
    if (index >= lengths.length)
      return null;
    return new GestureDetector(
      key: new ValueKey<double>(lengths[index]),
      onTap: () => removeBox(index),
      child: new Container(
        decoration: new BoxDecoration(
          backgroundColor: new Color((0xFF000000 + 0xFFFFFF * lengths[index] / kMaxLength).round())
        ),
        height: lengths[index] + 12.0
      )
    );
  }

}

void main() {
  runApp(new MixedViewportApp());
}
