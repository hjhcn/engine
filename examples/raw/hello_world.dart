// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import "dart:math";
import 'dart:sky';

Picture draw(int a, int r, int g, int b) {
  double width = view.width;
  double height = view.height;

  PictureRecorder recorder = new PictureRecorder(width, height);
  double radius = min(width, height) * 0.45;

  Paint paint = new Paint()..color = new Color.fromARGB(a, r, g, b);
  recorder.drawCircle(width / 2, height / 2, radius, paint);
  return recorder.endRecording();
}

bool handleEvent(Event event) {
  if (event.type == "pointerdown") {
    view.picture = draw(255, 0, 0, 255);
    view.scheduleFrame();
    return true;
  }

  if (event.type == "pointerup") {
    view.picture = draw(255, 0, 255, 0);
    view.scheduleFrame();
    return true;
  }

  if (event.type == "back") {
    print("Pressed back button.");
    return true;
  }

  return false;
}

void main() {
  print("Hello, world");
  view.picture = draw(255, 0, 255, 0);
  view.scheduleFrame();

  view.setEventCallback(handleEvent);
}
