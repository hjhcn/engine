import 'dart:ui';

import 'package:test/test.dart';

import 'dom_utils.dart';

void main() {
  Document document = new Document();

  test("should throw with invalid arguments", () {
    var parent = document.createElement("div");
    expect(() {
      Function.apply(parent.appendChild, []);
    }, throws);
    expect(() {
      parent.appendChild(null);
    }, throws);
    expect(() {
      Function.apply(parent.appendChild, [{"tagName": "div"}]);
    }, throws);
  });

  test("should insert children", () {
    var parent = document.createElement("div");
    var child1 = parent.appendChild(document.createElement("div"));
    var child2 = parent.appendChild(document.createText(" text "));
    var child3 = parent.appendChild(document.createText(" "));
    var child4 = parent.appendChild(document.createElement("div"));
    expect(child1.parentNode, equals(parent));
    expect(child2.parentNode, equals(parent));
    expect(child3.parentNode, equals(parent));
    expect(child4.parentNode, equals(parent));
    expect(childNodeCount(parent), equals(4));
    expect(childElementCount(parent), equals(2));
  });

  test("should insert children with a fragment", () {
    var fragment = document.createDocumentFragment();
    var child1 = fragment.appendChild(document.createElement("div"));
    var child2 = fragment.appendChild(document.createText(" text "));
    var child3 = fragment.appendChild(document.createText(" "));
    var child4 = fragment.appendChild(document.createElement("div"));
    var parent = document.createElement("div");
    parent.appendChild(fragment);
    expect(child1.parentNode, equals(parent));
    expect(child2.parentNode, equals(parent));
    expect(child3.parentNode, equals(parent));
    expect(child4.parentNode, equals(parent));
    expect(childNodeCount(parent), equals(4));
    expect(childElementCount(parent), equals(2));
  });

  // TODO(dart): These might be real bugs too.
  // test("should throw when appending to a text", () {
  //   var parent = new Text();
  //   expect(() {
  //     parent.appendChild(document.createElement("div"));
  //   }, throws);
  // });
}
