/*
 * Copyright 2017 Google Inc.
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

#ifndef LIB_TXT_SRC_PARAGRAPH_STYLE_H_
#define LIB_TXT_SRC_PARAGRAPH_STYLE_H_

#include <climits>
#include <string>

#include "lib/txt/include/minikin/LineBreaker.h"
#include "lib/txt/src/font_style.h"
#include "lib/txt/src/font_weight.h"
#include "lib/txt/src/text_align.h"

namespace txt {

class ParagraphStyle {
 public:
  FontWeight font_weight = FontWeight::w400;
  FontStyle font_style = FontStyle::normal;
  std::string font_family = "";
  double font_size = 14;

  TextAlign text_align = TextAlign::left;
  size_t max_lines = UINT_MAX;
  double line_height = 1.0;
  std::string ellipsis = "...";
  // Default strategy is kBreakStrategy_Greedy. Sometimes,
  // kBreakStrategy_HighQuality will produce more desireable layouts (eg, very
  // long words are more likely to be reasonably placed).
  // kBreakStrategy_Balanced will balance between the two.
  minikin::BreakStrategy break_strategy =
      minikin::BreakStrategy::kBreakStrategy_Greedy;
  // TODO(garyq): Implement right to left.
  // Right to left (Arabic, Hebrew, etc).
  bool rtl = false;
};

}  // namespace txt

#endif  // LIB_TXT_SRC_PARAGRAPH_STYLE_H_
