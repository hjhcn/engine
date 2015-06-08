// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import '../fn2.dart';

class MenuDivider extends Component {
  MenuDivider({ Object key }) : super(key: key);

  UINode build() {
    return new Container(
      height: 0.0,
      decoration: const BoxDecoration(
        border: const Border(
          bottom: const BorderSide(
            color: const Color.fromARGB(31, 0, 0, 0)
          )
        )
      ),
      margin: const EdgeDims.symmetric(vertical: 8.0)
    );
  }
}
