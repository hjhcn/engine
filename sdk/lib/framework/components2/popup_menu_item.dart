// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import '../fn2.dart';
import 'ink_well.dart';

class PopupMenuItem extends Component {
  List<UINode> children;
  double opacity;

  PopupMenuItem({ Object key, this.children, this.opacity}) : super(key: key);

  UINode build() {
    return new Container(
      constraints: const BoxConstraints(minWidth: 112.0),
      padding: const EdgeDims.all(16.0),
      // TODO(abarth): opacity: opacity,
      child: new BlockContainer(children: children)
    );
  }
}
