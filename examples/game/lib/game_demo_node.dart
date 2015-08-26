part of game;

final double _gameSizeWidth = 320.0;
double _gameSizeHeight = 320.0;

final double _chunkSpacing = 640.0;
final int _chunksPerLevel = 5;

final bool _drawDebug = false;

class GameDemoNode extends NodeWithSize {

  GameDemoNode(
    this._images,
    this._spritesGame,
    this._spritesUI,
    this._sounds,
    this._gameOverCallback
  ): super(new Size(320.0, 320.0)) {
    // Add background
    _background = new RepeatedImage(_images["assets/starfield.png"]);
    addChild(_background);

    // Create starfield
    _starField = new StarField(_spritesGame, 200);
    addChild(_starField);

    // Add nebula
    _nebula = new RepeatedImage(_images["assets/nebula.png"], sky.TransferMode.plus);
    addChild(_nebula);

    // Setup game screen, it will always be anchored to the bottom of the screen
    _gameScreen = new Node();
    addChild(_gameScreen);

    // Setup the level and add it to the screen, the level is the node where
    // all our game objects live. It is moved to scroll the game
    _level = new Level();
    _gameScreen.addChild(_level);

    // Add heads up display
    _playerState = new PlayerState(_spritesUI);
    addChild(_playerState);

    _objectFactory = new GameObjectFactory(_spritesGame, _sounds, _level, _playerState);

    _level.ship = new Ship(_objectFactory);
    _level.addChild(_level.ship);

    // Add the joystick
    _joystick = new VirtualJoystick();
    _gameScreen.addChild(_joystick);

    // Add initial game objects
    addObjects();
  }

  // Resources
  ImageMap _images;
  Map<String, SoundEffect> _sounds;
  SpriteSheet _spritesGame;
  SpriteSheet _spritesUI;

  // Sounds
  SoundEffectPlayer _effectPlayer = SoundEffectPlayer.sharedInstance();

  // Callback
  Function _gameOverCallback;

  // Game screen nodes
  Node _gameScreen;
  VirtualJoystick _joystick;

  GameObjectFactory _objectFactory;
  Level _level;
  StarField _starField;
  RepeatedImage _background;
  RepeatedImage _nebula;
  PlayerState _playerState;

  // Game properties
  double _scrollSpeed = 2.0;
  double _scroll = 0.0;

  int _framesToFire = 0;
  int _framesBetweenShots = 20;

  bool _gameOver = false;

  void spriteBoxPerformedLayout() {
    _gameSizeHeight = spriteBox.visibleArea.height;
    _gameScreen.position = new Point(0.0, _gameSizeHeight);
  }

  void update(double dt) {
    // Scroll the level
    _scroll = _level.scroll(_scrollSpeed);
    _starField.move(0.0, _scrollSpeed);

    _background.move(_scrollSpeed * 0.1);
    _nebula.move(_scrollSpeed);

    // Add objects
    addObjects();

    // Move the ship
    if (!_gameOver) {
      _level.ship.applyThrust(_joystick.value, _scroll);
    }

    // Add shots
    if (_framesToFire == 0 && _joystick.isDown && !_gameOver) {
      fire();
      _framesToFire = _framesBetweenShots;
    }
    if (_framesToFire > 0) _framesToFire--;

    // Move game objects
    for (Node node in _level.children) {
      if (node is GameObject) {
        node.move();
      }
    }

    // Remove offscreen game objects
    for (int i = _level.children.length - 1; i >= 0; i--) {
      Node node = _level.children[i];
      if (node is GameObject) {
        node.removeIfOffscreen(_scroll);
      }
    }

    if (_gameOver) return;

    // Check for collisions between lasers and objects that can take damage
    List<Laser> lasers = [];
    for (Node node in _level.children) {
      if (node is Laser) lasers.add(node);
    }

    List<GameObject> damageables = [];
    for (Node node in _level.children) {
      if (node is GameObject && node.canBeDamaged) damageables.add(node);
    }

    for (Laser laser in lasers) {
      for (GameObject damageable in damageables) {
        if (laser.collidingWith(damageable)) {
          // Hit something that can take damage
          damageable.addDamage(laser.impact);
          laser.destroy();
        }
      }
    }

    // Check for collsions between ship and objects that can damage the ship
    List<Node> nodes = new List.from(_level.children);
    for (Node node in nodes) {
      if (node is GameObject && node.canDamageShip) {
        if (node.collidingWith(_level.ship)) {
          // The ship was hit :(
          killShip();
          _level.ship.visible = false;
        }
      } else if (node is GameObject && node.canBeCollected) {
        if (node.collidingWith(_level.ship)) {
          // The ship ran over something collectable
          node.removeFromParent();
        }
      }
    }
  }

  int _chunk = 0;

  void addObjects() {

    while (_scroll + _chunkSpacing >= _chunk * _chunkSpacing) {
      addLevelChunk(
        _chunk,
        -_chunk * _chunkSpacing - _chunkSpacing);

      _chunk += 1;
    }
  }

  void addLevelChunk(int chunk, double yPos) {
    if (chunk == 0) {
      // Leave the first chunk empty
      return;
    }

    chunk -= 1;

    int level = chunk ~/ _chunksPerLevel;
    int part = chunk % _chunksPerLevel;

    if (part == 0) {
      _objectFactory.addAsteroids(10 + level * 4, yPos, 0.0 + (level * 0.2).clamp(0.0, 0.7));
    } else if  (part == 1) {
      _objectFactory.addSwarm(4 + level * 2, yPos);
    } else if (part == 2) {
      _objectFactory.addAsteroids(10 + level * 4, yPos, 0.0 + (level * 0.2).clamp(0.0, 0.7));
    } else if (part == 3) {
      _objectFactory.addAsteroids(10 + level * 4, yPos, 0.0 + (level * 0.2).clamp(0.0, 0.7));
    } else if (part == 4) {
      _objectFactory.addAsteroids(10 + level * 4, yPos, 0.0 + (level * 0.2).clamp(0.0, 0.7));
    }
  }

  void fire() {
    Laser shot0 = new Laser(_objectFactory);
    shot0.position = _level.ship.position + new Offset(17.0, -10.0);
    _level.addChild(shot0);

    Laser shot1 = new Laser(_objectFactory);
    shot1.position = _level.ship.position + new Offset(-17.0, -10.0);
    _level.addChild(shot1);

    _effectPlayer.play(_sounds["laser"]);
  }

  void killShip() {
    // Hide ship
    _level.ship.visible = false;

    _effectPlayer.play(_sounds["explosion"]);

    // Add explosion
    Explosion explo = new Explosion(_spritesGame);
    explo.scale = 1.5;
    explo.position = _level.ship.position;
    _level.addChild(explo);

    // Add flash
    Flash flash = new Flash(size, 1.0);
    addChild(flash);

    // Set the state to game over
    _gameOver = true;

    // Return to main scene and report the score back in 2 seconds
    new Timer(new Duration(seconds: 2), () { _gameOverCallback(_playerState.score); });
  }
}

class Level extends Node {
  Level() {
    position = new Point(160.0, 0.0);
  }

  Ship ship;

  double scroll(double scrollSpeed) {
    position += new Offset(0.0, scrollSpeed);
    return position.y;
  }
}

enum GameObjectType {
  asteroidBig,
  asteroidSmall,
  movingEnemy,
  coin,
}

class GameObjectFactory {
  GameObjectFactory(this.sheet, this.sounds, this.level, this.playerState);

  SpriteSheet sheet;
  Map<String,SoundEffect> sounds;
  Level level;
  PlayerState playerState;

  void addAsteroids(int numAsteroids, double yPos, double distribution) {
    for (int i = 0; i < numAsteroids; i++) {
      GameObjectType type = (randomDouble() < distribution) ? GameObjectType.asteroidBig : GameObjectType.asteroidSmall;
      Point pos = new Point(randomSignedDouble() * 160.0,
                            yPos + _chunkSpacing * randomDouble());
      addGameObject(type, pos);
    }
  }

  void addSwarm(int numEnemies, double yPos) {
    for (int i = 0; i < numEnemies; i++) {
      double spacing = math.max(_chunkSpacing / (numEnemies + 1.0), 80.0);
      double y = yPos + _chunkSpacing / 2.0 - (numEnemies - 1) * spacing / 2.0 + i * spacing;
      addGameObject(GameObjectType.movingEnemy, new Point(0.0, y));
    }
  }

  void addGameObject(GameObjectType type, Point pos) {
    GameObject obj;
    if (type == GameObjectType.asteroidBig)
      obj = new AsteroidBig(this);
    else if (type == GameObjectType.asteroidSmall)
      obj = new AsteroidSmall(this);
    else if (type == GameObjectType.movingEnemy)
      obj = new MovingEnemy(this);
    else if (type == GameObjectType.coin)
      obj = new Coin(this);

    obj.position = pos;
    obj.setupActions();

    level.addChild(obj);
  }
}

class PlayerState extends Node {
  SpriteSheet sheet;
  Sprite sprtBgScore;

  bool _dirtyScore = true;
  int _score = 0;

  int get score => _score;

  set score(int score) {
    _score = score;
    _dirtyScore = true;
  }

  PlayerState(this.sheet) {
    position = new Point(310.0, 10.0);
    scale = 0.6;

    sprtBgScore = new Sprite(sheet["scoreboard.png"]);
    sprtBgScore.pivot = new Point(1.0, 0.0);
    sprtBgScore.scale = 0.6;
    addChild(sprtBgScore);
  }

  void update(double dt) {
    // Update score
    if (_dirtyScore) {

      sprtBgScore.removeAllChildren();

      String scoreStr = _score.toString();
      double xPos = -50.0;
      for (int i = scoreStr.length - 1; i >= 0; i--) {
        String numStr = scoreStr.substring(i, i + 1);
        Sprite numSprt = new Sprite(sheet["number_$numStr.png"]);
        numSprt.position = new Point(xPos, 49.0);
        sprtBgScore.addChild(numSprt);
        xPos -= 37.0;
      }
      _dirtyScore = false;
    }
  }
}
