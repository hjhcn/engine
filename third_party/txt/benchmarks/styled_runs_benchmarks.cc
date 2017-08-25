/*
 * Copyright 2017 Google, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "third_party/benchmark/include/benchmark/benchmark_api.h"

#include "lib/ftl/command_line.h"
#include "lib/ftl/logging.h"
#include "lib/txt/src/styled_runs.h"
#include "lib/txt/src/text_style.h"
#include "utils.h"

namespace txt {

static void BM_StyledRunsGetRun(benchmark::State& state) {
  StyledRuns runs;
  TextStyle style;
  runs.AddStyle(style);
  runs.StartRun(0, 0);
  runs.EndRunIfNeeded(11);
  while (state.KeepRunning()) {
    runs.GetRun(0);
  }
}
BENCHMARK(BM_StyledRunsGetRun);

}  // namespace txt
