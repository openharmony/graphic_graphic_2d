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

#include <gtest/gtest.h>

#include "image/gpu_context.h"
#include "platform/ohos/backend/native_buffer_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class NativeBufferUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeBufferUtilsTest::SetUpTestCase() {}
void NativeBufferUtilsTest::TearDownTestCase() {}
void NativeBufferUtilsTest::SetUp() {}
void NativeBufferUtilsTest::TearDown() {}

/**
 * @tc.name: DeleteVkImage001
 * @tc.desc: test results of DeleteVkImage
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(NativeBufferUtilsTest, DeleteVkImage001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    auto cachedBackendTexture = NativeBufferUtils::MakeBackendTexture(width, height, 0);
    auto vkTextureInfo = cachedBackendTexture.GetTextureInfo().GetVKTextureInfo();
    NativeBufferUtils::VulkanCleanupHelper* vulkanCleanupHelper = new NativeBufferUtils::VulkanCleanupHelper(
        RsVulkanContext::GetSingleton(), vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
    NativeBufferUtils::DeleteVkImage(vulkanCleanupHelper);
}

/**
 * @tc.name: MakeFromNativeWindowBuffer001
 * @tc.desc: test results of MakeFromNativeWindowBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(NativeBufferUtilsTest, MakeFromNativeWindowBuffer001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    NativeBufferUtils::NativeSurfaceInfo nativeSurfaceInfo;
    auto skContext = std::make_shared<Drawing::GPUContext>();
    auto ret = NativeBufferUtils::MakeFromNativeWindowBuffer(skContext, nullptr, nativeSurfaceInfo, width, height);
    EXPECT_FALSE(ret);

    NativeWindowBuffer nativeWindowBuffer;
    ret = NativeBufferUtils::MakeFromNativeWindowBuffer(skContext, &nativeWindowBuffer, nativeSurfaceInfo, width, height);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: IsYcbcrModelOrRangeNotEqualTest
 * @tc.desc: test results of IsYcbcrModelOrRangeNotEqual
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(NativeBufferUtilsTest, IsYcbcrModelOrRangeNotEqualTest, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    OH_NativeBuffer* nativeBufferPtr = nullptr;
    VkSamplerYcbcrModelConversion model = VkSamplerYcbcrModelConversion::VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_601;
    VkSamplerYcbcrRange range = VkSamplerYcbcrRange::VK_SAMPLER_YCBCR_RANGE_ITU_FULL;
    auto ret = NativeBufferUtils::IsYcbcrModelOrRangeNotEqual(nativeBufferPtr, model, range);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: MakeBackendTextureFromNativeBuffer001
 * @tc.desc: test results of MakeBackendTextureFromNativeBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(NativeBufferUtilsTest, MakeBackendTextureFromNativeBuffer001, TestSize.Level1)
{
    int32_t width = 1;
    int32_t height = 1;
    auto ret = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(nullptr, width, height);
    EXPECT_FALSE(ret.IsValid());

    NativeWindowBuffer nativeWindowBuffer;
    ret = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(&nativeWindowBuffer, width, height);
    EXPECT_TRUE(ret.IsValid());
}

} // namespace Rosen
} // namespace OHOS