/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "skia_adapter/skia_font_mgr.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaFontMgrTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaFontMgrTest::SetUpTestCase() {}
void SkiaFontMgrTest::TearDownTestCase() {}
void SkiaFontMgrTest::SetUp() {}
void SkiaFontMgrTest::TearDown() {}

#ifndef USE_TEXGINE
/**
 * @tc.name: CreateDynamicFontMgr001
 * @tc.desc: Test CreateDynamicFontMgr
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaFontMgrTest, CreateDynamicFontMgr001, TestSize.Level1)
{
    std::shared_ptr<FontMgrImpl> skiaFontMgr = SkiaFontMgr::CreateDynamicFontMgr();
    ASSERT_TRUE(skiaFontMgr != nullptr);
}

/**
 * @tc.name: LoadDynamicFont001
 * @tc.desc: Test LoadDynamicFont
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaFontMgrTest, LoadDynamicFont001, TestSize.Level1)
{
    std::shared_ptr<FontMgrImpl> skiaFontMgr = SkiaFontMgr::CreateDynamicFontMgr();
    ASSERT_TRUE(skiaFontMgr != nullptr);
    skiaFontMgr->LoadDynamicFont("0", nullptr, 0);
    skiaFontMgr->LoadDynamicFont("", nullptr, 0);
}

/**
 * @tc.name: LoadThemeFont001
 * @tc.desc: Test LoadThemeFont
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaFontMgrTest, LoadThemeFont001, TestSize.Level1)
{
    std::shared_ptr<FontMgrImpl> skiaFontMgr = SkiaFontMgr::CreateDynamicFontMgr();
    ASSERT_TRUE(skiaFontMgr != nullptr);
    skiaFontMgr->LoadThemeFont("0", "none", nullptr, 0);
}
#endif

/**
 * @tc.name: MatchFamily001
 * @tc.desc: Test MatchFamily
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaFontMgrTest, MatchFamily001, TestSize.Level1)
{
    std::shared_ptr<FontMgrImpl> skiaFontMgr = SkiaFontMgr::CreateDynamicFontMgr();
    ASSERT_TRUE(skiaFontMgr != nullptr);
    auto fontStyleSet = skiaFontMgr->MatchFamily("0");
    ASSERT_TRUE(fontStyleSet != nullptr);
}

/**
 * @tc.name: MatchFamilyStyle001
 * @tc.desc: Test MatchFamilyStyle
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaFontMgrTest, MatchFamilyStyle001, TestSize.Level1)
{
    std::shared_ptr<FontMgrImpl> skiaFontMgr = SkiaFontMgr::CreateDynamicFontMgr();
    ASSERT_TRUE(skiaFontMgr != nullptr);
    FontStyle fontStyle;
    auto typeface = skiaFontMgr->MatchFamilyStyle("0", fontStyle);
    ASSERT_TRUE(typeface == nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS