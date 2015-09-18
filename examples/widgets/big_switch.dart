// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'package:sky/material.dart';
import 'package:sky/widgets.dart';

class BigSwitchApp extends App {
  bool _value = false;

  void _handleOnChanged(bool value) {
    setState(() {
      _value = value;
    });
  }

  Widget build() {
    Matrix4 scale = new Matrix4.identity();
    scale.scale(5.0, 5.0);
    return new Container(
        child: new Switch(value: _value, onChanged: _handleOnChanged),
        padding: new EdgeDims.all(20.0),
        transform: scale,
        decoration: new BoxDecoration(
          backgroundColor: Colors.teal[600]
        )
    );
  }
}

void main() {
  runApp(new BigSwitchApp());
}
