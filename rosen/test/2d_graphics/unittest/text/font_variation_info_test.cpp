/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "text/font_variation_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontVariationInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FontVariationInfoTest::SetUpTestCase() {}
void FontVariationInfoTest::TearDownTestCase() {}
void FontVariationInfoTest::SetUp() {}
void FontVariationInfoTest::TearDown() {}

/**
 * @tc.name: GenerateFontVariationAxisInfoNull
 * @tc.desc: Test GenerateFontVariationAxisInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053110
 */
HWTEST_F(FontVariationInfoTest, GenerateFontVariationAxisInfoNull, TestSize.Level1)
{
    auto fontaxisinfo = FontVariationInfo::GenerateFontVariationAxisInfo(nullptr, {"en"});
    EXPECT_TRUE(fontaxisinfo.empty());
}

/**
 * @tc.name: GenerateFontVariationAxisInfoValid
 * @tc.desc: Test GenerateFontVariationAxisInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053110
 */
HWTEST_F(FontVariationInfoTest, GenerateFontVariationAxisInfoInValid, TestSize.Level1)
{
    std::string pathNotVar = "/system/fonts/HMOSColorEmojiCompat.ttf";
    auto typeface = Typeface::MakeFromFile(pathNotVar.c_str());
    auto fontaxisinfo = FontVariationInfo::GenerateFontVariationAxisInfo(typeface, {"en"});
    EXPECT_TRUE(fontaxisinfo.empty());
}

/**
 * @tc.name: GenerateFontVariationAxisInfoValid
 * @tc.desc: Test GenerateFontVariationAxisInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053110
 */
HWTEST_F(FontVariationInfoTest, GenerateFontVariationAxisInfoValid, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    auto typeface = Typeface::MakeFromFile(pathDefault.c_str());
    auto fontaxisinfo = FontVariationInfo::GenerateFontVariationAxisInfo(typeface, {"en"});
    EXPECT_EQ(fontaxisinfo.size(), 1);
    EXPECT_EQ(fontaxisinfo[0].axisTag, "wght");
    EXPECT_EQ(fontaxisinfo[0].axisTagMapForLanguage["en"], "Weight");
    EXPECT_EQ(fontaxisinfo[0].nameId, 265);
    EXPECT_NEAR(fontaxisinfo[0].minValue, 40.000000, 1e-6);
    EXPECT_NEAR(fontaxisinfo[0].maxValue, 900.000000, 1e-6);
    EXPECT_NEAR(fontaxisinfo[0].defaultValue, 400.000000, 1e-6);
    EXPECT_FALSE(fontaxisinfo[0].isHidden);
}

/**
 * @tc.name: GenerateFontVariationInstanceInfoNull
 * @tc.desc: Test GenerateFontVariationInstanceInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053110
 */
HWTEST_F(FontVariationInfoTest, GenerateFontVariationInstanceInfoNull, TestSize.Level1)
{
    auto fontinstanceinfo = FontVariationInfo::GenerateFontVariationInstanceInfo(nullptr, {""});
    EXPECT_TRUE(fontinstanceinfo.empty());
}

/**
 * @tc.name: GenerateFontVariationInstanceInfoInvalid
 * @tc.desc: Test GenerateFontVariationInstanceInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053110
 */
HWTEST_F(FontVariationInfoTest, GenerateFontVariationInstanceInfoInvalid, TestSize.Level1)
{
    std::string pathNotVar = "/system/fonts/HMOSColorEmojiCompat.ttf";
    auto typeface = Typeface::MakeFromFile(pathNotVar.c_str());
    auto fontinstanceinfo = FontVariationInfo::GenerateFontVariationInstanceInfo(typeface, {""});
    EXPECT_TRUE(fontinstanceinfo.empty());
}

/**
 * @tc.name: GenerateFontVariationInstanceInfoValid01
 * @tc.desc: Test GenerateFontVariationInstanceInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053110
 */
HWTEST_F(FontVariationInfoTest, GenerateFontVariationInstanceInfoValid01, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    auto typeface = Typeface::MakeFromFile(pathDefault.c_str());
    auto fontinstanceinfo = FontVariationInfo::GenerateFontVariationInstanceInfo(typeface, {""});
    EXPECT_EQ(fontinstanceinfo.size(), 9);
    EXPECT_EQ(fontinstanceinfo[0].subfamilyNameMapForLanguage[""], "Thin");
    EXPECT_TRUE(fontinstanceinfo[0].postScriptNameMapForLanguage.empty());
    EXPECT_NEAR(fontinstanceinfo[0].coordinates["wght"], 100.000000, 1e-6);
    EXPECT_FALSE(fontinstanceinfo[0].isDefault);

    EXPECT_EQ(fontinstanceinfo[1].subfamilyNameMapForLanguage[""], "UltraLight");
    EXPECT_TRUE(fontinstanceinfo[1].postScriptNameMapForLanguage.empty());
    EXPECT_NEAR(fontinstanceinfo[1].coordinates["wght"], 173.000000, 1e-6);
    EXPECT_FALSE(fontinstanceinfo[1].isDefault);

    EXPECT_EQ(fontinstanceinfo[2].subfamilyNameMapForLanguage[""], "Light");
    EXPECT_TRUE(fontinstanceinfo[2].postScriptNameMapForLanguage.empty());
    EXPECT_NEAR(fontinstanceinfo[2].coordinates["wght"], 247.000000, 1e-6);
    EXPECT_FALSE(fontinstanceinfo[2].isDefault);

    EXPECT_EQ(fontinstanceinfo[3].subfamilyNameMapForLanguage[""], "Regular");
    EXPECT_TRUE(fontinstanceinfo[3].postScriptNameMapForLanguage.empty());
    EXPECT_NEAR(fontinstanceinfo[3].coordinates["wght"], 400.000000, 1e-6);
    EXPECT_TRUE(fontinstanceinfo[3].isDefault);

    EXPECT_EQ(fontinstanceinfo[4].subfamilyNameMapForLanguage[""], "Medium");
    EXPECT_TRUE(fontinstanceinfo[4].postScriptNameMapForLanguage.empty());
    EXPECT_NEAR(fontinstanceinfo[4].coordinates["wght"], 500.000000, 1e-6);
    EXPECT_FALSE(fontinstanceinfo[4].isDefault);

    EXPECT_EQ(fontinstanceinfo[5].subfamilyNameMapForLanguage[""], "SemiBold");
    EXPECT_TRUE(fontinstanceinfo[5].postScriptNameMapForLanguage.empty());
    EXPECT_NEAR(fontinstanceinfo[5].coordinates["wght"], 603.000000, 1e-6);
    EXPECT_FALSE(fontinstanceinfo[5].isDefault);
}

/**
 * @tc.name: GenerateFontVariationInstanceInfoValid02
 * @tc.desc: Test GenerateFontVariationInstanceInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053110
 */
HWTEST_F(FontVariationInfoTest, GenerateFontVariationInstanceInfoValid02, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    auto typeface = Typeface::MakeFromFile(pathDefault.c_str());
    auto fontinstanceinfo = FontVariationInfo::GenerateFontVariationInstanceInfo(typeface, {""});
    EXPECT_EQ(fontinstanceinfo.size(), 9);

    EXPECT_EQ(fontinstanceinfo[6].subfamilyNameMapForLanguage[""], "Bold");
    EXPECT_TRUE(fontinstanceinfo[6].postScriptNameMapForLanguage.empty());
    EXPECT_NEAR(fontinstanceinfo[6].coordinates["wght"], 706.000000, 1e-6);
    EXPECT_FALSE(fontinstanceinfo[6].isDefault);

    EXPECT_EQ(fontinstanceinfo[7].subfamilyNameMapForLanguage[""], "Heavy");
    EXPECT_TRUE(fontinstanceinfo[7].postScriptNameMapForLanguage.empty());
    EXPECT_NEAR(fontinstanceinfo[7].coordinates["wght"], 772.000000, 1e-6);
    EXPECT_FALSE(fontinstanceinfo[7].isDefault);

    EXPECT_EQ(fontinstanceinfo[8].subfamilyNameMapForLanguage[""], "Black");
    EXPECT_TRUE(fontinstanceinfo[8].postScriptNameMapForLanguage.empty());
    EXPECT_NEAR(fontinstanceinfo[8].coordinates["wght"], 844.000000, 1e-6);
    EXPECT_FALSE(fontinstanceinfo[8].isDefault);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS