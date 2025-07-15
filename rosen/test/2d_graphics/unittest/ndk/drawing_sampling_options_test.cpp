/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "drawing_error_code.h"
#include "drawing_sampling_options.h"
#include "drawing_shader_effect.h"
#include "gtest/gtest.h"
#include "utils/sampling_options.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingSamplingOptionsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};


void NativeDrawingSamplingOptionsTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeDrawingSamplingOptionsTest::TearDownTestCase() {}
void NativeDrawingSamplingOptionsTest::SetUp() {}
void NativeDrawingSamplingOptionsTest::TearDown() {}


/*
 * @tc.name: OH_Drawing_SamplingOptionsCreate
 * @tc.desc: Test OH_Drawing_SamplingOptionsCreate
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingSamplingOptionsTest, OH_Drawing_SamplingOptionsCreate, TestSize.Level1)
{
    OH_Drawing_SamplingOptions* option1 = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_LINEAR,
        OH_Drawing_MipmapMode::MIPMAP_MODE_NEAREST);
    OH_Drawing_SamplingOptions* option2 = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_LINEAR,
        static_cast<OH_Drawing_MipmapMode>(-1));
    OH_Drawing_SamplingOptions* option3 = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_LINEAR,
        static_cast<OH_Drawing_MipmapMode>(99));
    EXPECT_NE(option1, nullptr);
    EXPECT_EQ(option2, nullptr);
    EXPECT_EQ(option3, nullptr);

    OH_Drawing_SamplingOptionsDestroy(option1);
    OH_Drawing_SamplingOptionsDestroy(option2);
    OH_Drawing_SamplingOptionsDestroy(option3);
}

/*
 * @tc.name: OH_Drawing_SamplingOptionsCopy
 * @tc.desc: Test OH_Drawing_SamplingOptionsCopy
 * @tc.type: FUNC
 * @tc.require: ICBC29
 */
HWTEST_F(NativeDrawingSamplingOptionsTest, OH_Drawing_SamplingOptionsCopy, TestSize.Level1)
{
    OH_Drawing_SamplingOptions* option1 = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_LINEAR,
        OH_Drawing_MipmapMode::MIPMAP_MODE_NEAREST);
    OH_Drawing_SamplingOptions* option2 = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_LINEAR,
        static_cast<OH_Drawing_MipmapMode>(-1));
    OH_Drawing_SamplingOptions* option3 = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_LINEAR,
        static_cast<OH_Drawing_MipmapMode>(99));
    EXPECT_NE(option1, nullptr);
    EXPECT_EQ(option2, nullptr);
    EXPECT_EQ(option3, nullptr);

    OH_Drawing_SamplingOptions* optionCopy1 = OH_Drawing_SamplingOptionsCopy(option1);
    OH_Drawing_SamplingOptions* optionCopy2 = OH_Drawing_SamplingOptionsCopy(option2);
    OH_Drawing_SamplingOptions* optionCopy3 = OH_Drawing_SamplingOptionsCopy(option3);
    EXPECT_NE(optionCopy1, nullptr);
    EXPECT_EQ(optionCopy2, nullptr);
    EXPECT_EQ(optionCopy3, nullptr);
    auto samplingOptionsOrigin = reinterpret_cast<const SamplingOptions&>(*option1);
    auto samplingOptionsCopy = reinterpret_cast<const SamplingOptions&>(*optionCopy1);
    EXPECT_EQ(samplingOptionsOrigin == samplingOptionsCopy, true);

    OH_Drawing_SamplingOptionsDestroy(option1);
    OH_Drawing_SamplingOptionsDestroy(option2);
    OH_Drawing_SamplingOptionsDestroy(option3);
    OH_Drawing_SamplingOptionsDestroy(optionCopy1);
    OH_Drawing_SamplingOptionsDestroy(optionCopy2);
    OH_Drawing_SamplingOptionsDestroy(optionCopy3);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS