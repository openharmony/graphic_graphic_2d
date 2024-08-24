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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "texgine_paint.h"

using namespace testing;
using namespace testing::ext;

void SkPaint::setColor(SkColor color)
{
}

void SkPaint::setAlphaf(float a)
{
}

void SkPaint::setStrokeWidth(SkScalar width)
{
}

void SkPaint::setARGB(U8CPU a, U8CPU r, U8CPU g, U8CPU b)
{
}

void SkPaint::setStyle(Style style)
{
}

void SkPaint::setPathEffect(sk_sp<SkPathEffect> pathEffect)
{
}

void SkPaint::setMaskFilter(sk_sp<SkMaskFilter> maskFilter)
{
}

void SkPaint::setBlendMode(SkBlendMode mode)
{
}

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexginePaintTest : public testing::Test {
};

/**
 * @tc.name:SetPaint
 * @tc.desc: Verify the SetPaint
 * @tc.type:FUNC
 */
HWTEST_F(TexginePaintTest, SetPaint, TestSize.Level1)
{
    std::shared_ptr<TexginePaint> tp = std::make_shared<TexginePaint>();
    std::shared_ptr<RSBrush> brush = std::make_shared<RSBrush>();
    std::shared_ptr<RSPen> pen = std::make_shared<RSPen>();
    EXPECT_NO_THROW({
        tp->SetBrush(*brush);
        EXPECT_EQ(tp->GetBrush(), *brush);
    });
    EXPECT_NO_THROW({
        tp->SetPen(*pen);
        EXPECT_EQ(tp->GetPen(), *pen);
    });
}

/**
 * @tc.name:SetFunctions
 * @tc.desc: Verify the SetFunctions
 * @tc.type:FUNC
 */
HWTEST_F(TexginePaintTest, SetFunctions, TestSize.Level1)
{
    std::shared_ptr<TexginePaint> tp = std::make_shared<TexginePaint>();
    std::shared_ptr<TexginePathEffect> tpe = std::make_shared<TexginePathEffect>();
    std::shared_ptr<TexgineMaskFilter> tmf = std::make_shared<TexgineMaskFilter>();
    EXPECT_NO_THROW({
        tp->SetColor(0);
        tp->SetAlphaf(0);
        tp->SetStrokeWidth(0);
        tp->SetAntiAlias(true);
        tp->SetARGB(0, 0, 0, 0);
        tp->SetStyle(TexginePaint::Style::FILL);
        tp->SetPathEffect(tpe);
        tpe = nullptr;
        tp->SetPathEffect(tpe);
        tp->SetMaskFilter(tmf);
        tmf = nullptr;
        tp->SetMaskFilter(tmf);
        tp->SetAlpha(0);
        tp->SetBlendMode(TexginePaint::TexgineBlendMode::CLEAR);
    });
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
