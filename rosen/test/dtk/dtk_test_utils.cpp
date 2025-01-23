/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "dtk_test_utils.h"
#include "text/font.h"
#include "text/font_mgr.h"
#include "text/font_style.h"
#include "text/font_style_set.h"

namespace OHOS {
namespace Rosen {
static const float RECT_SIZE = 100.0f;
static const float RECT_ROUND_RADIUS = 3.0f;
static Drawing::Rect DRAW_RECT = Drawing::Rect(0, 0, RECT_SIZE, RECT_SIZE);
static Drawing::RectI DRAW_RECTI = Drawing::RectI(0, 0, RECT_SIZE, RECT_SIZE);
std::vector<std::function<void(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)>> TestUtils::gDrawFunctions = {
    TestUtils::OnDrawPoint, TestUtils::OnDrawPoints, TestUtils::OnDrawLine, TestUtils::OnDrawRect,
    TestUtils::OnDrawRegion, TestUtils::OnDrawOval, TestUtils::OnDrawRRect, TestUtils::OnDrawDRRect,
    TestUtils::OnDrawCircle, TestUtils::OnDrawArc, TestUtils::OnDrawPath, TestUtils::OnDrawImageRect,
    TestUtils::OnDrawImageNine, TestUtils::OnDrawImageLattice, TestUtils::OnDrawTextBlob, TestUtils::OnDrawVertices,
    TestUtils::OnDrawAtlas, TestUtils::OnDrawPatch};

void TestUtils::OnDrawPoint(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    pen.SetWidth(RECT_SIZE / 2); // 2: half of rect size
    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawPoint(Drawing::Point(RECT_SIZE / 2, RECT_SIZE / 2)); // 2: rect center
    playbackCanvas_->DetachPen();
}

void TestUtils::OnDrawPoints(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    Drawing::Point pts[2] = {Drawing::Point(25, 25), Drawing::Point(75, 75)};
    playbackCanvas_->DrawPoints(Drawing::PointMode::LINES_POINTMODE, sizeof(pts) / sizeof(pts[0]), pts);
}

void TestUtils::OnDrawLine(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    playbackCanvas_->DrawLine(Drawing::Point(10, 10), Drawing::Point(90, 90)); // 10: point pos, 90: point pos
}

void TestUtils::OnDrawRect(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    playbackCanvas_->DrawRect(DRAW_RECT);
}

void TestUtils::OnDrawRegion(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    Drawing::Region region;
    region.SetRect(DRAW_RECTI);
    playbackCanvas_->DrawRegion(region);
}

void TestUtils::OnDrawOval(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    playbackCanvas_->DrawOval(DRAW_RECT);
}

void TestUtils::OnDrawRRect(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    Drawing::RoundRect rrect = Drawing::RoundRect(DRAW_RECT, RECT_ROUND_RADIUS, RECT_ROUND_RADIUS);
    playbackCanvas_->DrawRoundRect(rrect);
}

void TestUtils::OnDrawDRRect(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    Drawing::RoundRect rrect = Drawing::RoundRect(DRAW_RECT, RECT_ROUND_RADIUS, RECT_ROUND_RADIUS);
    Drawing::RoundRect rrect2 = Drawing::RoundRect(Drawing::Rect(20, 20, RECT_SIZE - 20, RECT_SIZE - 20),
        RECT_ROUND_RADIUS, RECT_ROUND_RADIUS);
    playbackCanvas_->DrawNestedRoundRect(rrect, rrect2);
}

void TestUtils::OnDrawCircle(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    playbackCanvas_->DrawCircle(Drawing::Point(RECT_SIZE / 2, RECT_SIZE / 2), RECT_SIZE / 2); // 2: half
}

void TestUtils::OnDrawArc(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    playbackCanvas_->DrawArc(Drawing::Rect(15.0f, 15.0f, RECT_SIZE - 15.0f, RECT_SIZE - 15.0f), // 15.0: offset
        0, 180); // 180: angle
}

void TestUtils::OnDrawPath(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    pen.SetWidth(10.0f); // 10.0: pen width
    Drawing::Path path;
    path.MoveTo(0, 0);
    path.LineTo(RECT_SIZE, RECT_SIZE);
    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawPath(path);
    playbackCanvas_->DetachPen();
}

void TestUtils::OnDrawImageRect(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    Drawing::SamplingOptions sampling = Drawing::SamplingOptions(
        Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
    playbackCanvas_->DrawImageRect(image, DRAW_RECT, Drawing::Rect(5, 5, RECT_SIZE + 5, RECT_SIZE + 5), // 5: offset
        sampling, Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
}
  
void TestUtils::OnDrawImageNine(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    playbackCanvas_->DrawImageNine(&image, Drawing::RectI(30, 30, 60, 60), // 30: TopLeft pos, 60: BottomRight pos
        Drawing::Rect(0, 0, 90, 90), Drawing::FilterMode::LINEAR); // 90: BottomRight pos
}

void TestUtils::OnDrawImageLattice(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    Drawing::Lattice lattice;
    playbackCanvas_->DrawImageLattice(&image, lattice, DRAW_RECT, Drawing::FilterMode::LINEAR);
}

void TestUtils::OnDrawTextBlob(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS-Sans";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(name.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle(
        {Drawing::FontStyle::BOLD_WEIGHT, Drawing::FontStyle::NORMAL_WIDTH, Drawing::FontStyle::ITALIC_SLANT}));
    auto font = Drawing::Font(typeface, 40.f, 1.0f, 0.f); // 40: font size
    std::string text = "DDGR";
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromText(text.c_str(), text.size(), font);
    playbackCanvas_->DrawTextBlob(textBlob.get(), 0, 40); // 40: y position
}

void TestUtils::OnDrawVertices(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    Drawing::BuilderFlags flags = Drawing::BuilderFlags::HAS_COLORS_BUILDER_FLAG;
    int indexCount = 0;
    int vertCounts = 4;
    Drawing::Vertices::Builder builder(Drawing::VertexMode::TRIANGLEFAN_VERTEXMODE,
        vertCounts, indexCount, static_cast<uint32_t>(flags));
    
    Drawing::Point points[] = {{0, 0}, {100, 0}, {40, 40}, {0, 100}};
    Drawing::ColorQuad colors[] = {Drawing::Color::COLOR_RED, Drawing::Color::COLOR_BLUE,
        Drawing::Color::COLOR_YELLOW, Drawing::Color::COLOR_CYAN};
    if (memcpy_s(builder.Positions(), vertCounts * sizeof(Drawing::Point),
        points, vertCounts * sizeof(Drawing::Point)) != EOK) {
        return;
    }
    Drawing::Point* posPtr = builder.Positions();
    for (int i = 0; i < vertCounts; i++) {
        posPtr[i] += Drawing::Point(10, 10); // 10: point position
    }
    if (memcpy_s(builder.Colors(), vertCounts * sizeof(Drawing::ColorQuad),
        colors, vertCounts * sizeof(Drawing::ColorQuad)) != EOK) {
        return;
    }
    playbackCanvas_->DrawVertices(*builder.Detach(), Drawing::BlendMode::SRC);
}

void TestUtils::OnDrawAtlas(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    brush.SetAntiAlias(true);
    int count = 1;
    Drawing::RSXform rsxform[] = {Drawing::RSXform::Make(2, 0, 0, 0)};
    Drawing::Rect tex[] = {{0, 0, 50, 50}};
    Drawing::ColorQuad colors[] = {0x7F55AA00};
    playbackCanvas_->DrawAtlas(&image, rsxform, tex, colors, count, Drawing::BlendMode::SRC_IN,
        Drawing::SamplingOptions(Drawing::FilterMode::NEAREST), nullptr);
}

void TestUtils::OnDrawPatch(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
    Drawing::Image& image)
{
    Drawing::Point cubics[12] = {{20, 20}, {40, 0}, {80, 40}, {100, 20},
        {80, 40}, {120, 80}, {100, 100}, {80, 80},
        {40, 120}, {20, 100}, {0, 80}, {40, 40}};
    Drawing::ColorQuad colors[4] = {Drawing::Color::COLOR_BLUE, Drawing::Color::COLOR_YELLOW,
        Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_RED };
    Drawing::Point texCoords[4] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
    playbackCanvas_->DrawPatch(cubics, colors, texCoords, Drawing::BlendMode::PLUS);
}
} // namespace Rosen
} // namespace OHOS