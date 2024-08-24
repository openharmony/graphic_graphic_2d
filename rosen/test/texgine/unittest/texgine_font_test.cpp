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

#include "texgine_font.h"

using namespace testing;
using namespace testing::ext;

void SkFont::setSize(SkScalar size)
{
}

SkScalar SkFont::getMetrics(SkFontMetrics *metrics) const
{
    return 1.0;     // set getMetrics return value
}

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineFontTest : public testing::Test {
};

/**
 * @tc.name:SetTypeface
 * @tc.desc: Verify the SetTypeface
 * @tc.type:FUNC
 */
HWTEST_F(TexgineFontTest, SetTypeface, TestSize.Level1)
{
    std::shared_ptr<TexgineFont> tf = std::make_shared<TexgineFont>();
    std::shared_ptr<TexgineTypeface> tt1 = nullptr;
    std::shared_ptr<TexgineTypeface> tt2 = std::make_shared<TexgineTypeface>();
    EXPECT_NO_THROW({
        tf->SetTypeface(tt1);
    tf->SetTypeface(tt2);
    });
}

/**
 * @tc.name:SetSize
 * @tc.desc: Verify the SetSize
 * @tc.type:FUNC
 */
HWTEST_F(TexgineFontTest, SetSize, TestSize.Level1)
{
    std::shared_ptr<TexgineFont> tf = std::make_shared<TexgineFont>();
    EXPECT_NO_THROW({
        tf->SetSize(0.0);
    });
}

/**
 * @tc.name:GetMetrics
 * @tc.desc: Verify the GetMetrics
 * @tc.type:FUNC
 */
HWTEST_F(TexgineFontTest, GetMetrics, TestSize.Level1)
{
    std::shared_ptr<TexgineFont> tf = std::make_shared<TexgineFont>();
    std::shared_ptr<TexgineFontMetrics> tfm = std::make_shared<TexgineFontMetrics>();
    EXPECT_NO_THROW({
        EXPECT_EQ(tf->GetMetrics(nullptr), 0.0);
        EXPECT_EQ(tf->GetMetrics(tfm), 1.0);
    });
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
