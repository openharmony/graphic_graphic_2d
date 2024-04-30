/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "drawing_painter_impl.h"
#include "recording_canvas.h"
#include "text_blob_builder.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::Drawing;
using namespace OHOS::Rosen::SPText;
using namespace OHOS::Rosen::TextEngine;
using namespace skia::textlayout;

namespace txt {
class RSCanvasParagraphPainterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static bool AnimationFunc(const std::shared_ptr<SymbolAnimationConfig>& symbolAnimationConfig);
    static inline std::shared_ptr<RSCanvasParagraphPainter> canvasParagraphPainter_ = nullptr;
    static inline std::vector<PaintRecord> paintRecords_;
    static inline std::shared_ptr<RecordingCanvas> recordingCanvas_ = nullptr;
};

void RSCanvasParagraphPainterTest::SetUpTestCase()
{
    Brush rsBrush;
    Pen rsPen;
    PaintRecord::RSColor color(0, 0, 0, 255); // 255 just fot test
    rsBrush.SetColor(color);
    rsPen.SetColor(color);
    std::optional<RSBrush> brush(rsBrush);
    std::optional<RSPen> pen(rsPen);
    PaintRecord paintRecord(brush, pen);
    paintRecords_.push_back(paintRecord);

    int32_t width = 200; // 200 just for test
    int32_t height = 100; // 100 just for test
    recordingCanvas_ = std::make_shared<RecordingCanvas>(width, height);
    if (!recordingCanvas_) {
        std::cout << "RSCanvasParagraphPainterTest::SetUpTestCase error recordingCanvas_ is nullptr"
            << std::endl;
        return;
    }
    canvasParagraphPainter_ = std::make_shared<RSCanvasParagraphPainter>(recordingCanvas_.get(), paintRecords_);
    if (!canvasParagraphPainter_) {
        std::cout << "RSCanvasParagraphPainterTest::SetUpTestCase error canvasParagraphPainter_ is nullptr"
            << std::endl;
    }
}

void RSCanvasParagraphPainterTest::TearDownTestCase()
{
}

bool RSCanvasParagraphPainterTest::AnimationFunc(const std::shared_ptr<SymbolAnimationConfig>& symbolAnimationConfig)
{
    return true;
}

/*
 * @tc.name: RSCanvasParagraphPainterTest001
 * @tc.desc: test for drawTextBlob
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest001, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);
    std::string str("TextBlob");
    Font font;
    std::shared_ptr<TextBlob> textBlob = TextBlob::MakeFromText(str.c_str(), str.length(), font);
    EXPECT_EQ(textBlob != nullptr, true);
    ParagraphPainter::PaintID paintID = static_cast<int>(paintRecords_.size()) - 1;
    ParagraphPainter::SkPaintOrID paint;
    paint.emplace<ParagraphPainter::PaintID>(paintID);
    canvasParagraphPainter_->drawTextBlob(textBlob, 0.0, 0.0, paint);
}

/*
 * @tc.name: RSCanvasParagraphPainterTest002
 * @tc.desc: test for drawTextShadow
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest002, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);
    std::string str("drawTextShadow");
    Font font;
    std::shared_ptr<TextBlob> textBlob = TextBlob::MakeFromText(str.c_str(), str.length(), font);
    EXPECT_EQ(textBlob != nullptr, true);
    SkColor color = 255; // 255 just fot test
    canvasParagraphPainter_->drawTextShadow(textBlob, 0.0, 0.0, color, 0.5); // 0.5 just fot test
}

/*
 * @tc.name: RSCanvasParagraphPainterTest003
 * @tc.desc: test for drawRect
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest003, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);
    ParagraphPainter::PaintID paintID = static_cast<int>(paintRecords_.size()) - 1;
    ParagraphPainter::SkPaintOrID paint;
    paint.emplace<ParagraphPainter::PaintID>(paintID);
    SkRect rect = SkRect::MakeEmpty();
    canvasParagraphPainter_->drawRect(rect, paint);
}

/*
 * @tc.name: RSCanvasParagraphPainterTest004
 * @tc.desc: test for drawRRect
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest004, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);
    SkRRect rect = SkRRect::MakeEmpty();
    SkColor color = 255; // 255 just fot test
    canvasParagraphPainter_->drawRRect(rect, color);
}

/*
 * @tc.name: RSCanvasParagraphPainterTest005
 * @tc.desc: test for drawFilledRect
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest005, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);
    SkRect rect = SkRect::MakeEmpty();
    ParagraphPainter::DecorationStyle decorStyle;
    canvasParagraphPainter_->drawFilledRect(rect, decorStyle);
}

/*
 * @tc.name: RSCanvasParagraphPainterTest006
 * @tc.desc: test for drawPath
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest006, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);
    ParagraphPainter::DecorationStyle decorStyle;
    Path path;
    canvasParagraphPainter_->drawPath(path, decorStyle);
}

/*
 * @tc.name: RSCanvasParagraphPainterTest007
 * @tc.desc: test for drawLine
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest007, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);
    ParagraphPainter::DecorationStyle decorStyle;
    canvasParagraphPainter_->drawLine(0.0, 0.0, 0.0, 0.0, decorStyle);
}

/*
 * @tc.name: RSCanvasParagraphPainterTest008
 * @tc.desc: test for SymbolAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest008, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);
    PaintRecord paintRecord;
    canvasParagraphPainter_->SymbolAnimation(paintRecord);
}

/*
 * @tc.name: RSCanvasParagraphPainterTest009
 * @tc.desc: test for DrawSymbolSkiaTxt
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest009, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);

    PaintRecord paintRecord;
    Point offset;
    std::string str("DrawSymbolSkiaTxt");
    Font font;
    std::shared_ptr<TextBlob> textBlob = TextBlob::MakeFromText(str.c_str(), str.length(), font);
    EXPECT_EQ(textBlob != nullptr, true);
    canvasParagraphPainter_->DrawSymbolSkiaTxt(textBlob.get(), offset, paintRecord);
}

/*
 * @tc.name: RSCanvasParagraphPainterTest010
 * @tc.desc: test for clipRect
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest010, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);
    SkRect rect = SkRect::MakeEmpty();
    canvasParagraphPainter_->clipRect(rect);
}

/*
 * @tc.name: RSCanvasParagraphPainterTest011
 * @tc.desc: test for translate
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest011, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);
    canvasParagraphPainter_->translate(0.0, 0.0);
}

/*
 * @tc.name: RSCanvasParagraphPainterTest012
 * @tc.desc: test for save
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest012, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);
    canvasParagraphPainter_->save();
}

/*
 * @tc.name: RSCanvasParagraphPainterTest013
 * @tc.desc: test for restore
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest013, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);
    canvasParagraphPainter_->restore();
}

/*
 * @tc.name: RSCanvasParagraphPainterTest014
 * @tc.desc: test for SetAnimation
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest014, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);
    std::function<bool(const std::shared_ptr<SymbolAnimationConfig>&)> animationFunc =
        RSCanvasParagraphPainterTest::AnimationFunc;
    canvasParagraphPainter_->SetAnimation(animationFunc);
    EXPECT_EQ(canvasParagraphPainter_->animationFunc_ != nullptr, true);
}

/*
 * @tc.name: RSCanvasParagraphPainterTest015
 * @tc.desc: test for SetParagraphId
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasParagraphPainterTest, RSCanvasParagraphPainterTest015, TestSize.Level1)
{
    EXPECT_EQ(canvasParagraphPainter_ != nullptr, true);
    EXPECT_EQ(recordingCanvas_ != nullptr, true);
    canvasParagraphPainter_->SetParagraphId(1);
    EXPECT_EQ(canvasParagraphPainter_->paragraphId_, 1);
}
} // namespace txt