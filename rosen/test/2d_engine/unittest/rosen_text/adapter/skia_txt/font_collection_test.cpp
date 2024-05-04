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
    OHOS::Rosen::Drawing::Typeface* typeface = fontCollection->LoadFont("familyname", data, 0);
    EXPECT_EQ(typeface == nullptr, true);
    fontCollection->LoadThemeFont("familynametest", data, 0);
    std::shared_ptr<Drawing::FontMgr> fontMgr = fontCollection->GetFontMgr();
    EXPECT_EQ(fontMgr != nullptr, true);
    fontCollection->AddLoadedFamilyName("familyname");
}
} // namespace Rosen
} // namespace OHOS