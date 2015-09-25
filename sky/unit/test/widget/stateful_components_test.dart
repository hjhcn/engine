import 'package:sky/src/fn3.dart';
import 'package:test/test.dart';

import '../fn3/widget_tester.dart';

class InnerComponent extends StatefulComponent {
  InnerComponent({ Key key }) : super(key: key);
  InnerComponentState createState() => new InnerComponentState();
}

class InnerComponentState extends State<InnerComponent> {
  bool _didInitState = false;

  void initState(BuildContext context) {
    super.initState(context);
    _didInitState = true;
  }

  Widget build(BuildContext context) {
    return new Container();
  }
}

class OuterContainer extends StatefulComponent {
  OuterContainer({ Key key, this.child }) : super(key: key);

  final InnerComponent child;

  OuterContainerState createState() => new OuterContainerState();
}

class OuterContainerState extends State<OuterContainer> {
  Widget build(BuildContext context) {
    return config.child;
  }
}

void main() {
  test('resync stateful widget', () {

    WidgetTester tester = new WidgetTester();

    Key innerKey = new Key('inner');
    Key outerKey = new Key('outer');

    InnerComponent inner1 = new InnerComponent(key: innerKey);
    InnerComponent inner2;
    OuterContainer outer1 = new OuterContainer(key: outerKey, child: inner1);
    OuterContainer outer2;

    tester.pumpFrame(outer1);

    StatefulComponentElement innerElement = tester.findElementByKey(innerKey);
    InnerComponentState innerElementState = innerElement.state;
    expect(innerElementState.config, equals(inner1));
    expect(innerElementState._didInitState, isTrue);
    expect(innerElement.renderObject.attached, isTrue);

    inner2 = new InnerComponent(key: innerKey);
    outer2 = new OuterContainer(key: outerKey, child: inner2);

    tester.pumpFrame(outer2);

    expect(tester.findElementByKey(innerKey), equals(innerElement));
    expect(innerElement.state, equals(innerElementState));

    expect(innerElementState.config, equals(inner2));
    expect(innerElementState._didInitState, isTrue);
    expect(innerElement.renderObject.attached, isTrue);

    StatefulComponentElement outerElement = tester.findElementByKey(outerKey);
    expect(outerElement.state.config, equals(outer2));
    outerElement.state.setState(() {});
    tester.pumpFrameWithoutChange(0.0);

    expect(tester.findElementByKey(innerKey), equals(innerElement));
    expect(innerElement.state, equals(innerElementState));
    expect(innerElementState.config, equals(inner2));
    expect(innerElement.renderObject.attached, isTrue);
  });
}
