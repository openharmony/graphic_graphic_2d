/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include <cstddef>

#include "gtest/gtest.h"
#include "skia_adapter/skia_text_blob.h"

#include "text/rs_xform.h"
#include "text/text_blob.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaTextBlobTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaTextBlobTest::SetUpTestCase() {}
void SkiaTextBlobTest::TearDownTestCase() {}
void SkiaTextBlobTest::SetUp() {}
void SkiaTextBlobTest::TearDown() {}

/**
 * @tc.name: MakeFromText001
 * @tc.desc: Test MakeFromText
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTextBlobTest, MakeFromText001, TestSize.Level1)
{
    const char* str = "asdf";
    Font font;
    font.SetSize(100);
    auto textblob = SkiaTextBlob::MakeFromText(str, strlen(str), font, TextEncoding::UTF8);
    ASSERT_TRUE(textblob != nullptr);
}

/**
 * @tc.name: MakeFromRSXform001
 * @tc.desc: Test MakeFromRSXform
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTextBlobTest, MakeFromRSXform001, TestSize.Level1)
{
    const char* str = "asdf";
    Font font;
    font.SetSize(100);
    RSXform xform[] = { RSXform::Make(10, 10, 10, 10) }; // 10: cos, sin, tx, ty
    auto textblob = SkiaTextBlob::MakeFromRSXform(str, strlen(str), xform, font, TextEncoding::UTF8);
    ASSERT_TRUE(textblob != nullptr);
}

/**
 * @tc.name: MakeFromPosText001
 * @tc.desc: Test MakeFromPosText
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTextBlobTest, MakeFromPosText001, TestSize.Level1)
{
    const char* str = "as";
    Font font;
    font.SetSize(100);
    Point p1 { 0, 0 };
    Point p2 { 1, 1 };
    Point pos[] = { p1, p2 };
    auto textblob = SkiaTextBlob::MakeFromPosText(str, strlen(str), pos, font, TextEncoding::UTF8);
    ASSERT_TRUE(textblob != nullptr);
}

/**
 * @tc.name: GetDrawingGlyphIDforTextBlob001
 * @tc.desc: Test GetDrawingGlyphIDforTextBlob
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTextBlobTest, GetDrawingGlyphIDforTextBlob001, TestSize.Level1)
{
    const char* str = "asdf";
    Font font;
    font.SetSize(100);
    auto textblob = SkiaTextBlob::MakeFromText(str, strlen(str), font, TextEncoding::UTF8);
    ASSERT_TRUE(textblob != nullptr);
    std::vector<uint16_t> glyphIds { 0, 1 };
    SkiaTextBlob::GetDrawingGlyphIDforTextBlob(textblob.get(), glyphIds);
}

/**
 * @tc.name: GetDrawingPathforTextBlob001
 * @tc.desc: Test GetDrawingPathforTextBlob
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTextBlobTest, GetDrawingPathforTextBlob001, TestSize.Level1)
{
    uint16_t glyphId = 1;
    const char* str = "asdf";
    Font font;
    font.SetSize(100);
    RSXform xform[] = {RSXform::Make(10, 10, 10, 10)};
    auto blob = SkiaTextBlob::MakeFromRSXform(str, strlen(str), xform, font, TextEncoding::UTF8);
    ASSERT_TRUE(blob != nullptr);
    auto path = SkiaTextBlob::GetDrawingPathforTextBlob(glyphId, blob.get());
    ASSERT_TRUE(path.IsValid() == false);
}

/**
 * @tc.name: GetDrawingPointsForTextBlob001
 * @tc.desc: Test GetDrawingPointsForTextBlob
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTextBlobTest, GetDrawingPointsForTextBlob001, TestSize.Level1)
{
    const char* str = "asdf";
    Font font;
    font.SetSize(100);
    Point p1 { 0, 0 };
    Point p2 { 1, 1 };
    Point pos[] = { p1, p2 };
    auto blob = SkiaTextBlob::MakeFromPosText(str, strlen(str), pos, font, TextEncoding::UTF8);
    ASSERT_TRUE(blob != nullptr);
    std::vector<Point> points;
    SkiaTextBlob::GetDrawingPointsForTextBlob(blob.get(), points);
    ASSERT_TRUE(points[0].GetX() == 0);
    ASSERT_TRUE(points[0].GetY() == 0);
    ASSERT_TRUE(points[1].GetX() == 1);
    ASSERT_TRUE(points[1].GetY() == 1);
}

/**
 * @tc.name: GetIntercepts001
 * @tc.desc: Test GetIntercepts
 * @tc.type: FUNC
 * @tc.require:IAKGJ7
 */
HWTEST_F(SkiaTextBlobTest, GetIntercepts001, TestSize.Level1)
{
    const char* str = "asdf";
    Font font;
    font.SetSize(100);
    Point p1 { 100, 100 }; // 100, 100 means point coordinate
    Point p2 { 300, 300 }; // 300, 300 means point coordinate
    Point pos[] = { p1, p2 };
    auto textBlob = SkiaTextBlob::MakeFromPosText(str, strlen(str), pos, font, TextEncoding::UTF8);
    ASSERT_TRUE(textBlob != nullptr);
    auto skiaBlobImpl = textBlob->GetImpl<SkiaTextBlob>();
    ASSERT_TRUE(skiaBlobImpl != nullptr);
    float bounds[2] = {100, 300}; // 100, 300 means lower and upper line parallel to the advance
    float intervals[5] = {0.f}; // 5 means intervals array size
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    paint.SetPathEffect(PathEffect::CreateCornerPathEffect(10)); // 10 means radius
    int retIntercepts = 0;
    retIntercepts = skiaBlobImpl->GetIntercepts(bounds, intervals, &paint);
    ASSERT_TRUE(retIntercepts != 0);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS