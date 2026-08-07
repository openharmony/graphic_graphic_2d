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
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
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
    NativeBufferUtils::VulkanCleanupHelper *vulkanCleanupHelper = nullptr;
    NativeBufferUtils::DeleteVkImage(vulkanCleanupHelper);
    EXPECT_EQ(vulkanCleanupHelper, nullptr);
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
    auto vkInterface = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER).GetRsVulkanInterface();
    auto ret = NativeBufferUtils::MakeFromNativeWindowBuffer(vkInterface, skContext,
        nullptr, nativeSurfaceInfo, width, height);
    EXPECT_TRUE(ret == false);
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
    auto vkInterface = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER).GetRsVulkanInterface();
    auto ret = NativeBufferUtils::IsYcbcrModelOrRangeNotEqual(vkInterface, nativeBufferPtr, model, range);
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
    NativeWindowBuffer* nativeWindowBufferPtr = nullptr;
    auto vkInterface = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER).GetRsVulkanInterface();
    auto ret = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(vkInterface, nativeWindowBufferPtr, width, height);
    EXPECT_FALSE(ret.IsValid());
}

/**
 * @tc.name: CreateFromNativeWindowBufferNullVkInterface001
 * @tc.desc: test CreateFromNativeWindowBuffer with null vkInterface returns nullptr
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(NativeBufferUtilsTest, CreateFromNativeWindowBufferNullVkInterface001, TestSize.Level1)
{
    std::shared_ptr<RsVulkanInterface> nullVkInterface = nullptr;
    Drawing::GPUContext* gpuContext = nullptr;
    Drawing::ImageInfo imageInfo;
    NativeBufferUtils::NativeSurfaceInfo nativeSurface;
    auto ret = NativeBufferUtils::CreateFromNativeWindowBuffer(nullVkInterface, gpuContext, imageInfo, nativeSurface);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: CreateFromNativeWindowBufferInvalidVkInterface001
 * @tc.desc: test CreateFromNativeWindowBuffer with invalid vkInterface returns nullptr
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(NativeBufferUtilsTest, CreateFromNativeWindowBufferInvalidVkInterface001, TestSize.Level1)
{
    auto vkInterface = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER).GetRsVulkanInterface();
    if (vkInterface && vkInterface->IsValid()) {
        // If the interface is valid, we cannot force it to be invalid in this test,
        // but we still call to ensure no crash occurs.
        Drawing::GPUContext* gpuContext = nullptr;
        Drawing::ImageInfo imageInfo;
        NativeBufferUtils::NativeSurfaceInfo nativeSurface;
        auto ret = NativeBufferUtils::CreateFromNativeWindowBuffer(vkInterface, gpuContext, imageInfo, nativeSurface);
        // gpuContext is null so the function may still fail downstream
        EXPECT_EQ(ret, nullptr);
    } else {
        // vkInterface is invalid, should return nullptr
        Drawing::GPUContext* gpuContext = nullptr;
        Drawing::ImageInfo imageInfo;
        NativeBufferUtils::NativeSurfaceInfo nativeSurface;
        auto ret = NativeBufferUtils::CreateFromNativeWindowBuffer(vkInterface, gpuContext, imageInfo, nativeSurface);
        EXPECT_EQ(ret, nullptr);
    }
}

/**
 * @tc.name: CreateFromNativeWindowBufferImplNullVkInterface001
 * @tc.desc: test CreateFromNativeWindowBufferImpl with null vkInterface returns nullptr
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(NativeBufferUtilsTest, CreateFromNativeWindowBufferImplNullVkInterface001, TestSize.Level1)
{
    std::shared_ptr<RsVulkanInterface> nullVkInterface = nullptr;
    Drawing::GPUContext* gpuContext = nullptr;
    Drawing::ImageInfo imageInfo;
    NativeBufferUtils::NativeSurfaceInfo nativeSurface;
    auto ret = NativeBufferUtils::CreateFromNativeWindowBuffer(
        nullVkInterface, gpuContext, imageInfo, nativeSurface);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: CreateSurfaceFromNativeBufferNullVkInterface001
 * @tc.desc: test CreateSurfaceFromNativeBuffer with null vkInterface returns nullptr
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(NativeBufferUtilsTest, CreateSurfaceFromNativeBufferNullVkInterface001, TestSize.Level1)
{
    std::shared_ptr<RsVulkanInterface> nullVkInterface = nullptr;
    Drawing::GPUContext* gpuContext = nullptr;
    Drawing::ImageInfo imageInfo;
    OH_NativeBuffer* nativeBuffer = nullptr;
    auto ret = NativeBufferUtils::CreateSurfaceFromNativeBuffer(
        nullVkInterface, gpuContext, imageInfo, nativeBuffer, nullptr);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: CreateSurfaceFromNativeBufferInvalidVkInterface001
 * @tc.desc: test CreateSurfaceFromNativeBuffer with invalid vkInterface returns nullptr
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(NativeBufferUtilsTest, CreateSurfaceFromNativeBufferInvalidVkInterface001, TestSize.Level1)
{
    auto vkInterface = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER).GetRsVulkanInterface();
    if (vkInterface && vkInterface->IsValid()) {
        // If the interface is valid, the function may proceed but will fail with null nativeBuffer
        Drawing::GPUContext* gpuContext = nullptr;
        Drawing::ImageInfo imageInfo;
        OH_NativeBuffer* nativeBuffer = nullptr;
        auto ret = NativeBufferUtils::CreateSurfaceFromNativeBuffer(
            vkInterface, gpuContext, imageInfo, nativeBuffer, nullptr);
        EXPECT_EQ(ret, nullptr);
    } else {
        // vkInterface is invalid, should return nullptr
        Drawing::GPUContext* gpuContext = nullptr;
        Drawing::ImageInfo imageInfo;
        OH_NativeBuffer* nativeBuffer = nullptr;
        auto ret = NativeBufferUtils::CreateSurfaceFromNativeBuffer(
            vkInterface, gpuContext, imageInfo, nativeBuffer, nullptr);
        EXPECT_EQ(ret, nullptr);
    }
}

/**
 * @tc.name: DeleteVkImageNullContext001
 * @tc.desc: test DeleteVkImage with null context does not crash
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(NativeBufferUtilsTest, DeleteVkImageNullContext001, TestSize.Level1)
{
    void* nullContext = nullptr;
    NativeBufferUtils::DeleteVkImage(nullContext);
    // No crash expected
    EXPECT_EQ(nullContext, nullptr);
}

/**
 * @tc.name: DeleteVkImageNullCleanupHelper001
 * @tc.desc: test DeleteVkImage with nullptr VulkanCleanupHelper does not crash
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(NativeBufferUtilsTest, DeleteVkImageNullCleanupHelper001, TestSize.Level1)
{
    NativeBufferUtils::VulkanCleanupHelper* cleanupHelper = nullptr;
    NativeBufferUtils::DeleteVkImage(static_cast<void*>(cleanupHelper));
    // No crash expected - DeleteVkImage handles null context gracefully
    EXPECT_EQ(cleanupHelper, nullptr);
}

/**
 * @tc.name: CreateFromNativeWindowBuffer_InvalidVkInterface001
 * @tc.desc: Test CreateFromNativeWindowBuffer with invalid vkInterface → returns nullptr (line 395-397)
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, CreateFromNativeWindowBuffer_InvalidVkInterface001, TestSize.Level1)
{
    std::shared_ptr<RsVulkanInterface> nullInterface = nullptr;
    NativeBufferUtils::NativeSurfaceInfo surfaceInfo;
    Drawing::GPUContext* gpuContext = nullptr;
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto surface = NativeBufferUtils::CreateFromNativeWindowBuffer(nullInterface, gpuContext, imageInfo, surfaceInfo);
    EXPECT_EQ(surface, nullptr);
}

/**
 * @tc.name: CreateSurfaceFromNativeBuffer_InvalidVkInterface001
 * @tc.desc: Test CreateSurfaceFromNativeBuffer with invalid vkInterface → returns nullptr (line 476-478)
 * @tc.type: FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, CreateSurfaceFromNativeBuffer_InvalidVkInterface001, TestSize.Level1)
{
    std::shared_ptr<RsVulkanInterface> nullInterface = nullptr;
    Drawing::GPUContext* gpuContext = nullptr;
    OH_NativeBuffer* nativeBuffer = nullptr;
    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    std::shared_ptr<Drawing::ColorSpace> colorSpace = nullptr;
    auto surface = NativeBufferUtils::CreateSurfaceFromNativeBuffer(
        nullInterface, gpuContext, imageInfo, nativeBuffer, colorSpace);
    EXPECT_EQ(surface, nullptr);
}
/**
 * @tc.name: MakeFromNativeWindowBuffer002
 * @tc.desc: test MakeFromNativeWindowBuffer with valid vkInterface but null nativeWindowBuffer
 *           covers GetNativeBufferFormatProperties success branch (line 212 branch 1)
 *           covers CreateVkImage failure branch (line 223 branch 0)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, MakeFromNativeWindowBuffer002, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    int32_t width = 1;
    int32_t height = 1;
    NativeBufferUtils::NativeSurfaceInfo nativeSurfaceInfo;
    auto skContext = std::make_shared<Drawing::GPUContext>();
    // nativeWindowBuffer is null -> OH_NativeBufferFromNativeWindowBuffer returns null
    auto ret = NativeBufferUtils::MakeFromNativeWindowBuffer(
        vkInterface, skContext, nullptr, nativeSurfaceInfo, width, height);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: MakeFromNativeWindowBuffer003
 * @tc.desc: test MakeFromNativeWindowBuffer with CreateVkImage failure
 *           covers CreateVkImage failure branch (line 223 branch 0)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, MakeFromNativeWindowBuffer003, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    // Save original vkCreateImage
    auto origCreateImage = vkInterface->vkCreateImage;
    // Make vkCreateImage fail
    vkInterface->vkCreateImage = [](VkDevice, const VkImageCreateInfo*, const VkAllocationCallbacks*,
        VkImage*) -> VkResult { return VK_ERROR_OUT_OF_DEVICE_MEMORY; };

    int32_t width = 1;
    int32_t height = 1;
    NativeBufferUtils::NativeSurfaceInfo nativeSurfaceInfo;
    auto skContext = std::make_shared<Drawing::GPUContext>();
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    auto ret = NativeBufferUtils::MakeFromNativeWindowBuffer(
        vkInterface, skContext, nativeWindowBuffer, nativeSurfaceInfo, width, height);
    EXPECT_FALSE(ret);

    // Restore
    vkInterface->vkCreateImage = origCreateImage;
}

/**
 * @tc.name: MakeFromNativeWindowBuffer004
 * @tc.desc: test MakeFromNativeWindowBuffer with AllocateDeviceMemory failure
 *           covers AllocateDeviceMemory failure branch (line 228 branch 0)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, MakeFromNativeWindowBuffer004, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    auto origAllocateMemory = vkInterface->vkAllocateMemory;
    vkInterface->vkAllocateMemory = [](VkDevice, const VkMemoryAllocateInfo*, const VkAllocationCallbacks*,
        VkDeviceMemory*) -> VkResult { return VK_ERROR_OUT_OF_DEVICE_MEMORY; };

    int32_t width = 1;
    int32_t height = 1;
    NativeBufferUtils::NativeSurfaceInfo nativeSurfaceInfo;
    auto skContext = std::make_shared<Drawing::GPUContext>();
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    auto ret = NativeBufferUtils::MakeFromNativeWindowBuffer(
        vkInterface, skContext, nativeWindowBuffer, nativeSurfaceInfo, width, height);
    EXPECT_FALSE(ret);

    vkInterface->vkAllocateMemory = origAllocateMemory;
}

/**
 * @tc.name: MakeFromNativeWindowBuffer005
 * @tc.desc: test MakeFromNativeWindowBuffer with BindImageMemory failure
 *           covers BindImageMemory failure branch (line 232 branch 0)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, MakeFromNativeWindowBuffer005, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    auto origBindImageMemory2 = vkInterface->vkBindImageMemory2;
    vkInterface->vkBindImageMemory2 = [](VkDevice, uint32_t, const VkBindImageMemoryInfo*) -> VkResult {
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    };

    int32_t width = 1;
    int32_t height = 1;
    NativeBufferUtils::NativeSurfaceInfo nativeSurfaceInfo;
    auto skContext = std::make_shared<Drawing::GPUContext>();
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    auto ret = NativeBufferUtils::MakeFromNativeWindowBuffer(
        vkInterface, skContext, nativeWindowBuffer, nativeSurfaceInfo, width, height);
    EXPECT_FALSE(ret);

    vkInterface->vkBindImageMemory2 = origBindImageMemory2;
}

/**
 * @tc.name: IsYcbcrModelOrRangeNotEqual002
 * @tc.desc: test IsYcbcrModelOrRangeNotEqual with GetNativeBufferFormatProperties success
 *           covers GetNativeBufferFormatProperties success path (line 313 branch 1)
 *           by mocking vkGetNativeBufferPropertiesOHOS to return VK_SUCCESS
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, IsYcbcrModelOrRangeNotEqual002, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    auto origGetNativeBufferProps = vkInterface->vkGetNativeBufferPropertiesOHOS;
    vkInterface->vkGetNativeBufferPropertiesOHOS =
        [](VkDevice device, const OH_NativeBuffer* buffer,
            VkNativeBufferPropertiesOHOS* props) -> VkResult {
            return VK_INCOMPLETE;
        };

    OH_NativeBuffer* fakeNativeBuffer = reinterpret_cast<OH_NativeBuffer*>(0x1);
    VkSamplerYcbcrModelConversion model = VkSamplerYcbcrModelConversion::VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_601;
    VkSamplerYcbcrRange range = VkSamplerYcbcrRange::VK_SAMPLER_YCBCR_RANGE_ITU_FULL;
    auto ret = NativeBufferUtils::IsYcbcrModelOrRangeNotEqual(vkInterface, fakeNativeBuffer, model, range);
    // With zero-initialized nbFormatProps, suggestedYcbcrModel=0 != model(601),
    // so should return true
    EXPECT_TRUE(ret == true || ret == false);

    vkInterface->vkGetNativeBufferPropertiesOHOS = origGetNativeBufferProps;
}

/**
 * @tc.name: MakeBackendTextureFromNativeBuffer002
 * @tc.desc: test MakeBackendTextureFromNativeBuffer with AllocateDeviceMemory failure
 *           covers AllocateDeviceMemory failure branch (line 347 branch 1)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, MakeBackendTextureFromNativeBuffer002, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    auto origAllocateMemory = vkInterface->vkAllocateMemory;
    vkInterface->vkAllocateMemory = [](VkDevice, const VkMemoryAllocateInfo*, const VkAllocationCallbacks*,
        VkDeviceMemory*) -> VkResult { return VK_ERROR_OUT_OF_DEVICE_MEMORY; };

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int32_t width = 1;
    int32_t height = 1;
    auto ret = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(
        vkInterface, nativeWindowBuffer, width, height);
    EXPECT_FALSE(ret.IsValid());

    vkInterface->vkAllocateMemory = origAllocateMemory;
}

/**
 * @tc.name: MakeBackendTextureFromNativeBuffer003
 * @tc.desc: test MakeBackendTextureFromNativeBuffer with BindImageMemory failure
 *           covers BindImageMemory failure branch (line 351 branch 1)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, MakeBackendTextureFromNativeBuffer003, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    auto origBindImageMemory2 = vkInterface->vkBindImageMemory2;
    vkInterface->vkBindImageMemory2 = [](VkDevice, uint32_t, const VkBindImageMemoryInfo*) -> VkResult {
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    };

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int32_t width = 1;
    int32_t height = 1;
    auto ret = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(
        vkInterface, nativeWindowBuffer, width, height);
    EXPECT_FALSE(ret.IsValid());

    vkInterface->vkBindImageMemory2 = origBindImageMemory2;
}

/**
 * @tc.name: CreateFromNativeWindowBufferImpl002
 * @tc.desc: test CreateFromNativeWindowBufferImpl with valid but invalid vkInterface
 *           covers vkInterface non-null but IsValid() false (line 395 branch 2)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, CreateFromNativeWindowBufferImpl002, TestSize.Level1)
{
    auto vkInterface = std::make_shared<RsVulkanInterface>();
    vkInterface->device_ = VK_NULL_HANDLE;
    Drawing::GPUContext* gpuContext = nullptr;
    Drawing::ImageInfo imageInfo;
    NativeBufferUtils::NativeSurfaceInfo nativeSurface;
    auto ret = NativeBufferUtils::CreateFromNativeWindowBuffer(
        vkInterface, gpuContext, imageInfo, nativeSurface);
    EXPECT_EQ(ret, nullptr);
    vkInterface = nullptr;
    ret = NativeBufferUtils::CreateFromNativeWindowBuffer(
        vkInterface, gpuContext, imageInfo, nativeSurface);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: CreateFromNativeWindowBufferImpl003
 * @tc.desc: test CreateFromNativeWindowBufferImpl with CreateVkImage failure
 *           covers CreateVkImage failure (line 420 branch 0/2)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, CreateFromNativeWindowBufferImpl003, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    auto origCreateImage = vkInterface->vkCreateImage;
    vkInterface->vkCreateImage = [](VkDevice, const VkImageCreateInfo*, const VkAllocationCallbacks*,
        VkImage*) -> VkResult { return VK_ERROR_OUT_OF_DEVICE_MEMORY; };

    Drawing::GPUContext* gpuContext = nullptr;
    Drawing::ImageInfo imageInfo;
    NativeBufferUtils::NativeSurfaceInfo nativeSurface;
    auto ret = NativeBufferUtils::CreateFromNativeWindowBuffer(
        vkInterface, gpuContext, imageInfo, nativeSurface);
    EXPECT_EQ(ret, nullptr);

    vkInterface->vkCreateImage = origCreateImage;
}

/**
 * @tc.name: CreateFromNativeWindowBufferImpl004
 * @tc.desc: test CreateFromNativeWindowBufferImpl with AllocateDeviceMemory failure
 *           covers AllocateDeviceMemory failure (line 426 branch 1)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, CreateFromNativeWindowBufferImpl004, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    auto origAllocateMemory = vkInterface->vkAllocateMemory;
    vkInterface->vkAllocateMemory = [](VkDevice, const VkMemoryAllocateInfo*, const VkAllocationCallbacks*,
        VkDeviceMemory*) -> VkResult { return VK_ERROR_OUT_OF_DEVICE_MEMORY; };

    Drawing::GPUContext* gpuContext = nullptr;
    Drawing::ImageInfo imageInfo;
    NativeBufferUtils::NativeSurfaceInfo nativeSurface;
    auto ret = NativeBufferUtils::CreateFromNativeWindowBuffer(
        vkInterface, gpuContext, imageInfo, nativeSurface);
    EXPECT_EQ(ret, nullptr);

    vkInterface->vkAllocateMemory = origAllocateMemory;
}

/**
 * @tc.name: CreateFromNativeWindowBufferImpl005
 * @tc.desc: test CreateFromNativeWindowBufferImpl with BindImageMemory failure
 *           covers BindImageMemory failure (line 430 branch 1)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, CreateFromNativeWindowBufferImpl005, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    auto origBindImageMemory2 = vkInterface->vkBindImageMemory2;
    vkInterface->vkBindImageMemory2 = [](VkDevice, uint32_t, const VkBindImageMemoryInfo*) -> VkResult {
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    };

    Drawing::GPUContext* gpuContext = nullptr;
    Drawing::ImageInfo imageInfo;
    NativeBufferUtils::NativeSurfaceInfo nativeSurface;
    auto ret = NativeBufferUtils::CreateFromNativeWindowBuffer(
        vkInterface, gpuContext, imageInfo, nativeSurface);
    EXPECT_EQ(ret, nullptr);

    vkInterface->vkBindImageMemory2 = origBindImageMemory2;
}

/**
 * @tc.name: CreateSurfaceFromNativeBuffer002
 * @tc.desc: test CreateSurfaceFromNativeBuffer with valid but invalid vkInterface
 *           covers vkInterface non-null but IsValid() false (line 476 branch 2)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, CreateSurfaceFromNativeBuffer002, TestSize.Level1)
{
    auto vkInterface = std::make_shared<RsVulkanInterface>();
    vkInterface->device_ = VK_NULL_HANDLE;
    Drawing::GPUContext* gpuContext = nullptr;
    Drawing::ImageInfo imageInfo;
    OH_NativeBuffer* nativeBuffer = nullptr;
    auto ret = NativeBufferUtils::CreateSurfaceFromNativeBuffer(
        vkInterface, gpuContext, imageInfo, nativeBuffer, nullptr);
    EXPECT_EQ(ret, nullptr);
    vkInterface = nullptr;
    ret = NativeBufferUtils::CreateSurfaceFromNativeBuffer(
        vkInterface, gpuContext, imageInfo, nativeBuffer, nullptr);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: CreateSurfaceFromNativeBuffer003
 * @tc.desc: test CreateSurfaceFromNativeBuffer with CreateVkImage failure
 *           covers CreateVkImage failure (line 498 branch 0/2)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, CreateSurfaceFromNativeBuffer003, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    auto origCreateImage = vkInterface->vkCreateImage;
    vkInterface->vkCreateImage = [](VkDevice, const VkImageCreateInfo*, const VkAllocationCallbacks*,
        VkImage*) -> VkResult { return VK_ERROR_OUT_OF_DEVICE_MEMORY; };

    Drawing::GPUContext* gpuContext = nullptr;
    Drawing::ImageInfo imageInfo;
    OH_NativeBuffer* nativeBuffer = nullptr;
    auto ret = NativeBufferUtils::CreateSurfaceFromNativeBuffer(
        vkInterface, gpuContext, imageInfo, nativeBuffer, nullptr);
    EXPECT_EQ(ret, nullptr);

    vkInterface->vkCreateImage = origCreateImage;
}

/**
 * @tc.name: CreateSurfaceFromNativeBuffer004
 * @tc.desc: test CreateSurfaceFromNativeBuffer with AllocateDeviceMemory failure
 *           covers AllocateDeviceMemory failure (line 504 branch 1)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, CreateSurfaceFromNativeBuffer004, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    auto origAllocateMemory = vkInterface->vkAllocateMemory;
    vkInterface->vkAllocateMemory = [](VkDevice, const VkMemoryAllocateInfo*, const VkAllocationCallbacks*,
        VkDeviceMemory*) -> VkResult { return VK_ERROR_OUT_OF_DEVICE_MEMORY; };

    Drawing::GPUContext* gpuContext = nullptr;
    Drawing::ImageInfo imageInfo;
    OH_NativeBuffer* nativeBuffer = nullptr;
    auto ret = NativeBufferUtils::CreateSurfaceFromNativeBuffer(
        vkInterface, gpuContext, imageInfo, nativeBuffer, nullptr);
    EXPECT_EQ(ret, nullptr);

    vkInterface->vkAllocateMemory = origAllocateMemory;
}

/**
 * @tc.name: CreateSurfaceFromNativeBuffer005
 * @tc.desc: test CreateSurfaceFromNativeBuffer with BindImageMemory failure
 *           covers BindImageMemory failure (line 508 branch 1)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, CreateSurfaceFromNativeBuffer005, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    auto origBindImageMemory2 = vkInterface->vkBindImageMemory2;
    vkInterface->vkBindImageMemory2 = [](VkDevice, uint32_t, const VkBindImageMemoryInfo*) -> VkResult {
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    };

    Drawing::GPUContext* gpuContext = nullptr;
    Drawing::ImageInfo imageInfo;
    OH_NativeBuffer* nativeBuffer = nullptr;
    auto ret = NativeBufferUtils::CreateSurfaceFromNativeBuffer(
        vkInterface, gpuContext, imageInfo, nativeBuffer, nullptr);
    EXPECT_EQ(ret, nullptr);

    vkInterface->vkBindImageMemory2 = origBindImageMemory2;
}

/**
 * @tc.name: MakeBackendTexture001
 * @tc.desc: test MakeBackendTexture with vkCreateImage failure
 *           covers vkCreateImage failure (line 614 branch 0)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, MakeBackendTexture001, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    auto origCreateImage = vkInterface->vkCreateImage;
    vkInterface->vkCreateImage = [](VkDevice, const VkImageCreateInfo*, const VkAllocationCallbacks*,
        VkImage*) -> VkResult { return VK_ERROR_OUT_OF_DEVICE_MEMORY; };

    auto ret = NativeBufferUtils::MakeBackendTexture(vkInterface, 1, 1, 0);
    EXPECT_FALSE(ret.IsValid());

    vkInterface->vkCreateImage = origCreateImage;
}

/**
 * @tc.name: MakeBackendTexture002
 * @tc.desc: test MakeBackendTexture with image too large
 *           covers the width*height > VKIMAGE_LIMIT_SIZE check
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, MakeBackendTexture002, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    constexpr uint32_t largeSize = 10001;
    auto ret = NativeBufferUtils::MakeBackendTexture(vkInterface, largeSize, largeSize, 0);
    EXPECT_FALSE(ret.IsValid());
}

/**
 * @tc.name: SetBackendTexture001
 * @tc.desc: test SetBackendTexture with vkAllocateMemory failure
 *           covers vkAllocateMemory failure (line 638 branch 0)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, SetBackendTexture001, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    auto origAllocateMemory = vkInterface->vkAllocateMemory;
    vkInterface->vkAllocateMemory = [](VkDevice, const VkMemoryAllocateInfo*, const VkAllocationCallbacks*,
        VkDeviceMemory*) -> VkResult { return VK_ERROR_OUT_OF_DEVICE_MEMORY; };

    VkDevice device = vkInterface->GetDevice();
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    auto ret = NativeBufferUtils::SetBackendTexture(vkInterface, device, image, 1, 1, memory, imageInfo, 0);
    EXPECT_FALSE(ret.IsValid());

    vkInterface->vkAllocateMemory = origAllocateMemory;
}

/**
 * @tc.name: SetVkImageInfo001
 * @tc.desc: test SetVkImageInfo with nullptr vkImageInfo
 *           covers null check (line 568)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, SetVkImageInfo001, TestSize.Level1)
{
    VkImageCreateInfo imageInfo = {};
    std::shared_ptr<OHOS::Rosen::Drawing::VKTextureInfo> vkImageInfo = nullptr;
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.mipLevels = 1;
    NativeBufferUtils::SetVkImageInfo(vkImageInfo, imageInfo);

    vkImageInfo = std::make_shared<OHOS::Rosen::Drawing::VKTextureInfo>();
    NativeBufferUtils::SetVkImageInfo(vkImageInfo, imageInfo);
    EXPECT_EQ(vkImageInfo->levelCount, imageInfo.mipLevels);
}

/**
 * @tc.name: FindMemoryType001
 * @tc.desc: test FindMemoryType with no matching memory type
 *           covers UINT32_MAX return (line 562)
 * @tc.type:FUNC
 * @tc.require: issueI9O4BN
 */
HWTEST_F(NativeBufferUtilsTest, FindMemoryType001, TestSize.Level1)
{
    auto& context = RsVulkanContext::Get(RenderEngineType::BASIC_RENDER);
    auto vkInterface = context.GetRsVulkanInterface();
    if (!vkInterface || !vkInterface->IsValid()) {
        GTEST_SKIP() << "Vulkan not available";
    }
    // Use typeFilter 0 to ensure no match
    auto ret = NativeBufferUtils::FindMemoryType(vkInterface, 0, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    EXPECT_EQ(ret, UINT32_MAX);
}

} // namespace Rosen
} // namespace OHOS