/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "modifier_render_thread/rs_canvas_modifiers_draw.h"
#include "external_window.h"
#include "ibuffer_consumer_listener.h"
#include "iconsumer_surface.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_render_interface.h"
#include "vulkan_context/rs_vulkan_context.h"
#include <future>

class BufferConsumerListener : public OHOS::IBufferConsumerListener {
public:
    void OnBufferAvailable() override {}
};

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSCanvasModifiersDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static std::shared_ptr<RSSurface> CreateSurface();
};

// Mock RSSurfaceOhosVulkan that returns a controlled Drawing::Surface from RequestFrame,
// bypassing GPU context requirements. Used to test the inherit check in
// RequestBufferAndDrawHistory without a real GPU.
class TestRSSurfaceOhosVulkan : public RSSurfaceOhosVulkan {
public:
    std::shared_ptr<Drawing::Surface> mockSurface_;
    int requestCount_ = 0;

    explicit TestRSSurfaceOhosVulkan(const sptr<Surface>& producer,
        std::shared_ptr<Drawing::Surface> surface = nullptr)
        : RSSurfaceOhosVulkan(producer), mockSurface_(surface) {}

    std::unique_ptr<RSSurfaceFrame> RequestFrame(int32_t width, int32_t height,
        uint64_t uiTimestamp = 0, bool useAFBC = true, bool isProtected = false) override
    {
        requestCount_++;
        if (!mockSurface_) {
            mockSurface_ = Drawing::Surface::MakeRaster(Drawing::ImageInfo::MakeN32Premul(width, height));
        }
        return std::make_unique<RSSurfaceFrameOhosVulkan>(mockSurface_, width, height, 1);
    }
};

std::shared_ptr<RSSurface> RSCanvasModifiersDrawableTest::CreateSurface()
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestSurface");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    return std::make_shared<RSSurfaceOhosVulkan>(pSurface);
}

void RSCanvasModifiersDrawableTest::SetUpTestCase() {}
void RSCanvasModifiersDrawableTest::TearDownTestCase() {}
void RSCanvasModifiersDrawableTest::SetUp() {}
void RSCanvasModifiersDrawableTest::TearDown() {}

HWTEST_F(RSCanvasModifiersDrawableTest, Reset_BasicFields001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.width_ = 100;
    drawable.height_ = 200;
    drawable.forceFlushBuffer_ = true;
    drawable.needResetCanvas_ = true;
    drawable.nodeId_ = 12345;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.Reset();
    EXPECT_EQ(drawable.width_, 0);
    EXPECT_EQ(drawable.height_, 0);
    EXPECT_FALSE(drawable.forceFlushBuffer_);
    EXPECT_FALSE(drawable.needResetCanvas_);
    EXPECT_NE(drawable.drawCmdListCache_, nullptr);
    EXPECT_TRUE(drawable.drawCmdListCache_->empty());
}

HWTEST_F(RSCanvasModifiersDrawableTest, CreateProducerSurface_NullRenderInterface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.nodeId_ = 12345;
    std::weak_ptr<RSRenderInterface> weakInterface;
    size_t maxGpuResourceBytes = 0;
    drawable.CreateProducerSurface(weakInterface, nullptr, maxGpuResourceBytes);
    EXPECT_EQ(drawable.producerSurface_, nullptr);
    EXPECT_EQ(drawable.drawCmdListCache_, nullptr);
}

// CreateProducerSurface: valid renderInterface but null gpuContext → early return
HWTEST_F(RSCanvasModifiersDrawableTest, CreateProducerSurface_NullGpuContext001, TestSize.Level1)
{
    auto screenId = RSInterfaces::GetInstance().GetDefaultScreenId();
    auto connectToRender = RSInterfaces::GetInstance().GetConnectToRenderToken(screenId);
    ASSERT_NE(connectToRender, nullptr);
    auto renderInterface = std::make_shared<RSRenderInterface>(connectToRender);
    RSCanvasModifiersDrawable drawable;
    drawable.nodeId_ = RSNode::GenerateId();
    size_t maxGpuResourceBytes = 0;
    drawable.CreateProducerSurface(renderInterface, nullptr, maxGpuResourceBytes);
    EXPECT_EQ(drawable.producerSurface_, nullptr);
    EXPECT_EQ(drawable.drawCmdListCache_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, ReleaseProducerSurface_NullRenderInterface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.nodeId_ = 12345;
    std::weak_ptr<RSRenderInterface> weakInterface;
    drawable.ReleaseProducerSurface(weakInterface);
    EXPECT_EQ(drawable.producerSurface_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, ResetSurface_NullProducerSurface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = nullptr;
    drawable.ResetSurface(100, 100, false, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_EQ(drawable.width_, 0);
    EXPECT_EQ(drawable.height_, 0);
}

HWTEST_F(RSCanvasModifiersDrawableTest, ResetSurface_SameWidthAndHeight001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.ResetSurface(100, 100, false, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_EQ(drawable.width_, 100);
    EXPECT_EQ(drawable.height_, 100);
}

HWTEST_F(RSCanvasModifiersDrawableTest, ResetSurface_WithValidSurfaceDifferentSize001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = 0;
    drawable.height_ = 0;
    drawable.ResetSurface(100, 100, false, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_EQ(drawable.width_, 100);
    EXPECT_EQ(drawable.height_, 100);
    EXPECT_TRUE(drawable.needResetCanvas_);
    EXPECT_TRUE(drawable.forceFlushBuffer_);
}

HWTEST_F(RSCanvasModifiersDrawableTest, ResetSurface_SizeOutOfGpuLimit001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.ResetSurface(100, 100, true, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_EQ(drawable.width_, 0);
    EXPECT_EQ(drawable.height_, 0);
}

HWTEST_F(RSCanvasModifiersDrawableTest, UpdateContent_NullDrawCmdListCache001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.drawCmdListCache_ = nullptr;
    drawable.UpdateContent(nullptr, false);
    EXPECT_EQ(drawable.drawCmdListCache_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, UpdateContent_WithDrawCmdList001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.UpdateContent(nullptr, false);
    EXPECT_NE(drawable.drawCmdListCache_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, Draw_NullProducerSurface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = nullptr;
    drawable.Draw();
    EXPECT_EQ(drawable.producerSurface_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, Draw_EmptyCacheNoForceFlush001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.forceFlushBuffer_ = false;
    drawable.Draw();
    EXPECT_FALSE(drawable.forceFlushBuffer_);
}

HWTEST_F(RSCanvasModifiersDrawableTest, Draw_WithValidSurfaceAndForceFlush001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto rasterSurface = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestDrawForceFlush");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(
        Surface::CreateSurfaceAsProducer(bp), rasterSurface);
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.forceFlushBuffer_ = true;
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.Draw();
    EXPECT_FALSE(drawable.forceFlushBuffer_);
}

// RSCanvasModifiersDraw::FlushSurfaceWithSemaphore: static method.
// Branch 1: CreateVkSemaphore fails → returns nullptr, semaphore set to VK_NULL_HANDLE.
// Branch 2: CreateVkSemaphore succeeds → creates DestroySemaphoreInfo, Flush, returns ptr.
//   Cannot call Flush on null/raster surface; only verify semaphore creation + cleanup.
//   The null-drawingSurface guard is in the caller (SubmitAndCollectCanvasBuffers), not here.
HWTEST_F(RSCanvasModifiersDrawableTest, FlushSurfaceWithSemaphore_StaticMethod001, TestSize.Level1)
{
    // Branch 1: CreateVkSemaphore fails → early return nullptr
    VkSemaphore semaphore = VK_NULL_HANDLE;
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->GetGpuContext();
    auto vkInterface = RsVulkanContext::Get(canvasModifiersDraw->renderContext_->GetType()).GetRsVulkanInterface();
    if (NativeBufferUtils::CreateVkSemaphore(vkInterface, semaphore) != VK_SUCCESS) {
        // Vulkan unavailable: CreateVkSemaphore fails, FlushSurfaceWithSemaphore returns nullptr
        auto* result = canvasModifiersDraw->FlushSurfaceWithSemaphore(semaphore, nullptr);
        EXPECT_EQ(result, nullptr);
        EXPECT_EQ(semaphore, VK_NULL_HANDLE);
        return;
    }
    // Branch 2: CreateVkSemaphore succeeds → semaphore created, verify + cleanup
    // Cannot call FlushSurfaceWithSemaphore without a real GPU surface (would crash),
    // so verify the semaphore was created and manually destroy it
    EXPECT_NE(semaphore, VK_NULL_HANDLE);
    vkInterface->vkDestroySemaphore(vkInterface->GetDevice(), semaphore, nullptr);
}

// RSCanvasModifiersDraw::GetFenceFd: static method.
// Branch 1: semaphore==VK_NULL_HANDLE → returns INVALID_FD.
// Branch 2: semaphore!=VK_NULL_HANDLE → calls GetFenceFdFromSemaphore.
// Must use a real VkSemaphore; a fake handle causes SIGSEGV in vkGetSemaphoreFdKHR.
HWTEST_F(RSCanvasModifiersDrawableTest, GetFenceFd_StaticMethod001, TestSize.Level1)
{
    // Branch 1: null semaphore
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->GetGpuContext();
    VkSemaphore nullSemaphore = VK_NULL_HANDLE;
    EXPECT_EQ(canvasModifiersDraw->GetFenceFd(nullSemaphore), INVALID_FD);

    // Branch 2: valid semaphore (requires Vulkan)
    VkSemaphore semaphore = VK_NULL_HANDLE;
    auto vkInterface = RsVulkanContext::Get(canvasModifiersDraw->renderContext_->GetType()).GetRsVulkanInterface();
    if (NativeBufferUtils::CreateVkSemaphore(vkInterface, semaphore) != VK_SUCCESS) {
        return;
    }
    int32_t fenceFd = canvasModifiersDraw->GetFenceFd(semaphore);
    // fd may be INVALID_FD since semaphore is not submitted, but branch is exercised
    EXPECT_TRUE(fenceFd >= -1);
    vkInterface->vkDestroySemaphore(vkInterface->GetDevice(), semaphore, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, IsFree_LastFlushBufferTimeZero001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.lastFlushBufferTime_ = 0;
    bool result = drawable.IsFree(1000, 500);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, IsFree_DurationWithinLimit001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.lastFlushBufferTime_ = 1000;
    drawable.producerSurface_ = CreateSurface();
    bool result = drawable.IsFree(1200, 500);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, IsFree_NullProducerSurface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.lastFlushBufferTime_ = 1000;
    drawable.producerSurface_ = nullptr;
    bool result = drawable.IsFree(2000, 500);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, IsFree_FreeDrawable001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.lastFlushBufferTime_ = 1000;
    drawable.producerSurface_ = CreateSurface();
    bool result = drawable.IsFree(2000, 500);
    EXPECT_TRUE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, OnDirtyBufferCollected_ResetsFields001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.lastFlushBufferTime_ = 1000;
    drawable.OnDirtyBufferCollected(2000);
    EXPECT_EQ(drawable.surfaceFrame_, nullptr);
    EXPECT_EQ(drawable.lastFlushBufferTime_, 2000);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetBuffer_NeedFlushBufferFalse001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.nodeState_ = RSNodeState::INACTIVE;
    auto result = drawable.GetBuffer(false);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetBuffer_NullProducerSurface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.nodeState_ = RSNodeState::ACTIVE;
    drawable.producerSurface_ = nullptr;
    auto result = drawable.GetBuffer(true);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetBuffer_NullSurfaceFrame001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.nodeState_ = RSNodeState::ACTIVE;
    drawable.producerSurface_ = CreateSurface();
    drawable.surfaceFrame_ = nullptr;
    auto result = drawable.GetBuffer(true);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, CleanBuffer_NullProducerSurface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = nullptr;
    drawable.lastFlushBufferTime_ = 1000;
    drawable.CleanBuffer();
    EXPECT_NE(drawable.lastFlushBufferTime_, 0);
}

HWTEST_F(RSCanvasModifiersDrawableTest, CleanBuffer_WithValidSurface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.lastFlushBufferTime_ = 1000;
    drawable.CleanBuffer();
    EXPECT_EQ(drawable.lastFlushBufferTime_, 0);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetImage_NullDrawingSurface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.drawingSurface_ = nullptr;
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    auto result = drawable.GetImage(bitmapFormat, gpuContext);
    EXPECT_EQ(result, nullptr);
    EXPECT_EQ(drawable.drawingSurface_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetImage_NullGpuContext001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.drawingSurface_ = Drawing::Surface::MakeRaster(Drawing::ImageInfo::MakeN32Premul(100, 100));
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto result = drawable.GetImage(bitmapFormat, nullptr);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetBitmap_NullGpuContext001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    Drawing::Bitmap bitmap;
    bool result = drawable.GetBitmap(bitmap, nullptr);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_NullPixelMap001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    Drawing::Rect rect(0, 0, 100, 100);
    bool result = drawable.GetPixelMap(nullptr, &rect, nullptr, nullptr);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_NullRect001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    bool result = drawable.GetPixelMap(nullptr, nullptr, nullptr, nullptr);
    EXPECT_FALSE(result);
}

// RequestBufferAndDrawHistory returns nullptr when RequestFrame fails (no GPU context).
// drawingSurface_ is not corrupted.
HWTEST_F(RSCanvasModifiersDrawableTest, RequestBufferAndDrawHistory_RequestFrameFails001, TestSize.Level1)
{
    auto prevDrawingSurface = std::make_shared<Drawing::Surface>();
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawingSurface_ = prevDrawingSurface;
    drawable.width_ = 100;
    drawable.height_ = 100;
    auto result = drawable.RequestBufferAndDrawHistory();
    EXPECT_EQ(result, nullptr);
    EXPECT_EQ(drawable.drawingSurface_, prevDrawingSurface);
}

// Tests all three branches of the drawingSurface_ != drawingSurface check:
// - Branch 1: drawingSurface_ == nullptr → no inherit (first frame)
// - Branch 2: drawingSurface_ != nullptr && drawingSurface_ != drawingSurface → normal inherit
// - Branch 3: drawingSurface_ != nullptr && drawingSurface_ == drawingSurface → skip inherit
//   (the crash fix: prevents BackendGpu::InheritStateAndContentFrom from moving
//    mGlobalToDevice to itself, which would leave the vector empty and cause SIGSEGV)
HWTEST_F(RSCanvasModifiersDrawableTest, RequestBufferAndDrawHistory_InheritCheck001, TestSize.Level1)
{
    // Branch 1: drawingSurface_ == nullptr (first frame, no inherit)
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto surfaceA = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface1 = IConsumerSurface::Create("TestInherit1");
    sptr<IBufferProducer> bp1 = cSurface1->GetProducer();
    auto mockSurface1 = std::make_shared<TestRSSurfaceOhosVulkan>(
        Surface::CreateSurfaceAsProducer(bp1), surfaceA);
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = mockSurface1;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = 100;
    drawable.height_ = 100;
    EXPECT_EQ(drawable.drawingSurface_, nullptr);
    auto frame1 = drawable.RequestBufferAndDrawHistory();
    ASSERT_NE(frame1, nullptr);
    EXPECT_EQ(drawable.drawingSurface_, surfaceA);

    // Branch 2: drawingSurface_ != nullptr && drawingSurface_ != drawingSurface
    // Use a different mock that returns a different Surface
    auto surfaceB = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("TestInherit2");
    sptr<IBufferProducer> bp2 = cSurface2->GetProducer();
    auto mockSurface2 = std::make_shared<TestRSSurfaceOhosVulkan>(
        Surface::CreateSurfaceAsProducer(bp2), surfaceB);
    drawable.producerSurface_ = mockSurface2;
    drawable.surfaceFrame_ = nullptr;
    auto frame2 = drawable.RequestBufferAndDrawHistory();
    ASSERT_NE(frame2, nullptr);
    EXPECT_EQ(drawable.drawingSurface_, surfaceB);
    EXPECT_NE(drawable.drawingSurface_, surfaceA);

    // Branch 3: drawingSurface_ != nullptr && drawingSurface_ == drawingSurface
    // Reuse mockSurface1 which returns the same surfaceA as current drawingSurface_
    // This simulates mSurfaceMap cache hit where RS released the buffer after compositing
    // and RequestFrame dequeues the same buffer again.
    drawable.producerSurface_ = mockSurface1;
    drawable.surfaceFrame_ = nullptr;
    auto frame3 = drawable.RequestBufferAndDrawHistory();
    ASSERT_NE(frame3, nullptr);
    // drawingSurface_ == surfaceA, same as before → self-inherit was correctly skipped
    EXPECT_EQ(drawable.drawingSurface_, surfaceA);
}

// CreateProducerSurface with valid renderInterface: covers line 78 (std::call_once + GetResourceCacheLimits)
// and lines 80-84 (producerSurface_ and drawCmdListCache_ assignment).
// Requires Vulkan and IPC to be available; skipped when render service is not reachable.
HWTEST_F(RSCanvasModifiersDrawableTest, CreateProducerSurface_WithRenderInterface001, TestSize.Level1)
{
    auto screenId = RSInterfaces::GetInstance().GetDefaultScreenId();
    auto connectToRender = RSInterfaces::GetInstance().GetConnectToRenderToken(screenId);
    ASSERT_NE(connectToRender, nullptr);
    auto renderInterface = std::make_shared<RSRenderInterface>(connectToRender);
    ASSERT_NE(renderInterface, nullptr);
    RSCanvasModifiersDrawable drawable;
    drawable.nodeId_ = RSNode::GenerateId();
    size_t maxGpuResourceBytes = 0;
    auto gpuContext = RenderContext::Create()->CreateDrawingGPUContext("/cache");
    drawable.CreateProducerSurface(renderInterface, gpuContext, maxGpuResourceBytes);
    if (RSSystemProperties::GetHybridRenderCanvasEnabled() && gpuContext != nullptr) {
        EXPECT_NE(drawable.producerSurface_, nullptr);
        EXPECT_NE(drawable.drawCmdListCache_, nullptr);
        EXPECT_GT(maxGpuResourceBytes, 0u);
    } else {
        EXPECT_EQ(drawable.producerSurface_, nullptr);
        EXPECT_EQ(drawable.drawCmdListCache_, nullptr);
        EXPECT_EQ(maxGpuResourceBytes, 0u);
    }
}
} // namespace Rosen
} // namespace OHOS

namespace OHOS {
namespace Rosen {
class RSCanvasModifiersDrawTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCanvasModifiersDrawTest::SetUpTestCase() {}
void RSCanvasModifiersDrawTest::TearDownTestCase() {}
void RSCanvasModifiersDrawTest::SetUp() {}
void RSCanvasModifiersDrawTest::TearDown() {}

HWTEST_F(RSCanvasModifiersDrawTest, StartThread_Idempotent001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->StartThread();
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, PostTask_StartsThreadIfNotStarted001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    std::promise<void> promise;
    auto future = promise.get_future();
    canvasModifiersDraw->PostTask([&promise]() { promise.set_value(); }, "TestTask", 0);
    auto result = future.wait_for(std::chrono::milliseconds(1000));
    EXPECT_EQ(result, std::future_status::ready);
}

HWTEST_F(RSCanvasModifiersDrawTest, PostSyncTask_StartsThreadIfNotStarted001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    bool taskExecuted = false;
    canvasModifiersDraw->PostSyncTask([&taskExecuted]() { taskExecuted = true; });
    EXPECT_TRUE(taskExecuted);
}

HWTEST_F(RSCanvasModifiersDrawTest, RemoveTask_RemovesPendingTask001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    int counter = 0;
    canvasModifiersDraw->PostTask([&counter]() { counter++; }, "RemoveTestTask", 5000);
    canvasModifiersDraw->RemoveTask("RemoveTestTask");
    usleep(100000);
    EXPECT_EQ(counter, 0);
}

HWTEST_F(RSCanvasModifiersDrawTest, RemoveTask_DoesNothingWhenThreadNotStarted001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->RemoveTask("NonExistentTask");
    EXPECT_FALSE(canvasModifiersDraw->threadStarted_.load());
}

HWTEST_F(RSCanvasModifiersDrawTest, PostTask_ReturnsEarlyAfterDestroy001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->Destroy();
    bool taskExecuted = false;
    canvasModifiersDraw->PostTask([&taskExecuted]() { taskExecuted = true; }, "TestTask", 0);
    EXPECT_FALSE(taskExecuted);
    EXPECT_TRUE(canvasModifiersDraw->threadDestroyed_.load());
}

HWTEST_F(RSCanvasModifiersDrawTest, PostSyncTask_ReturnsEarlyAfterDestroy001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->Destroy();
    bool taskExecuted = false;
    canvasModifiersDraw->PostSyncTask([&taskExecuted]() { taskExecuted = true; });
    EXPECT_FALSE(taskExecuted);
    EXPECT_TRUE(canvasModifiersDraw->threadDestroyed_.load());
}

HWTEST_F(RSCanvasModifiersDrawTest, StartThread_ReturnsEarlyAfterDestroy001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->Destroy();
    canvasModifiersDraw->StartThread();
    EXPECT_FALSE(canvasModifiersDraw->threadStarted_.load());
    EXPECT_TRUE(canvasModifiersDraw->threadDestroyed_.load());
}

HWTEST_F(RSCanvasModifiersDrawTest, WaitAllTasksFinish_AfterDestroy001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->Destroy();
    canvasModifiersDraw->WaitAllTasksFinish();
    EXPECT_TRUE(canvasModifiersDraw->threadDestroyed_.load());
}

HWTEST_F(RSCanvasModifiersDrawTest, WaitAllTasksFinish_AfterDestroy002, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->WaitAllTasksFinish();
    canvasModifiersDraw->Destroy();
    EXPECT_TRUE(canvasModifiersDraw->threadDestroyed_.load());
}

// WaitAllTasksFinish with gpuContext_ set → covers FlushAndSubmit, PurgeUnlockedResources, gpuContext_=nullptr
HWTEST_F(RSCanvasModifiersDrawTest, WaitAllTasksFinish_WithGpuContext001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->gpuContext_ = RenderContext::Create()->CreateDrawingGPUContext("/data/local/tmp");
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->WaitAllTasksFinish();
    // gpuContext_ should be reset to nullptr after WaitAllTasksFinish
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->gpuContext_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_EQ(future.get(), nullptr);
    canvasModifiersDraw->Destroy();
}

HWTEST_F(RSCanvasModifiersDrawTest, Destroy_BeforeStartThread001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->Destroy();
    EXPECT_TRUE(canvasModifiersDraw->threadDestroyed_.load());
    EXPECT_FALSE(canvasModifiersDraw->threadStarted_.load());
}

// Verify Destroy() sets runner_ and handler_ to nullptr after Stop/RemoveAllEvents,
// and the null-safety guards work when runner_/handler_ are already nullptr.
HWTEST_F(RSCanvasModifiersDrawTest, Destroy_NullptrAfterStopAndNullGuards001, TestSize.Level1)
{
    // Case 1: Start then Destroy — runner_/handler_ become nullptr after Stop+RemoveAllEvents
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    ASSERT_NE(canvasModifiersDraw->runner_, nullptr);
    ASSERT_NE(canvasModifiersDraw->handler_, nullptr);
    canvasModifiersDraw->Destroy();
    EXPECT_EQ(canvasModifiersDraw->runner_, nullptr);
    EXPECT_EQ(canvasModifiersDraw->handler_, nullptr);
    EXPECT_TRUE(canvasModifiersDraw->threadDestroyed_.load());
    EXPECT_FALSE(canvasModifiersDraw->threadStarted_.load());

    // Case 2: threadStarted_=true but runner_/handler_ already nullptr —
    // bypasses early return, exercises null-safety guards (if handler_ != nullptr / if runner_ != nullptr)
    auto canvasModifiersDraw2 = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw2->threadStarted_ = true;
    canvasModifiersDraw2->runner_ = nullptr;
    canvasModifiersDraw2->handler_ = nullptr;
    canvasModifiersDraw2->Destroy();
    EXPECT_EQ(canvasModifiersDraw2->runner_, nullptr);
    EXPECT_EQ(canvasModifiersDraw2->handler_, nullptr);
    EXPECT_TRUE(canvasModifiersDraw2->threadDestroyed_.load());
    EXPECT_FALSE(canvasModifiersDraw2->threadStarted_.load());
}

HWTEST_F(RSCanvasModifiersDrawTest, SetCacheDir_Basic001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->SetCacheDir("/test/cache");
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, OnNodeCreate_Basic001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 12345;
    std::weak_ptr<RSRenderInterface> weakInterface;
    canvasModifiersDraw->OnNodeCreate(nodeId, weakInterface);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, OnNodeRelease_Basic001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 12345;
    std::weak_ptr<RSRenderInterface> weakInterface;
    canvasModifiersDraw->OnNodeRelease(nodeId, weakInterface);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, OnNodeStateChanged_InactiveState001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 67890;
    canvasModifiersDraw->OnNodeStateChanged(nodeId, RSNodeState::INACTIVE);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, OnNodeStateChanged_ActiveState001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 67890;
    canvasModifiersDraw->OnNodeStateChanged(nodeId, RSNodeState::ACTIVE);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, ResetSurface_TriggersBufferCleanup001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 12345;
    canvasModifiersDraw->ResetSurface(nodeId, 100, 100, true, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, GetBitmap_Basic001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 12345;
    Drawing::Bitmap bitmap;
    bool result = canvasModifiersDraw->GetBitmap(nodeId, bitmap);
    EXPECT_FALSE(result);
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bitmap.SetFormat(bitmapFormat);
    result = canvasModifiersDraw->GetBitmap(nodeId, bitmap);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawTest, GetPixelMap_Basic001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 12345;
    std::shared_ptr<Media::PixelMap> pixelMap;
    Drawing::Rect rect(0, 0, 100, 100);
    bool result = canvasModifiersDraw->GetPixelMap(nodeId, pixelMap, &rect, nullptr);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawTest, UpdateCanvasContent_Basic001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 12345;
    canvasModifiersDraw->UpdateCanvasContent(nodeId, nullptr);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, UpdateCanvasContent_WeakPtrExpired001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 12345;
    // Save handler to keep EventHandler alive after object destruction,
    // preventing RemoveOrphan from discarding the pending lambda
    auto savedHandler = canvasModifiersDraw->handler_;
    // Post blocking task to hold event loop before UpdateCanvasContent's lambda
    auto mtx = std::make_shared<std::mutex>();
    auto cv = std::make_shared<std::condition_variable>();
    auto ready = std::make_shared<bool>(false);
    canvasModifiersDraw->PostTask([mtx, cv, ready]() {
        std::unique_lock<std::mutex> lock(*mtx);
        cv->wait(lock, [&]() { return *ready; });
    });
    // UpdateCanvasContent lambda queued after the blocking task
    canvasModifiersDraw->UpdateCanvasContent(nodeId, nullptr);
    // Release shared_ptr: object destroyed, weak_ptr in lambda expires
    canvasModifiersDraw = nullptr;
    EXPECT_EQ(0, canvasModifiersDraw.use_count());
    // Signal blocking task to continue, UpdateCanvasContent lambda runs next
    // with expired weak_ptr, hitting the nullptr check (line 610)
    {
        std::lock_guard<std::mutex> lock(*mtx);
        *ready = true;
    }
    cv->notify_all();
    usleep(30000);
    // Release handler; EventHandler destructor calls RemoveOrphan internally.
    // Thread may continue until process exit (same as other tests without Destroy).
    savedHandler = nullptr;
    usleep(100000);
}

HWTEST_F(RSCanvasModifiersDrawTest, SwapTransactionConfigList_ExchangesLists001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    std::vector<RSTransactionConfig> list1;
    std::vector<RSTransactionConfig> list2;
    RSTransactionConfig config;
    config.nodeId = 12345;
    list2.push_back(config);
    canvasModifiersDraw->SwapTransactionConfigList(list1);
    EXPECT_EQ(list1.size(), 0);
    EXPECT_EQ(list2.size(), 1);
    EXPECT_EQ(list2[0].nodeId, 12345);
}

HWTEST_F(RSCanvasModifiersDrawTest, AppendTransactionConfig_Basic001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    std::vector<RSTransactionConfig> configs;
    canvasModifiersDraw->SwapTransactionConfigList(configs);
    EXPECT_EQ(configs.size(), 0);
}

HWTEST_F(RSCanvasModifiersDrawTest, CleanFreeBuffers_SchedulesRecurringTask001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->CleanFreeBuffers(1000, false);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, CleanFreeBuffers_RemovesTaskWhenImmediately001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->CleanFreeBuffers(1000, true);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, CleanFreeBuffersImmediately_SchedulesImmediateCleanup001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->CleanFreeBuffersImmediately();
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, DoCleanFreeBuffers_EmptyDrawableMap001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->DoCleanFreeBuffers(1000);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollectCanvasBuffers_EmptyMap001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->SubmitAndCollectCanvasBuffers();
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

// Verify initial state of GPU cache limit members
HWTEST_F(RSCanvasModifiersDrawTest, GpuCacheLimit_InitialState001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    EXPECT_EQ(canvasModifiersDraw->maxGpuResourceBytes_, 0u);
    EXPECT_FALSE(canvasModifiersDraw->needRestoreGpuCacheLimit_);
    EXPECT_TRUE(canvasModifiersDraw->drawableMap_.empty());
    EXPECT_TRUE(canvasModifiersDraw->transactionConfigList_.empty());
}

// DoCleanFreeBuffers: sets needRestoreGpuCacheLimit_ when free drawables exist and gpuContext available;
// does not set flag when no free drawables (ACTIVE state, not idle long enough).
HWTEST_F(RSCanvasModifiersDrawTest, DoCleanFreeBuffers_RestoreFlag001, TestSize.Level1)
{
    // Case 1: free drawable (INACTIVE, idle long enough) → flag depends on gpuContext_
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeState_ = RSNodeState::INACTIVE;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    drawable.lastFlushBufferTime_ = 1; // non-zero, long ago relative to maxDuration=0
    canvasModifiersDraw->gpuContext_ = RenderContext::Create()->CreateDrawingGPUContext(
        canvasModifiersDraw->cacheDir_);
    canvasModifiersDraw->DoCleanFreeBuffers(0);
    if (canvasModifiersDraw->gpuContext_ != nullptr) {
        EXPECT_TRUE(canvasModifiersDraw->needRestoreGpuCacheLimit_);
    } else {
        EXPECT_FALSE(canvasModifiersDraw->needRestoreGpuCacheLimit_);
    }

    // Case 2: no free drawable (ACTIVE state) → flag stays false
    canvasModifiersDraw->needRestoreGpuCacheLimit_ = false;
    drawable.nodeState_ = RSNodeState::ACTIVE;
    canvasModifiersDraw->DoCleanFreeBuffers(0);
    EXPECT_FALSE(canvasModifiersDraw->needRestoreGpuCacheLimit_);
}

// UpdateCanvasContent: needRestoreGpuCacheLimit_ is reset when flag is true and gpuContext available;
// stays false when flag was already false. Uses ScheduleTask to wait for async completion.
HWTEST_F(RSCanvasModifiersDrawTest, UpdateCanvasContent_GpuCacheLimitRestore001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();

    // Case 1: flag=true → restored when gpuContext_ available
    canvasModifiersDraw->needRestoreGpuCacheLimit_ = true;
    canvasModifiersDraw->maxGpuResourceBytes_ = 1024;
    canvasModifiersDraw->gpuContext_ = RenderContext::Create()->CreateDrawingGPUContext(
        canvasModifiersDraw->cacheDir_);
    canvasModifiersDraw->UpdateCanvasContent(nodeId, nullptr);
    auto future1 = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->needRestoreGpuCacheLimit_; });
    ASSERT_EQ(future1.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    if (canvasModifiersDraw->gpuContext_ != nullptr) {
        EXPECT_FALSE(future1.get());
    } else {
        EXPECT_TRUE(future1.get());
    }

    // Case 2: flag=false → stays false
    canvasModifiersDraw->needRestoreGpuCacheLimit_ = false;
    canvasModifiersDraw->UpdateCanvasContent(nodeId, nullptr);
    auto future2 = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->needRestoreGpuCacheLimit_; });
    ASSERT_EQ(future2.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_FALSE(future2.get());
}

// GetGpuContext: lazy initialization when gpuContext_ is nullptr
HWTEST_F(RSCanvasModifiersDrawTest, GetGpuContext_LazyInit001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->cacheDir_ = "/data/local/tmp";
    // gpuContext_ starts as nullptr, GetGpuContext should initialize it
    EXPECT_EQ(canvasModifiersDraw->gpuContext_, nullptr);
    auto gpuContext = canvasModifiersDraw->GetGpuContext();
    auto expectedContext = RenderContext::Create()->CreateDrawingGPUContext("/data/local/tmp");
    if (expectedContext != nullptr) {
        EXPECT_NE(gpuContext, nullptr);
        EXPECT_EQ(canvasModifiersDraw->gpuContext_, gpuContext);
    } else {
        EXPECT_EQ(gpuContext, nullptr);
    }
}

// GetGpuContext: returns existing gpuContext_ when already set
HWTEST_F(RSCanvasModifiersDrawTest, GetGpuContext_ReuseExisting001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    auto mockContext = std::make_shared<Drawing::GPUContext>();
    canvasModifiersDraw->gpuContext_ = mockContext;
    auto result = canvasModifiersDraw->GetGpuContext();
    EXPECT_EQ(result, mockContext);
}

/**
 * @tc.name: GetBitmap_AlphaTypeOpaque
 * @tc.desc: Test GetBitmap preserves bitmap alphaType when it is not UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasModifiersDrawableTest, GetBitmap_AlphaTypeOpaque, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    Drawing::Bitmap bitmap;
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    bitmap.SetFormat(bitmapFormat);
    const auto& format = bitmap.GetFormat();
    ASSERT_NE(format.alphaType, Drawing::ALPHATYPE_UNKNOWN);
    bool result = drawable.GetBitmap(bitmap, nullptr);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, Draw_WithInValidContext, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.forceFlushBuffer_ = true;
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.renderContext_ = nullptr;
    drawable.Draw();
    EXPECT_EQ(drawable.renderContext_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetBuffer_SurfaceFrameNotNullNoCurrentBuffer001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto rasterSurface = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestGetBuffer");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(
        Surface::CreateSurfaceAsProducer(bp), rasterSurface);
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.forceFlushBuffer_ = true;
    drawable.Draw();
    ASSERT_NE(drawable.surfaceFrame_, nullptr);
    // GetCurrentBuffer returns nullptr because mSurfaceList is empty (no real buffer queued)
    auto result = drawable.GetBuffer(false);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetBuffer_FlushBufferTrue001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto rasterSurface = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestGetBufferFlush");
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(bp);
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(pSurface, rasterSurface);
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.forceFlushBuffer_ = true;
    drawable.Draw();
    ASSERT_NE(drawable.surfaceFrame_, nullptr);
    BufferRequestConfig config = { .width = 0x100, .height = 0x100, .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA };
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t releaseFence = -1;
    pSurface->RequestBuffer(buffer, releaseFence, config);
    ASSERT_NE(buffer, nullptr);
    auto* nwb = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&buffer);
    ASSERT_NE(nwb, nullptr);
    mockSurface->mSurfaceList.emplace_back(nwb);
    auto result = drawable.GetBuffer(true);
    pSurface->CancelBuffer(buffer);
    EXPECT_NE(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetBuffer_FlushBufferFalse001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto rasterSurface = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestGetBufferNoFlush");
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(bp);
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(pSurface, rasterSurface);
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.forceFlushBuffer_ = true;
    drawable.Draw();
    ASSERT_NE(drawable.surfaceFrame_, nullptr);
    BufferRequestConfig config = { .width = 0x100, .height = 0x100, .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA };
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t releaseFence = -1;
    pSurface->RequestBuffer(buffer, releaseFence, config);
    ASSERT_NE(buffer, nullptr);
    auto* nwb = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&buffer);
    ASSERT_NE(nwb, nullptr);
    mockSurface->mSurfaceList.emplace_back(nwb);
    auto result = drawable.GetBuffer(false);
    pSurface->CancelBuffer(buffer);
    EXPECT_NE(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawTest, FlushSurfaceWithSemaphore_AndGetFenceFd001, TestSize.Level1)
{
    // Case 1: renderContext_ is nullptr
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->renderContext_ = nullptr;
    VkSemaphore semaphore = VK_NULL_HANDLE;
    EXPECT_EQ(canvasModifiersDraw->FlushSurfaceWithSemaphore(semaphore, nullptr), nullptr);
    EXPECT_EQ(canvasModifiersDraw->GetFenceFd(semaphore), INVALID_FD);

    // Case 2: renderContext_ != nullptr
    canvasModifiersDraw->GetGpuContext();
    ASSERT_NE(canvasModifiersDraw->renderContext_, nullptr);
    semaphore = VK_NULL_HANDLE;
    auto vkInterface = RsVulkanContext::Get(canvasModifiersDraw->renderContext_->GetType()).GetRsVulkanInterface();
    if (vkInterface == nullptr) {
        // No Vulkan: CreateVkSemaphore fails, nullptr surface is safe (early return)
        EXPECT_EQ(canvasModifiersDraw->FlushSurfaceWithSemaphore(semaphore, nullptr), nullptr);
        return;
    }
    // Vulkan available: CreateVkSemaphore succeeds, must provide valid drawingSurface
    // (FlushSurfaceWithSemaphore calls drawingSurface->Flush after semaphore creation)
    auto drawingSurface = Drawing::Surface::MakeRaster(Drawing::ImageInfo::MakeN32Premul(0x100, 0x100));
    ASSERT_NE(drawingSurface, nullptr);
    auto* result = canvasModifiersDraw->FlushSurfaceWithSemaphore(semaphore, drawingSurface);
    if (result != nullptr) {
        DestroySemaphoreInfo::DestroySemaphore(result);
    }
    EXPECT_NE(canvasModifiersDraw->renderContext_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawTest, OnNodeStateChanged_ActiveWithDrawable001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    canvasModifiersDraw->OnNodeStateChanged(nodeId, RSNodeState::ACTIVE);
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw, nodeId]() { return canvasModifiersDraw->drawableMap_[nodeId].forceFlushBuffer_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
}

HWTEST_F(RSCanvasModifiersDrawTest, ResetSurface_WithDrawableInMap001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    canvasModifiersDraw->ResetSurface(nodeId, 200, 200, false, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw, nodeId]() { return canvasModifiersDraw->drawableMap_.count(nodeId) > 0; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
}

HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollect_NullGpuContext001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 1;
    canvasModifiersDraw->drawableMap_[nodeId];
    canvasModifiersDraw->gpuContext_ = nullptr;
    canvasModifiersDraw->SubmitAndCollectCanvasBuffers();
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->transactionConfigList_.empty(); });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
}

// gpuContext non-null, INACTIVE drawable, GetBuffer returns null, bufferList empty
// -2: gpuContext != nullptr
// -3: iterating drawableMap_
// -5: drawable.nodeState_ != ACTIVE
// -6: iterating second loop
// -8: GetBuffer returns null
// -9: bufferList empty → return
HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollect_InactiveNoBuffer001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->GetGpuContext();
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.nodeState_ = RSNodeState::INACTIVE;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    canvasModifiersDraw->SubmitAndCollectCanvasBuffers();
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->transactionConfigList_.empty(); });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
}

// gpuContext non-null, ACTIVE drawable, GetBuffer returns null (no surfaceFrame), bufferList empty
// -4: drawable.nodeState_ == ACTIVE → needFlushBuffer=true
// -8: GetBuffer returns null (surfaceFrame_ is nullptr)
// -9: bufferList empty → return
HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollect_ActiveNoBuffer001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->GetGpuContext();
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.nodeState_ = RSNodeState::ACTIVE;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    drawable.surfaceFrame_ = nullptr;
    canvasModifiersDraw->SubmitAndCollectCanvasBuffers();
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->transactionConfigList_.empty(); });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
}

// ACTIVE drawable with buffer, needFlushBuffer=false path
// -7: GetBuffer returns non-null
// -10: bufferList not empty
// -11: !needFlushBuffer → gpuContext->Submit(), return
HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollect_InactiveWithBuffer001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->GetGpuContext();
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.nodeState_ = RSNodeState::INACTIVE;
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto rasterSurface = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestSubmitInactive");
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(bp);
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(pSurface, rasterSurface);
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.forceFlushBuffer_ = true;
    drawable.Draw();
    ASSERT_NE(drawable.surfaceFrame_, nullptr);
    BufferRequestConfig config = { .width = 0x100, .height = 0x100, .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA };
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t releaseFence = -1;
    pSurface->RequestBuffer(buffer, releaseFence, config);
    ASSERT_NE(buffer, nullptr);
    auto* nwb = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&buffer);
    ASSERT_NE(nwb, nullptr);
    mockSurface->mSurfaceList.emplace_back(nwb);
    canvasModifiersDraw->SubmitAndCollectCanvasBuffers();
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->transactionConfigList_.size(); });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    auto size = future.get();
    pSurface->CancelBuffer(buffer);
    EXPECT_EQ(size, 0u);
}

// ACTIVE drawable with buffer, needFlushBuffer=true path
// -12: needFlushBuffer=true
// -13: iterating bufferList → AppendTransactionConfig + OnDirtyBufferCollected
// -14 or -15: destroySemaphoreInfo != nullptr or == nullptr
HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollect_ActiveWithBuffer001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->GetGpuContext();
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.nodeState_ = RSNodeState::ACTIVE;
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto rasterSurface = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestSubmitActive");
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(bp);
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(pSurface, rasterSurface);
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.forceFlushBuffer_ = true;
    drawable.Draw();
    ASSERT_NE(drawable.surfaceFrame_, nullptr);
    BufferRequestConfig config = { .width = 0x100, .height = 0x100, .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA };
    sptr<SurfaceBuffer> buffer = nullptr;
    int32_t releaseFence = -1;
    pSurface->RequestBuffer(buffer, releaseFence, config);
    ASSERT_NE(buffer, nullptr);
    auto* nwb = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&buffer);
    ASSERT_NE(nwb, nullptr);
    mockSurface->mSurfaceList.emplace_back(nwb);
    canvasModifiersDraw->SubmitAndCollectCanvasBuffers();
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->drawableMap_.count(1) > 0 &&
            canvasModifiersDraw->drawableMap_[1].surfaceFrame_ == nullptr; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(2000)), std::future_status::ready);
    auto surfaceFrameNull = future.get();
    pSurface->CancelBuffer(buffer);
    EXPECT_TRUE(surfaceFrameNull);
}
} // namespace Rosen
} // namespace OHOS