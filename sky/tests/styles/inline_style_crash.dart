import "../resources/third_party/unittest/unittest.dart";
import "../resources/unit.dart";

import "dart:sky";

void main() {
  initUnit();

  test('should not crash when setting style to null', () {
    LayoutRoot layoutRoot = new LayoutRoot();
    var document = new Document();
    var foo = document.createElement('foo');
    layoutRoot.rootElement = foo;

    expect(foo.style['color'], isNull);
    foo.style["color"] = null; // This used to crash.
    expect(foo.style['color'], isNull);
    foo.style["color"] = "blue";
    expect(foo.style['color'], equals("rgb(0, 0, 255)"));
    foo.style["color"] = null;
    expect(foo.style['color'], isNull);
    foo.style["color"] = "blue";
    expect(foo.style['color'], equals("rgb(0, 0, 255)"));
    foo.style.removeProperty("color");
    expect(foo.style['color'], isNull);

    layoutRoot.layout();
  });
}
