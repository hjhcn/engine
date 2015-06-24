// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sky/engine/core/script/dart_dependency_catcher.h"

#include "sky/engine/core/script/dart_loader.h"

namespace blink {

DartDependencyCatcher::DartDependencyCatcher(DartLoader& loader) : loader_(loader) {
  loader_.set_dependency_catcher(this);
}

DartDependencyCatcher::~DartDependencyCatcher() {
  loader_.set_dependency_catcher(nullptr);
}

void DartDependencyCatcher::AddDependency(DartDependency* dependency) {
  dependencies_.add(dependency);
}

}  // namespace blink
