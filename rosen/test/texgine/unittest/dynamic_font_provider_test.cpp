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

#include "dynamic_font_provider.h"

#include <gtest/gtest.h>
#include <iostream>

#include "dynamic_font_style_set.h"
#include "param_test_macros.h"
#include "texgine_exception.h"
#include "texgine_font_style_set.h"
#include "texgine_stream.h"
#include "texgine_typeface.h"
#include "typeface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
std::unique_ptr<TexgineMemoryStream> memoryStream = nullptr;
std::shared_ptr<TexgineTypeface> typeface = nullptr;

struct Style {
    std::unique_ptr<TexgineMemoryStream> memoryStream = std::make_unique<TexgineMemoryStream>();
    std::shared_ptr<TexgineTypeface> typeface = std::make_shared<TexgineTypeface>();
};

void InitMyMockVars(Style style)
{
    memoryStream = std::move(style.memoryStream);
    typeface = style.typeface;
}

#ifndef USE_ROSEN_DRAWING
std::unique_ptr<TexgineMemoryStream> TexgineMemoryStream::MakeCopy(const void *data, size_t length)
{
    return std::move(memoryStream);
}
#endif

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<TexgineTypeface> TexgineTypeface::MakeFromStream(
    std::unique_ptr<TexgineMemoryStream> stream, int index)
{
    return typeface;
}
#endif

class DynamicFontProviderTest : public testing::Test {
public:
    std::shared_ptr<DynamicFontProvider> dynamicFontProvider = DynamicFontProvider::Create();
};

/**
 * @tc.name: Create
 * @tc.desc: Verify the Create
 * @tc.type:FUNC
 * @tc.require: issueI6V6JP
 */
HWTEST_F(DynamicFontProviderTest, Create, TestSize.Level1)
{
    EXPECT_NE(DynamicFontProvider::Create(), nullptr);
}

/**
 * @tc.name: LoadFont1
 * @tc.desc: Verify the LoadFont
 * @tc.type:FUNC
 * @tc.require: issueI6V6JP
 */
HWTEST_F(DynamicFontProviderTest, LoadFont1, TestSize.Level1)
{
    InitMyMockVars({});
    // 4 is data length, "LF1" length, 1 is PARAMETERERROR
    EXPECT_NO_THROW({ EXPECT_EQ(dynamicFontProvider->LoadFont("LF1", nullptr, 4), 1); });
}

/**
 * @tc.name: LoadFont2
 * @tc.desc: Verify the LoadFont
 * @tc.type:FUNC
 * @tc.require: issueI6V6JP
 */
HWTEST_F(DynamicFontProviderTest, LoadFont2, TestSize.Level1)
{
    InitMyMockVars({});
    dynamicFontProvider = DynamicFontProvider::Create();
    EXPECT_NO_THROW({ EXPECT_EQ(dynamicFontProvider->LoadFont("LF2", this, 0), 1); });
}

/**
 * @tc.name: LoadFont3
 * @tc.desc: Verify the LoadFont
 * @tc.type:FUNC
 * @tc.require: issueI6V6JP
 */
HWTEST_F(DynamicFontProviderTest, LoadFont3, TestSize.Level1)
{
    InitMyMockVars({.memoryStream = nullptr});
    // APIERROR
    EXPECT_NO_THROW({ EXPECT_EQ(dynamicFontProvider->LoadFont("LF3", this, 4), 0); });
}

/**
 * @tc.name: LoadFont4
 * @tc.desc: Verify the LoadFont
 * @tc.type:FUNC
 * @tc.require: issueI6V6JP
 */
HWTEST_F(DynamicFontProviderTest, LoadFont4, TestSize.Level1)
{
    InitMyMockVars({.typeface = nullptr});
    EXPECT_NO_THROW({ EXPECT_EQ(dynamicFontProvider->LoadFont("LF4", this, 4), 0); });
}

/**
 * @tc.name: LoadFont5
 * @tc.desc: Verify the LoadFont
 * @tc.type:FUNC
 * @tc.require: issueI6V6JP
 */
HWTEST_F(DynamicFontProviderTest, LoadFont5, TestSize.Level1)
{
    InitMyMockVars({});
    EXPECT_NO_THROW({
        auto ret = dynamicFontProvider->LoadFont("LF5", this, 4);
        EXPECT_EQ(ret, 0);
    });
}

/**
 * @tc.name: MatchFamily1
 * @tc.desc: Verify the MatchFamily
 * @tc.type:FUNC
 * @tc.require: issueI6V6JP
 */
HWTEST_F(DynamicFontProviderTest, MatchFamily1, TestSize.Level1)
{
    EXPECT_EQ(dynamicFontProvider->MatchFamily("aaa"), nullptr);
}

/**
 * @tc.name: MatchFamily2
 * @tc.desc: Verify the MatchFamily
 * @tc.type:FUNC
 * @tc.require: issueI6V6JP
 */
HWTEST_F(DynamicFontProviderTest, MatchFamily2, TestSize.Level1)
{
    InitMyMockVars({});
    dynamicFontProvider->LoadFont("aaa", this, 4);
    EXPECT_NE(dynamicFontProvider->MatchFamily("aaa"), nullptr);
}

/**
 * @tc.name: MatchFamily3
 * @tc.desc: Verify the MatchFamily
 * @tc.type:FUNC
 * @tc.require: issueI6V6JP
 */
HWTEST_F(DynamicFontProviderTest, MatchFamily3, TestSize.Level1)
{
    InitMyMockVars({});
    dynamicFontProvider->LoadFont("aaa", this, 4);
    EXPECT_EQ(dynamicFontProvider->MatchFamily("bbb"), nullptr);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
