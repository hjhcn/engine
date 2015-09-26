// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

part of stocks;

class Stocklist extends StatelessComponent {
  Stocklist({ Key key, this.stocks }) : super(key: key);

  final List<Stock> stocks;

  Widget build(BuildContext context) {
    return new Material(
      type: MaterialType.canvas,
      child: new ScrollableList<Stock>(
        items: stocks,
        itemExtent: StockRow.kHeight,
        itemBuilder: (BuildContext context, Stock stock) => new StockRow(stock: stock)
      )
    );
  }
}
