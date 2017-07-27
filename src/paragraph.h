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

#ifndef LIB_TXT_SRC_PARAGRAPH_H_
#define LIB_TXT_SRC_PARAGRAPH_H_

#include <tuple>
#include <vector>

#include "lib/ftl/macros.h"
#include "lib/txt/src/font_collection.h"
#include "lib/txt/src/paint_record.h"
#include "lib/txt/src/paragraph_style.h"
#include "lib/txt/src/styled_runs.h"
#include "minikin/LineBreaker.h"
#include "third_party/gtest/include/gtest/gtest_prod.h"
#include "third_party/skia/include/core/SkPoint.h"
#include "third_party/skia/include/core/SkRect.h"
#include "third_party/skia/include/core/SkTextBlob.h"

class SkCanvas;

namespace txt {

using GlyphID = uint32_t;

// Paragraph provides Layout, metrics, and painting capabilites for text. Once a
// Paragraph is constructed with ParagraphBuilder::Build(), an example basic
// workflow can be this:
//
//   std::unique_ptr<Paragraph> paragraph = paragraph_builder.Build();
//   paragraph->Layout(<somewidthgoeshere>);
//   paragraph->Paint(<someSkCanvas>, <xpos>, <ypos>);
class Paragraph {
 public:
  // Constructor. I is highly recommended to construct a paragrph with a
  // ParagraphBuilder.
  Paragraph();

  ~Paragraph();

  // NOTE: Minikin Layout doLayout() and LineBreaker addStyleRun() has an
  // O(N^2) (according to benchmarks) time complexity where N is the total
  // number of characters. However, this is not significant for reasonably sized
  // paragraphs. It is currently recommended to break up very long paragraphs
  // (10k+ characters) to ensure speedy layout.
  //
  // Layout calculates the positioning of all the glyphs. Must call this method
  // before Painting and getting any statistics from this class.
  void Layout(double width, bool force = false);

  // Paints the Laid out text onto the supplied SkCanvas at (x, y) offset from
  // the origin. Only valid after Layout() is called.
  void Paint(SkCanvas* canvas, double x, double y);

  // Getter for paragraph_style_.
  const ParagraphStyle& GetParagraphStyle() const;

  // Returns the number of characters/unicode characters. AKA text_.size()
  size_t TextSize() const;

  // Returns the height of the laid out paragraph. NOTE this is not a tight
  // bounding height of the glyphs, as some glyphs do not reach as low as they
  // can.
  double GetHeight() const;

  // Returns the actual max width of the longest line after Layout().
  double GetLayoutWidth() const;

  // Returns the width provided in the Layout() method. This is the maximum
  // width any line in the laid out paragraph can occupy. We expect that
  // GetMaxWidth() >= GetLayoutWidth().
  double GetMaxWidth() const;

  // Distance from top of paragraph to the Alphabetic baseline of the first
  // line.
  double GetAlphabeticBaseline() const;

  // Distance from top of paragraph to the Ideographic baseline of the first
  // line.
  double GetIdeographicBaseline() const;

  // Returns the total width covered by the paragraph without linebreaking.
  double GetMaxIntrinsicWidth() const;

  // Currently, calculated similarly to as GetLayoutWidth(), however this is not
  // nessecarily 100% correct in all cases.
  double GetMinIntrinsicWidth() const;

  // Returns a vector of bounding boxes that enclose all text between start and
  // end glyph indexes, including start and excluding end.
  std::vector<SkRect> GetRectsForRange(size_t start, size_t end) const;

  // Returns the index of the glyph that corresponds to the provided coordinate,
  // with the top left corner as the origin, and +y direction as down.
  //
  // When using_glyph_center_as_boundary == true, coords to the + direction of
  // the center x-position of the glyph will be considered as the next glyph.
  size_t GetGlyphPositionAtCoordinate(
      double dx,
      double dy,
      bool using_glyph_center_as_boundary = false) const;

  // Returns a bounding box that encloses the glyph at the index pos.
  SkRect GetCoordinatesForGlyphPosition(size_t pos) const;

  // Finds the first and last glyphs that define a word containing the glyph at
  // index offset.
  SkIPoint GetWordBoundary(size_t offset) const;

  // Returns the number of lines the paragraph takes up. If the text exceeds the
  // amount width and maxlines provides, Layout() truncates the extra text from
  // the layout and this will return the max lines allowed.
  int GetLineCount() const;

  // Checks if the layout extends past the maximum lines and had to be
  // truncated.
  bool DidExceedMaxLines() const;

  // Sets the needs_layout_ to dirty. When Layout() is called, a new Layout will
  // be performed when this is set to true. Can also be used to prevent a new
  // Layout from being calculated by setting to false.
  void SetDirty(bool dirty = true);

 private:
  friend class ParagraphBuilder;
  FRIEND_TEST(RenderTest, SimpleParagraph);
  FRIEND_TEST(RenderTest, SimpleRedParagraph);
  FRIEND_TEST(RenderTest, RainbowParagraph);
  FRIEND_TEST(RenderTest, DefaultStyleParagraph);
  FRIEND_TEST(RenderTest, BoldParagraph);
  FRIEND_TEST(RenderTest, LeftAlignParagraph);
  FRIEND_TEST(RenderTest, RightAlignParagraph);
  FRIEND_TEST(RenderTest, CenterAlignParagraph);
  FRIEND_TEST(RenderTest, JustifyAlignParagraph);
  FRIEND_TEST(RenderTest, DecorationsParagraph);
  FRIEND_TEST(RenderTest, ItalicsParagraph);
  FRIEND_TEST(RenderTest, ChineseParagraph);
  FRIEND_TEST(RenderTest, DISABLED_ArabicParagraph);
  FRIEND_TEST(RenderTest, SpacingParagraph);
  FRIEND_TEST(RenderTest, LongWordParagraph);
  FRIEND_TEST(RenderTest, KernParagraph);

  // Starting data to layout.
  std::vector<uint16_t> text_;
  StyledRuns runs_;
  ParagraphStyle paragraph_style_;
  FontCollection* font_collection_;

  minikin::LineBreaker breaker_;

  // Stores the result of Layout().
  std::vector<PaintRecord> records_;

  // TODO(garyq): Can we access this info without redundantly storing it here?
  std::vector<double> line_widths_;
  std::vector<double> line_heights_;
  // Holds the laid out x positions of each glyph, as well as padding to make
  // math on it simpler.
  std::vector<std::vector<double>> glyph_position_x_;

  // Set of glyph IDs that correspond to whitespace.
  std::set<GlyphID> whitespace_set_;

  // The max width of the paragraph as provided in the most recent Layout()
  // call.
  double width_ = 0.0f;
  SkScalar height_ = 0.0f;
  size_t lines_ = 0;
  double max_intrinsic_width_ = 0;
  double min_intrinsic_width_ = 0;
  double alphabetic_baseline_ = FLT_MAX;
  double ideographic_baseline_ = FLT_MAX;

  bool needs_layout_ = true;

  struct WaveCoordinates {
    double x_start;
    double y_start;
    double x_end;
    double y_end;

    WaveCoordinates(double x_s, double y_s, double x_e, double y_e)
        : x_start(x_s), y_start(y_s), x_end(x_e), y_end(y_e) {}
  };

  void SetText(std::vector<uint16_t> text, StyledRuns runs);

  void SetParagraphStyle(const ParagraphStyle& style);

  void SetFontCollection(FontCollection* font_collection);

  // Pass the runs to breaker_.
  // NOTE: This is O(N^2) due to minikin breaking being O(N^2) where N = sum of
  // all text added using this method. This is insignificant with normal usage.
  void AddRunsToLineBreaker(
      std::unordered_map<std::string, std::shared_ptr<minikin::FontCollection>>&
          collection_map);

  // Calculates the GlyphIDs of all whitespace characters present in the text
  // between start and end. THis is used to correctly add extra whitespace when
  // justifying.
  void FillWhitespaceSet(size_t start, size_t end, hb_font_t* hb_font);

  // Calculates and amends the layout for one line to be justified.
  void JustifyLine(std::vector<const SkTextBlobBuilder::RunBuffer*>& buffers,
                   std::vector<size_t>& buffer_sizes,
                   int word_count,
                   double& justify_spacing,
                   double multiplier = 1);

  // Creates and draws the decorations onto the canvas.
  void PaintDecorations(SkCanvas* canvas,
                        double x,
                        double y,
                        size_t record_index);

  // Calculates wavy decorations and Draws them onto the canvas.
  void PaintWavyDecoration(SkCanvas* canvas,
                           std::vector<WaveCoordinates> wave_coords,
                           SkPaint paint,
                           double x,
                           double y,
                           double y_offset,
                           double width);

  void CalculateIntrinsicWidths();

  FTL_DISALLOW_COPY_AND_ASSIGN(Paragraph);
};

}  // namespace txt

#endif  // LIB_TXT_SRC_PARAGRAPH_H_
