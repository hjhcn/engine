// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

part of dart_ui;

abstract class Image extends NativeFieldWrapperClass2 {
  int get width native "Image_width";
  int get height native "Image_height";

  void dispose() native "Image_dispose";
}

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

/// Blur styles. These mirror SkBlurStyle and must be kept in sync.
enum BlurStyle {
  normal,  /// Fuzzy inside and outside.
  solid,  /// Solid inside, fuzzy outside.
  outer,  /// Nothing inside, fuzzy outside.
  inner,  /// Fuzzy inside, nothing outside.
}

// Convert constructor parameters to the SkBlurMaskFilter::BlurFlags type.
int _makeBlurFlags(bool ignoreTransform, bool highQuality) {
  int flags = 0;
  if (ignoreTransform)
    flags |= 0x01;
  if (highQuality)
    flags |= 0x02;
  return flags;
}

class MaskFilter extends NativeFieldWrapperClass2 {
  void _constructor(int style, double sigma, int flags) native "MaskFilter_constructor";
  MaskFilter.blur(BlurStyle style, double sigma,
                  {bool ignoreTransform: false, bool highQuality: false}) {
    _constructor(style.index, sigma, _makeBlurFlags(ignoreTransform, highQuality));
  }
}

class ColorFilter extends NativeFieldWrapperClass2 {
  void _constructor(Color color, TransferMode transferMode) native "ColorFilter_constructor";
  ColorFilter.mode(Color color, TransferMode transferMode) {
    _constructor(color, transferMode);
  }
}

abstract class Shader extends NativeFieldWrapperClass2 {
}

/// Defines what happens at the edge of the gradient.
enum TileMode {
  /// Edge is clamped to the final color.
  clamp,
  /// Edge is repeated from first color to last.
  repeated,
  /// Edge is mirrored from last color to first.
  mirror
}

void _validateColorStops(List<Color> colors, List<double> colorStops) {
  if (colorStops != null && (colors == null || colors.length != colorStops.length)) {
    throw new ArgumentError(
        "[colors] and [colorStops] parameters must be equal length.");
  }
}

class Gradient extends Shader {
  void _constructor() native "Gradient_constructor";
  void _initLinear(List<Point> endPoints, List<Color> colors, List<double> colorStops, int tileMode) native "Gradient_initLinear";
  void _initRadial(Point center, double radius, List<Color> colors, List<double> colorStops, int tileMode) native "Gradient_initRadial";

  /// Creates a linear gradient from [endPoint[0]] to [endPoint[1]]. If
  /// [colorStops] is provided, [colorStops[i]] is a number from 0 to 1 that
  /// specifies where [color[i]] begins in the gradient.
  // TODO(mpcomplete): Maybe pass a list of (color, colorStop) pairs instead?
  Gradient.linear(List<Point> endPoints,
                  List<Color> colors,
                  [List<double> colorStops = null,
                  TileMode tileMode = TileMode.clamp]) {
    _constructor();
    if (endPoints == null || endPoints.length != 2)
      throw new ArgumentError("Expected exactly 2 [endPoints].");
    _validateColorStops(colors, colorStops);
    _initLinear(endPoints, colors, colorStops, tileMode.index);
  }

  /// Creates a radial gradient centered at [center] that ends at [radius]
  /// distance from the center. If [colorStops] is provided, [colorStops[i]] is
  /// a number from 0 to 1 that specifies where [color[i]] begins in the
  /// gradient.
  Gradient.radial(Point center,
                  double radius,
                  List<Color> colors,
                  [List<double> colorStops = null,
                  TileMode tileMode = TileMode.clamp]) {
    _constructor();
    _validateColorStops(colors, colorStops);
    _initRadial(center, radius, colors, colorStops, tileMode.index);
  }
}

class ImageShader extends Shader {
  void _constructor() native "ImageShader_constructor";
  void _initWithImage(Image image, int tmx, int tmy, Float64List matrix4) native "ImageShader_initWithImage";

  ImageShader(Image image, TileMode tmx, TileMode tmy, Float64List matrix4) {
    if (image == null)
      throw new ArgumentError("[image] argument cannot be null");
    if (tmx == null)
      throw new ArgumentError("[tmx] argument cannot be null");
    if (tmy == null)
      throw new ArgumentError("[tmy] argument cannot be null");
    if (matrix4 == null)
      throw new ArgumentError("[matrix4] argument cannot be null");
    _initWithImage(image, tmx.index, tmy.index, matrix4);
  }
}

/// Defines how a list of points is interpreted when drawing a set of triangles.
/// See Skia or OpenGL documentation for more details.
enum VertexMode {
  triangles,
  triangleStrip,
  triangleFan,
}

class Canvas extends NativeFieldWrapperClass2 {
  void _constructor(PictureRecorder recorder, Rect bounds) native "Canvas_constructor";
  Canvas(PictureRecorder recorder, Rect bounds) {
    if (recorder == null)
      throw new ArgumentError("[recorder] argument cannot be null.");
    if (recorder.isRecording)
      throw new ArgumentError("You must call endRecording() before reusing a PictureRecorder to create a new Canvas object.");
    _constructor(recorder, bounds);
  }

  void save() native "Canvas_save";
  // TODO(jackson): Paint should be optional, but making it optional causes crash
  void saveLayer(Rect bounds, Paint paint) native "Canvas_saveLayer";
  void restore() native "Canvas_restore";
  
  /// returns 1 for a clean canvas; each call to save() or saveLayer() increments it, and each call to 
  int getSaveCount() native "Canvas_getSaveCount";
  
  void translate(double dx, double dy) native "Canvas_translate";
  void scale(double sx, double sy) native "Canvas_scale";
  void rotate(double radians) native "Canvas_rotate";
  void skew(double sx, double sy) native "Canvas_skew";
  void concat(Float64List matrix4) native "Canvas_concat";
  void setMatrix(Float64List matrix4) native "Canvas_setMatrix";
  Float64List getTotalMatrix() native "Canvas_getTotalMatrix";
  void clipRect(Rect rect) native "Canvas_clipRect";
  void clipRRect(RRect rrect) native "Canvas_clipRRect";
  void clipPath(Path path) native "Canvas_clipPath";
  void drawColor(Color color, TransferMode transferMode) native "Canvas_drawColor";
  void drawLine(Point p1, Point p2, Paint paint) native "Canvas_drawLine";
  void drawPaint(Paint paint) native "Canvas_drawPaint";
  void drawRect(Rect rect, Paint paint) native "Canvas_drawRect";
  void drawRRect(RRect rrect, Paint paint) native "Canvas_drawRRect";
  void drawDRRect(RRect outer, RRect inner, Paint paint) native "Canvas_drawDRRect";
  void drawOval(Rect rect, Paint paint) native "Canvas_drawOval";
  void drawCircle(Point c, double radius, Paint paint) native "Canvas_drawCircle";
  void drawPath(Path path, Paint paint) native "Canvas_drawPath";
  void drawImage(Image image, Point p, Paint paint) native "Canvas_drawImage";
  void drawImageRect(Image image, Rect src, Rect dst, Paint paint) native "Canvas_drawImageRect";
  void drawImageNine(Image image, Rect center, Rect dst, Paint paint) native "Canvas_drawImageNine";
  void drawPicture(Picture picture) native "Canvas_drawPicture";

  void _drawVertices(int vertexMode,
                     List<Point> vertices,
                     List<Point> textureCoordinates,
                     List<Color> colors,
                     TransferMode transferMode,
                     List<int> indicies,
                     Paint paint) native "Canvas_drawVertices";

  void drawVertices(VertexMode vertexMode,
                    List<Point> vertices,
                    List<Point> textureCoordinates,
                    List<Color> colors,
                    TransferMode transferMode,
                    List<int> indicies,
                    Paint paint) {
    int vertexCount = vertices.length;
    if (textureCoordinates.isNotEmpty && textureCoordinates.length != vertexCount)
      throw new ArgumentError("[vertices] and [textureCoordinates] lengths must match");
    if (colors.isNotEmpty && colors.length != vertexCount)
      throw new ArgumentError("[vertices] and [colors] lengths must match");
    for (Point point in vertices) {
      if (point == null)
        throw new ArgumentError("[vertices] cannot contain a null");
    }
    for (Point point in textureCoordinates) {
      if (point == null)
        throw new ArgumentError("[textureCoordinates] cannot contain a null");
    }
    _drawVertices(vertexMode.index, vertices, textureCoordinates, colors, transferMode, indicies, paint);
  }

  // TODO(eseidel): Paint should be optional, but optional doesn't work.
  void _drawAtlas(Image image,
                  List<RSTransform> transforms,
                  List<Rect> rects,
                  List<Color> colors,
                  TransferMode mode,
                  Rect cullRect,
                  Paint paint) native "Canvas_drawAtlas";

  void drawAtlas(Image image,
                 List<RSTransform> transforms,
                 List<Rect> rects,
                 List<Color> colors,
                 TransferMode mode,
                 Rect cullRect,
                 Paint paint) {
    if (transforms.length != rects.length)
      throw new ArgumentError("[transforms] and [rects] lengths must match");
    if (colors.isNotEmpty && colors.length != rects.length)
      throw new ArgumentError("if supplied, [colors] length must match that of [transforms] and [rects]");
    for (RSTransform transform in transforms) {
      if (transform == null)
        throw new ArgumentError("[transforms] cannot contain a null");
    }
    for (Rect rect in rects) {
      if (rect == null)
        throw new ArgumentError("[rects] cannot contain a null");
    }
    _drawAtlas(image, transforms, rects, colors, mode, cullRect, paint);
  }
}

abstract class Picture extends NativeFieldWrapperClass2 {
  /// Replays the drawing commands on the specified canvas. Note that
  /// this has the effect of unfurling this picture into the destination
  /// canvas. Using the Canvas drawPicture entry point gives the destination
  /// canvas the option of just taking a ref.
  void playback(Canvas canvas) native "Picture_playback";

  void dispose() native "Picture_dispose";
}

class PictureRecorder extends NativeFieldWrapperClass2 {
  void _constructor() native "PictureRecorder_constructor";
  PictureRecorder() { _constructor(); }

  bool get isRecording native "PictureRecorder_isRecording";
  Picture endRecording() native "PictureRecorder_endRecording";
}
