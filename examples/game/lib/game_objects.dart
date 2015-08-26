part of game;

abstract class GameObject extends Node {
  double radius = 0.0;
  double removeLimit = 1280.0;
  bool canDamageShip = true;
  bool canBeDamaged = true;
  double maxDamage = 3.0;
  double damage = 0.0;

  Paint _paintDebug = new Paint()
    ..color=new Color(0xffff0000)
    ..strokeWidth = 1.0
    ..setStyle(sky.PaintingStyle.stroke);

  bool collidingWith(GameObject obj) {
    return (GameMath.pointQuickDist(position, obj.position)
      < radius + obj.radius);
  }

  void move() {
  }

  void removeIfOffscreen(double scroll) {
    ;
    if (-position.y > scroll + removeLimit ||
        -position.y < scroll - 50.0) {
      removeFromParent();
    }
  }

  void destroy() {
    if (parent != null) {
      Explosion explo = createExplosion();
      if (explo != null) {
        explo.position = position;
        parent.addChild(explo);
      }

      removeFromParent();
    }
  }

  int addDamage(double d) {
    if (!canBeDamaged) return 0;

    damage += d;
    if (damage >= maxDamage) {
      destroy();
      return (maxDamage * 10).ceil();
    }
    return 10;
  }

  Explosion createExplosion() {
    return null;
  }

  void paint(PaintingCanvas canvas) {
    if (_drawDebug) {
      canvas.drawCircle(Point.origin, radius, _paintDebug);
    }
    super.paint(canvas);
  }

  void setupActions() {
  }
}

class Ship extends GameObject {
  Ship(GameObjectFactory f) {
    // Add main ship sprite
    _sprt = new Sprite(f.sheet["ship.png"]);
    _sprt.scale = 0.3;
    _sprt.rotation = -90.0;
    addChild(_sprt);
    radius = 20.0;

    canBeDamaged = false;
    canDamageShip = false;

    // Set start position
    position = new Point(0.0, 50.0);
  }

  Sprite _sprt;

  void applyThrust(Point joystickValue, double scroll) {
    Point oldPos = position;
    Point target = new Point(joystickValue.x * 160.0, joystickValue.y * 220.0 - 250.0 - scroll);
    double filterFactor = 0.2;

    position = new Point(
      GameMath.filter(oldPos.x, target.x, filterFactor),
      GameMath.filter(oldPos.y, target.y, filterFactor));
  }
}

class Laser extends GameObject {
  double impact = 1.0;

  Laser(GameObjectFactory f) {
    // Add sprite
    _sprt = new Sprite(f.sheet["laser.png"]);
    _sprt.scale = 0.3;
    _sprt.transferMode = sky.TransferMode.plus;
    addChild(_sprt);
    radius = 10.0;
    removeLimit = 640.0;

    canDamageShip = false;
    canBeDamaged = false;
  }

  Sprite _sprt;

  void move() {
    position += new Offset(0.0, -10.0);
  }
}

abstract class Obstacle extends GameObject {

  Obstacle(this._f);

  double explosionScale = 1.0;
  GameObjectFactory _f;

  Explosion createExplosion() {
    SoundEffectPlayer.sharedInstance().play(_f.sounds["explosion"]);
    Explosion explo = new Explosion(_f.sheet);
    explo.scale = explosionScale;
    return explo;
  }
}

abstract class Asteroid extends Obstacle {
  Asteroid(GameObjectFactory f) : super(f);

  Sprite _sprt;

  void setupActions() {
    // Rotate obstacle
    int direction = 1;
    if (randomBool()) direction = -1;
    ActionTween rotate = new ActionTween(
      (a) => _sprt.rotation = a,
      0.0, 360.0 * direction, 5.0 + 5.0 * randomDouble());
    _sprt.actions.run(new ActionRepeatForever(rotate));
  }

  set damage(double d) {
    super.damage = d;
    int alpha = ((200.0 * d) ~/ maxDamage).clamp(0, 200);
    _sprt.colorOverlay = new Color.fromARGB(alpha, 255, 3, 86);
  }
}

class AsteroidBig extends Asteroid {
  AsteroidBig(GameObjectFactory f) : super(f) {
    _sprt = new Sprite(f.sheet["asteroid_big_${randomInt(3)}.png"]);
    _sprt.scale = 0.3;
    radius = 25.0;
    maxDamage = 5.0;
    addChild(_sprt);
  }
}

class AsteroidSmall extends Asteroid {
  AsteroidSmall(GameObjectFactory f) : super(f) {
    _sprt = new Sprite(f.sheet["asteroid_small_${randomInt(3)}.png"]);
    _sprt.scale = 0.3;
    radius = 12.0;
    maxDamage = 3.0;
    addChild(_sprt);
  }
}

class MovingEnemy extends Obstacle {
  MovingEnemy(GameObjectFactory f) : super(f) {
    _sprt = new Sprite(f.sheet["ship.png"]);
    _sprt.scale = 0.2;
    radius = 12.0;
    maxDamage = 2.0;
    addChild(_sprt);

    constraints = [new ConstraintRotationToMovement(dampening: 0.5)];
  }

  final double _swirlSpacing = 80.0;

  _addRandomSquare(List<Offset> offsets, double x, double y) {
    double xMove = (randomBool()) ? _swirlSpacing : -_swirlSpacing;
    double yMove = (randomBool()) ? _swirlSpacing : -_swirlSpacing;

    if (randomBool()) {
      offsets.addAll([
        new Offset(x, y),
        new Offset(xMove + x, y),
        new Offset(xMove + x, yMove + y),
        new Offset(x, yMove + y),
        new Offset(x, y)
      ]);
    } else {
      offsets.addAll([
        new Offset(x, y),
        new Offset(x, y + yMove),
        new Offset(xMove + x, yMove + y),
        new Offset(xMove + x, y),
        new Offset(x, y)
      ]);
    }
  }

  void setupActions() {

    List<Offset> offsets = [];
    _addRandomSquare(offsets, -_swirlSpacing, 0.0);
    _addRandomSquare(offsets, _swirlSpacing, 0.0);
    offsets.add(new Offset(-_swirlSpacing, 0.0));

    List<Point> points = [];
    for (Offset offset in offsets) {
      points.add(position + offset);
    }

    ActionSpline spline = new ActionSpline((a) => position = a, points, 6.0);
    spline.tension = 0.7;
    actions.run(new ActionRepeatForever(spline));
  }

  Sprite _sprt;
}
