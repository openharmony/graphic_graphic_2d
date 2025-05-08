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

#include "drawing_pixel_map.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingPixelMapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingPixelMapTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeDrawingPixelMapTest::TearDownTestCase() {}
void NativeDrawingPixelMapTest::SetUp() {}
void NativeDrawingPixelMapTest::TearDown() {}

/*
 * @tc.name: NativeDrawingPixelMapTest_PixelMapGetFromOhPixelMapNative001
 * @tc.desc: test for gets an <b>OH_Drawing_PixelMap</b> object.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPixelMapTest, NativeDrawingPixelMapTest_PixelMapGetFromOhPixelMapNative001, TestSize.Level1)
{
    OH_PixelmapNative *native = nullptr;
    OH_Drawing_PixelMap* pixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(native);
    EXPECT_EQ(pixelMap, nullptr);
}

/*
 * @tc.name: NativeDrawingPixelMapTest_PixelMapDissolve002
 * @tc.desc: test for dissolves the relationship between <b>OH_Drawing_PixelMap</b> object and <b>NativePixelMap_</b>
 * or <b>OH_PixelmapNative</b> which is build by 'GetFrom' function.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPixelMapTest, NativeDrawingPixelMapTest_PixelMapDissolve002, TestSize.Level1)
{
    OH_PixelmapNative *native = nullptr;
    OH_Drawing_PixelMap* pixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(native);
    OH_Drawing_PixelMapDissolve(pixelMap);
    EXPECT_EQ(pixelMap, nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS