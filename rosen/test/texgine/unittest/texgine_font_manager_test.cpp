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

#include "texgine_font_manager.h"
#include "texgine_font_style_set.h"

using namespace testing;
using namespace testing::ext;

struct MockVars {
    sk_sp<SkTypeface> skTypeface_ = SkTypeface::MakeDefault();
    std::shared_ptr<SkFontStyle> skFontStyle_ = std::make_shared<SkFontStyle>();
    std::shared_ptr<OHOS::Rosen::TextEngine::TexgineFontStyle> texgineFontStyle_ =
        std::make_shared<OHOS::Rosen::TextEngine::TexgineFontStyle>();
    std::shared_ptr<OHOS::Rosen::TextEngine::TexgineFontStyleSet> texgineFontStyleSet_ =
        OHOS::Rosen::TextEngine::TexgineFontStyleSet::CreateEmpty();
} g_tfmMockvars;

void InitTfmMockVars(struct MockVars &&vars)
{
    g_tfmMockvars = std::move(vars);
}

SkTypeface* SkFontMgr::matchFamilyStyleCharacter(const char familyName[], const SkFontStyle& style,
                                                 const char* bcp47[], int bcp47Count,
                                                 SkUnichar character) const
{
    return g_tfmMockvars.skTypeface_.get();
}

SkFontStyleSet *SkFontMgr::matchFamily(const char familyName[]) const
{
    return g_tfmMockvars.texgineFontStyleSet_->GetFontStyleSet();
}

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineFontManagerTest : public testing::Test {
};

/**
 * @tc.name:MatchFamilyStyleCharacter
 * @tc.desc: Verify the MatchFamilyStyleCharacter
 * @tc.type:FUNC
 */
HWTEST_F(TexgineFontManagerTest, MatchFamilyStyleCharacter, TestSize.Level1)
{
    InitTfmMockVars({});
    std::string str = "";
    std::shared_ptr<TexgineFontManager> tfm1 = std::make_shared<TexgineFontManager>();
    tfm1->SetFontMgr(nullptr);
    EXPECT_EQ(tfm1->MatchFamilyStyleCharacter(str, *g_tfmMockvars.texgineFontStyle_, nullptr, 0, 0), nullptr);

    std::shared_ptr<TexgineFontManager> tfm2 = TexgineFontManager::RefDefault();
    g_tfmMockvars.texgineFontStyle_->SetFontStyle(nullptr);
    EXPECT_EQ(tfm2->MatchFamilyStyleCharacter(str, *g_tfmMockvars.texgineFontStyle_, nullptr, 0, 0), nullptr);

    InitTfmMockVars({});
    std::shared_ptr<TexgineFontManager> tfm3 = TexgineFontManager::RefDefault();
    auto tf = tfm3->MatchFamilyStyleCharacter(str, *g_tfmMockvars.texgineFontStyle_, nullptr, 0, 0);
    EXPECT_EQ(tf->GetTypeface().get(), g_tfmMockvars.skTypeface_.get());
}

/**
 * @tc.name:MatchFamily
 * @tc.desc: Verify the MatchFamily
 * @tc.type:FUNC
 */
HWTEST_F(TexgineFontManagerTest, MatchFamily, TestSize.Level1)
{
    InitTfmMockVars({});
    std::string str = "";
    std::shared_ptr<TexgineFontManager> tfm1 = std::make_shared<TexgineFontManager>();
    tfm1->SetFontMgr(nullptr);
    EXPECT_EQ(tfm1->MatchFamily(str), nullptr);

    InitTfmMockVars({});
    std::shared_ptr<TexgineFontManager> tfm2 = TexgineFontManager::RefDefault();
    auto tfss = tfm2->MatchFamily(str);
    EXPECT_EQ(tfss->GetFontStyleSet(), g_tfmMockvars.texgineFontStyleSet_->GetFontStyleSet());
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
