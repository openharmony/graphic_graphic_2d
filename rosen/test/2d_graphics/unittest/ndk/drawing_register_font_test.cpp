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

#include <fstream>
#include <gtest/gtest.h>

#include "drawing_font_collection.h"
#include "drawing_register_font.h"
#include "drawing_text_declaration.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
static const char* g_fontFamily = "Roboto-Black";
static const char* g_fontPath = "/data/Roboto-Black.ttf";

class NativeDrawingRegisterFontTest : public testing::Test {
};

/*
 * @tc.name: NativeDrawingRegisterFontTest001
 * @tc.desc: test for register font
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRegisterFontTest, NativeDrawingRegisterFontTest001, TestSize.Level1)
{
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    uint32_t errorCode = OH_Drawing_RegisterFont(fontCollection, g_fontFamily, g_fontPath);
    std::ifstream fileStream(g_fontPath);
    if (fileStream.is_open()) {
        EXPECT_EQ(errorCode, 0);
        fileStream.close();
    } else {
        EXPECT_EQ(errorCode, 1);
    }
    OH_Drawing_DestroyFontCollection(fontCollection);
}
}