import 'package:quiver/testing/async.dart';
import 'package:sky/widgets.dart';
import 'package:test/test.dart';

import 'widget_tester.dart';

const Size pageSize = const Size(800.0, 600.0);
const List<int> pages = const <int>[0, 1, 2, 3, 4, 5];
int currentPage = null;

Widget buildPage(int page) {
  return new Container(
    key: new ValueKey<int>(page),
    width: pageSize.width,
    height: pageSize.height,
    child: new Text(page.toString())
  );
}

Widget buildFrame({ bool itemsWrap: false }) {
  // The test framework forces the frame (and so the PageableList)
  // to be 800x600. The pageSize constant reflects as much.
  return new PageableList<int>(
    items: pages,
    itemBuilder: buildPage,
    itemsWrap: itemsWrap,
    itemExtent: pageSize.width,
    scrollDirection: ScrollDirection.horizontal,
    pageChanged: (int page) { currentPage = page; }
  );
}

void page(WidgetTester tester, Offset offset) {
  String itemText = currentPage != null ? currentPage.toString() : '0';
  new FakeAsync().run((async) {
    tester.scroll(tester.findText(itemText), offset);
    // One frame to start the animation, a second to complete it.
    tester.pumpFrame(buildFrame);
    tester.pumpFrame(buildFrame, 1000.0);
    async.elapse(new Duration(seconds: 1));
  });
}

void pageLeft(WidgetTester tester) {
  page(tester, new Offset(-pageSize.width, 0.0));
}

void pageRight(WidgetTester tester) {
  page(tester, new Offset(pageSize.width, 0.0));
}

void main() {
  // PageableList with itemsWrap: false

  test('Scroll left from page 0 to page 1', () {
    WidgetTester tester = new WidgetTester();
    currentPage = null;
    tester.pumpFrame(buildFrame);
    expect(currentPage, isNull);
    pageLeft(tester);
    expect(currentPage, equals(1));
  });

  test('Underscroll (scroll right), return to page 0', () {
    WidgetTester tester = new WidgetTester();
    currentPage = null;
    tester.pumpFrame(buildFrame);
    expect(currentPage, isNull);
    pageRight(tester);
    expect(currentPage, equals(0));
  });

  // PageableList with itemsWrap: true

  test('Scroll left page 0 to page 1, itemsWrap: true', () {
    WidgetTester tester = new WidgetTester();
    currentPage = null;
    tester.pumpFrame(() { return buildFrame(itemsWrap: true); });
    expect(currentPage, isNull);
    pageLeft(tester);
    expect(currentPage, equals(1));
  });

  test('Scroll right from page 0 to page 5, itemsWrap: true', () {
    WidgetTester tester = new WidgetTester();
    currentPage = null;
    tester.pumpFrame(() { return buildFrame(itemsWrap: true); });
    expect(currentPage, isNull);
    pageRight(tester);
    expect(currentPage, equals(5));
  });
}
