// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'dart:sky' as sky;
import 'dart:math' as math;
import 'dart:typed_data';

void beginFrame(double timeStamp) {
  sky.PictureRecorder context = new sky.PictureRecorder(sky.view.width, 200.0);

  sky.Paint paint = new sky.Paint();
  sky.Point mid = new sky.Point(context.width / 2.0, context.height / 2.0);
  double radius = math.min(mid.x, mid.y);

  context.drawPaint(new sky.Paint()..color = const sky.Color(0xFFFFFFFF));

  context.save();

  context.translate(-mid.x/2.0, context.height*2.0);
  context.clipRect(
      new sky.Rect.fromLTRB(0.0, -context.height, context.width, radius));

  context.translate(mid.x, mid.y);
  paint.color = const sky.Color.fromARGB(128, 255, 0, 255);
  context.rotate(math.PI/4.0);

  sky.Gradient yellowBlue = new sky.Gradient.linear(
      [new sky.Point(-radius, -radius), new sky.Point(0.0, 0.0)],
      [const sky.Color(0xFFFFFF00), const sky.Color(0xFF0000FF)]);
  context.drawRect(new sky.Rect.fromLTRB(-radius, -radius, radius, radius),
                   new sky.Paint()..setShader(yellowBlue));

  // Scale x and y by 0.5.
  var scaleMatrix = new Float32List.fromList([
      0.5, 0.0, 0.0, 0.0,
      0.0, 0.5, 0.0, 0.0,
      0.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 1.0,
  ]);
  context.concat(scaleMatrix);
  paint.color = const sky.Color.fromARGB(128, 0, 255, 0);
  context.drawCircle(0.0, 0.0, radius, paint);

  context.restore();

  context.translate(0.0, 50.0);
  var builder = new sky.LayerDrawLooperBuilder()
      ..addLayerOnTop(
          new sky.DrawLooperLayerInfo()
            ..setOffset(const sky.Point(150.0, 0.0))
            ..setColorMode(sky.TransferMode.src)
            ..setPaintBits(sky.PaintBits.all),
          (sky.Paint layerPaint) {
        layerPaint.color = const sky.Color.fromARGB(128, 255, 255, 0);
        layerPaint.setColorFilter(new sky.ColorFilter.mode(
            const sky.Color.fromARGB(128, 0, 0, 255), sky.TransferMode.srcIn));
        layerPaint.setMaskFilter(new sky.MaskFilter.blur(
            sky.BlurStyle.normal, 3.0, highQuality: true));
      })
      ..addLayerOnTop(
          new sky.DrawLooperLayerInfo()
            ..setOffset(const sky.Point(75.0, 75.0))
            ..setColorMode(sky.TransferMode.src)
            ..setPaintBits(sky.PaintBits.shader),
          (sky.Paint layerPaint) {
        sky.Gradient redYellow = new sky.Gradient.radial(
            new sky.Point(0.0, 0.0), radius/3.0,
            [const sky.Color(0xFFFFFF00), const sky.Color(0xFFFF0000)],
            null, sky.TileMode.mirror);
        layerPaint.setShader(redYellow);
        // Since we're don't set sky.PaintBits.maskFilter, this has no effect.
        layerPaint.setMaskFilter(new sky.MaskFilter.blur(
            sky.BlurStyle.normal, 50.0, highQuality: true));
      })
      ..addLayerOnTop(
          new sky.DrawLooperLayerInfo()..setOffset(const sky.Point(225.0, 75.0)),
          (sky.Paint layerPaint) {
        // Since this layer uses a DST color mode, this has no effect.
        layerPaint.color = const sky.Color.fromARGB(128, 255, 0, 0);
      });
  paint.setDrawLooper(builder.build());
  context.drawCircle(0.0, 0.0, radius, paint);

  sky.view.picture = context.endRecording();
}

void main() {
  sky.view.setBeginFrameCallback(beginFrame);
  sky.view.scheduleFrame();
}
