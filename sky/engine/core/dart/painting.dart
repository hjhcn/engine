// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

part of dart_ui;

typedef void _ImageDecoderCallback(Image result);

void decodeImageFromDataPipe(int handle, _ImageDecoderCallback callback)
    native "decodeImageFromDataPipe";

void decodeImageFromList(Uint8List list, _ImageDecoderCallback callback)
    native "decodeImageFromList";

abstract class DrawLooper extends NativeFieldWrapperClass2 {
}

/// Paint masks for DrawLooperLayerInfo.setPaintBits. These specify which
/// aspects of the layer's paint should replace the corresponding aspects on
/// the draw's paint.
///
/// PaintBits.all means use the layer's paint completely.
/// 0 means ignore the layer's paint... except for colorMode, which is
/// always applied.
class PaintBits {
  static const int style       = 0x1;
  static const int testSkewx   = 0x2;
  static const int pathEffect  = 0x4;
  static const int maskFilter  = 0x8;
  static const int shader      = 0x10;
  static const int colorFilter = 0x20;
  static const int xfermode    = 0x40;
  static const int all         = 0xFFFFFFFF;
}

class DrawLooperLayerInfo extends NativeFieldWrapperClass2 {
  void _constructor() native "DrawLooperLayerInfo_constructor";
  DrawLooperLayerInfo() { _constructor(); }

  void setPaintBits(int bits) native "DrawLooperLayerInfo_setPaintBits";
  void setColorMode(TransferMode mode) native "DrawLooperLayerInfo_setColorMode";
  void setOffset(Offset offset) native "DrawLooperLayerInfo_setOffset";
  void setPostTranslate(bool postTranslate) native "DrawLooperLayerInfo_setPostTranslate";
}

class LayerDrawLooperBuilder extends NativeFieldWrapperClass2 {
  void _constructor() native "LayerDrawLooperBuilder_constructor";
  LayerDrawLooperBuilder() { _constructor(); }

  DrawLooper build() native "LayerDrawLooperBuilder_build";
  void addLayerOnTop(DrawLooperLayerInfo info, Paint paint) native "LayerDrawLooperBuilder_addLayerOnTop";
}
