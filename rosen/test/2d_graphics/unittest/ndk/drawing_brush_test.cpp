/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "c/drawing_brush.h"
#include "c/drawing_color.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingBrushTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingBrushTest::SetUpTestCase() {}
void NativeDrawingBrushTest::TearDownTestCase() {}
void NativeDrawingBrushTest::SetUp() {}
void NativeDrawingBrushTest::TearDown() {}

/*
 * @tc.name: NativeDrawingBrushTest_brush001
 * @tc.desc: test for create brush and destroy brush.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_brush001, TestSize.Level1)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_EQ(brush == nullptr, false);
    OH_Drawing_BrushDestroy(brush);
}

/*
 * @tc.name: NativeDrawingBrushTest_brush002
 * @tc.desc: test for the set methods of brush.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingBrushTest, NativeDrawingBrushTest_brush002, TestSize.Level1)
{
    OH_Drawing_Brush* brush1 = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetAntiAlias(brush1, false);
    EXPECT_EQ(OH_Drawing_BrushIsAntiAlias(brush1), false);
    OH_Drawing_BrushSetColor(brush1, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    EXPECT_EQ(OH_Drawing_BrushGetColor(brush1), 0xFFFF0000);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS