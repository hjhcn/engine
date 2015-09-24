// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import 'package:sky/src/fn3.dart';

List<Route> routes = [
  new Route(
    name: 'home',
    builder: (navigator, route) => new Container(
      padding: const EdgeDims.all(30.0),
      decoration: new BoxDecoration(backgroundColor: const Color(0xFFCCCCCC)),
      child: new Column([
        new Text("You are at home"),
        new RaisedButton(
          child: new Text('GO SHOPPING'),
          onPressed: () => navigator.pushNamed('shopping')
        ),
        new RaisedButton(
          child: new Text('START ADVENTURE'),
          onPressed: () => navigator.pushNamed('adventure')
        )],
        justifyContent: FlexJustifyContent.center
      )
    )
  ),
  new Route(
    name: 'shopping',
    builder: (navigator, route) => new Container(
      padding: const EdgeDims.all(20.0),
      decoration: new BoxDecoration(backgroundColor: const Color(0xFFBF5FFF)),
      child: new Column([
        new Text("Village Shop"),
        new RaisedButton(
          child: new Text('RETURN HOME'),
          onPressed: () => navigator.pop()
        ),
        new RaisedButton(
          child: new Text('GO TO DUNGEON'),
          onPressed: () => navigator.push(routes[2])
        )],
        justifyContent: FlexJustifyContent.center
      )
    )
  ),
  new Route(
    name: 'adventure',
    builder: (navigator, route) => new Container(
      padding: const EdgeDims.all(20.0),
      decoration: new BoxDecoration(backgroundColor: const Color(0xFFDC143C)),
      child: new Column([
        new Text("Monster's Lair"),
        new RaisedButton(
          child: new Text('RUN!!!'),
          onPressed: () => navigator.pop()
        )],
        justifyContent: FlexJustifyContent.center
      )
    )
  )
];

class NavigationExampleApp extends StatefulComponent {
  NavigationExampleAppState createState() => new NavigationExampleAppState(this);
}

class NavigationExampleAppState extends ComponentState<NavigationExampleApp> {
  NavigationExampleAppState(NavigationExampleApp config) : super(config);

  NavigatorHistory _history = new NavigatorHistory(routes);

  void onBack() {
    if (_history.hasPrevious()) {
      setState(() {
        _history.pop();
      });
    } else {
      // TODO(abarth): Integrate with the system navigator.
      // super.onBack();
    }
  }

  Widget build(BuildContext context) {
    return new Row([new Navigator(_history)]);
  }
}

void main() {
  runApp(new NavigationExampleApp());
}
