/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "drawing_color_space.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingColorSpaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingColorSpaceTest::SetUpTestCase() {}
void NativeDrawingColorSpaceTest::TearDownTestCase() {}
void NativeDrawingColorSpaceTest::SetUp() {}
void NativeDrawingColorSpaceTest::TearDown() {}

/*
 * @tc.name: NativeDrawingColorSpaceTest_ColorSpace001
 * @tc.desc: test for ColorSpace.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingColorSpaceTest, NativeDrawingColorSpaceTest_ColorSpace001, TestSize.Level1)
{
    OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgb();
    EXPECT_NE(colorSpace, nullptr);
    OH_Drawing_ColorSpaceDestroy(colorSpace);
}

/*
 * @tc.name: NativeDrawingColorSpaceTest_ColorSpace002
 * @tc.desc: test for ColorSpace.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingColorSpaceTest, NativeDrawingColorSpaceTest_ColorSpace002, TestSize.Level1)
{
    OH_Drawing_ColorSpace* colorSpace = OH_Drawing_ColorSpaceCreateSrgbLinear();
    EXPECT_NE(colorSpace, nullptr);
    OH_Drawing_ColorSpaceDestroy(colorSpace);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS