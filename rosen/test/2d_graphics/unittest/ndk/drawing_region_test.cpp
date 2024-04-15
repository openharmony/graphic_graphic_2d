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

#include "drawing_rect.h"
#include "drawing_region.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingRegionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingRegionTest::SetUpTestCase() {}
void NativeDrawingRegionTest::TearDownTestCase() {}
void NativeDrawingRegionTest::SetUp() {}
void NativeDrawingRegionTest::TearDown() {}

/*
 * @tc.name: NativeDrawingRegionTest_region001
 * @tc.desc: test for create drawing_region.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionTest_region001, TestSize.Level1)
{
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    EXPECT_NE(region, nullptr);
    OH_Drawing_RegionDestroy(region);
}

/*
 * @tc.name: NativeDrawingRegionSetRectTest_region002
 * @tc.desc: test for constructs a rectangular Region matching the bounds of rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionSetRectTest_region002, TestSize.Level1)
{
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect=OH_Drawing_RectCreate(0.0f, 0.0f, 256.0f, 256.0f);
    EXPECT_TRUE(OH_Drawing_RegionSetRect(region, rect));
    OH_Drawing_RegionDestroy(region);
    OH_Drawing_RectDestroy(rect);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS