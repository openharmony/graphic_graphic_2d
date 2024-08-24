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

#include "texgine_font_style_set.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineFontStyleSetTest : public testing::Test {
};

/**
 * @tc.name:Count
 * @tc.desc: Verify the Count
 * @tc.type:FUNC
 */
HWTEST_F(TexgineFontStyleSetTest, Count, TestSize.Level1)
{
    EXPECT_NO_THROW({
        std::shared_ptr<TexgineFontStyleSet> tfss1 =
            std::make_shared<TexgineFontStyleSet>(nullptr);
        tfss1->Count();

        std::shared_ptr<TexgineFontStyleSet> tfss2 =
            std::make_shared<TexgineFontStyleSet>(RSFontStyleSet::CreateEmpty());
        tfss2->Count();
    });
}

/**
 * @tc.name:GetStyle
 * @tc.desc: Verify the GetStyle
 * @tc.type:FUNC
 */
HWTEST_F(TexgineFontStyleSetTest, GetStyle, TestSize.Level1)
{
    std::shared_ptr<TexgineFontStyle> tfs = std::make_shared<TexgineFontStyle>();
    std::shared_ptr<TexgineString> name = std::make_shared<TexgineString>();
    EXPECT_NO_THROW({
        std::shared_ptr<TexgineFontStyleSet> tfss1 =
            std::make_shared<TexgineFontStyleSet>(nullptr);
        tfss1->GetStyle(0, tfs, name);

        std::shared_ptr<TexgineFontStyleSet> tfss2 =
            std::make_shared<TexgineFontStyleSet>(RSFontStyleSet::CreateEmpty());
        tfss2->GetStyle(0, nullptr, name);

        std::shared_ptr<TexgineFontStyleSet> tfss3 =
            std::make_shared<TexgineFontStyleSet>(RSFontStyleSet::CreateEmpty());
        tfss3->GetStyle(0, tfs, nullptr);

        std::shared_ptr<TexgineFontStyleSet> tfss4 =
            std::make_shared<TexgineFontStyleSet>(RSFontStyleSet::CreateEmpty());
        tfss4->GetStyle(0, tfs, name);
    });
}

/**
 * @tc.name:CreateTypeface
 * @tc.desc: Verify the CreateTypeface
 * @tc.type:FUNC
 */
HWTEST_F(TexgineFontStyleSetTest, CreateTypeface, TestSize.Level1)
{
    EXPECT_NO_THROW({
        std::shared_ptr<TexgineFontStyleSet> tfss1 =
            std::make_shared<TexgineFontStyleSet>(nullptr);
        tfss1->CreateTypeface(0);

        std::shared_ptr<TexgineFontStyleSet> tfss2 =
            std::make_shared<TexgineFontStyleSet>(RSFontStyleSet::CreateEmpty());
        tfss2->CreateTypeface(0);
    });
}

/**
 * @tc.name:MatchStyle
 * @tc.desc: Verify the MatchStyle
 * @tc.type:FUNC
 */
HWTEST_F(TexgineFontStyleSetTest, MatchStyle, TestSize.Level1)
{
    std::shared_ptr<TexgineFontStyle> tfs = std::make_shared<TexgineFontStyle>();
    EXPECT_NO_THROW({
        std::shared_ptr<TexgineFontStyleSet> tfss1 =
            std::make_shared<TexgineFontStyleSet>(nullptr);
        tfss1->MatchStyle(tfs);

        std::shared_ptr<TexgineFontStyleSet> tfss2 =
            std::make_shared<TexgineFontStyleSet>(RSFontStyleSet::CreateEmpty());
        tfss2->MatchStyle(nullptr);

        std::shared_ptr<TexgineFontStyleSet> tfss3 =
            std::make_shared<TexgineFontStyleSet>(RSFontStyleSet::CreateEmpty());
        tfss3->MatchStyle(tfs);

        tfs->SetFontStyle(nullptr);
        std::shared_ptr<TexgineFontStyleSet> tfss4 =
            std::make_shared<TexgineFontStyleSet>(RSFontStyleSet::CreateEmpty());
        tfss4->MatchStyle(tfs);
    });
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
