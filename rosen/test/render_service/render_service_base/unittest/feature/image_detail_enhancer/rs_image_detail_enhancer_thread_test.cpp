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

#include "feature/image_detail_enhancer/rs_image_detail_enhancer_thread.h"
#include "gtest/gtest.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_pixel_map_util.h"
#include "surface_buffer_impl.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
using namespace Drawing;
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
 * @tc.name: SetOutImageTest
 * @tc.desc: SetOutImageTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, SetOutImageTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    uint64_t imageId = 12345;
    auto image = std::make_shared<Drawing::Image>();
    rsImageDetailEnhancerThread.SetOutImage(imageId, image);
    EXPECT_EQ(rsImageDetailEnhancerThread.outImageMap_.count(imageId), 1);
    EXPECT_EQ(rsImageDetailEnhancerThread.outImageMap_[imageId], image);
    imageId = 23456;
    auto oldImage = std::make_shared<Drawing::Image>();
    auto newImage = std::make_shared<Drawing::Image>();
    rsImageDetailEnhancerThread.SetOutImage(imageId, oldImage);
    rsImageDetailEnhancerThread.SetOutImage(imageId, newImage);
    EXPECT_EQ(rsImageDetailEnhancerThread.outImageMap_.count(imageId), 1);
    imageId = 0;
    image = std::make_shared<Drawing::Image>();
    rsImageDetailEnhancerThread.SetOutImage(imageId, image);
    EXPECT_EQ(rsImageDetailEnhancerThread.outImageMap_.count(imageId), 1);
    EXPECT_EQ(rsImageDetailEnhancerThread.outImageMap_[imageId], image);
    imageId = 34567;
    std::shared_ptr<Drawing::Image> shareImage = nullptr;
    rsImageDetailEnhancerThread.SetOutImage(imageId, shareImage);
    EXPECT_EQ(rsImageDetailEnhancerThread.outImageMap_.count(imageId), 1);
    EXPECT_EQ(rsImageDetailEnhancerThread.outImageMap_[imageId], shareImage);
}

/**
 * @tc.name: GetOutImageTest
 * @tc.desc: GetOutImageTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, GetOutImageTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    uint64_t imageId = 12345;
    auto image = std::make_shared<Drawing::Image>();
    rsImageDetailEnhancerThread.outImageMap_[imageId] = image;
    auto result = rsImageDetailEnhancerThread.GetOutImage(imageId);
    EXPECT_EQ(result, image);
    imageId = 56789;
    result = rsImageDetailEnhancerThread.GetOutImage(imageId);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ReleaseOutImageTest
 * @tc.desc: ReleaseOutImageTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, ReleaseOutImageTest, TestSize.Level1)
{
    uint64_t imageId = 12345;
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    rsImageDetailEnhancerThread.ReleaseOutImage(imageId);
    EXPECT_EQ(rsImageDetailEnhancerThread.outImageMap_.count(imageId), 0);
    auto image = std::make_shared<Drawing::Image>();
    rsImageDetailEnhancerThread.outImageMap_[imageId] = image;
    EXPECT_EQ(rsImageDetailEnhancerThread.outImageMap_.count(imageId), 1);
    rsImageDetailEnhancerThread.ReleaseOutImage(imageId);
    EXPECT_EQ(rsImageDetailEnhancerThread.outImageMap_.count(imageId), 0);
    imageId = 67890;
    rsImageDetailEnhancerThread.ReleaseOutImage(imageId);
    EXPECT_EQ(rsImageDetailEnhancerThread.outImageMap_.count(imageId), 0);
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
    dstImage->BuildFromBitmap(bmp);
    int srcWidth = 1000;
    int srcHeight = 1000;
    int dstWidth = 800;
    int dstHeight = 800;
    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, true);
    rsImageDetailEnhancerThread.SetOutImage(imageId, dstImage);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 800, 800, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 800, 801, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 800, 810, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 800, 790, imageId);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, 801, 801, imageId);
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
    EXPECT_EQ(imageId, 12345);
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
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, true);
    rsImageDetailEnhancerThread.SetOutImage(imageId, dstImage);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

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
    rsImageDetailEnhancerThread.SetOutImage(imageId, nullptr);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, false);
    rsImageDetailEnhancerThread.SetOutImage(imageId, nullptr);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    rsImageDetailEnhancerThread.SetProcessStatus(imageId, false);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, false);
    rsImageDetailEnhancerThread.SetOutImage(imageId, nullptr);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    EXPECT_EQ(imageId, 12345);
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
    dstImage->BuildFromBitmap(bmp);
    int srcWidth = 1000;
    int srcHeight = 1000;
    int dstWidth = 800;
    int dstHeight = 800;
    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, true);
    rsImageDetailEnhancerThread.SetOutImage(imageId, dstImage);

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
    rsImageDetailEnhancerThread.SetOutImage(imageId, nullptr);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    rsImageDetailEnhancerThread.SetProcessStatus(imageId, true);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, false);
    rsImageDetailEnhancerThread.SetOutImage(imageId, nullptr);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);

    rsImageDetailEnhancerThread.SetProcessStatus(imageId, false);
    rsImageDetailEnhancerThread.SetProcessReady(imageId, false);
    rsImageDetailEnhancerThread.SetOutImage(imageId, nullptr);
    rsImageDetailEnhancerThread.ResetStatus(srcWidth, srcHeight, dstWidth, dstHeight, imageId);
    EXPECT_EQ(imageId, 12345);
}

/**
 * @tc.name: MarkDirtyTest
 * @tc.desc: MarkDirtyTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, MarkDirtyTest, TestSize.Level1)
{
    uint64_t nodeId = 123456;
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    rsImageDetailEnhancerThread.MarkDirty(nodeId);
    rsImageDetailEnhancerThread.callback_ = [](uint64_t) -> void {};
    rsImageDetailEnhancerThread.MarkDirty(nodeId);
    EXPECT_EQ(nodeId, 123456);
}

/**
 * @tc.name: IsSizeSupportTest
 * @tc.desc: IsSizeSupportTest001
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, IsSizeSupportTest001, TestSize.Level1)
{
    int srcWidth = 100;
    int srcHeight = 100;
    int dstWidth = 150;
    int dstHeight = 150;
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    bool result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    srcWidth = -10;
    srcHeight = 10;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    srcWidth = 10;
    srcHeight = -10;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    srcWidth = 0;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    srcWidth = -1;
    srcHeight = -1;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    srcWidth = 100;
    srcHeight = 100;
    dstWidth = 80;
    dstHeight = 100;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    srcWidth = 80;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    srcWidth = 100;
    dstWidth = 100;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    dstWidth = 501;
    dstHeight = 501;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_TRUE(result);
    dstWidth = 95;
    dstHeight = 95;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
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
    int srcWidth = 100;
    int srcHeight = 100;
    int dstWidth = 501;
    int dstHeight = 499;
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    bool result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    dstWidth = 499;
    dstHeight = 501;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    dstWidth = 3001;
    dstHeight = 3001;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    dstWidth = 2999;
    dstHeight = 3001;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    dstWidth = 3001;
    dstHeight = 2999;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    srcWidth = 1000;
    srcHeight = 1000;
    dstWidth = 950;
    dstHeight = 950;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    dstWidth = 600;
    dstHeight = 600;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_TRUE(result);
    srcWidth = 950;
    srcHeight = 950;
    dstWidth = 1000;
    dstHeight = 1000;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_FALSE(result);
    srcWidth = 600;
    srcHeight = 600;
    result = rsImageDetailEnhancerThread.IsSizeSupport(srcWidth, srcHeight, dstWidth, dstHeight);
    EXPECT_TRUE(result);
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
    rsImageDetailEnhancerThread.isEnable_ = true;
    bool result = rsImageDetailEnhancerThread.RegisterCallback(callback);
    EXPECT_TRUE(result);
    rsImageDetailEnhancerThread.isEnable_ = false;
    result = rsImageDetailEnhancerThread.RegisterCallback(callback);
    EXPECT_FALSE(result);
    rsImageDetailEnhancerThread.isEnable_ = true;
    result = rsImageDetailEnhancerThread.RegisterCallback(nullptr);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: GetEnableStatusTest
 * @tc.desc: GetEnableStatusTest
 * @tc.type: FUNC
 * @tc.require: issueIBZ6NM
 */
HWTEST_F(RSImageDetailEnhancerThreadTest, GetEnableStatusTest, TestSize.Level1)
{
    RSImageDetailEnhancerThread& rsImageDetailEnhancerThread = RSImageDetailEnhancerThread::Instance();
    bool result = rsImageDetailEnhancerThread.GetEnableStatus();
    rsImageDetailEnhancerThread.isEnable_ = true;
    result = rsImageDetailEnhancerThread.GetEnableStatus();
    EXPECT_TRUE(result);
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
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
    const sptr<SurfaceBuffer>& surfaceBuffer = new SurfaceBufferImpl();
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto image = std::make_shared<Drawing::Image>();
    std::shared_ptr<Drawing::Image> dstImage = rsImageDetailEnhancerThread.ScaleByAAE(
        const_cast<sptr<SurfaceBuffer>&>(surfaceBuffer), image, canvas);
    EXPECT_EQ(dstImage, nullptr);
    sptr<SurfaceBuffer> surfaceBuffer2 = nullptr;
    dstImage = rsImageDetailEnhancerThread.ScaleByAAE(surfaceBuffer2, image, canvas);
    EXPECT_EQ(dstImage, nullptr);
    canvas = nullptr;
    dstImage = rsImageDetailEnhancerThread.ScaleByAAE(const_cast<sptr<SurfaceBuffer>&>(surfaceBuffer), image, canvas);
    EXPECT_EQ(dstImage, nullptr);
    image = nullptr;
    dstImage = rsImageDetailEnhancerThread.ScaleByAAE(const_cast<sptr<SurfaceBuffer>&>(surfaceBuffer), image, canvas);
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
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE};
    bmp.Build(1000, 1000, format);
    auto srcImage = std::make_shared<Drawing::Image>();
    srcImage->BuildFromBitmap(bmp);
    int dstWidth = 700;
    int dstHeight = 700;
    auto new_surface = std::make_shared<Drawing::Surface>();
    auto new_canvas = std::make_shared<Drawing::Canvas>();
    auto result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, new_surface, new_canvas, srcImage);
    EXPECT_EQ(result, nullptr);
    dstWidth = 1000;
    dstHeight = 1000;
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, new_surface, new_canvas, srcImage);
    EXPECT_EQ(result, nullptr);
    dstWidth = 1300;
    dstHeight = 1300;
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, new_surface, new_canvas, srcImage);
    EXPECT_EQ(result, nullptr);
    dstWidth = 700;
    dstHeight = 1300;
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, new_surface, new_canvas, srcImage);
    EXPECT_EQ(result, nullptr);
    dstHeight = 700;
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, new_surface, new_canvas, srcImage);
    EXPECT_EQ(result, nullptr);
    new_canvas = nullptr;
    result = rsImageDetailEnhancerThread.ScaleByHDSampler(dstWidth, dstHeight, new_surface, new_canvas, srcImage);
    EXPECT_EQ(dstHeight, 700);
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
    rsImageDetailEnhancerThread.ReleaseOutImage(imageId);
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
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE};
    bmp.Build(800, 800, format);
    auto image = std::make_shared<Drawing::Image>();
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
    EXPECT_EQ(imageId, 45678);
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
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto image = std::make_shared<Drawing::Image>();
    DetailEnhancerUtils& detailEnhancerUtils = DetailEnhancerUtils::Instance();
    sptr<SurfaceBuffer> surfaceBuffer = detailEnhancerUtils.CreateSurfaceBuffer(600, 600);
    auto result = detailEnhancerUtils.MakeImageFromSurfaceBuffer(canvas, surfaceBuffer, image);
    result = detailEnhancerUtils.MakeImageFromSurfaceBuffer(canvas, surfaceBuffer, nullptr);
    canvas->gpuContext_ = std::make_shared<Drawing::GPUContext>();
    result = detailEnhancerUtils.MakeImageFromSurfaceBuffer(canvas, surfaceBuffer, image);
    surfaceBuffer = nullptr;
    result = detailEnhancerUtils.MakeImageFromSurfaceBuffer(canvas, surfaceBuffer, image);
    surfaceBuffer = new SurfaceBufferImpl();
    canvas = nullptr;
    result = detailEnhancerUtils.MakeImageFromSurfaceBuffer(canvas, surfaceBuffer, image);
    sptr<SurfaceBuffer> surfaceBuffer2 = detailEnhancerUtils.CreateSurfaceBuffer(10001, 10001);
    result = detailEnhancerUtils.MakeImageFromSurfaceBuffer(canvas, surfaceBuffer2, image);
    int srcWidth = 1024;
    int srcHeight = 1024;
    sptr<SurfaceBuffer> srcSurfaceBuffer = CreateSurfaceBuffer(srcWidth, srcHeight);
    std::shared_ptr<Image> srcImage = MakeImageFromSurfaceBuffer(srcSurfaceBuffer);
    int dstWidth = 512;
    int dstHeight = 512;
    sptr<SurfaceBuffer> surfaceBuffer3 = CreateSurfaceBuffer(dstWidth, dstHeight);
    canvas = std::make_shared<Drawing::Canvas>();
    canvas->gpuContext_ = RsVulkanContext::GetSingleton().CreateDrawingContext();
    detailEnhancerUtils.MakeImageFromSurfaceBuffer(canvas, surfaceBuffer3, srcImage);
    EXPECT_EQ(dstWidth, 512);
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
HWTEST_F(DetailEnhancerUtilsTest, SetMemoryName, TestSize.Level1)
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
#endif
} // namespace OHOS::Rosen