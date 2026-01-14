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

#include "text/font_metadata.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontMetaDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FontMetaDataTest::SetUpTestCase() {}
void FontMetaDataTest::TearDownTestCase() {}
void FontMetaDataTest::SetUp() {}
void FontMetaDataTest::TearDown() {}


/**
 * @tc.name: OtNameIdMapper
 * @tc.desc: Test OtNameIdMapper
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontMetaDataTest, OtNameIdMapper, TestSize.Level1)
{
    hb_ot_name_id_t result01 = OtNameIdMapper::ToHarfBuzzNameId(OtNameId::COUNT);
    EXPECT_EQ(result01, HB_OT_NAME_ID_INVALID);
}

/**
 * @tc.name: GenerateFontLegalInfoNull
 * @tc.desc: Test GenerateFontLegalInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontMetaDataTest, GenerateFontLegalInfoNull, TestSize.Level1)
{
    auto map = FontMetaDataCollector::GenerateFontLegalInfo(nullptr, {});
    EXPECT_TRUE(map.empty());
}

/**
 * @tc.name: GenerateFontLegalInfoInvalidTypeface
 * @tc.desc: Test GenerateFontLegalInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontMetaDataTest, GenerateFontLegalInfoInvalidTypeface, TestSize.Level1)
{
    std::string path = "/system/fonts/hm_symbol_config.json";
    auto typeface = Typeface::MakeFromFile(path.c_str());
    auto map = FontMetaDataCollector::GenerateFontLegalInfo(typeface, {});
    EXPECT_TRUE(map.empty());
}

/**
 * @tc.name: GenerateFontLegalInfoValidTypefaceWithEmptyLanguage01
 * @tc.desc: Test GenerateFontLegalInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontMetaDataTest, GenerateFontLegalInfoValidTypefaceWithEmptyLanguage01, TestSize.Level1)
{
    std::string path = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto map = FontMetaDataCollector::GenerateFontLegalInfo(typeface, {});
    EXPECT_TRUE(map.empty());
}

/**
 * @tc.name: GenerateFontLegalInfoValidTypefaceWithEmptyLanguage02
 * @tc.desc: Test GenerateFontLegalInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontMetaDataTest, GenerateFontLegalInfoValidTypefaceWithEmptyLanguage02, TestSize.Level1)
{
    std::string path = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto map = FontMetaDataCollector::GenerateFontLegalInfo(typeface, {""});
    EXPECT_EQ(map[""].version, "Version 2.040");
    EXPECT_EQ(map[""].manufacturer, "Hanyi Fonts");
    EXPECT_EQ(map[""].copyright, "Copyright 2021-2024 Huawei Device Co., Ltd. All Rights Reserved.");
    EXPECT_TRUE(map[""].trademark.empty());
    EXPECT_TRUE(map[""].license.empty());
}

/**
 * @tc.name: GenerateFontLegalInfoValidTypefaceWithen_XXX
 * @tc.desc: Test GenerateFontLegalInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontMetaDataTest, GenerateFontLegalInfoValidTypefaceWithen_XXX, TestSize.Level1)
{
    std::string path = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto map = FontMetaDataCollector::GenerateFontLegalInfo(typeface, {"en-XXX"});
    EXPECT_EQ(map["en-XXX"].version, "Version 2.040");
    EXPECT_EQ(map["en-XXX"].manufacturer, "Hanyi Fonts");
    EXPECT_EQ(map["en-XXX"].copyright, "Copyright 2021-2024 Huawei Device Co., Ltd. All Rights Reserved.");
    EXPECT_TRUE(map["en-XXX"].trademark.empty());
    EXPECT_TRUE(map["en-XXX"].license.empty());
}

/**
 * @tc.name: GenerateFontLegalInfoValidTypefaceWithEN
 * @tc.desc: Test GenerateFontLegalInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontMetaDataTest, GenerateFontLegalInfoValidTypefaceWithEN, TestSize.Level1)
{
    std::string path = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto map = FontMetaDataCollector::GenerateFontLegalInfo(typeface, {"EN"});
    EXPECT_EQ(map["EN"].version, "Version 2.040");
    EXPECT_EQ(map["EN"].manufacturer, "Hanyi Fonts");
    EXPECT_EQ(map["EN"].copyright, "Copyright 2021-2024 Huawei Device Co., Ltd. All Rights Reserved.");
    EXPECT_TRUE(map["EN"].trademark.empty());
    EXPECT_TRUE(map["EN"].license.empty());
}

/**
 * @tc.name: GenerateFontLegalInfoValidTypefaceWithen
 * @tc.desc: Test GenerateFontLegalInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontMetaDataTest, GenerateFontLegalInfoValidTypefaceWithen, TestSize.Level1)
{
    std::string path = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto map = FontMetaDataCollector::GenerateFontLegalInfo(typeface, {"en"});
    EXPECT_EQ(map["en"].version, "Version 2.040");
    EXPECT_EQ(map["en"].manufacturer, "Hanyi Fonts");
    EXPECT_EQ(map["en"].copyright, "Copyright 2021-2024 Huawei Device Co., Ltd. All Rights Reserved.");
    EXPECT_TRUE(map["en"].trademark.empty());
    EXPECT_TRUE(map["en"].license.empty());
}

/**
 * @tc.name: GenerateFontLegalInfoValidTypefaceWithInvalidLanguage
 * @tc.desc: Test GenerateFontLegalInfo
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontMetaDataTest, GenerateFontLegalInfoValidTypefaceWithInvalidLanguage, TestSize.Level1)
{
    std::string path = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto map = FontMetaDataCollector::GenerateFontLegalInfo(typeface, {"zh-HK"});
    EXPECT_TRUE(map.empty());
}

/**
 * @tc.name: GenerateFontIdentificationEmpty
 * @tc.desc: Test GenerateFontIdentification
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontMetaDataTest, GenerateFontIdentificationEmpty, TestSize.Level1)
{
    auto map = FontMetaDataCollector::GenerateFontIdentification(nullptr, {"zh-CN", "zh-HK", ""});
    EXPECT_TRUE(map.empty());
}

/**
 * @tc.name: GenerateFontIdentificationInvalid
 * @tc.desc: Test GenerateFontIdentification
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontMetaDataTest, GenerateFontIdentificationInvalid, TestSize.Level1)
{
    auto typeface = Typeface::MakeDefault();
    ASSERT_TRUE(typeface);
    auto data = typeface->Serialize();
    auto typefaceInvalid = Typeface::Deserialize(data->GetData(), 1); // wrong size for invalid tf
    auto map = FontMetaDataCollector::GenerateFontIdentification(typefaceInvalid, {"zh-CN", "zh-HK", ""});
    EXPECT_TRUE(map.empty());
}

/**
 * @tc.name: GenerateFontIdentificationValid
 * @tc.desc: Test GenerateFontIdentification
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontMetaDataTest, GenerateFontIdentificationValid, TestSize.Level1)
{
    std::string pathDefault = "/system/fonts/HarmonyOS_Sans_SC.ttf";
    auto typeface = Typeface::MakeFromFile(pathDefault.c_str());
    ASSERT_TRUE(typeface);
    auto map = FontMetaDataCollector::GenerateFontIdentification(typeface, {"zh", "zh-CN", "zh_CN", "",
                                                                            "zh-Hans", "zh-Hans-CN"});
    auto it = map.find("zh");
    EXPECT_TRUE(it == map.end());

    FontIdentification idInfoCN1 = map["zh-CN"];
    EXPECT_EQ(idInfoCN1.fontFamily, "鸿蒙黑体");
    EXPECT_EQ(idInfoCN1.fontSubFamily, "Regular");
    EXPECT_EQ(idInfoCN1.fullName, "鸿蒙黑体");
    EXPECT_TRUE(idInfoCN1.postScriptName.empty());

    FontIdentification idInfoCN2 = map["zh_CN"];
    EXPECT_EQ(idInfoCN2.fontFamily, "鸿蒙黑体");
    EXPECT_EQ(idInfoCN2.fontSubFamily, "Regular");
    EXPECT_EQ(idInfoCN2.fullName, "鸿蒙黑体");
    EXPECT_TRUE(idInfoCN2.postScriptName.empty());

    it = map.find("zh-Hans");
    EXPECT_TRUE(it == map.end());

    it = map.find("zh-Hans-CN");
    EXPECT_TRUE(it == map.end());

    FontIdentification idInfoEmpty = map[""];
    EXPECT_EQ(idInfoEmpty.fontFamily, "HarmonyOS Sans SC");
    EXPECT_EQ(idInfoEmpty.fontSubFamily, "Regular");
    EXPECT_EQ(idInfoEmpty.fullName, "HarmonyOS Sans SC");
    EXPECT_EQ(idInfoEmpty.postScriptName, "HarmonyOS_Sans_SC");
}

/**
 * @tc.name: GetFirstAvailableString01
 * @tc.desc: Test GetFirstAvailableString
 * @tc.type: FUNC
 * @tc.require: SR20250918053090
 */
HWTEST_F(FontMetaDataTest, GetFirstAvailableString01, TestSize.Level1)
{
    auto typeface = Typeface::MakeDefault();
    ASSERT_TRUE(typeface);
    Drawing::OtNameId id = OtNameId::FONT_FAMILY;
    std::string name = FontMetaDataCollector::GetFirstAvailableString(typeface, id);
    EXPECT_EQ(name, "HarmonyOS Sans");
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS