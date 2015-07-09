// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import '../animation/animation_performance.dart';
import '../painting/box_painter.dart';
import 'animated_component.dart';
import 'animated_container.dart';
import 'basic.dart';
import 'default_text_style.dart';
import 'theme.dart';

enum MaterialType { canvas, card, circle, button }

const Map<MaterialType, double> edges = const {
  MaterialType.canvas: null,
  MaterialType.card: 2.0,
  MaterialType.circle: null,
  MaterialType.button: 2.0,
};

const Duration _kAnimateShadowDuration = const Duration(milliseconds: 100);
const Duration _kAnimateColorDuration = const Duration(milliseconds: 100);

class Material extends AnimatedComponent {

  Material({
    String key,
    this.child,
    MaterialType type: MaterialType.card,
    int level: 0,
    Color color: null
  }) : super(key: key) {
    if (level == null) level = 0;
    _container = new AnimatedContainer()
      ..shadow = new AnimatedType<double>(level.toDouble())
      ..backgroundColor = _getBackgroundColor(type, color)
      ..borderRadius = edges[type]
      ..shape = type == MaterialType.circle ? Shape.circle : Shape.rectangle;
    watchPerformance(_container.createPerformance(
        _container.shadow, duration: _kAnimateShadowDuration));
    watchPerformance(_container.createPerformance(
        _container.backgroundColor, duration: _kAnimateColorDuration));
  }

  Widget child;

  AnimatedContainer _container;

  void syncFields(Material source) {
    child = source.child;
    _container.syncFields(source._container);
    super.syncFields(source);
  }

  AnimatedColor _getBackgroundColor(MaterialType type, Color color) {
    if (color == null) {
      switch (type) {
        case MaterialType.canvas: color = Theme.of(this).canvasColor; break;
        case MaterialType.card: color = Theme.of(this).cardColor; break;
      }
    }
    return color == null ? null : new AnimatedColor(color);
  }

  Widget build() {
    return _container.build(
        new DefaultTextStyle(style: Theme.of(this).text.body1, child: child)
    );
  }

}
