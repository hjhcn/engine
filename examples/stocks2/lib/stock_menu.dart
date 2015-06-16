// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'package:sky/widgets/checkbox.dart';
import 'package:sky/widgets/popup_menu.dart';
import 'package:sky/widgets/basic.dart';
import 'package:sky/framework/theme/view_configuration.dart';

class StockMenu extends Component {

  StockMenu({
    String key,
    this.controller,
    this.autorefresh: false,
    this.onAutorefreshChanged
  }) : super(key: key);

  final PopupMenuController controller;
  final bool autorefresh;
  final ValueChanged onAutorefreshChanged;

  Widget build() {
    var checkbox = new Checkbox(
      value: this.autorefresh,
      onChanged: this.onAutorefreshChanged
    );

    return new Positioned(
      child: new PopupMenu(
        controller: controller,
        items: [
          new Text('Add stock'),
          new Text('Remove stock'),
          new Flex([new Flexible(child: new Text('Autorefresh')), checkbox]),
        ],
        level: 4
      ),
      right: 8.0,
      top: 8.0 + kStatusBarHeight
    );
  }
}
