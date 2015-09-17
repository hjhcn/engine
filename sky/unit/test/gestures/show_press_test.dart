import 'package:quiver/testing/async.dart';
import 'package:sky/gestures.dart';
import 'package:test/test.dart';

import '../engine/mock_events.dart';

final TestPointerEvent down = new TestPointerEvent(
  pointer: 5,
  type: 'pointerdown',
  x: 10.0,
  y: 10.0
);

final TestPointerEvent up = new TestPointerEvent(
  pointer: 5,
  type: 'pointerup',
  x: 11.0,
  y: 9.0
);

void main() {
  test('Should recognize show press', () {
    PointerRouter router = new PointerRouter();
    ShowPressGestureRecognizer showPress = new ShowPressGestureRecognizer(router: router);

    bool showPressRecognized = false;
    showPress.onShowPress = () {
      showPressRecognized = true;
    };

    new FakeAsync().run((async) {
      showPress.addPointer(down);
      GestureArena.instance.close(5);
      expect(showPressRecognized, isFalse);
      router.route(down);
      expect(showPressRecognized, isFalse);
      async.elapse(new Duration(milliseconds: 300));
      expect(showPressRecognized, isTrue);
    });

    showPress.dispose();
  });

  test('Up cancels show press', () {
    PointerRouter router = new PointerRouter();
    ShowPressGestureRecognizer showPress = new ShowPressGestureRecognizer(router: router);

    bool showPressRecognized = false;
    showPress.onShowPress = () {
      showPressRecognized = true;
    };

    new FakeAsync().run((async) {
      showPress.addPointer(down);
      GestureArena.instance.close(5);
      expect(showPressRecognized, isFalse);
      router.route(down);
      expect(showPressRecognized, isFalse);
      async.elapse(new Duration(milliseconds: 50));
      expect(showPressRecognized, isFalse);
      router.route(up);
      expect(showPressRecognized, isFalse);
      async.elapse(new Duration(seconds: 1));
      expect(showPressRecognized, isFalse);
    });

    showPress.dispose();
  });
}
