#!/bin/bash
set -ex

./sky/tools/gn --release
ninja -j 4 -C out/Release
./sky/tools/skyanalyzer --congratulate sky/packages/sky/example/stocks/lib/main.dart
