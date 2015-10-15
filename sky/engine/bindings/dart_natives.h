// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKY_ENGINE_BINDINGS_DART_NATIVES_H_
#define SKY_ENGINE_BINDINGS_DART_NATIVES_H_

#include "base/macros.h"
#include "dart/runtime/include/dart_api.h"

namespace blink {

class DartNatives {
 public:
  enum IsolateType {
    MainIsolate,
    DartIOIsolate,
  };

  static Dart_NativeFunction NativeLookup(Dart_Handle name,
                                          int argument_count,
                                          bool* auto_setup_scope);
  static const uint8_t* NativeSymbol(Dart_NativeFunction native_function);

  static void Init(IsolateType isolate_type);

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(DartNatives);
};

}  // namespace blink

#endif  // SKY_ENGINE_BINDINGS_DART_NATIVES_H_
