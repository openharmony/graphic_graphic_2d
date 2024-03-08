/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>

#include "mock/mock_any_span.h"
#include "param_test_macros.h"
#include "texgine/typography_types.h"
#include "variant_span.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct Mockvars {
    double tsWidth = 0;
    double tsHeight = 0;
    int calledTimesPaint = 0;
    int calledTimesPaintShadow = 0;
    TexgineCanvas *catchedPaintCanvas = nullptr;
    double catchedPaintOffsetX = 0;
    double catchedPaintOffsetY = 0;
    TexgineCanvas *catchedPaintShadowCanvas = nullptr;
    double catchedPaintShadowOffsetX = 0;
    double catchedPaintShadowOffsetY = 0;
} g_variantMockvars;

void InitMockvars(struct Mockvars &&vars)
{
    g_variantMockvars = std::move(vars);
}

double TextSpan::GetWidth() const
{
    return g_variantMockvars.tsWidth;
}

double TextSpan::GetHeight() const
{
    return g_variantMockvars.tsHeight;
}

void TextSpan::Paint(TexgineCanvas &canvas, double offsetX, double offsetY, const TextStyle &xs,
    const RoundRectType &rType)
{
    g_variantMockvars.calledTimesPaint++;
    g_variantMockvars.catchedPaintCanvas = &canvas;
    g_variantMockvars.catchedPaintOffsetX = offsetX;
    g_variantMockvars.catchedPaintOffsetY = offsetY;
}

void TextSpan::PaintShadow(TexgineCanvas &canvas, double offsetX, double offsetY,
    const std::vector<TextShadow> &shadows)
{
    g_variantMockvars.calledTimesPaintShadow++;
    g_variantMockvars.catchedPaintShadowCanvas = &canvas;
    g_variantMockvars.catchedPaintShadowOffsetX = offsetX;
    g_variantMockvars.catchedPaintShadowOffsetY = offsetY;
}

class VariantSpanTest : public testing::Test {
public:
    VariantSpanTest()
    {
        struct AttackStruct {
            std::shared_ptr<TextSpan> ts_ = nullptr;
            std::shared_ptr<AnySpan> as_ = nullptr;
        } *p = reinterpret_cast<struct AttackStruct *>(&span0);
        p->as_ = as;
        p->ts_ = ts;
    }

    std::shared_ptr<TextSpan> tsNullptr = nullptr;
    std::shared_ptr<AnySpan> asNullptr = nullptr;
    std::shared_ptr<TextSpan> ts = TextSpan::MakeFromText("");
    std::shared_ptr<MockAnySpan> as = std::make_shared<MockAnySpan>();
    VariantSpan span0;
    VariantSpan span1 = nullptr;
    VariantSpan span2 = tsNullptr;
    VariantSpan span3 = asNullptr;
    VariantSpan span4 = ts;
    VariantSpan span5 = as;
};

/**
 * @tc.name: TryToTextAndAnySpan
 * @tc.desc: Verify the TryToTextAndAnySpan
 * @tc.type:FUNC
 */
HWTEST_F(VariantSpanTest, TryToTextAndAnySpan, TestSize.Level1)
{
    ASSERT_EQ(span1.TryToTextSpan(), nullptr);
    ASSERT_EQ(span2.TryToTextSpan(), nullptr);
    ASSERT_EQ(span3.TryToTextSpan(), nullptr);
    ASSERT_EQ(span4.TryToTextSpan(), ts);
    ASSERT_EQ(span5.TryToTextSpan(), nullptr);

    ASSERT_EQ(span1.TryToAnySpan(), nullptr);
    ASSERT_EQ(span2.TryToAnySpan(), nullptr);
    ASSERT_EQ(span3.TryToAnySpan(), nullptr);
    ASSERT_EQ(span4.TryToAnySpan(), nullptr);
    ASSERT_EQ(span5.TryToAnySpan(), as);
}

/**
 * @tc.name: GetWidthAndHeight
 * @tc.desc: Verify the GetWidthAndHeight
 * @tc.type:FUNC
 */
HWTEST_F(VariantSpanTest, GetWidthAndHeight, TestSize.Level1)
{
    InitMockvars({.tsWidth = 8, .tsHeight = 4});
    EXPECT_CALL(*as, GetWidth).Times(1).WillOnce(testing::Return(2));
    EXPECT_CALL(*as, GetHeight).Times(1).WillOnce(testing::Return(1));

    ASSERT_EXCEPTION(ExceptionType::ERROR_STATUS, span0.GetWidth());
    ASSERT_EXCEPTION(ExceptionType::NULLPTR, span1.GetWidth());
    ASSERT_EXCEPTION(ExceptionType::NULLPTR, span2.GetWidth());
    ASSERT_EXCEPTION(ExceptionType::NULLPTR, span3.GetWidth());
    ASSERT_EQ(span4.GetWidth(), 8);
    ASSERT_EQ(span5.GetWidth(), 2);

    ASSERT_EXCEPTION(ExceptionType::ERROR_STATUS, span0.GetHeight());
    ASSERT_EXCEPTION(ExceptionType::NULLPTR, span1.GetHeight());
    ASSERT_EXCEPTION(ExceptionType::NULLPTR, span2.GetHeight());
    ASSERT_EXCEPTION(ExceptionType::NULLPTR, span3.GetHeight());
    ASSERT_EQ(span4.GetHeight(), 4);
    ASSERT_EQ(span5.GetHeight(), 1);
}

/**
 * @tc.name: PaintAndPaintShadow
 * @tc.desc: Verify the PaintAndPaintShadow
 * @tc.type:FUNC
 */
HWTEST_F(VariantSpanTest, PaintAndPaintShadow, TestSize.Level1)
{
    TexgineCanvas canvas, canvas1, canvas2, canvas3, canvas4;
    int paintX1 = 2048;
    int paintX2 = 512;
    int paintY1 = 4096;
    int paintY2 = 1024;
    InitMockvars({});
    TexgineCanvas *pcanvas = nullptr;
    double asX = 0;
    double asY = 0;
    EXPECT_CALL(*as, Paint).Times(1).WillOnce([&](TexgineCanvas &canvas, double offsetX, double offsetY) {
        pcanvas = &canvas;
        asX = offsetX;
        asY = offsetY;
    });

    ASSERT_EXCEPTION(ExceptionType::ERROR_STATUS, span0.Paint(canvas, 0, 0));
    ASSERT_EXCEPTION(ExceptionType::NULLPTR, span1.Paint(canvas, 0, 0));
    ASSERT_EXCEPTION(ExceptionType::NULLPTR, span2.Paint(canvas, 0, 0));
    ASSERT_EXCEPTION(ExceptionType::NULLPTR, span3.Paint(canvas, 0, 0));
    EXPECT_NO_THROW({
        ASSERT_EQ(g_variantMockvars.calledTimesPaint, 0);
        span4.Paint(canvas2, paintX1, paintY1);
        ASSERT_EQ(g_variantMockvars.calledTimesPaint, 1);
        ASSERT_EQ(g_variantMockvars.catchedPaintCanvas, &canvas2);
        ASSERT_EQ(g_variantMockvars.catchedPaintOffsetX, paintX1);
        ASSERT_EQ(g_variantMockvars.catchedPaintOffsetY, paintY1);
    });
    EXPECT_NO_THROW({
        span5.Paint(canvas1, paintX2, paintY2);
        ASSERT_EQ(pcanvas, &canvas1);
        ASSERT_EQ(asX, paintX2);
        ASSERT_EQ(asY, paintY2);
    });

    ASSERT_EXCEPTION(ExceptionType::ERROR_STATUS, span0.PaintShadow(canvas, 0, 0));
    ASSERT_EXCEPTION(ExceptionType::NULLPTR, span1.PaintShadow(canvas, 0, 0));
    ASSERT_EXCEPTION(ExceptionType::NULLPTR, span2.PaintShadow(canvas, 0, 0));
    ASSERT_EXCEPTION(ExceptionType::NULLPTR, span3.PaintShadow(canvas, 0, 0));
    EXPECT_NO_THROW({
        ASSERT_EQ(g_variantMockvars.calledTimesPaintShadow, 0);
        span4.PaintShadow(canvas4, 32768, 65536);
        ASSERT_EQ(g_variantMockvars.calledTimesPaintShadow, 1);
        ASSERT_EQ(g_variantMockvars.catchedPaintShadowCanvas, &canvas4);
        ASSERT_EQ(g_variantMockvars.catchedPaintShadowOffsetX, 32768);
        ASSERT_EQ(g_variantMockvars.catchedPaintShadowOffsetY, 65536);
    });
    EXPECT_NO_THROW({span5.PaintShadow(canvas3, 8192, 16384);});
}

/**
 * @tc.name: Operator
 * @tc.desc: Verify the Operator
 * @tc.type:FUNC
 */
HWTEST_F(VariantSpanTest, Operator, TestSize.Level1)
{
    ASSERT_EXCEPTION(ExceptionType::ERROR_STATUS, (void)(bool)span0);
    ASSERT_FALSE(span1);
    ASSERT_FALSE(span2);
    ASSERT_FALSE(span3);
    ASSERT_TRUE(span4);
    ASSERT_TRUE(span5);

    ASSERT_EXCEPTION(ExceptionType::ERROR_STATUS, (void)(span0 == nullptr));
    ASSERT_EXCEPTION(ExceptionType::ERROR_STATUS, (void)(span0 != nullptr));
    ASSERT_EQ(span1, nullptr);
    ASSERT_EQ(span2, nullptr);
    ASSERT_EQ(span3, nullptr);
    ASSERT_NE(span4, nullptr);
    ASSERT_NE(span5, nullptr);

    ASSERT_EXCEPTION(ExceptionType::ERROR_STATUS, (void)(span0 == span1));
    ASSERT_EXCEPTION(ExceptionType::ERROR_STATUS, (void)(span0 != span1));
    ASSERT_EQ(span1, span1);
    ASSERT_EQ(span1, span2);
    ASSERT_EQ(span1, span3);
    ASSERT_NE(span1, span4);
    ASSERT_NE(span1, span5);

    ASSERT_NE(span4, span5);

    auto span6 = span4;
    ASSERT_EQ(span6, span4);

    auto span7 = span5;
    ASSERT_EQ(span7, span5);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
