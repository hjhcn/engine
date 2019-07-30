Kraken Engine
==============
本仓库Fork自Flutter Engine [GitHub - flutter/engine: The Flutter engine](https://github.com/flutter/engine)，为Kraken项目接入JS2Dart增加Engine生命周期hook以及Dart API hook。

# 开发环境搭建
仅在对Engine进行修改或Flutter Engine升级时需要进行如下操作，Kraken业务开发请遵循 https://yuque.antfin-inc.com/kraken/guide。

1. 新建engine目录，并在root下新建.gclient文件
    `mkdir engine && cd engine && touch .gclient`
    
    .gclient 文件中输入如下内容
    ```
    solutions = [
      {
        "managed": False,
        "name": "src/flutter",
        "url": "git@gitlab.alibaba-inc.com:kraken/flutter-engine.git",
        "custom_deps": {},
        "deps_file": "DEPS",
        "safesync_url": "",
      },
    ]
    ```
    其中url为Kraken Engine gitlab仓库地址；name为src/flutter，不可以修改，否则编译报错！

2. 执行同步
    `gclient sync`
    此命令将下载Engine以及所有三方依赖，耗时可能比较久，公司网络尚可以接受，非公司网络请开加速。

3. 覆盖Kraken Engine(可选)
    2执行成功后，3级目录应该如下所示
    ```
    ├── engine
    │   ├── src
    │   ├── ├──  flutter
    │   ├── ├──  fuchsia
    │   ├── ├──  buildtools
    │   ├── ├──  third_party
    │   ├── ├──  tools
    │   ├── ├──  …
    ```
    其中src/flutter、third_party中的依赖库都是gclient拉取的对应仓库对应commit的源码。如果需要对Engine本身进行维护，找到对应git@gitlab.alibaba-inc.com:kraken/flutter-engine.git对应的commit，覆盖src/flutter。下面就可以直接在src/flutter下进行修改并提交。

# Engine编译
以iOS为例
1. 在src目录执行`./flutter/tools/gn —ios —unoptimized` 
构建真机编译需要的xcode工程(如需模拟器需要增加参数：—simulator )‘，构建产物在src/out/ios_debug_unopt中

2. 打开all.xcworkspace，cmd+b编译，成功后构建产物为src/out/ios_debug_unopt/Flutter.framework。

