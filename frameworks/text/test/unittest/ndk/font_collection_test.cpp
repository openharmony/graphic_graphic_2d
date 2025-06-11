/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "drawing_font_collection.h"

#include "gtest/gtest.h"
#include "drawing_text_declaration.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
class NdkFontCollectionTest : public testing::Test {
};

/*
 * @tc.name: NativeDrawingTest001
 * @tc.desc: test for fontcollection
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontCollectionTest, NdkFontCollectionTest001, TestSize.Level1)
{
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    OH_Drawing_FontCollection* fontCollection2 = OH_Drawing_CreateSharedFontCollection();
    OH_Drawing_FontCollection* fontCollection3 = OH_Drawing_GetFontCollectionGlobalInstance();
    EXPECT_NE(fontCollection, nullptr);
    EXPECT_NE(fontCollection2, nullptr);
    EXPECT_NE(fontCollection3, nullptr);
    OH_Drawing_DisableFontCollectionFallback(fontCollection);
    OH_Drawing_DisableFontCollectionFallback(nullptr);
    OH_Drawing_DisableFontCollectionSystemFont(fontCollection);
    OH_Drawing_DisableFontCollectionSystemFont(nullptr);
    OH_Drawing_ClearFontCaches(fontCollection);
    OH_Drawing_ClearFontCaches(nullptr);
    OH_Drawing_DestroyFontCollection(fontCollection);
    OH_Drawing_DestroyFontCollection(fontCollection2);
    OH_Drawing_DestroyFontCollection(nullptr);
}
} // namespace OHOS