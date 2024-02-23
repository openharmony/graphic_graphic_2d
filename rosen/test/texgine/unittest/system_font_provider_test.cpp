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
#include <gtest/gtest.h>

#include "param_test_macros.h"
#include "texgine_data.h"
#include "texgine_font_manager.h"
#include "texgine_stream.h"
#include "texgine/system_font_provider.h"
#include "texgine_typeface.h"
#include "variant_font_style_set.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct MockVars {
    std::shared_ptr<TexgineFontStyleSet> fontStyleSet = nullptr;
    std::shared_ptr<TexgineFontManager> fontMgr = std::make_shared<TexgineFontManager>();
} g_sfpMockvars;

void InitSfpMockVars(MockVars vars)
{
    g_sfpMockvars = std::move(vars);
}

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<TexgineFontManager> TexgineFontManager::RefDefault()
{
    return g_sfpMockvars.fontMgr;
}
#endif

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<TexgineFontStyleSet> TexgineFontManager::MatchFamily(const std::string &familyName)
{
    return g_sfpMockvars.fontStyleSet;
}
#endif

class SystemFontProviderTest : public testing::Test {
public:
    std::shared_ptr<SystemFontProvider> systemFontProvider = SystemFontProvider::GetInstance();
};

/**
 * @tc.name: GetInstance
 * @tc.desc: Verify the GetInstance
 * @tc.type:FUNC
 * @tc.require: issueI6V6JD
 */
HWTEST_F(SystemFontProviderTest, GetInstance, TestSize.Level1)
{
    EXPECT_NE(SystemFontProvider::GetInstance(), nullptr);
}

/**
 * @tc.name: MatchFamily1
 * @tc.desc: Verify the MatchFamily
 * @tc.type:FUNC
 * @tc.require: issueI6V6JD
 */
HWTEST_F(SystemFontProviderTest, MatchFamily1, TestSize.Level1)
{
    InitSfpMockVars({.fontMgr = nullptr});
    auto fss = systemFontProvider->MatchFamily("");
    EXPECT_NE(fss, nullptr);
}

/**
 * @tc.name: MatchFamily2
 * @tc.desc: Verify the MatchFamily
 * @tc.type:FUNC
 * @tc.require: issueI6V6JD
 */
HWTEST_F(SystemFontProviderTest, MatchFamily2, TestSize.Level1)
{
    InitSfpMockVars({});
    auto fss = systemFontProvider->MatchFamily("");
    EXPECT_NE(fss, nullptr);
    EXPECT_NE(fss->TryToTexgineFontStyleSet(), nullptr);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
