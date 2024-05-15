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

#include "gtest/gtest.h"
#include "font_collection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OH_Drawing_FontCollectionTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_FontCollectionTest001
 * @tc.desc: test for constuctor of FontCollection
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest001, TestSize.Level1)
{
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    fontCollection->DisableFallback();
    fontCollection->DisableSystemFont();
    const uint8_t* data = nullptr;
    auto typeface = fontCollection->LoadFont("familyname", data, 0);
    EXPECT_EQ(typeface == nullptr, true);
    typeface = fontCollection->LoadThemeFont("familynametest", data, 0);
    EXPECT_EQ(typeface == nullptr, true);
    std::shared_ptr<Drawing::FontMgr> fontMgr = fontCollection->GetFontMgr();
    EXPECT_EQ(fontMgr != nullptr, true);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest002
 * @tc.desc: test for RegisterTypeface
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest002, TestSize.Level1)
{
    auto fontCollection = OHOS::Rosen::FontCollection::From(nullptr);
    auto typeface = OHOS::Rosen::Drawing::Typeface::MakeDefault();
    OHOS::Rosen::Drawing::Typeface::RegisterCallBackFunc([](std::shared_ptr<OHOS::Rosen::Drawing::Typeface> typeface) {
        return false;
    });
    EXPECT_EQ(fontCollection->RegisterTypeface(typeface), false);
    OHOS::Rosen::Drawing::Typeface::RegisterCallBackFunc([](std::shared_ptr<OHOS::Rosen::Drawing::Typeface> typeface) {
        return typeface != nullptr;
    });
    EXPECT_EQ(fontCollection->RegisterTypeface(nullptr), false);
    EXPECT_EQ(fontCollection->RegisterTypeface(typeface), true);
    EXPECT_EQ(fontCollection->RegisterTypeface(typeface), true);
}
} // namespace Rosen
} // namespace OHOS