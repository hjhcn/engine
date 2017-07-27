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

#ifndef LIB_TXT_SRC_PARAGRAPH_BUILDER_H_
#define LIB_TXT_SRC_PARAGRAPH_BUILDER_H_

#include <memory>
#include <string>

#include "lib/ftl/macros.h"
#include "lib/txt/src/font_collection.h"
#include "lib/txt/src/paragraph.h"
#include "lib/txt/src/paragraph_style.h"
#include "lib/txt/src/styled_runs.h"
#include "lib/txt/src/text_style.h"

namespace txt {

class ParagraphBuilder {
 public:
  explicit ParagraphBuilder(ParagraphStyle style);

  ParagraphBuilder(ParagraphStyle style, FontCollection* font_collection);

  ParagraphBuilder();

  ~ParagraphBuilder();

  // Push a style to the stack. The corresponding text added with AddText will
  // use the top-most style.
  void PushStyle(const TextStyle& style);

  // Remove a style from the stack. Useful to apply different styles to chunks
  // of text such as bolding.
  // Example:
  //   builder.PushStyle(normal_style);
  //   builder.AddText("Hello this is normal. ");
  //
  //   builder.PushStyle(bold_style);
  //   builder.AddText("And this is BOLD. ");
  //
  //   builder.Pop();
  //   builder.AddText(" Back to normal again.");
  void Pop();

  // Adds text to the builder. Forms the proper runs to use the upper-most style
  // on the style_stack_;
  void AddText(const std::u16string& text);

  // Converts to u16string before adding.
  void AddText(const std::string& text);

  // Converts to u16string before adding.
  void AddText(const char* text);

  void SetParagraphStyle(const ParagraphStyle& style);

  // It is recommended to initialize the ParagraphBuilder with a font collection
  // as default font collection fallback will be deprecated.
  void SetFontCollection(FontCollection* font_collection);

  // Constructs a Paragraph object that can be used to layout and paint the text
  // to a SkCanvas.
  std::unique_ptr<Paragraph> Build();

 private:
  std::vector<uint16_t> text_;
  std::vector<size_t> style_stack_;
  StyledRuns runs_;
  ParagraphStyle paragraph_style_;
  FontCollection* font_collection_ = nullptr;

  FTL_DISALLOW_COPY_AND_ASSIGN(ParagraphBuilder);
};

}  // namespace txt

#endif  // LIB_TXT_SRC_PARAGRAPH_BUILDER_H_
