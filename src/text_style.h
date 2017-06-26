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

#ifndef LIB_TXT_SRC_TEXT_STYLE_H_
#define LIB_TXT_SRC_TEXT_STYLE_H_

#include <string>

#include "lib/txt/src/font_style.h"
#include "lib/txt/src/font_weight.h"
#include "lib/txt/src/text_baseline.h"
#include "lib/txt/src/text_decoration.h"
#include "third_party/skia/include/core/SkColor.h"

namespace txt {

class TextStyle {
 public:
  SkColor color = SK_ColorWHITE;
  TextDecoration decoration = TextDecoration::kNone;
  SkColor decoration_color = SK_ColorWHITE;
  TextDecorationStyle decoration_style = TextDecorationStyle::kSolid;
  FontWeight font_weight = FontWeight::w400;
  FontStyle font_style = FontStyle::normal;
  bool fake_bold = false;
  TextBaseline text_baseline = TextBaseline::kAlphabetic;
  std::string font_family = "";
  double font_size = 14.0;
  double letter_spacing = 0.0;
  double word_spacing = 0.0;
  double height = 1.0;

  bool equals(const TextStyle& other) const;
};

}  // namespace txt

#endif  // LIB_TXT_SRC_TEXT_STYLE_H_
