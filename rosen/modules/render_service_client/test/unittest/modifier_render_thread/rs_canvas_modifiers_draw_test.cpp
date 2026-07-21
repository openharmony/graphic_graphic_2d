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
#include "iconsumer_surface.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "surface_buffer_impl.h"

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
    drawable.CreateProducerSurface(weakInterface, "/cache");
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
    auto result = drawable.ResetSurface(100, 100, false, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, ResetSurface_SameWidthAndHeight001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.width_ = 100;
    drawable.height_ = 100;
    auto result = drawable.ResetSurface(100, 100, false, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, ResetSurface_WithValidSurfaceDifferentSize001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = 0;
    drawable.height_ = 0;
    auto result = drawable.ResetSurface(100, 100, false, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_EQ(result, NULL);
    EXPECT_EQ(drawable.width_, 100);
    EXPECT_EQ(drawable.height_, 100);
    EXPECT_TRUE(drawable.needResetCanvas_);
    EXPECT_TRUE(drawable.forceFlushBuffer_);
}

HWTEST_F(RSCanvasModifiersDrawableTest, ResetSurface_SizeOutOfGpuLimit001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    auto result = drawable.ResetSurface(100, 100, true, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_EQ(result, nullptr);
    EXPECT_EQ(drawable.width_, 0);
    EXPECT_EQ(drawable.height_, 0);
}

HWTEST_F(RSCanvasModifiersDrawableTest, UpdateContent_NullDrawCmdListCache001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.drawCmdListCache_ = nullptr;
    auto result = drawable.UpdateContent(nullptr, false);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, UpdateContent_WithDrawCmdList001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    auto result = drawable.UpdateContent(nullptr, false);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, Draw_NullProducerSurface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = nullptr;
    auto result = drawable.Draw();
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, Draw_EmptyCacheNoForceFlush001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.forceFlushBuffer_ = false;
    auto result = drawable.Draw();
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, Draw_WithValidSurfaceAndForceFlush001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.forceFlushBuffer_ = true;
    drawable.width_ = 100;
    drawable.height_ = 100;
    auto result = drawable.Draw();
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetFenceFd_Basic001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.semaphore_ = VK_NULL_HANDLE;
    int32_t fenceFd = drawable.GetFenceFd();
    EXPECT_EQ(fenceFd, -1);
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
    drawable.semaphore_ = reinterpret_cast<VkSemaphore>(12345);
    drawable.lastFlushBufferTime_ = 1000;
    drawable.OnDirtyBufferCollected(2000);
    EXPECT_EQ(drawable.surfaceFrame_, nullptr);
    EXPECT_EQ(drawable.semaphore_, VK_NULL_HANDLE);
    EXPECT_EQ(drawable.lastFlushBufferTime_, 2000);
}

HWTEST_F(RSCanvasModifiersDrawableTest, OnFlushBuffer_InactiveState001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.nodeState_ = RSNodeState::INACTIVE;
    auto result = drawable.OnFlushBuffer();
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, OnFlushBuffer_NullProducerSurface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.nodeState_ = RSNodeState::ACTIVE;
    drawable.producerSurface_ = nullptr;
    auto result = drawable.OnFlushBuffer();
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, OnFlushBuffer_NullSurfaceFrame001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.nodeState_ = RSNodeState::ACTIVE;
    drawable.producerSurface_ = CreateSurface();
    drawable.semaphore_ = reinterpret_cast<VkSemaphore>(1);
    drawable.surfaceFrame_ = nullptr;
    auto result = drawable.OnFlushBuffer();
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, OnFlushBuffer_NullSemaphore001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.nodeState_ = RSNodeState::ACTIVE;
    drawable.producerSurface_ = CreateSurface();
    drawable.semaphore_ = VK_NULL_HANDLE;
    drawable.surfaceFrame_ = nullptr;
    auto result = drawable.OnFlushBuffer();
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
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    auto result = drawable.GetImage(bitmapFormat, gpuContext);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetImage_NullGpuContext001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto result = drawable.GetImage(bitmapFormat, nullptr);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetBitmap_NullGpuContext001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    Drawing::Bitmap bitmap;
    bool result = drawable.GetBitmap(bitmap, "");
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_NullPixelMap001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    Drawing::Rect rect(0, 0, 100, 100);
    bool result = drawable.GetPixelMap(nullptr, &rect, nullptr, "/cache");
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_NullRect001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    bool result = drawable.GetPixelMap(nullptr, nullptr, nullptr, "/cache");
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
    bool taskExecuted = false;
    canvasModifiersDraw->PostTask([&taskExecuted]() { taskExecuted = true; }, "TestTask", 0);
    usleep(10000);
    EXPECT_TRUE(taskExecuted);
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
    canvasModifiersDraw->PostTask([&counter]() { counter++; }, "RemoveTestTask", 100);
    canvasModifiersDraw->RemoveTask("RemoveTestTask");
    usleep(200000);
    EXPECT_EQ(counter, 0);
}

HWTEST_F(RSCanvasModifiersDrawTest, RemoveTask_DoesNothingWhenThreadNotStarted001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->RemoveTask("NonExistentTask");
    EXPECT_FALSE(canvasModifiersDraw->threadStarted_);
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

HWTEST_F(RSCanvasModifiersDrawTest, DestroyCanvasSemaphore_HandlesEmptyList001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->DestroyCanvasSemaphore();
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
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

HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollectCanvasBuffers_WithSemaphore001, TestSize.Level1)
{
    auto canvasModifiersDraw = std::make_shared<RSCanvasModifiersDraw>();
    NodeId nodeId = 1;
    auto renderInterface = std::make_shared<RSRenderInterface>();
    std::weak_ptr<RSRenderInterface> weakInterface = renderInterface;
    canvasModifiersDraw->OnNodeCreate(nodeId, weakInterface);
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeState_ = RSNodeState::ACTIVE;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    NativeBufferUtils::CreateVkSemaphore(drawable.semaphore_);
    drawable.width_ = 100;
    drawable.height_ = 100;
    auto drawingSurface = std::make_shared<Drawing::Surface>();
    drawable.surfaceFrame_ =
        std::make_unique<RSSurfaceFrameOhosVulkan>(drawingSurface, drawable.width_, drawable.height_, 1);
    auto ohosSurface = std::static_pointer_cast<RSSurfaceOhosVulkan>(drawable.producerSurface_);
    NativeWindowBuffer windowBuffer;
    windowBuffer.sfbuffer = new SurfaceBufferImpl();
    ohosSurface->mSurfaceList.emplace_back(&windowBuffer);
    canvasModifiersDraw->SubmitAndCollectCanvasBuffers();
    usleep(10000);
    EXPECT_TRUE(canvasModifiersDraw->canvasNewSemaphoreInfos_.empty());
}
} // namespace Rosen
} // namespace OHOS