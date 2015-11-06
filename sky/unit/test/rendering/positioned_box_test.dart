import 'package:flutter/rendering.dart';
import 'package:test/test.dart';

import 'rendering_tester.dart';

void main() {
  test('RenderPositionedBox expands', () {
    RenderConstrainedBox sizer = new RenderConstrainedBox(
      additionalConstraints: new BoxConstraints.tight(new Size(100.0, 100.0)),
      child: new RenderDecoratedBox(decoration: new BoxDecoration())
    );
    RenderPositionedBox positioner = new RenderPositionedBox(child: sizer);
    layout(positioner, constraints: new BoxConstraints.loose(new Size(200.0, 200.0)));

    expect(positioner.size.width, equals(200.0), reason: "positioner width");
    expect(positioner.size.height, equals(200.0), reason: "positioner height");
  });

  test('RenderPositionedBox shrink wraps', () {
    RenderConstrainedBox sizer = new RenderConstrainedBox(
      additionalConstraints: new BoxConstraints.tight(new Size(100.0, 100.0)),
      child: new RenderDecoratedBox(decoration: new BoxDecoration())
    );
    RenderPositionedBox positioner = new RenderPositionedBox(child: sizer, widthFactor: 1.0);
    layout(positioner, constraints: new BoxConstraints.loose(new Size(200.0, 200.0)));

    expect(positioner.size.width, equals(100.0), reason: "positioner width");
    expect(positioner.size.height, equals(200.0), reason: "positioner height");

    positioner.widthFactor = null;
    positioner.heightFactor = 1.0;
    pumpFrame();

    expect(positioner.size.width, equals(200.0), reason: "positioner width");
    expect(positioner.size.height, equals(100.0), reason: "positioner height");

    positioner.widthFactor = 1.0;
    pumpFrame();

    expect(positioner.size.width, equals(100.0), reason: "positioner width");
    expect(positioner.size.height, equals(100.0), reason: "positioner height");
  });

  test('RenderPositionedBox width and height factors', () {
    RenderConstrainedBox sizer = new RenderConstrainedBox(
      additionalConstraints: new BoxConstraints.tight(new Size(100.0, 100.0)),
      child: new RenderDecoratedBox(decoration: new BoxDecoration())
    );
    RenderPositionedBox positioner = new RenderPositionedBox(child: sizer, widthFactor: 1.0, heightFactor: 0.0);
    layout(positioner, constraints: new BoxConstraints.loose(new Size(200.0, 200.0)));

    expect(positioner.size.width, equals(100.0));
    expect(positioner.size.height, equals(0.0));

    positioner.widthFactor = 0.5;
    positioner.heightFactor = 0.5;
    pumpFrame();

    expect(positioner.size.width, equals(50.0));
    expect(positioner.size.height, equals(50.0));

    positioner.widthFactor = null;
    positioner.heightFactor = null;
    pumpFrame();

    expect(positioner.size.width, equals(200.0));
    expect(positioner.size.height, equals(200.0));
  });
}
