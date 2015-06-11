// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'wrappers.dart';

abstract class ButtonBase extends Component {

  ButtonBase({ Object key, this.highlight: false }) : super(key: key);

  bool highlight;

  void syncFields(ButtonBase source) {
    highlight = source.highlight;
  }

  void _handlePointerDown(_) {
    setState(() {
      highlight = true;
    });
  }
  void _handlePointerUp(_) {
    setState(() {
      highlight = false;
    });
  }
  void _handlePointerCancel(_) {
    setState(() {
      highlight = false;
    });
  }

  UINode build() {
    return new EventListenerNode(
      buildContent(),
      onPointerDown: _handlePointerDown,
      onPointerUp: _handlePointerUp,
      onPointerCancel: _handlePointerCancel
    );
  }

  UINode buildContent();

}
