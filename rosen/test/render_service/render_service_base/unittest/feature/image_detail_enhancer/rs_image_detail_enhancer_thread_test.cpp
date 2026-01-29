/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <parameters.h>
#include <unordered_map>

#include "feature/image_detail_enhancer/rs_image_detail_enhancer_thread.h"
#include "gtest/gtest.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_pixel_map_util.h"
#include "skia_image.h"
#include "skia_image_info.h"
#include "surface_buffer_impl.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
using namespace Drawing;
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
// Sample valid params for unit test
static RSImageDetailEnhanceParams params = { true, 500, 3000, 0.5f, 5.0f };
static RSImageDetailEnhanceAlgoParams slrParams = {
    true,
    {{ 0.0f, 0.5f, 0.3f }},
    360000,
    640000,
};
static RSImageDetailEnhanceAlgoParams esrParams = {
    true,
    {{ 1.0f, 2.0f, 0.3f }, { 2.0f, 4.0f, 0.2f }, { 4.0f, 5.0f, 0.15f }},
    360000,
    9000000,
};
#endif

class RSImageDetailEnhancerThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSImageDetailEnhancerThreadTest::SetUpTestCase() {}
void RSImageDetailEnhancerThreadTest::TearDownTestCase() {}
void RSImageDetailEnhancerThreadTest::SetUp() {}
void RSImageDetailEnhancerThreadTest::TearDown() {}

class DetailEnhancerUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DetailEnhancerUtilsTest::SetUpTestCase() {}
void DetailEnhancerUtilsTest::TearDownTestCase() {}
void DetailEnhancerUtilsTest::SetUp() {}
void DetailEnhancerUtilsTest::TearDown() {}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
static sptr<SurfaceBuffer> CreateSurfaceBuffer(int width, int height)
{
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    BufferRequestConfig bufConfig = {
        .width = static_cast<int32_t>(width),
        .height = static_cast<int32_t>(height),
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA |
            BUFFER_USAGE_MEM_MMZ_CACHE | BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE,
        .timeout = 0,
        .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE,
    };
    surfaceBuffer->Alloc(bufConfig);
    return surfaceBuffer;
}

static ColorType GetColorTypeWithVKFormat(VkFormat vkFormat)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return COLORTYPE_RGBA_8888;
    }
    switch (vkFormat) {
        case VK_FORMAT_R8G8B8A8_UNORM:
            return COLORTYPE_RGBA_8888;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return COLORTYPE_RGBA_F16;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            return COLORTYPE_RGB_565;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            return COLORTYPE_RGBA_1010102;
        default:
            return COLORTYPE_RGBA_8888;
    }
}

static std::shared_ptr<Image> MakeImageFromSurfaceBuffer(sptr<SurfaceBuffer> surfaceBuffer)
{
    auto drawingContext = RsVulkanContext::GetSingleton().CreateDrawingContext();
    std::shared_ptr<GPUContext> gpuContext(drawingContext);
    if (surfaceBuffer == nullptr || !gpuContext) {
        return nullptr;
    }
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
        return nullptr;
    }
    OHNativeWindowBuffer* nativeWindowBuffer = CreateNativeWindowBufferFromSurfaceBuffer(&surfaceBuffer);
    if (!nativeWindowBuffer) {
        return nullptr;
    }
    BackendTexture backendTexture = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(
        nativeWindowBuffer, surfaceBuffer->GetWidth(), surfaceBuffer->GetHeight(), false);
    DestroyNativeWindowBuffer(nativeWindowBuffer);
    if (!backendTexture.IsValid()) {
        return nullptr;
    }
    auto vkTextureInfo = backendTexture.GetTextureInfo().GetVKTextureInfo();
    if (!vkTextureInfo) {
        return nullptr;
    }
    NativeBufferUtils::VulkanCleanupHelper* cleanUpHelper = new NativeBufferUtils::VulkanCleanupHelper(
        RsVulkanContext::GetSingleton(), vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
    std::shared_ptr<Image> dmaImage = std::make_shared<Image>();
    if (cleanUpHelper == nullptr || dmaImage == nullptr) {
        return nullptr;
    }
    BitmapFormat bitmapFormat = {GetColorTypeWithVKFormat(vkTextureInfo->format),
        AlphaType::ALPHATYPE_PREMUL};
    if (!dmaImage->BuildFromTexture(*gpuContext, backendTexture.GetTextureInfo(), TextureOrigin::TOP_LEFT,
        bitmapFormat, nullptr, NativeBufferUtils::DeleteVkImage, cleanUpHelper->Ref())) {
        NativeBufferUtils::DeleteVkImage(cleanUpHelper);
        return nullptr;
    }
    NativeBufferUtils::DeleteVkImage(cleanUpHelper);
    return dmaImage;
}
#endif

/**
 * @tc.name: PostTaskTest
 * @tc.desc: test results of PostTaskTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, PostTaskTest, TestSize.Level1)
{
    std::function<void()> func = []() -> void {};
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    rsImageDetailEnhancerThread.PostTask(func);
    rsImageDetailEnhancerThread.handler_ = nullptr;
    rsImageDetailEnhancerThread.PostTask(func);
    EXPECT_EQ(rsImageDetailEnhancerThread.handler_, nullptr);
}

/**
 * @tc.name: RSImageDetailEnhancerThreadInstanceTest
 * @tc.desc: test results of InstanceTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, InstanceTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& instance1 = RSImageDetailEnhancerThread::Instance();
    RSImageDetailEnhancerThread& instance2 = RSImageDetailEnhancerThread::Instance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: DetailEnhancerUtilsInstanceTest
 * @tc.desc: test results of InstanceTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(DetailEnhancerUtilsTest, InstanceTest, TestSize.Level1)
{
    DetailEnhancerUtils& instance1 = DetailEnhancerUtils::Instance();
    DetailEnhancerUtils& instance2 = DetailEnhancerUtils::Instance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: GetProcessReadyTest
 * @tc.desc: GetProcessReadyTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, GetProcessReadyTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    uint64_t imageId = 12345;
    rsImageDetailEnhancerThread.processReadyMap_[imageId] = true;
    bool result = rsImageDetailEnhancerThread.GetProcessReady(imageId);
    EXPECT_TRUE(result);
    imageId = 23456;
    rsImageDetailEnhancerThread.processReadyMap_[imageId] = false;
    result = rsImageDetailEnhancerThread.GetProcessReady(imageId);
    EXPECT_FALSE(result);
    imageId = 34567;
    result = rsImageDetailEnhancerThread.GetProcessReady(imageId);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SetProcessReadyTest
 * @tc.desc: SetProcessReadyTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, SetProcessReadyTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    uint64_t imageId = 12345;
    bool flag = true;
    rsImageDetailEnhancerThread.SetProcessReady(imageId, flag);
    EXPECT_EQ(rsImageDetailEnhancerThread.processReadyMap_.count(imageId), 1);
    EXPECT_EQ(rsImageDetailEnhancerThread.processReadyMap_[imageId], flag);
    imageId = 23456;
    bool oldFlag = true;
    bool newFlag = false;
    rsImageDetailEnhancerThread.SetProcessReady(imageId, oldFlag);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, newFlag);
    EXPECT_EQ(rsImageDetailEnhancerThread.processReadyMap_.count(imageId), 1);
    EXPECT_EQ(rsImageDetailEnhancerThread.processReadyMap_[imageId], newFlag);
    imageId = 0;
    flag = true;
    rsImageDetailEnhancerThread.SetProcessReady(imageId, flag);
    EXPECT_EQ(rsImageDetailEnhancerThread.processReadyMap_.count(imageId), 1);
    EXPECT_EQ(rsImageDetailEnhancerThread.processReadyMap_[imageId], flag);
}

/**
 * @tc.name: GetProcessStatusTest
 * @tc.desc: GetProcessStatusTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, GetProcessStatusTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    uint64_t imageId = 12345;
    rsImageDetailEnhancerThread.processStatusMap_[imageId] = true;
    bool result = rsImageDetailEnhancerThread.GetProcessStatus(imageId);
    EXPECT_TRUE(result);
    imageId = 23456;
    rsImageDetailEnhancerThread.processStatusMap_[imageId] = false;
    result = rsImageDetailEnhancerThread.GetProcessStatus(imageId);
    EXPECT_FALSE(result);
    imageId = 34567;
    result = rsImageDetailEnhancerThread.GetProcessStatus(imageId);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SetProcessStatusTest
 * @tc.desc: SetProcessStatusTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, SetProcessStatusTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    uint64_t imageId = 12345;
    bool flag = true;
    rsImageDetailEnhancerThread.SetProcessStatus(imageId, flag);
    EXPECT_EQ(rsImageDetailEnhancerThread.processStatusMap_.count(imageId), 1);
    EXPECT_EQ(rsImageDetailEnhancerThread.processStatusMap_[imageId], flag);
    imageId = 12345;
    bool oldFlag = true;
    bool newFlag = false;
    rsImageDetailEnhancerThread.SetProcessStatus(imageId, oldFlag);
    rsImageDetailEnhancerThread.SetProcessStatus(imageId, newFlag);
    EXPECT_EQ(rsImageDetailEnhancerThread.processStatusMap_.count(imageId), 1);
    EXPECT_EQ(rsImageDetailEnhancerThread.processStatusMap_[imageId], newFlag);
    imageId = 0;
    flag = true;
    rsImageDetailEnhancerThread.SetProcessStatus(imageId, flag);
    EXPECT_EQ(rsImageDetailEnhancerThread.processStatusMap_.count(imageId), 1);
    EXPECT_EQ(rsImageDetailEnhancerThread.processStatusMap_[imageId], flag);
}

/**
 * @tc.name: SetScaledImageTest
 * @tc.desc: SetScaledImageTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, SetScaledImageTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    uint64_t imageId = 12345;
    auto image = std::make_shared<Drawing::Image>();
    rsImageDetailEnhancerThread.SetScaledImage(imageId, image);
    imageId = 23456;
    auto dstImage = std::make_shared<Drawing::Image>();
    EXPECT_NE(dstImage, nullptr);
    rsImageDetailEnhancerThread.imageList_.emplace_front(imageId, dstImage);
    rsImageDetailEnhancerThread.keyMap_[imageId] = rsImageDetailEnhancerThread.imageList_.begin();
    rsImageDetailEnhancerThread.SetScaledImage(imageId, dstImage);
    imageId = 34567;
    dstImage = nullptr;
    rsImageDetailEnhancerThread.SetScaledImage(imageId, dstImage);
}

/**
 * @tc.name: ReleaseScaledImageTest
 * @tc.desc: ReleaseScaledImageTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, ReleaseScaledImageTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    auto image = std::make_shared<Drawing::Image>();
    EXPECT_NE(image, nullptr);
    uint64_t imageId = 1234567;
    rsImageDetailEnhancerThread.SetScaledImage(imageId, image);
    rsImageDetailEnhancerThread.ReleaseScaledImage(imageId);
    imageId = 567890;
    rsImageDetailEnhancerThread.ReleaseScaledImage(imageId);
}

/**
 * @tc.name: GetScaledImageTest
 * @tc.desc: GetScaledImageTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, GetScaledImageTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    uint64_t imageId = 12345;
    auto image = std::make_shared<Drawing::Image>();
    rsImageDetailEnhancerThread.SetScaledImage(imageId, image);
    auto result = rsImageDetailEnhancerThread.GetScaledImage(imageId);
    imageId = 56789;
    result = rsImageDetailEnhancerThread.GetScaledImage(imageId);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ResetStatusTest
 * @tc.desc: ResetStatusTest001
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, ResetStatusTest001, TestSize.Level1)
{
    uint64_t imageId = 12345;
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE};
    bmp.Build(800, 800, format);
    auto dstImage = std::make_shared<Drawing::Image>();
    EXPECT_NE(dstImage, nullptr);
    dstImage->BuildFromBitmap(bmp);
    int srcWidth = 1000;
    int srcHeight = 1000;
    int dstWidth = 800;
    int dstHeight = 800;
    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, true);
    rsImageDetailEnhancerThread.SetScaledImage(imageId, dstImage);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 800, 800, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 800, 801, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 800, 810, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 800, 790, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 801, 801, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 801, 799, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 801, 800, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 801, 810, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 801, 790, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 810, 810, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 810, 801, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 810, 799, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 810, 790, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 799, 799, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 799, 810, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 799, 801, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 799, 800, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 790, 790, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 790, 799, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 790, 800, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 790, 810, imageId);
    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, false);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);
    dstImage = nullptr;
    rsImageDetailEnhancerThread.SetProcessReady(imageId, true);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);
}

/**
 * @tc.name: ResetStatusTest
 * @tc.desc: ResetStatusTest002
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, ResetStatusTest002, TestSize.Level1)
{
    uint64_t imageId = 12345;
    int srcWidth = 1920;
    int srcHeight = 1080;
    int dstWidth = 0;
    int dstHeight = 0;
    auto dstImage = std::make_shared<Drawing::Image>();
    EXPECT_NE(dstImage, nullptr);
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, true);
    rsImageDetailEnhancerThread.SetScaledImage(imageId, dstImage);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);
    dstImage = nullptr;
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    dstImage = std::make_shared<Drawing::Image>();
    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, false);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    rsImageDetailEnhancerThread.SetProcessStatus(imageId, false);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, true);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    rsImageDetailEnhancerThread.SetProcessReady(imageId, false);
    rsImageDetailEnhancerThread.SetProcessStatus(imageId, false);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, true);
    rsImageDetailEnhancerThread.SetScaledImage(imageId, nullptr);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, false);
    rsImageDetailEnhancerThread.SetScaledImage(imageId, nullptr);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    rsImageDetailEnhancerThread.SetProcessStatus(imageId, false);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, false);
    rsImageDetailEnhancerThread.SetScaledImage(imageId, nullptr);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);
}

/**
 * @tc.name: ResetStatusTest
 * @tc.desc: ResetStatusTest003
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, ResetStatusTest003, TestSize.Level1)
{
    uint64_t imageId = 12345;
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE};
    bmp.Build(800, 800, format);
    auto dstImage = std::make_shared<Drawing::Image>();
    EXPECT_NE(dstImage, nullptr);
    dstImage->BuildFromBitmap(bmp);
    int srcWidth = 1000;
    int srcHeight = 1000;
    int dstWidth = 800;
    int dstHeight = 800;
    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, true);
    rsImageDetailEnhancerThread.SetScaledImage(imageId, dstImage);

    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, false);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    rsImageDetailEnhancerThread.SetProcessStatus(imageId, false);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, true);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    rsImageDetailEnhancerThread.SetProcessReady(imageId, false);
    rsImageDetailEnhancerThread.SetProcessStatus(imageId, false);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, true);
    rsImageDetailEnhancerThread.SetScaledImage(imageId, nullptr);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, false);
    rsImageDetailEnhancerThread.SetScaledImage(imageId, nullptr);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    rsImageDetailEnhancerThread.SetProcessStatus(imageId, false);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, false);
    rsImageDetailEnhancerThread.SetScaledImage(imageId, nullptr);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);
}

/**
 * @tc.name: MarkScaledImageDirtyTest
 * @tc.desc: MarkScaledImageDirtyTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, MarkScaledImageDirtyTest, TestSize.Level1)
{
    uint64_t nodeId = 123456;
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    rsImageDetailEnhancerThread.MarkScaledImageDirty(nodeId);
    rsImageDetailEnhancerThread.callback_ = [](uint64_t) -> void {};
    EXPECT_NE(rsImageDetailEnhancerThread.callback_, nullptr);
    rsImageDetailEnhancerThread.MarkScaledImageDirty(nodeId);
}

/**
 * @tc.name: IsSizeSupportTest
 * @tc.desc: IsSizeSupportTest001
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, IsSizeSupportTest001, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    bool result = rsImageDetailEnhancerThread.IsSizeSupported(100, 100, 150, 150);
    result = rsImageDetailEnhancerThread.IsSizeSupported(-10, 10, 800, 800);
    result = rsImageDetailEnhancerThread.IsSizeSupported(10, -10, 800, 800);
    result = rsImageDetailEnhancerThread.IsSizeSupported(0, -10, 800, 800);
    result = rsImageDetailEnhancerThread.IsSizeSupported(0, 10, 800, 800);
    result = rsImageDetailEnhancerThread.IsSizeSupported(0, 0, 800, 800);
    result = rsImageDetailEnhancerThread.IsSizeSupported(-10, 0, 800, 800);
    result = rsImageDetailEnhancerThread.IsSizeSupported(-10, -10, 800, 800);
    result = rsImageDetailEnhancerThread.IsSizeSupported(800, 800, -10, 10);
    result = rsImageDetailEnhancerThread.IsSizeSupported(800, 800, 10, -10);
    result = rsImageDetailEnhancerThread.IsSizeSupported(800, 800, 0, -10);
    result = rsImageDetailEnhancerThread.IsSizeSupported(800, 800, 0, 10);
    result = rsImageDetailEnhancerThread.IsSizeSupported(800, 800, 0, 0);
    result = rsImageDetailEnhancerThread.IsSizeSupported(800, 800, -10, 0);
    result = rsImageDetailEnhancerThread.IsSizeSupported(800, 800, -10, -10);
    result = rsImageDetailEnhancerThread.IsSizeSupported(100, 100, 80, 100);
    result = rsImageDetailEnhancerThread.IsSizeSupported(80, 100, 80, 100);
    result = rsImageDetailEnhancerThread.IsSizeSupported(100, 100, 80, 80);
    result = rsImageDetailEnhancerThread.IsSizeSupported(100, 100, 501, 501);
    result = rsImageDetailEnhancerThread.IsSizeSupported(100, 100, 95, 95);
    result = rsImageDetailEnhancerThread.IsSizeSupported(600, 600, 100, 100);
    result = rsImageDetailEnhancerThread.IsSizeSupported(600, 300, 100, 100);
    result = rsImageDetailEnhancerThread.IsSizeSupported(300, 600, 100, 100);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: IsSizeSupportTest
 * @tc.desc: IsSizeSupportTest002
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, IsSizeSupportTest002, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    bool result = rsImageDetailEnhancerThread.IsSizeSupported(100, 100, 501, 499);
    result = rsImageDetailEnhancerThread.IsSizeSupported(2000, 2000, 2100, 2100);
    result = rsImageDetailEnhancerThread.IsSizeSupported(2000, 2100, 2100, 2000);
    result = rsImageDetailEnhancerThread.IsSizeSupported(2100, 2000, 2100, 2000);
    result = rsImageDetailEnhancerThread.IsSizeSupported(2100, 2000, 2000, 2100);
    result = rsImageDetailEnhancerThread.IsSizeSupported(1000, 1000, 1100, 1100);
    result = rsImageDetailEnhancerThread.IsSizeSupported(2000, 2000, 499, 501);
    result = rsImageDetailEnhancerThread.IsSizeSupported(499, 501, 499, 501);
    result = rsImageDetailEnhancerThread.IsSizeSupported(501, 499, 499, 501);
    result = rsImageDetailEnhancerThread.IsSizeSupported(501, 499, 501, 499);
    result = rsImageDetailEnhancerThread.IsSizeSupported(2000, 2000, 3001, 3001);
    result = rsImageDetailEnhancerThread.IsSizeSupported(2000, 2000, 2999, 3001);
    result = rsImageDetailEnhancerThread.IsSizeSupported(2000, 2000, 3001, 2999);
    result = rsImageDetailEnhancerThread.IsSizeSupported(1000, 1000, 950, 950);
    result = rsImageDetailEnhancerThread.IsSizeSupported(1000, 1000, 700, 700);
    result = rsImageDetailEnhancerThread.IsSizeSupported(1000, 700, 1000, 700);
    result = rsImageDetailEnhancerThread.IsSizeSupported(1000, 950, 950, 1000);
    result = rsImageDetailEnhancerThread.IsSizeSupported(950, 1000, 1000, 950);
    result = rsImageDetailEnhancerThread.IsSizeSupported(950, 950, 1000, 1000);
    result = rsImageDetailEnhancerThread.IsSizeSupported(700, 700, 1000, 1000);
    result = rsImageDetailEnhancerThread.IsSizeSupported(700, 1000, 1000, 700);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: RegisterCallbackTest
 * @tc.desc: RegisterCallbackTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, RegisterCallbackTest, TestSize.Level1)
{
    std::function<void(uint64_t)> callback = [](uint64_t) -> void {};
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    auto type = system::GetParameter("rosen.isEnabledScaleImageAsync.enabled", "0");
    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", "1");
    bool result = rsImageDetailEnhancerThread.RegisterCallback(callback);
    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", "0");
    result = rsImageDetailEnhancerThread.RegisterCallback(callback);
    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", "1");
    result = rsImageDetailEnhancerThread.RegisterCallback(nullptr);
    EXPECT_FALSE(result);
    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", type);
}

/**
 * @tc.name: GetSharpnessTest
 * @tc.desc: GetSharpnessTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, GetSharpnessTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    RSImageDetailEnhanceAlgoParams algoParams = {
        true,
        {{ 0.1f, 0.5f, 0.2f }},
        360000,
        640000,
    };
    float result;
    EXPECT_TRUE(rsImageDetailEnhancerThread.GetSharpness(algoParams, 0.2f, result));
    EXPECT_FLOAT_EQ(result, 0.2f);
    EXPECT_FALSE(rsImageDetailEnhancerThread.GetSharpness(algoParams, 0.6f, result));
    EXPECT_FALSE(rsImageDetailEnhancerThread.GetSharpness(algoParams, 0.0f, result));

    RSImageDetailEnhanceAlgoParams algoParams2 = {
        true,
        {{ 0.5f, 0.2f, 0.2f }},
        360000,
        640000,
    };
    EXPECT_FALSE(rsImageDetailEnhancerThread.GetSharpness(algoParams2, 0.1f, result));
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)

/**
 * @tc.name: GetEnabledTest
 * @tc.desc: GetEnabledTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, GetEnabledTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    rsImageDetailEnhancerThread.isParamValidate_ = true;
    auto type = system::GetParameter("rosen.isEnabledScaleImageAsync.enabled", "0");
    bool result = rsImageDetailEnhancerThread.GetEnabled();
    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", "1");
    result = rsImageDetailEnhancerThread.GetEnabled();
    system::SetParameter("resourceschedule.memmgr.min.memmory.watermark", "false");
    rsImageDetailEnhancerThread.isParamValidate_ = true;
    result = rsImageDetailEnhancerThread.GetEnabled();
    EXPECT_TRUE(result);
    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", "0");
    result = rsImageDetailEnhancerThread.GetEnabled();
    system::SetParameter("resourceschedule.memmgr.min.memmory.watermark", "false");
    result = rsImageDetailEnhancerThread.GetEnabled();
    EXPECT_FALSE(result);
    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", type);
}

/**
 * @tc.name: IsTypeSupportTest
 * @tc.desc: IsTypeSupportTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, IsTypeSupportTest, TestSize.Level1)
{
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->editable_ = false;
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    bool result = rsImageDetailEnhancerThread.IsTypeSupport(pixelMap);
    EXPECT_FALSE(result);
    pixelMap->editable_ = true;
    result = rsImageDetailEnhancerThread.IsTypeSupport(pixelMap);
    EXPECT_FALSE(result);
    pixelMap->editable_ = false;
    result = rsImageDetailEnhancerThread.IsTypeSupport(pixelMap);
    pixelMap->allocatorType_ = Media::AllocatorType::DMA_ALLOC;
    result = rsImageDetailEnhancerThread.IsTypeSupport(pixelMap);
    EXPECT_TRUE(result);
    pixelMap->allocatorType_ = Media::AllocatorType::SHARE_MEM_ALLOC;
    result = rsImageDetailEnhancerThread.IsTypeSupport(pixelMap);
    EXPECT_FALSE(result);
    pixelMap->imageInfo_.pixelFormat = Media::PixelFormat::ASTC_4x4;
    pixelMap->astcHdr_ = true;
    result = rsImageDetailEnhancerThread.IsTypeSupport(pixelMap);
    EXPECT_FALSE(result);
    result = rsImageDetailEnhancerThread.IsTypeSupport(nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: ScaleByAAETest
 * @tc.desc: ScaleByAAETest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, ScaleByAAETest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    DetailEnhancerUtils& detailEnhancerUtils = DetailEnhancerUtils::Instance();
    sptr<SurfaceBuffer> surfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(1000, 1000);
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE};
    bmp.Build(700, 700, format);
    auto srcImage = std::make_shared<Drawing::Image>();
    srcImage->BuildFromBitmap(bmp);
    std::shared_ptr<Drawing::Image> dstImage = rsImageDetailEnhancerThread.ScaleByAAE(surfaceBuffer, srcImage);
    sptr<SurfaceBuffer> surfaceBuffer2 = nullptr;
    dstImage = rsImageDetailEnhancerThread.ScaleByAAE(surfaceBuffer2, srcImage);
    dstImage = rsImageDetailEnhancerThread.ScaleByAAE(const_cast<sptr<SurfaceBuffer>&>(surfaceBuffer), srcImage);
    EXPECT_EQ(dstImage, nullptr);
}

/**
 * @tc.name: ScaleByHDSamplerTest
 * @tc.desc: ScaleByHDSamplerTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, ScaleByHDSamplerTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    rsImageDetailEnhancerThread.params_ = params;
    rsImageDetailEnhancerThread.slrParams_ = slrParams;
    rsImageDetailEnhancerThread.esrParams_ = esrParams;
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE};
    bmp.Build(1000, 1000, format);
    auto srcImage = std::make_shared<Drawing::Image>();
    EXPECT_NE(srcImage, nullptr);
    srcImage->BuildFromBitmap(bmp);
    int dstWidth = 700;
    int dstHeight = 700;
    DetailEnhancerUtils& detailEnhancerUtils = DetailEnhancerUtils::Instance();
    sptr<SurfaceBuffer> dstSurfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(dstWidth, dstHeight);
    auto result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, dstSurfaceBuffer, srcImage);
    dstWidth = 1000;
    dstHeight = 1000;
    dstSurfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(dstWidth, dstHeight);
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, dstSurfaceBuffer, srcImage);
    dstWidth = 1300;
    dstHeight = 1300;
    dstSurfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(dstWidth, dstHeight);
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, dstSurfaceBuffer, srcImage);
    dstWidth = 700;
    dstHeight = 1300;
    dstSurfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(dstWidth, dstHeight);
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, dstSurfaceBuffer, srcImage);
    dstHeight = 700;
    dstSurfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(dstWidth, dstHeight);
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, dstSurfaceBuffer, srcImage);
    dstWidth = 300;
    dstHeight = 300;
    dstSurfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(dstWidth, dstHeight);
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, dstSurfaceBuffer, srcImage);
    dstWidth = 500;
    dstHeight = 500;
    dstSurfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(dstWidth, dstHeight);
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, dstSurfaceBuffer, srcImage);
    dstWidth = 600;
    dstHeight = 600;
    dstSurfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(dstWidth, dstHeight);
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, dstSurfaceBuffer, srcImage);
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(300, 300, dstSurfaceBuffer, srcImage);
    dstWidth = 800;
    dstHeight = 800;
    dstSurfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(dstWidth, dstHeight);
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, dstSurfaceBuffer, srcImage);
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, nullptr, srcImage);
}

/**
 * @tc.name: ExecuteTaskAsyncTest
 * @tc.desc: ExecuteTaskAsyncTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, ExecuteTaskAsyncTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    rsImageDetailEnhancerThread.params_ = params;
    rsImageDetailEnhancerThread.slrParams_ = slrParams;
    rsImageDetailEnhancerThread.esrParams_ = esrParams;
    uint64_t nodeId = 12345;
    uint64_t imageId = 45678;
    int dstWidth = 800;
    int dstHeight = 800;
    auto image = std::make_shared<Drawing::Image>();
    rsImageDetailEnhancerThread.ExecuteTaskAsync(dstWidth, dstHeight, image, nodeId, imageId);
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format{Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE};
    bmp.Build(600, 600, format);
    auto srcImage = std::make_shared<Drawing::Image>();
    srcImage->BuildFromBitmap(bmp);
    rsImageDetailEnhancerThread.ExecuteTaskAsync(dstWidth, dstHeight, srcImage, nodeId, imageId);
    dstWidth = 0;
    dstHeight = 0;
    rsImageDetailEnhancerThread.ExecuteTaskAsync(dstWidth, dstHeight, srcImage, nodeId, imageId);
    dstWidth = -1;
    dstHeight = -1;
    rsImageDetailEnhancerThread.ExecuteTaskAsync(dstWidth, dstHeight, srcImage, nodeId, imageId);
    dstWidth = 400;
    dstHeight = 800;
    rsImageDetailEnhancerThread.ExecuteTaskAsync(dstWidth, dstHeight, srcImage, nodeId, imageId);
    dstWidth = 800;
    dstHeight = 400;
    rsImageDetailEnhancerThread.ExecuteTaskAsync(dstWidth, dstHeight, srcImage, nodeId, imageId);
    Drawing::Bitmap bmp2;
    bmp2.Build(1000, 1000, format);
    auto srcImage2 = std::make_shared<Drawing::Image>();
    srcImage2->BuildFromBitmap(bmp2);
    dstWidth = 1000;
    dstHeight = 1000;
    rsImageDetailEnhancerThread.ExecuteTaskAsync(dstWidth, dstHeight, srcImage2, nodeId, imageId);
    EXPECT_NE(srcImage2, nullptr);
    rsImageDetailEnhancerThread.ExecuteTaskAsync(dstWidth, dstHeight, nullptr, nodeId, imageId);
    int srcWidth = 1024;
    int srcHeight = 1024;
    sptr<SurfaceBuffer> srcSurfaceBuffer = CreateSurfaceBuffer(srcWidth, srcHeight);
    std::shared_ptr<Image> srcImage3 = MakeImageFromSurfaceBuffer(srcSurfaceBuffer);
    dstWidth = 512;
    dstHeight = 512;
    rsImageDetailEnhancerThread.ExecuteTaskAsync(dstWidth, dstHeight, srcImage3, nodeId, imageId);
    rsImageDetailEnhancerThread.ReleaseScaledImage(imageId);
}

/**
 * @tc.name: EnhanceImageAsyncTest
 * @tc.desc: Verify function EnhanceImageAsyncTest
 * @tc.type: FUNC
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, EnhanceImageAsyncTest, TestSize.Level1)
{
    auto image = std::make_shared<RSImage>();
    image->pixelMap_ = nullptr;
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    RSImageParams rsImageParams = {
        image->pixelMap_, image->nodeId_, image->dst_, image->uniqueId_, image->image_, 1.0f
    };
    std::shared_ptr<Drawing::Image> result = rsImageDetailEnhancerThread.EnhanceImageAsync(
        image->isScaledImageAsync_, rsImageParams);
    EXPECT_EQ(result, nullptr);
    auto type = system::GetParameter("rosen.isEnabledScaleImageAsync.enabled", "0");
    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", "1");
    auto pixelMap = std::make_shared<Media::PixelMap>();
    image->pixelMap_ = pixelMap;
    rsImageParams.mPixelMap = image->pixelMap_;
    result = rsImageDetailEnhancerThread.EnhanceImageAsync(image->isScaledImageAsync_, rsImageParams);
    EXPECT_EQ(result, nullptr);
    pid_t pid = ExtractPid(image->nodeId_);
    RsCommonHook::Instance().SetImageEnhancePidList({pid});
    int addr1 = 1;
    int* addr = &addr1;
    Drawing::ImageInfo imageInfo1(100, 100, Drawing::ColorType::COLORTYPE_ALPHA_8,
        Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto pixmap1 = Drawing::Pixmap(imageInfo1, addr, 400);
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    Drawing::ReleaseContext releaseContext = nullptr;
    auto dstImage = Drawing::Image::MakeFromRaster(pixmap1, rasterReleaseProc, releaseContext);
    uint64_t imageId = image->GetUniqueId();
    rsImageDetailEnhancerThread.SetScaledImage(imageId, dstImage);
    rsImageParams.mImage = dstImage;
    result = rsImageDetailEnhancerThread.EnhanceImageAsync(image->isScaledImageAsync_, rsImageParams);
    EXPECT_NE(result, nullptr);
    image->pixelMap_ = nullptr;
    rsImageParams.mPixelMap = image->pixelMap_;
    image->image_ = nullptr;
    rsImageParams.mImage = image->image_;
    result = rsImageDetailEnhancerThread.EnhanceImageAsync(image->isScaledImageAsync_, rsImageParams);
    EXPECT_EQ(result, nullptr);
    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", type);
}

/**
 * @tc.name: EnhanceImageAsyncTest01
 * @tc.desc: Verify function EnhanceImageAsyncTest01
 * @tc.type: FUNC
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, EnhanceImageAsyncTest01, TestSize.Level1)
{
    auto type = system::GetParameter("rosen.isEnabledScaleImageAsync.enabled", "0");
    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", "1");
    auto image = std::make_shared<RSImage>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    image->pixelMap_ = pixelMap;
    int addr1 = 1;
    int* addr = &addr1;
    Drawing::ImageInfo imageInfo1(100, 100, Drawing::ColorType::COLORTYPE_ALPHA_8,
        Drawing::AlphaType::ALPHATYPE_OPAQUE);
    Drawing::ImageInfo imageInfo2(120, 120, Drawing::ColorType::COLORTYPE_ALPHA_8,
        Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto pixmap1 = Drawing::Pixmap(imageInfo1, addr, 400);
    auto pixmap2 = Drawing::Pixmap(imageInfo2, addr, 480);
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    Drawing::ReleaseContext releaseContext = nullptr;
    auto image1 = Drawing::Image::MakeFromRaster(pixmap1, rasterReleaseProc, releaseContext);
    auto image2 = Drawing::Image::MakeFromRaster(pixmap2, rasterReleaseProc, releaseContext);
    Drawing::Rect dstRect(0.0, 0.0, 100, 100);
    image->dst_ = dstRect;
    pid_t pid = ExtractPid(image->nodeId_);
    RsCommonHook::Instance().SetImageEnhancePidList({pid});
    RSImageParams rsImageParams = {
        image->pixelMap_, image->nodeId_, image->dst_, image->uniqueId_, image->image_, 1.0f
    };

    rsImageParams.mImage = image1;
    rsImageDetailEnhancerThread.SetScaledImage(image->uniqueId_, image2);
    EXPECT_EQ(rsImageDetailEnhancerThread.EnhanceImageAsync(image->isScaledImageAsync_, rsImageParams), nullptr);

    rsImageParams.mImage = image2;
    rsImageDetailEnhancerThread.SetScaledImage(image->uniqueId_, image1);
    EXPECT_NE(rsImageDetailEnhancerThread.EnhanceImageAsync(image->isScaledImageAsync_, rsImageParams), nullptr);

    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", type);
}

/**
 * @tc.name: EnhanceImageAsyncTest02
 * @tc.desc: Verify function EnhanceImageAsyncTest02
 * @tc.type: FUNC
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, EnhanceImageAsyncTest02, TestSize.Level1)
{
    auto type = system::GetParameter("rosen.isEnabledScaleImageAsync.enabled", "0");
    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", "1");
    auto image = std::make_shared<RSImage>();
    auto pixelMap = std::make_shared<Media::PixelMap>();
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    image->pixelMap_ = pixelMap;
    auto image1 = std::make_shared<Drawing::Image>();
    auto image2 = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo1(100, 100, Drawing::ColorType::COLORTYPE_ALPHA_8,
        Drawing::AlphaType::ALPHATYPE_OPAQUE);
    Drawing::ImageInfo imageInfo2(0, 1, Drawing::ColorType::COLORTYPE_ALPHA_8,
        Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo1 = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo1);
    auto skImageInfo2 = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo2);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap1 = SkPixmap(skImageInfo1, addr, 1);
    auto skiaPixmap2 = SkPixmap(skImageInfo2, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
#ifdef USE_M133_SKIA
    sk_sp<SkImage> skImage1 = SkImages::RasterFromPixmap(skiaPixmap1, rasterReleaseProc, releaseContext);
    sk_sp<SkImage> skImage2 = SkImages::RasterFromPixmap(skiaPixmap2, rasterReleaseProc, releaseContext);
#else
    sk_sp<SkImage> skImage1 = SkImage::MakeFromRaster(skiaPixmap1, rasterReleaseProc, releaseContext);
    sk_sp<SkImage> skImage2 = SkImage::MakeFromRaster(skiaPixmap2, rasterReleaseProc, releaseContext);
#endif
    auto skiaImage1 = std::make_shared<Drawing::SkiaImage>(skImage1);
    auto skiaImage2 = std::make_shared<Drawing::SkiaImage>(skImage2);
    image1->imageImplPtr = skiaImage1;
    image2->imageImplPtr = skiaImage2;
    Drawing::Rect dstRect(0.0, 0.0, 100, 100);
    image->dst_ = dstRect;
    pid_t pid = ExtractPid(image->nodeId_);
    RsCommonHook::Instance().SetImageEnhancePidList({pid});
    RSImageParams rsImageParams = {
        image->pixelMap_, image->nodeId_, image->dst_, image->uniqueId_, image->image_, 1.0f
    };
    rsImageParams.mImage = image1;
    rsImageDetailEnhancerThread.SetScaledImage(image->uniqueId_, image2);
    EXPECT_EQ(rsImageDetailEnhancerThread.EnhanceImageAsync(image->isScaledImageAsync_, rsImageParams), nullptr);
    rsImageParams.mImage = image2;
    rsImageDetailEnhancerThread.SetScaledImage(image->uniqueId_, image1);
    EXPECT_EQ(rsImageDetailEnhancerThread.EnhanceImageAsync(image->isScaledImageAsync_, rsImageParams), nullptr);
    system::SetParameter("rosen.isEnabledScaleImageAsync.enabled", type);
}

/**
 * @tc.name: ScaleImageAsyncTest
 * @tc.desc: ScaleImageAsyncTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, ScaleImageAsyncTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    rsImageDetailEnhancerThread.params_ = params;
    rsImageDetailEnhancerThread.slrParams_ = slrParams;
    rsImageDetailEnhancerThread.esrParams_ = esrParams;
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE};
    bmp.Build(800, 800, format);
    auto image = std::make_shared<Drawing::Image>();
    EXPECT_NE(image, nullptr);
    image->BuildFromBitmap(bmp);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    pixelMap->editable_ = false;
    pixelMap->allocatorType_ = Media::AllocatorType::DMA_ALLOC;
    Drawing::Rect dst(0, 0, 1200, 1200);
    uint64_t nodeId = 12345;
    uint64_t imageId = 45678;
    rsImageDetailEnhancerThread.processStatusMap_[imageId] = false;
    rsImageDetailEnhancerThread.ScaleImageAsync(pixelMap, dst, nodeId, imageId, image);
    rsImageDetailEnhancerThread.processStatusMap_[imageId] = true;
    rsImageDetailEnhancerThread.ScaleImageAsync(pixelMap, dst, nodeId, imageId, image);
    rsImageDetailEnhancerThread.processStatusMap_[imageId] = false;
    pixelMap->allocatorType_ = Media::AllocatorType::SHARE_MEM_ALLOC;
    rsImageDetailEnhancerThread.ScaleImageAsync(pixelMap, dst, nodeId, imageId, image);
    pixelMap->allocatorType_ = Media::AllocatorType::DMA_ALLOC;
    Drawing::Rect dst2(0, 0, 800, 800);
    rsImageDetailEnhancerThread.ScaleImageAsync(pixelMap, dst2, nodeId, imageId, image);
    Drawing::Rect dst3(0, 0, -100, -100);
    rsImageDetailEnhancerThread.ScaleImageAsync(pixelMap, dst3, nodeId, imageId, image);
    pixelMap = nullptr;
    rsImageDetailEnhancerThread.ScaleImageAsync(pixelMap, dst, nodeId, imageId, image);
    image = nullptr;
    rsImageDetailEnhancerThread.ScaleImageAsync(pixelMap, dst, nodeId, imageId, image);
}

/**
 * @tc.name: PushImageListTest
 * @tc.desc: PushImageListTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, PushImageListTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    rsImageDetailEnhancerThread.curCache_ = 150;
    uint64_t imageId = 12345;
    auto dstImage = std::make_shared<Drawing::Image>();
    EXPECT_NE(dstImage, nullptr);
    rsImageDetailEnhancerThread.imageList_.emplace_front(imageId, dstImage);
    rsImageDetailEnhancerThread.keyMap_[imageId] = rsImageDetailEnhancerThread.imageList_.begin();
    rsImageDetailEnhancerThread.PushImageList(imageId, dstImage);
    dstImage = nullptr;
    rsImageDetailEnhancerThread.PushImageList(imageId, dstImage);

    auto image = nullptr;
    imageId = 23456;
    rsImageDetailEnhancerThread.imageList_.emplace_front(imageId, image);
    rsImageDetailEnhancerThread.keyMap_[imageId] = rsImageDetailEnhancerThread.imageList_.begin();
    rsImageDetailEnhancerThread.PushImageList(imageId, image);
}

/**
 * @tc.name: IsPidEnabledTest
 * @tc.desc: IsPidEnabledTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, IsPidEnabledTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    uint64_t nodeId = 123456;
    rsImageDetailEnhancerThread.IsPidEnabled(nodeId);
    DetailEnhancerUtils& detailEnhancerUtils = DetailEnhancerUtils::Instance();
    rsImageDetailEnhancerThread.releaseTime_ = detailEnhancerUtils.GetCurTime();
    rsImageDetailEnhancerThread.IsPidEnabled(nodeId);
    rsImageDetailEnhancerThread.releaseTime_ = 1;
    rsImageDetailEnhancerThread.IsPidEnabled(nodeId);
    rsImageDetailEnhancerThread.imageList_.clear();
    rsImageDetailEnhancerThread.IsPidEnabled(nodeId);
    nodeId = 0;
    rsImageDetailEnhancerThread.IsPidEnabled(nodeId);
    rsImageDetailEnhancerThread.releaseTime_ = detailEnhancerUtils.GetCurTime();
    rsImageDetailEnhancerThread.IsPidEnabled(nodeId);
    EXPECT_NE(rsImageDetailEnhancerThread.releaseTime_, 0);
}

/**
 * @tc.name: InitSurfaceTest
 * @tc.desc: InitSurfaceTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(DetailEnhancerUtilsTest, InitSurfaceTest, TestSize.Level1)
{
    int dstWidth = 800;
    int dstHeight = 600;
    auto image = std::make_shared<Drawing::Image>();
    DetailEnhancerUtils& detailEnhancerUtils = DetailEnhancerUtils::Instance();
    sptr<SurfaceBuffer> dstSurfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(dstWidth, dstHeight);
    EXPECT_NE(detailEnhancerUtils.InitSurface(dstWidth, dstHeight, dstSurfaceBuffer, image), nullptr);
    EXPECT_EQ(detailEnhancerUtils.InitSurface(dstWidth, dstHeight, dstSurfaceBuffer, nullptr), nullptr);
    dstSurfaceBuffer = nullptr;
    EXPECT_EQ(detailEnhancerUtils.InitSurface(dstWidth, dstHeight, dstSurfaceBuffer, image), nullptr);
}

/**
 * @tc.name: MakeImageFromSurfaceBufferTest
 * @tc.desc: MakeImageFromSurfaceBufferTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(DetailEnhancerUtilsTest, MakeImageFromSurfaceBufferTest, TestSize.Level1)
{
    auto image = std::make_shared<Drawing::Image>();
    EXPECT_NE(image, nullptr);
    DetailEnhancerUtils& detailEnhancerUtils = DetailEnhancerUtils::Instance();
    sptr<SurfaceBuffer> surfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(600, 600);
    auto result = detailEnhancerUtils.MakeImageFromSurfaceBuffer(surfaceBuffer, image);
    result = detailEnhancerUtils.MakeImageFromSurfaceBuffer(surfaceBuffer, nullptr);
    result = detailEnhancerUtils.MakeImageFromSurfaceBuffer(surfaceBuffer, image);
    surfaceBuffer = nullptr;
    result = detailEnhancerUtils.MakeImageFromSurfaceBuffer(surfaceBuffer, image);
    surfaceBuffer = new SurfaceBufferImpl();
    result = detailEnhancerUtils.MakeImageFromSurfaceBuffer(surfaceBuffer, image);
    sptr<SurfaceBuffer> surfaceBuffer2 = detailEnhancerUtils.CreateSurfaceBuffer(10001, 10001);
    result = detailEnhancerUtils.MakeImageFromSurfaceBuffer(surfaceBuffer2, image);
    int srcWidth = 1024;
    int srcHeight = 1024;
    sptr<SurfaceBuffer> srcSurfaceBuffer = CreateSurfaceBuffer(srcWidth, srcHeight);
    std::shared_ptr<Image> srcImage = MakeImageFromSurfaceBuffer(srcSurfaceBuffer);
    int dstWidth = 512;
    int dstHeight = 512;
    sptr<SurfaceBuffer> surfaceBuffer3 = CreateSurfaceBuffer(dstWidth, dstHeight);
    detailEnhancerUtils.MakeImageFromSurfaceBuffer(surfaceBuffer3, srcImage);
}

/**
 * @tc.name: GetColorTypeWithVKFormatTest
 * @tc.desc: GetColorTypeWithVKFormatTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(DetailEnhancerUtilsTest, GetColorTypeWithVKFormatTest, TestSize.Level1)
{
    VkFormat vkFormat = VK_FORMAT_R8G8B8A8_UNORM;
    DetailEnhancerUtils& detailEnhancerUtils = DetailEnhancerUtils::Instance();
    EXPECT_EQ(detailEnhancerUtils.GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGBA_8888);
    vkFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    EXPECT_EQ(detailEnhancerUtils.GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGBA_F16);
    vkFormat = VK_FORMAT_R5G6B5_UNORM_PACK16;
    EXPECT_EQ(detailEnhancerUtils.GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGB_565);
    vkFormat = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
    EXPECT_EQ(detailEnhancerUtils.GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGBA_1010102);
    vkFormat = static_cast<VkFormat>(0xFFFFFFFF);
    EXPECT_EQ(detailEnhancerUtils.GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGBA_8888);
    EXPECT_EQ(detailEnhancerUtils.GetColorTypeWithVKFormat(vkFormat), Drawing::COLORTYPE_RGBA_8888);
}

/**
 * @tc.name: SetMemoryNameTest
 * @tc.desc: SetMemoryNameTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(DetailEnhancerUtilsTest, SetMemoryNameTest, TestSize.Level1)
{
    DetailEnhancerUtils& detailEnhancerUtils = DetailEnhancerUtils::Instance();
    sptr<SurfaceBuffer> surfaceBuffer = new SurfaceBufferImpl();
    int result = detailEnhancerUtils.SetMemoryName(surfaceBuffer);
    EXPECT_EQ(result, false);
    surfaceBuffer = nullptr;
    result = detailEnhancerUtils.SetMemoryName(surfaceBuffer);
    EXPECT_EQ(result, false);
    int width = 1920;
    int height = 1080;
    sptr<SurfaceBuffer> dstSurfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(width, height);
    result = detailEnhancerUtils.SetMemoryName(dstSurfaceBuffer);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: CreateSurfaceBufferTest
 * @tc.desc: CreateSurfaceBufferTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(DetailEnhancerUtilsTest, CreateSurfaceBufferTest, TestSize.Level1)
{
    int width = 1920;
    int height = 1080;
    DetailEnhancerUtils& detailEnhancerUtils = DetailEnhancerUtils::Instance();
    sptr<SurfaceBuffer> surfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(width, height);
    EXPECT_NE(surfaceBuffer, nullptr);
    EXPECT_EQ(surfaceBuffer->GetWidth(), width);
    EXPECT_EQ(surfaceBuffer->GetHeight(), height);
    width = 0;
    height = 0;
    surfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(width, height);
    EXPECT_NE(surfaceBuffer, nullptr);
    EXPECT_NE(surfaceBuffer->GetWidth(), width);
    EXPECT_NE(surfaceBuffer->GetHeight(), height);
    width = -1920;
    height = -1080;
    surfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(width, height);
    EXPECT_NE(surfaceBuffer, nullptr);
    EXPECT_NE(surfaceBuffer->GetWidth(), width);
    EXPECT_NE(surfaceBuffer->GetHeight(), height);
}

/**
 * @tc.name: GetImageSizeTest
 * @tc.desc: GetImageSizeTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(DetailEnhancerUtilsTest, GetImageSizeTest, TestSize.Level1)
{
    DetailEnhancerUtils& detailEnhancerUtils = DetailEnhancerUtils::Instance();
    auto image = std::make_shared<Drawing::Image>();
    float result = detailEnhancerUtils.GetImageSize(image);
    image = nullptr;
    result = detailEnhancerUtils.GetImageSize(image);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: DumpImageTest
 * @tc.desc: DumpImageTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, DumpImageTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE};
    bmp.Build(500, 500, format);
    auto image = std::make_shared<Drawing::Image>();
    EXPECT_EQ(bmp.GetWidth(), 500);
    image->BuildFromBitmap(bmp);
    uint64_t imageId = 123456;
    auto type = system::GetParameter("rosen.dumpUICaptureEnabled.enabled", "0");
    system::SetParameter("rosen.dumpUICaptureEnabled.enabled", "0");
    rsImageDetailEnhancerThread.DumpImage(image, imageId);
    system::SetParameter("rosen.dumpUICaptureEnabled.enabled", "1");
    rsImageDetailEnhancerThread.DumpImage(image, imageId);
    rsImageDetailEnhancerThread.DumpImage(nullptr, imageId);

    Drawing::Bitmap bmp1;
    bmp1.Build(0, 1, format);
    auto image2 = std::make_shared<Drawing::Image>();
    image2->BuildFromBitmap(bmp1);
    rsImageDetailEnhancerThread.DumpImage(image2, imageId);

    Drawing::Bitmap bmp2;
    bmp2.Build(1, 0, format);
    auto image3 = std::make_shared<Drawing::Image>();
    image3->BuildFromBitmap(bmp2);
    rsImageDetailEnhancerThread.DumpImage(image3, imageId);

    Drawing::Bitmap bmp3;
    bmp3.Build(0, 0, format);
    auto image4 = std::make_shared<Drawing::Image>();
    image4->BuildFromBitmap(bmp3);
    rsImageDetailEnhancerThread.DumpImage(image4, imageId);
    system::SetParameter("rosen.dumpUICaptureEnabled.enabled", type);
}

/**
 * @tc.name: SavePixelmapToFileTest
 * @tc.desc: SavePixelmapToFileTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(DetailEnhancerUtilsTest, SavePixelmapToFileTest, TestSize.Level1)
{
    DetailEnhancerUtils& detailEnhancerUtils = DetailEnhancerUtils::Instance();
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE};
    bmp.Build(500, 500, format);
    detailEnhancerUtils.SavePixelmapToFile(bmp, "scaledPixelmap_");
    detailEnhancerUtils.SavePixelmapToFile(bmp, "/data/scaledPixelmap_");
    EXPECT_EQ(bmp.GetWidth(), 500);
}

/**
 * @tc.name: ImageSamplingDumpTest
 * @tc.desc: ImageSamplingDumpTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, ImageSamplingDumpTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE};
    bmp.Build(500, 500, format);
    EXPECT_EQ(bmp.GetWidth(), 500);
    auto image = std::make_shared<Drawing::Image>();
    image->BuildFromBitmap(bmp);
    uint64_t imageId = 123456;
    rsImageDetailEnhancerThread.ImageSamplingDump(imageId);
    rsImageDetailEnhancerThread.SetScaledImage(imageId, image);
    rsImageDetailEnhancerThread.ImageSamplingDump(imageId);

    uint64_t imageId1 = 234567;
    Drawing::Bitmap bmp1;
    bmp1.Build(0, 0, format);
    auto image1 = std::make_shared<Drawing::Image>();
    image1->BuildFromBitmap(bmp1);
    rsImageDetailEnhancerThread.SetScaledImage(imageId1, image1);
    rsImageDetailEnhancerThread.ImageSamplingDump(imageId1);

    uint64_t imageId3 = 456789;
    Drawing::Bitmap bmp3;
    bmp3.Build(1, 0, format);
    auto image3 = std::make_shared<Drawing::Image>();
    image3->BuildFromBitmap(bmp3);
    rsImageDetailEnhancerThread.SetScaledImage(imageId3, image3);
    rsImageDetailEnhancerThread.ImageSamplingDump(imageId3);

    uint64_t imageId2 = 345678;
    Drawing::Bitmap bmp2;
    bmp2.Build(0, 1, format);
    auto image2 = std::make_shared<Drawing::Image>();
    image2->BuildFromBitmap(bmp2);
    rsImageDetailEnhancerThread.SetScaledImage(imageId2, image2);
    rsImageDetailEnhancerThread.ImageSamplingDump(imageId2);
}
#endif
} // namespace OHOS::Rosen