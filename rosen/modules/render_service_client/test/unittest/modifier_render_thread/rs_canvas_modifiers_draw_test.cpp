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
#include "command/rs_delegate_composite_command.h"
#include "recording/draw_cmd.h"
#include "external_window.h"
#include "ibuffer_consumer_listener.h"
#include "iconsumer_surface.h"
#include "image_type.h"
#include "pixel_map.h"
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "render_context/render_context.h"
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
    static std::shared_ptr<RSCanvasModifiersDraw> CreateCanvasModifiersDraw();
    static std::shared_ptr<RSCanvasModifiersContext> CreateCanvasModifiersContext();
};

std::shared_ptr<RSCanvasModifiersDraw> RSCanvasModifiersDrawableTest::CreateCanvasModifiersDraw()
{
    auto screenId = RSInterfaces::GetInstance().GetDefaultScreenId();
    auto connectToRender = RSInterfaces::GetInstance().GetConnectToRenderToken(screenId);
    if (connectToRender == nullptr) {
        return nullptr;
    }
    auto renderInterface = std::make_shared<RSRenderInterface>(connectToRender);
    return std::make_shared<RSCanvasModifiersDraw>(renderInterface);
}

std::shared_ptr<RSCanvasModifiersContext> RSCanvasModifiersDrawableTest::CreateCanvasModifiersContext()
{
    auto screenId = RSInterfaces::GetInstance().GetDefaultScreenId();
    auto connectToRender = RSInterfaces::GetInstance().GetConnectToRenderToken(screenId);
    auto renderInterface = std::make_shared<RSRenderInterface>(connectToRender);
    return std::make_shared<RSCanvasModifiersContext>(renderInterface);
}

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

namespace {
constexpr int GPU_TEST_SIZE = 10;
constexpr int OUT_OF_BOUNDS_COORD = 1000;

// Builds a drawable backed by a real GPU render target so that GetImage/GetBitmap/GetPixelMap
// can reach their success paths. Returns false when the test process has no GPU context.
bool PrepareGpuDrawable(RSCanvasModifiersDrawable& drawable,
    const std::shared_ptr<RSCanvasModifiersContext>& context, int width, int height)
{
    context->cacheDir_ = "/data/local/tmp";
    context->GetGpuContext();
    if (context->gpuContext_ == nullptr) {
        return false;
    }
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(width, height);
    drawable.drawingSurface_ = Drawing::Surface::MakeRenderTarget(context->gpuContext_.get(), false, imageInfo);
    if (drawable.drawingSurface_ == nullptr) {
        return false;
    }
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = width;
    drawable.height_ = height;
    return true;
}

std::shared_ptr<Media::PixelMap> CreateTestPixelMap(int width, int height)
{
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    return std::shared_ptr<Media::PixelMap>(Media::PixelMap::Create(opts));
}

// Stub that makes vkCreateSemaphore report a failure, used to exercise the
// semaphore-creation failure branch without a real Vulkan fault.
VKAPI_ATTR VkResult VKAPI_CALL StubCreateSemaphoreFail(VkDevice, const VkSemaphoreCreateInfo*,
    const VkAllocationCallbacks*, VkSemaphore*)
{
    return VK_ERROR_OUT_OF_HOST_MEMORY;
}
} // namespace

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
    drawable.bufferRequested_ = true;
    drawable.bufferDirty_ = true;
    drawable.drawingSurface_ = Drawing::Surface::MakeRaster(Drawing::ImageInfo::MakeN32Premul(1, 1));
    drawable.nodeId_ = 12345;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.Reset();
    EXPECT_EQ(drawable.width_, 0);
    EXPECT_EQ(drawable.height_, 0);
    EXPECT_FALSE(drawable.bufferRequested_);
    EXPECT_FALSE(drawable.bufferDirty_);
    EXPECT_EQ(drawable.drawingSurface_, nullptr);
    EXPECT_NE(drawable.drawCmdListCache_, nullptr);
    EXPECT_TRUE(drawable.drawCmdListCache_->empty());
}

HWTEST_F(RSCanvasModifiersDrawableTest, CreateProducerSurface_NullRenderInterface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.nodeId_ = 12345;
    auto context = CreateCanvasModifiersContext();
    drawable.CreateProducerSurface(context, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_EQ(drawable.producerSurface_, nullptr);
    EXPECT_EQ(drawable.drawCmdListCache_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, ReleaseProducerSurface_NullRenderInterface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.nodeId_ = 12345;
    std::weak_ptr<RSRenderInterface> weakInterface;
    auto context = std::make_shared<RSCanvasModifiersContext>(weakInterface);
    drawable.ReleaseProducerSurface(context);
    EXPECT_EQ(drawable.producerSurface_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, ResetSurface_NullProducerSurface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = nullptr;
    drawable.ResetSurface(100, 100, false);
    EXPECT_EQ(drawable.width_, 0);
    EXPECT_EQ(drawable.height_, 0);
}

HWTEST_F(RSCanvasModifiersDrawableTest, ResetSurface_SameWidthAndHeight001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.ResetSurface(100, 100, false);
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
    drawable.ResetSurface(100, 100, false);
    EXPECT_EQ(drawable.width_, 100);
    EXPECT_EQ(drawable.height_, 100);
    // hasDrawn_ is false → the if (hasDrawn_) block is not entered
    EXPECT_FALSE(drawable.bufferRequested_);
    EXPECT_FALSE(drawable.bufferDirty_);
    // Set hasDrawn_ = true so the next ResetSurface enters the if block,
    // which calls RequestBufferAndDrawHistory(width, height, true) to obtain a
    // placeholder buffer (needResetCanvas=true is now passed as a parameter).
    drawable.hasDrawn_ = true;
    drawable.ResetSurface(50, 50, false);
    EXPECT_EQ(drawable.width_, 50);
    EXPECT_EQ(drawable.height_, 50);
    EXPECT_TRUE(drawable.hasDrawn_);
}

HWTEST_F(RSCanvasModifiersDrawableTest, ResetSurface_SizeOutOfGpuLimit001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawingSurface_ = Drawing::Surface::MakeRaster(Drawing::ImageInfo::MakeN32Premul(1, 1));
    drawable.hasDrawn_ = true;
    drawable.ResetSurface(100, 100, true);
    EXPECT_EQ(drawable.width_, 0);
    EXPECT_EQ(drawable.height_, 0);
    // sizeOutOfGpuLimit path: Reset() clears drawingSurface_, no placeholder buffer requested
    EXPECT_EQ(drawable.drawingSurface_, nullptr);
    EXPECT_FALSE(drawable.bufferRequested_);
    EXPECT_FALSE(drawable.bufferDirty_);
    // hasDrawn_ is preserved so next ResetSurface with valid size can request buffer promptly
    EXPECT_TRUE(drawable.hasDrawn_);
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
    drawable.Draw(false);
    EXPECT_EQ(drawable.producerSurface_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, Draw_EmptyCacheNoForceFlush001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.Draw(false);
    // Empty cache + forceFlushBuffer=false → early return, buffer not dirty
    EXPECT_FALSE(drawable.bufferDirty_);
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
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.Draw(true);
    // forceFlushBuffer=true bypasses the empty cache check; after successful draw,
    // bufferDirty_ is set to true.
    EXPECT_TRUE(drawable.bufferDirty_);
}

HWTEST_F(RSCanvasModifiersDrawableTest, IsFree_LastFlushBufferTimeZero001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.lastBufferExpiredTime_ = 0;
    bool result = drawable.IsFree(1000, 500);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, IsFree_DurationWithinLimit001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.lastBufferExpiredTime_ = 1000;
    drawable.producerSurface_ = CreateSurface();
    bool result = drawable.IsFree(1200, 500);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, IsFree_NullProducerSurface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.lastBufferExpiredTime_ = 1000;
    drawable.producerSurface_ = nullptr;
    bool result = drawable.IsFree(2000, 500);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, IsFree_FreeDrawable001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.lastBufferExpiredTime_ = 1000;
    drawable.producerSurface_ = CreateSurface();
    bool result = drawable.IsFree(2000, 500);
    EXPECT_TRUE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, OnDirtyBufferCollected_ResetsFields001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.lastBufferExpiredTime_ = 1000;
    drawable.OnDirtyBufferCollected(2000);
    EXPECT_FALSE(drawable.bufferRequested_);
    EXPECT_FALSE(drawable.bufferDirty_);
    EXPECT_EQ(drawable.lastBufferExpiredTime_, 2000);
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

HWTEST_F(RSCanvasModifiersDrawableTest, GetBuffer_BufferNotDirty001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.nodeState_ = RSNodeState::ACTIVE;
    drawable.producerSurface_ = CreateSurface();
    // bufferDirty_ defaults to false → GetBuffer returns nullptr
    auto result = drawable.GetBuffer(true);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, CleanBuffer_NullProducerSurface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = nullptr;
    drawable.lastBufferExpiredTime_ = 1000;
    drawable.CleanBuffer();
    EXPECT_NE(drawable.lastBufferExpiredTime_, 0);
}

HWTEST_F(RSCanvasModifiersDrawableTest, CleanBuffer_WithValidSurface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.lastBufferExpiredTime_ = 1000;
    drawable.CleanBuffer();
    EXPECT_EQ(drawable.lastBufferExpiredTime_, 0);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetImage_NullDrawingSurface001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.drawingSurface_ = nullptr;
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto context = CreateCanvasModifiersContext();
    auto result = drawable.GetImage(bitmapFormat, context);
    EXPECT_EQ(result, nullptr);
    EXPECT_EQ(drawable.drawingSurface_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetImage_NullGpuContext001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.drawingSurface_ = Drawing::Surface::MakeRaster(Drawing::ImageInfo::MakeN32Premul(100, 100));
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    auto context = CreateCanvasModifiersContext();
    auto result = drawable.GetImage(bitmapFormat, context);
    EXPECT_EQ(result, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetBitmap_NullGpuContext001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    Drawing::Bitmap bitmap;
    auto context = CreateCanvasModifiersContext();
    bool result = drawable.GetBitmap(bitmap, context);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_NullPixelMap001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    Drawing::Rect rect(0, 0, 100, 100);
    auto context = CreateCanvasModifiersContext();
    bool result = drawable.GetPixelMap(nullptr, &rect, nullptr, context);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_NullRect001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    auto context = CreateCanvasModifiersContext();
    bool result = drawable.GetPixelMap(nullptr, nullptr, nullptr, context);
    EXPECT_FALSE(result);
}

// RequestBufferAndDrawHistory returns false when RequestFrame fails (no GPU context).
// drawingSurface_ is not corrupted.
HWTEST_F(RSCanvasModifiersDrawableTest, RequestBufferAndDrawHistory_RequestFrameFails001, TestSize.Level1)
{
    auto prevDrawingSurface = std::make_shared<Drawing::Surface>();
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawingSurface_ = prevDrawingSurface;
    drawable.width_ = 100;
    drawable.height_ = 100;
    auto result = drawable.RequestBufferAndDrawHistory(100, 100, false);
    EXPECT_FALSE(result);
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
    auto result1 = drawable.RequestBufferAndDrawHistory(100, 100, false);
    ASSERT_TRUE(result1);
    EXPECT_EQ(drawable.drawingSurface_, surfaceA);

    // Branch 2: drawingSurface_ != nullptr && drawingSurface_ != drawingSurface
    // Use a different mock that returns a different Surface
    auto surfaceB = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("TestInherit2");
    sptr<IBufferProducer> bp2 = cSurface2->GetProducer();
    auto mockSurface2 = std::make_shared<TestRSSurfaceOhosVulkan>(
        Surface::CreateSurfaceAsProducer(bp2), surfaceB);
    drawable.producerSurface_ = mockSurface2;
    auto result2 = drawable.RequestBufferAndDrawHistory(100, 100, false);
    ASSERT_TRUE(result2);
    EXPECT_EQ(drawable.drawingSurface_, surfaceB);
    EXPECT_NE(drawable.drawingSurface_, surfaceA);

    // Branch 3: drawingSurface_ != nullptr && drawingSurface_ == drawingSurface
    // Reuse mockSurface1 which returns the same surfaceA as current drawingSurface_
    // This simulates mSurfaceMap cache hit where RS released the buffer after compositing
    // and RequestFrame dequeues the same buffer again.
    drawable.producerSurface_ = mockSurface1;
    auto result3 = drawable.RequestBufferAndDrawHistory(100, 100, false);
    ASSERT_TRUE(result3);
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
    auto context = std::make_shared<RSCanvasModifiersContext>(renderInterface);
    context->cacheDir_ = "/cache";
    context->GetGpuContext();
    RSCanvasModifiersDrawable drawable;
    drawable.nodeId_ = RSNode::GenerateId();
    drawable.CreateProducerSurface(context, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    if (RSSystemProperties::GetHybridRenderCanvasEnabled() && context->gpuContext_ != nullptr) {
        EXPECT_NE(drawable.producerSurface_, nullptr);
        EXPECT_NE(drawable.drawCmdListCache_, nullptr);
    } else {
        EXPECT_EQ(drawable.producerSurface_, nullptr);
        EXPECT_EQ(drawable.drawCmdListCache_, nullptr);
        EXPECT_EQ(context->maxGpuResourceBytes_, 0u);
    }
}

/**
 * @tc.name: CleanGpuCache_NullGpuContext001
 * @tc.desc: Test CleanGpuCache when gpuContext_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasModifiersDrawableTest, CleanGpuCache_NullGpuContext001, TestSize.Level1)
{
    auto context = CreateCanvasModifiersContext();
    ASSERT_NE(context, nullptr);
    context->gpuContext_ = nullptr;
    context->CleanGpuCache();
    EXPECT_FALSE(context->needRestoreGpuCacheLimit_);
}

/**
 * @tc.name: GetGpuCacheLimits_NullGpuContext001
 * @tc.desc: Test GetGpuCacheLimits when gpuContext_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasModifiersDrawableTest, GetGpuCacheLimits_NullGpuContext001, TestSize.Level1)
{
    auto context = CreateCanvasModifiersContext();
    ASSERT_NE(context, nullptr);
    context->gpuContext_ = nullptr;
    context->GetGpuCacheLimits();
    EXPECT_EQ(context->maxGpuResourceBytes_, 0u);
}

/**
 * @tc.name: GetGpuCacheLimits_WithGpuContext001
 * @tc.desc: Test GetGpuCacheLimits when gpuContext_ is non-null
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasModifiersDrawableTest, GetGpuCacheLimits_WithGpuContext001, TestSize.Level1)
{
    auto context = CreateCanvasModifiersContext();
    ASSERT_NE(context, nullptr);
    context->cacheDir_ = "/data/local/tmp";
    context->GetGpuContext();
    if (context->gpuContext_ != nullptr) {
        context->GetGpuCacheLimits();
        EXPECT_GE(context->maxGpuResourceBytes_, 0u);
    } else {
        SUCCEED();
    }
}

/**
 * @tc.name: GetPixelMap_WithValidPixelMapAndRect001
 * @tc.desc: Test GetPixelMap with valid pixelMap and rect
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_WithValidPixelMapAndRect001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.nodeId_ = 12345;
    auto context = CreateCanvasModifiersContext();
    Media::InitializationOptions opts;
    opts.size.width = 10;
    opts.size.height = 10;
    std::shared_ptr<Media::PixelMap> pixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(pixelMap, nullptr);
    Drawing::Rect rect(0, 0, 10, 10);
    bool result = drawable.GetPixelMap(pixelMap, &rect, nullptr, context);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: GetPixelMap_WithDrawCmdList001
 * @tc.desc: Test GetPixelMap with non-empty drawCmdList
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_WithDrawCmdList001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.nodeId_ = 12345;
    drawable.width_ = 10;
    drawable.height_ = 10;
    auto context = CreateCanvasModifiersContext();
    Media::InitializationOptions opts;
    opts.size.width = 10;
    opts.size.height = 10;
    std::shared_ptr<Media::PixelMap> pixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(pixelMap, nullptr);
    Drawing::Rect rect(0, 0, 10, 10);
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    bool result = drawable.GetPixelMap(pixelMap, &rect, drawCmdList, context);
    EXPECT_FALSE(result);
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
    static std::shared_ptr<RSCanvasModifiersDraw> CreateCanvasModifiersDraw();
};

std::shared_ptr<RSCanvasModifiersDraw> RSCanvasModifiersDrawTest::CreateCanvasModifiersDraw()
{
    auto screenId = RSInterfaces::GetInstance().GetDefaultScreenId();
    auto connectToRender = RSInterfaces::GetInstance().GetConnectToRenderToken(screenId);
    if (connectToRender == nullptr) {
        return nullptr;
    }
    auto renderInterface = std::make_shared<RSRenderInterface>(connectToRender);
    return std::make_shared<RSCanvasModifiersDraw>(renderInterface);
}

void RSCanvasModifiersDrawTest::SetUpTestCase() {}
void RSCanvasModifiersDrawTest::TearDownTestCase() {}
void RSCanvasModifiersDrawTest::SetUp() {}
void RSCanvasModifiersDrawTest::TearDown() {}

HWTEST_F(RSCanvasModifiersDrawTest, StartThread_Idempotent001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->StartThread();
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, PostTask_StartsThreadIfNotStarted001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    std::promise<void> promise;
    auto future = promise.get_future();
    canvasModifiersDraw->PostTask([&promise]() { promise.set_value(); }, "TestTask", 0);
    auto result = future.wait_for(std::chrono::milliseconds(1000));
    EXPECT_EQ(result, std::future_status::ready);
}

HWTEST_F(RSCanvasModifiersDrawTest, PostSyncTask_StartsThreadIfNotStarted001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    bool taskExecuted = false;
    canvasModifiersDraw->PostSyncTask([&taskExecuted]() { taskExecuted = true; });
    EXPECT_TRUE(taskExecuted);
}

HWTEST_F(RSCanvasModifiersDrawTest, RemoveTask_RemovesPendingTask001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    int counter = 0;
    canvasModifiersDraw->PostTask([&counter]() { counter++; }, "RemoveTestTask", 5000);
    canvasModifiersDraw->RemoveTask("RemoveTestTask");
    usleep(100000);
    EXPECT_EQ(counter, 0);
}

HWTEST_F(RSCanvasModifiersDrawTest, RemoveTask_DoesNothingWhenThreadNotStarted001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->RemoveTask("NonExistentTask");
    EXPECT_FALSE(canvasModifiersDraw->threadStarted_.load());
}

HWTEST_F(RSCanvasModifiersDrawTest, PostTask_ReturnsEarlyAfterDestroy001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->Destroy();
    bool taskExecuted = false;
    canvasModifiersDraw->PostTask([&taskExecuted]() { taskExecuted = true; }, "TestTask", 0);
    EXPECT_FALSE(taskExecuted);
    EXPECT_TRUE(canvasModifiersDraw->threadDestroyed_.load());
}

HWTEST_F(RSCanvasModifiersDrawTest, PostSyncTask_ReturnsEarlyAfterDestroy001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->Destroy();
    bool taskExecuted = false;
    canvasModifiersDraw->PostSyncTask([&taskExecuted]() { taskExecuted = true; });
    EXPECT_FALSE(taskExecuted);
    EXPECT_TRUE(canvasModifiersDraw->threadDestroyed_.load());
}

HWTEST_F(RSCanvasModifiersDrawTest, StartThread_ReturnsEarlyAfterDestroy001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->Destroy();
    canvasModifiersDraw->StartThread();
    EXPECT_FALSE(canvasModifiersDraw->threadStarted_.load());
    EXPECT_TRUE(canvasModifiersDraw->threadDestroyed_.load());
}

HWTEST_F(RSCanvasModifiersDrawTest, ReleaseResource_AfterDestroy001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->Destroy();
    canvasModifiersDraw->ReleaseResource();
    EXPECT_TRUE(canvasModifiersDraw->threadDestroyed_.load());
}

// ReleaseResource with gpuContext_ set → covers FlushAndSubmit, PurgeUnlockedResources, gpuContext_=nullptr
HWTEST_F(RSCanvasModifiersDrawTest, ReleaseResource_WithGpuContext001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->canvasModifiersContext_->gpuContext_ =
        RenderContext::Create()->CreateDrawingGPUContext("/data/local/tmp");
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->ReleaseResource();
    // gpuContext_ should be reset to nullptr after ReleaseResource
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->canvasModifiersContext_->gpuContext_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_EQ(future.get(), nullptr);
    canvasModifiersDraw->Destroy();
}

// ReleaseResource with gpuContext_ set → covers FlushAndSubmit, PurgeUnlockedResources, gpuContext_=nullptr
HWTEST_F(RSCanvasModifiersDrawTest, ReleaseResource_WithGpuContext002, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
    auto vkInterface = RsVulkanContext::Get(canvasModifiersDraw->canvasModifiersContext_->renderContext_->GetType())
                           .GetRsVulkanInterface();
    VkSemaphore semaphore = VK_NULL_HANDLE;
    canvasModifiersDraw->destroySemaphoreInfo_ =
        new DestroySemaphoreInfo(vkInterface->vkDestroySemaphore, vkInterface->GetDevice(), semaphore);
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->ReleaseResource();
    // gpuContext_ should be reset to nullptr after ReleaseResource
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->canvasModifiersContext_->gpuContext_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_EQ(future.get(), nullptr);
    canvasModifiersDraw->Destroy();
}

HWTEST_F(RSCanvasModifiersDrawTest, Destroy_BeforeStartThread001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->Destroy();
    EXPECT_TRUE(canvasModifiersDraw->threadDestroyed_.load());
    EXPECT_FALSE(canvasModifiersDraw->threadStarted_.load());
}

// Verify Destroy() sets runner_ and handler_ to nullptr after Stop/RemoveAllEvents,
// and the null-safety guards work when runner_/handler_ are already nullptr.
HWTEST_F(RSCanvasModifiersDrawTest, Destroy_NullptrAfterStopAndNullGuards001, TestSize.Level1)
{
    // Case 1: Start then Destroy — runner_/handler_ become nullptr after Stop+RemoveAllEvents
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
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
    auto canvasModifiersDraw2 = CreateCanvasModifiersDraw();
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
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->SetCacheDir("/test/cache");
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, OnNodeCreate_Basic001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 12345;
    std::weak_ptr<RSRenderInterface> weakInterface;
    canvasModifiersDraw->OnNodeCreate(nodeId, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, OnNodeRelease_Basic001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 12345;
    std::weak_ptr<RSRenderInterface> weakInterface;
    canvasModifiersDraw->OnNodeRelease(nodeId);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, OnNodeStateChanged_InactiveState001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 67890;
    canvasModifiersDraw->OnNodeStateChanged(nodeId, RSNodeState::INACTIVE);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, OnNodeStateChanged_ActiveState001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 67890;
    canvasModifiersDraw->OnNodeStateChanged(nodeId, RSNodeState::ACTIVE);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, ResetSurface_TriggersBufferCleanup001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 12345;
    canvasModifiersDraw->ResetSurface(nodeId, 100, 100, true);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, GetBitmap_Basic001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
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
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 12345;
    std::shared_ptr<Media::PixelMap> pixelMap;
    Drawing::Rect rect(0, 0, 100, 100);
    bool result = canvasModifiersDraw->GetPixelMap(nodeId, pixelMap, &rect, nullptr);
    EXPECT_FALSE(result);
    canvasModifiersDraw->Destroy();
}

/**
 * @tc.name: GetPixelMap_WithGpuSurfaceAndDrawCmdList001
 * @tc.desc: Test GetPixelMap with GPU surface and non-empty drawCmdList
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasModifiersDrawTest, GetPixelMap_WithGpuSurfaceAndDrawCmdList001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    if (canvasModifiersDraw == nullptr) {
        return;
    }
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->canvasModifiersContext_->cacheDir_ = "/data/local/tmp";
    NodeId nodeId = 67890;
    bool gpuReady = false;
    canvasModifiersDraw->PostSyncTask(
        [canvasModifiersDraw, nodeId, &gpuReady]() {
            canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
            if (canvasModifiersDraw->canvasModifiersContext_->gpuContext_ == nullptr) {
                return;
            }
            gpuReady = true;
            auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
            drawable.nodeId_ = nodeId;
            drawable.width_ = 10;
            drawable.height_ = 10;
            drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
            auto gpuContext = canvasModifiersDraw->canvasModifiersContext_->gpuContext_;
            auto imageInfo = Drawing::ImageInfo::MakeN32Premul(10, 10);
            drawable.drawingSurface_ = Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, imageInfo);
        });
    if (!gpuReady) {
        canvasModifiersDraw->Destroy();
        return;
    }
    Media::InitializationOptions opts;
    opts.size.width = 10;
    opts.size.height = 10;
    std::shared_ptr<Media::PixelMap> pixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(pixelMap, nullptr);
    Drawing::Rect rect(0, 0, 10, 10);
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    drawCmdList->AddOp<Drawing::DrawColorOpItem>(0xFF000000, Drawing::BlendMode::SRC);
    bool result = canvasModifiersDraw->GetPixelMap(nodeId, pixelMap, &rect, drawCmdList);
    EXPECT_TRUE(result);
    canvasModifiersDraw->OnNodeRelease(nodeId);
    // Release GPU resources before stopping the thread to avoid SIGSEGV
    // in Vulkan destructors during process exit.
    if (canvasModifiersDraw->RemoveCleanFreeBuffersTask()) {
        canvasModifiersDraw->PostSyncTask(
            [canvasModifiersDraw] { canvasModifiersDraw->ReleaseResource(); });
    }
    canvasModifiersDraw->Destroy();
}

HWTEST_F(RSCanvasModifiersDrawTest, UpdateCanvasContent_Basic001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 12345;
    canvasModifiersDraw->UpdateCanvasContent(nodeId, nullptr);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, UpdateCanvasContent_WeakPtrExpired001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
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

HWTEST_F(RSCanvasModifiersDrawTest, SyncAndWait_EmptyList001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    auto command = canvasModifiersDraw->SyncAndWait();
    EXPECT_EQ(command, nullptr);
}

// SyncAndWait with non-empty transactionConfigList_ → returns non-null TransactionBufferCommand
HWTEST_F(RSCanvasModifiersDrawTest, SyncAndWait_NonEmptyList001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    RSTransactionConfig config;
    config.nodeId = 12345;
    config.hasBuffer = false;
    canvasModifiersDraw->transactionConfigList_.push_back(config);
    auto command = canvasModifiersDraw->SyncAndWait();
    EXPECT_NE(command, nullptr);
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->transactionConfigList_.empty(); });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
}

// RemoveCleanFreeBuffersTask returns false when thread not started
HWTEST_F(RSCanvasModifiersDrawTest, RemoveCleanFreeBuffersTask_ThreadNotStarted001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    EXPECT_FALSE(canvasModifiersDraw->threadStarted_.load());
    bool result = canvasModifiersDraw->RemoveCleanFreeBuffersTask();
    EXPECT_FALSE(result);
}

// RemoveCleanFreeBuffersTask returns true when thread started
HWTEST_F(RSCanvasModifiersDrawTest, RemoveCleanFreeBuffersTask_ThreadStarted001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    bool result = canvasModifiersDraw->RemoveCleanFreeBuffersTask();
    EXPECT_TRUE(result);
}

HWTEST_F(RSCanvasModifiersDrawTest, CleanFreeBuffers_SchedulesRecurringTask001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->CleanFreeBuffers(false);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, CleanFreeBuffers_RemovesTaskWhenImmediately001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->CleanFreeBuffers(true);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, DoCleanFreeBuffers_EmptyDrawableMap001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->DoCleanFreeBuffers(1000);
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollectCanvasBuffers_EmptyMap001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->SubmitAndCollectCanvasBuffers();
    EXPECT_TRUE(canvasModifiersDraw->threadStarted_);
}

// Verify initial state of GPU cache limit members
HWTEST_F(RSCanvasModifiersDrawTest, GpuCacheLimit_InitialState001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    EXPECT_EQ(canvasModifiersDraw->canvasModifiersContext_->maxGpuResourceBytes_, 0u);
    EXPECT_FALSE(canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_);
    EXPECT_TRUE(canvasModifiersDraw->drawableMap_.empty());
    EXPECT_TRUE(canvasModifiersDraw->transactionConfigList_.empty());
}

// DoCleanFreeBuffers: sets needRestoreGpuCacheLimit_ when free drawables exist and gpuContext available;
// does not set flag when no free drawables (ACTIVE state, not idle long enough).
HWTEST_F(RSCanvasModifiersDrawTest, DoCleanFreeBuffers_RestoreFlag001, TestSize.Level1)
{
    // Case 1: free drawable (INACTIVE, idle long enough) → flag depends on gpuContext_
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeState_ = RSNodeState::INACTIVE;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    drawable.lastBufferExpiredTime_ = 1; // non-zero, long ago relative to maxDuration=0
    canvasModifiersDraw->canvasModifiersContext_->gpuContext_ = RenderContext::Create()->CreateDrawingGPUContext(
        canvasModifiersDraw->canvasModifiersContext_->cacheDir_);
    canvasModifiersDraw->lastGpuContextActiveTime_ = 1; // non-zero, long ago
    canvasModifiersDraw->DoCleanFreeBuffers(0);
    if (canvasModifiersDraw->canvasModifiersContext_->gpuContext_ != nullptr) {
        EXPECT_TRUE(canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_);
    } else {
        EXPECT_FALSE(canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_);
    }

    // Case 2: no free drawable (ACTIVE state) → flag stays false
    canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_ = false;
    drawable.nodeState_ = RSNodeState::ACTIVE;
    canvasModifiersDraw->DoCleanFreeBuffers(0);
    EXPECT_FALSE(canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_);
}

// UpdateCanvasContent: needRestoreGpuCacheLimit_ is reset when flag is true and gpuContext available;
// stays false when flag was already false. Uses ScheduleTask to wait for async completion.
HWTEST_F(RSCanvasModifiersDrawTest, UpdateCanvasContent_GpuCacheLimitRestore001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();

    // Case 1: flag=true → restored when gpuContext_ available
    canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_ = true;
    canvasModifiersDraw->canvasModifiersContext_->maxGpuResourceBytes_ = 1024;
    canvasModifiersDraw->canvasModifiersContext_->gpuContext_ = RenderContext::Create()->CreateDrawingGPUContext(
        canvasModifiersDraw->canvasModifiersContext_->cacheDir_);
    canvasModifiersDraw->UpdateCanvasContent(nodeId, nullptr);
    auto future1 = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_; });
    ASSERT_EQ(future1.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    if (canvasModifiersDraw->canvasModifiersContext_->gpuContext_ != nullptr) {
        EXPECT_FALSE(future1.get());
    } else {
        EXPECT_TRUE(future1.get());
    }

    // Case 2: flag=false → stays false
    canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_ = false;
    canvasModifiersDraw->UpdateCanvasContent(nodeId, nullptr);
    auto future2 = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_; });
    ASSERT_EQ(future2.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_FALSE(future2.get());
}

// GetGpuContext: lazy initialization when gpuContext_ is nullptr
HWTEST_F(RSCanvasModifiersDrawTest, GetGpuContext_LazyInit001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->canvasModifiersContext_->cacheDir_ = "/data/local/tmp";
    // gpuContext_ starts as nullptr, GetGpuContext should initialize it
    EXPECT_EQ(canvasModifiersDraw->canvasModifiersContext_->gpuContext_, nullptr);
    auto gpuContext = canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
    auto expectedContext = RenderContext::Create()->CreateDrawingGPUContext("/data/local/tmp");
    if (expectedContext != nullptr) {
        EXPECT_NE(gpuContext, nullptr);
        EXPECT_EQ(canvasModifiersDraw->canvasModifiersContext_->gpuContext_, gpuContext);
    } else {
        EXPECT_EQ(gpuContext, nullptr);
    }
}

// GetGpuContext: returns existing gpuContext_ when already set
HWTEST_F(RSCanvasModifiersDrawTest, GetGpuContext_ReuseExisting001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    auto mockContext = std::make_shared<Drawing::GPUContext>();
    canvasModifiersDraw->canvasModifiersContext_->gpuContext_ = mockContext;
    canvasModifiersDraw->canvasModifiersContext_->renderContext_ = RenderContext::Create();
    auto result = canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
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
    auto context = CreateCanvasModifiersContext();
    bool result = drawable.GetBitmap(bitmap, context);
    EXPECT_FALSE(result);
}

HWTEST_F(RSCanvasModifiersDrawableTest, Draw_WithInValidContext, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.Draw(true);
    // Draw with invalid context (real RSSurfaceOhosVulkan without GPU): RequestFrame fails
    // → RequestBufferAndDrawHistory returns false → Draw returns early.
    EXPECT_FALSE(drawable.bufferRequested_);
    EXPECT_FALSE(drawable.bufferDirty_);
}

HWTEST_F(RSCanvasModifiersDrawableTest, GetBuffer_BufferDirtyNoCurrentBuffer001, TestSize.Level1)
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
    drawable.Draw(true);
    ASSERT_TRUE(drawable.bufferDirty_);
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
    drawable.Draw(true);
    ASSERT_TRUE(drawable.bufferDirty_);
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
    drawable.Draw(true);
    ASSERT_TRUE(drawable.bufferDirty_);
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
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->canvasModifiersContext_->renderContext_ = nullptr;
    VkSemaphore semaphore = VK_NULL_HANDLE;
    EXPECT_EQ(canvasModifiersDraw->FlushSurfaceWithSemaphore(semaphore, nullptr), nullptr);
    EXPECT_EQ(canvasModifiersDraw->GetFenceFd(semaphore), INVALID_FD);

    // Case 2: renderContext_ != nullptr
    canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
    ASSERT_NE(canvasModifiersDraw->canvasModifiersContext_->renderContext_, nullptr);
    semaphore = VK_NULL_HANDLE;
    auto vkInterface = RsVulkanContext::Get(canvasModifiersDraw->canvasModifiersContext_->renderContext_->GetType())
                           .GetRsVulkanInterface();
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
    EXPECT_NE(canvasModifiersDraw->canvasModifiersContext_->renderContext_, nullptr);
}

HWTEST_F(RSCanvasModifiersDrawTest, OnNodeStateChanged_ActiveWithDrawable001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto rasterSurface = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestOnNodeStateChanged");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(
        Surface::CreateSurfaceAsProducer(bp), rasterSurface);
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = 100;
    drawable.height_ = 100;
    canvasModifiersDraw->OnNodeStateChanged(nodeId, RSNodeState::ACTIVE);
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw, nodeId]() { return canvasModifiersDraw->drawableMap_[nodeId].bufferDirty_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
}

HWTEST_F(RSCanvasModifiersDrawTest, ResetSurface_WithDrawableInMap001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    canvasModifiersDraw->ResetSurface(nodeId, 200, 200, false);
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw, nodeId]() { return canvasModifiersDraw->drawableMap_.count(nodeId) > 0; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
}

HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollect_NullGpuContext001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 1;
    canvasModifiersDraw->drawableMap_[nodeId];
    canvasModifiersDraw->canvasModifiersContext_->gpuContext_ = nullptr;
    canvasModifiersDraw->SubmitAndCollectCanvasBuffers();
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->transactionConfigList_.empty(); });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
}

// gpuContext non-null, INACTIVE drawable, GetBuffer returns null, bufferList empty
HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollect_InactiveNoBuffer001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
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

// gpuContext non-null, ACTIVE drawable, GetBuffer returns null (bufferDirty_ is false), bufferList empty
HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollect_ActiveNoBuffer001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.nodeState_ = RSNodeState::ACTIVE;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    // bufferDirty_ defaults to false → GetBuffer returns nullptr
    canvasModifiersDraw->SubmitAndCollectCanvasBuffers();
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->transactionConfigList_.empty(); });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
}

// ACTIVE drawable with buffer, needFlushBuffer=false path
HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollect_InactiveWithBuffer001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
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
    drawable.Draw(true);
    ASSERT_TRUE(drawable.bufferDirty_);
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
HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollect_ActiveWithBuffer001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
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
    drawable.Draw(true);
    ASSERT_TRUE(drawable.bufferDirty_);
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
            !canvasModifiersDraw->drawableMap_[1].bufferRequested_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(2000)), std::future_status::ready);
    auto bufferCleaned = future.get();
    pSurface->CancelBuffer(buffer);
    EXPECT_TRUE(bufferCleaned);
}

HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollect_ActiveWithBuffer002, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
    auto vkInterface = RsVulkanContext::Get(canvasModifiersDraw->canvasModifiersContext_->renderContext_->GetType())
                           .GetRsVulkanInterface();
    VkSemaphore semaphore = VK_NULL_HANDLE;
    canvasModifiersDraw->destroySemaphoreInfo_ =
        new DestroySemaphoreInfo(vkInterface->vkDestroySemaphore, vkInterface->GetDevice(), semaphore);
    canvasModifiersDraw->StartThread();
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
    drawable.Draw(true);
    ASSERT_TRUE(drawable.bufferDirty_);
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
            !canvasModifiersDraw->drawableMap_[1].bufferRequested_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(2000)), std::future_status::ready);
    auto bufferCleaned = future.get();
    pSurface->CancelBuffer(buffer);
    EXPECT_TRUE(bufferCleaned);
}

HWTEST_F(RSCanvasModifiersDrawTest, OnNodeCreate_SetsLastGpuContextActiveTime001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->canvasModifiersContext_->cacheDir_ = "/data/local/tmp";
    NodeId nodeId = 100;
    std::weak_ptr<RSRenderInterface> weakInterface;
    canvasModifiersDraw->OnNodeCreate(nodeId, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->lastGpuContextActiveTime_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(100)), std::future_status::ready);
    EXPECT_GT(future.get(), 0);
}

HWTEST_F(RSCanvasModifiersDrawTest, OnNodeRelease_SetsLastGpuContextActiveTime001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 200;
    canvasModifiersDraw->drawableMap_[nodeId];
    std::weak_ptr<RSRenderInterface> weakInterface;
    canvasModifiersDraw->OnNodeRelease(nodeId);
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw, nodeId]() {
            return std::make_pair(canvasModifiersDraw->lastGpuContextActiveTime_,
                canvasModifiersDraw->drawableMap_.count(nodeId) == 0);
        });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(100)), std::future_status::ready);
    auto [activeTime, erased] = future.get();
    EXPECT_GT(activeTime, 0);
    EXPECT_TRUE(erased);
}

HWTEST_F(RSCanvasModifiersDrawTest, OnNodeStateChanged_NonActiveTriggersClean001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 300;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    drawable.lastBufferExpiredTime_ = 1;
    drawable.nodeState_ = RSNodeState::ACTIVE;
    canvasModifiersDraw->OnNodeStateChanged(nodeId, RSNodeState::INACTIVE);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw, nodeId]() { return canvasModifiersDraw->drawableMap_[nodeId].lastBufferExpiredTime_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(200)), std::future_status::ready);
    EXPECT_EQ(future.get(), 0);
}

// ResetSurface now calls CleanFreeBuffers(true) unconditionally (not only when sizeOutOfGpuLimit),
// so a free drawable's lastBufferExpiredTime_ is reset to 0 regardless of the sizeOutOfGpuLimit flag.
HWTEST_F(RSCanvasModifiersDrawTest, ResetSurface_TriggersCleanUnconditionally001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 400;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    drawable.lastBufferExpiredTime_ = 1;
    drawable.width_ = 50;
    drawable.height_ = 50;
    // sizeOutOfGpuLimit=true still triggers CleanFreeBuffers(true)
    canvasModifiersDraw->ResetSurface(nodeId, 100, 100, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw, nodeId]() { return canvasModifiersDraw->drawableMap_[nodeId].lastBufferExpiredTime_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(200)), std::future_status::ready);
    EXPECT_EQ(future.get(), 0);
}

HWTEST_F(RSCanvasModifiersDrawTest, CleanFreeBuffers_RecurringTaskContinues001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 600;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    drawable.lastBufferExpiredTime_ = 1;
    canvasModifiersDraw->CleanFreeBuffers(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(2100));
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw, nodeId]() { return canvasModifiersDraw->drawableMap_[nodeId].lastBufferExpiredTime_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(100)), std::future_status::ready);
    EXPECT_EQ(future.get(), 0);
}

HWTEST_F(RSCanvasModifiersDrawTest, DoCleanFreeBuffers_ZeroActiveTimeSkipClean001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 500;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeState_ = RSNodeState::INACTIVE;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    drawable.lastBufferExpiredTime_ = 1;
    canvasModifiersDraw->canvasModifiersContext_->gpuContext_ =
        RenderContext::Create()->CreateDrawingGPUContext(canvasModifiersDraw->canvasModifiersContext_->cacheDir_);
    ASSERT_EQ(canvasModifiersDraw->lastGpuContextActiveTime_, 0);
    canvasModifiersDraw->DoCleanFreeBuffers(0);
    EXPECT_FALSE(canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_);
    EXPECT_EQ(canvasModifiersDraw->lastGpuContextActiveTime_, 0);
    canvasModifiersDraw->lastGpuContextActiveTime_ = static_cast<int64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    canvasModifiersDraw->DoCleanFreeBuffers(0);
    EXPECT_FALSE(canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_);
}

HWTEST_F(RSCanvasModifiersDrawTest, RestoreGpuCacheLimitsTest, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    EXPECT_FALSE(canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_);
    // needRestoreGpuCacheLimit_ is false → early return, flag stays false
    canvasModifiersDraw->canvasModifiersContext_->RestoreGpuCacheLimits();
    EXPECT_FALSE(canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_);
    // needRestoreGpuCacheLimit_ = true but gpuContext_ is null → flag stays true
    canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_ = true;
    canvasModifiersDraw->canvasModifiersContext_->gpuContext_ = nullptr;
    canvasModifiersDraw->canvasModifiersContext_->RestoreGpuCacheLimits();
    EXPECT_TRUE(canvasModifiersDraw->canvasModifiersContext_->needRestoreGpuCacheLimit_);
}

/**
 * @tc.name: OnNodeCreate_NullGpuContext001
 * @tc.desc: Test OnNodeCreate when GetGpuContext returns null
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasModifiersDrawTest, OnNodeCreate_NullGpuContext001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    ASSERT_NE(canvasModifiersDraw, nullptr);
    canvasModifiersDraw->StartThread();
    // Set renderContext_ to a non-null value so GetGpuContext skips re-creation,
    // but keep gpuContext_ null so GetGpuContext returns null
    canvasModifiersDraw->canvasModifiersContext_->renderContext_ = RenderContext::Create();
    canvasModifiersDraw->canvasModifiersContext_->gpuContext_ = nullptr;
    NodeId nodeId = RSNode::GenerateId();
    canvasModifiersDraw->OnNodeCreate(nodeId, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    // OnNodeCreate returns early when gpuContext_ is null, so node should NOT be in drawableMap_
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw, nodeId]() { return canvasModifiersDraw->drawableMap_.count(nodeId) > 0; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_FALSE(future.get());
    canvasModifiersDraw->Destroy();
}

// Cover RequestBufferAndDrawHistory needResetCanvas=true branch (now passed as a parameter)
HWTEST_F(RSCanvasModifiersDrawableTest, RequestBufferAndDrawHistory_NeedResetCanvas001, TestSize.Level1)
{
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto surfaceA = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestNeedResetCanvas");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(Surface::CreateSurfaceAsProducer(bp), surfaceA);
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = 100;
    drawable.height_ = 100;
    auto result = drawable.RequestBufferAndDrawHistory(100, 100, true);
    ASSERT_TRUE(result);
    EXPECT_TRUE(drawable.bufferRequested_);
}

// Cover OnNodeCreate else branch when drawableMap_.size() != 1
HWTEST_F(RSCanvasModifiersDrawTest, OnNodeCreate_MultipleNodes001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    ASSERT_NE(canvasModifiersDraw, nullptr);
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->canvasModifiersContext_->cacheDir_ = "/data/local/tmp";
    canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
    if (canvasModifiersDraw->canvasModifiersContext_->gpuContext_ == nullptr) {
        // GPU not available in test environment, skip
        canvasModifiersDraw->Destroy();
        return;
    }
    NodeId nodeId1 = RSNode::GenerateId();
    NodeId nodeId2 = RSNode::GenerateId();
    // First node: drawableMap_.size() becomes 1 → if branch
    canvasModifiersDraw->OnNodeCreate(nodeId1, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    // Second node: drawableMap_.size() becomes 2 → else branch
    canvasModifiersDraw->OnNodeCreate(nodeId2, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    auto future = canvasModifiersDraw->ScheduleTask([canvasModifiersDraw, nodeId1, nodeId2]() {
        return canvasModifiersDraw->drawableMap_.count(nodeId1) > 0 &&
               canvasModifiersDraw->drawableMap_.count(nodeId2) > 0;
    });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
    canvasModifiersDraw->Destroy();
}

// Cover CleanFreeBuffers weak_ptr.lock() == nullptr branch.
// We must keep both runner_ and handler_ alive after releasing the RSCanvasModifiersDraw
// shared_ptr. If handler_ is destroyed, EventHandler's destructor removes the queued
// lambda from the EventRunner, so the weak_ptr.lock()==nullptr branch is never reached.
HWTEST_F(RSCanvasModifiersDrawTest, CleanFreeBuffers_WeakPtrNull001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    ASSERT_NE(canvasModifiersDraw, nullptr);
    canvasModifiersDraw->StartThread();
    // Keep handler_ alive so the queued lambda is not removed when the object is destroyed.
    auto handler = canvasModifiersDraw->handler_;
    ASSERT_NE(handler, nullptr);
    auto runner = canvasModifiersDraw->runner_;
    ASSERT_NE(runner, nullptr);
    // Schedule CleanFreeBuffers with immediate delay, then release the shared_ptr
    // so that when the lambda runs, weak_ptr.lock() returns nullptr.
    canvasModifiersDraw->CleanFreeBuffers(true);
    canvasModifiersDraw.reset();
    // Wait long enough for the deferred lambda (50ms) to execute on the runner thread
    usleep(200 * 1000);
    ASSERT_NE(handler, nullptr);
    ASSERT_NE(runner, nullptr);
}

// Cover CleanFreeBuffers else branch when drawableMap_ is empty
HWTEST_F(RSCanvasModifiersDrawTest, CleanFreeBuffers_EmptyDrawableMap001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    // drawableMap_ is empty, CleanFreeBuffers lambda should hit the else branch
    canvasModifiersDraw->CleanFreeBuffers(true);
    // The deferred lambda is posted with a 100ms delay; wait longer than that so the
    // empty-map branch (no reschedule) is deterministically reached before asserting.
    usleep(300 * 1000);
    EXPECT_TRUE(canvasModifiersDraw->drawableMap_.empty());
    canvasModifiersDraw->Destroy();
    ASSERT_EQ(canvasModifiersDraw->runner_, nullptr);
}

// ResetSurface (Draw layer) sets needForceSubmit_ = true so that the next
// SubmitAndCollectCanvasBuffers or SyncAndWait triggers a forced Submit to
// release deferred DDGR surface resources (DMA leak fix).
HWTEST_F(RSCanvasModifiersDrawTest, ResetSurface_SetsNeedForceSubmit001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 700;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    ASSERT_FALSE(canvasModifiersDraw->needForceSubmit_);
    canvasModifiersDraw->ResetSurface(nodeId, 200, 200, false);
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->needForceSubmit_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_TRUE(future.get());
}

// ResetSurface (Draw layer) updates lastGpuContextActiveTime_ even when the
// drawable is found (DMA fix refreshes active time to delay idle cleanup).
HWTEST_F(RSCanvasModifiersDrawTest, ResetSurface_UpdatesLastGpuContextActiveTime001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 800;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    canvasModifiersDraw->lastGpuContextActiveTime_ = 0;
    canvasModifiersDraw->ResetSurface(nodeId, 200, 200, false);
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->lastGpuContextActiveTime_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_GT(future.get(), 0);
}

// SubmitAndCollectCanvasBuffers with empty bufferList and needForceSubmit_=true
// calls gpuContext_->Submit() and clears the flag (DMA fix main path).
HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollect_EmptyBufferListForceSubmit001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
    if (canvasModifiersDraw->canvasModifiersContext_->gpuContext_ == nullptr) {
        return; // GPU not available in test environment
    }
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.nodeState_ = RSNodeState::INACTIVE;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    // bufferDirty_ defaults to false → GetBuffer returns nullptr → bufferList empty
    canvasModifiersDraw->needForceSubmit_ = true;
    canvasModifiersDraw->SubmitAndCollectCanvasBuffers();
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->needForceSubmit_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    // needForceSubmit_ is consumed (cleared) by SubmitAndCollectCanvasBuffers
    EXPECT_FALSE(future.get());
}

// SubmitAndCollectCanvasBuffers with empty bufferList and needForceSubmit_=false
// does NOT call Submit and leaves the flag untouched.
HWTEST_F(RSCanvasModifiersDrawTest, SubmitAndCollect_EmptyBufferListNoForceSubmit001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
    if (canvasModifiersDraw->canvasModifiersContext_->gpuContext_ == nullptr) {
        return;
    }
    NodeId nodeId = 1;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.nodeState_ = RSNodeState::INACTIVE;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    canvasModifiersDraw->needForceSubmit_ = false;
    canvasModifiersDraw->SubmitAndCollectCanvasBuffers();
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw]() { return canvasModifiersDraw->needForceSubmit_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);
    EXPECT_FALSE(future.get());
}

// SyncAndWait with empty transactionConfigList_, needForceSubmit_=true and
// gpuContext_!=nullptr calls gpuContext_->Submit() and clears the flag
// (DMA fix fallback path when SubmitAndCollectCanvasBuffers was not called).
HWTEST_F(RSCanvasModifiersDrawTest, SyncAndWait_ForceSubmitFallback001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
    if (canvasModifiersDraw->canvasModifiersContext_->gpuContext_ == nullptr) {
        return;
    }
    ASSERT_TRUE(canvasModifiersDraw->transactionConfigList_.empty());
    canvasModifiersDraw->needForceSubmit_ = true;
    auto command = canvasModifiersDraw->SyncAndWait();
    // Empty config list → no TransactionBufferCommand produced
    EXPECT_EQ(command, nullptr);
    // Fallback path consumed needForceSubmit_ via Submit()
    EXPECT_FALSE(canvasModifiersDraw->needForceSubmit_);
}

// SyncAndWait with empty transactionConfigList_, needForceSubmit_=true but
// gpuContext_==nullptr does NOT call Submit and leaves the flag set.
HWTEST_F(RSCanvasModifiersDrawTest, SyncAndWait_ForceSubmitNullGpuContext001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    canvasModifiersDraw->canvasModifiersContext_->gpuContext_ = nullptr;
    ASSERT_TRUE(canvasModifiersDraw->transactionConfigList_.empty());
    canvasModifiersDraw->needForceSubmit_ = true;
    auto command = canvasModifiersDraw->SyncAndWait();
    EXPECT_EQ(command, nullptr);
    // gpuContext_ is null → fallback branch guard fails → flag stays true
    EXPECT_TRUE(canvasModifiersDraw->needForceSubmit_);
}

// ResetSurface (Drawable layer) with invalid width (<=0) returns early without
// mutating width_/height_ (new guard added in the refactor).
HWTEST_F(RSCanvasModifiersDrawableTest, ResetSurface_InvalidWidth001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.ResetSurface(0, 100, false);
    // Invalid width → early return, dimensions unchanged
    EXPECT_EQ(drawable.width_, 100);
    EXPECT_EQ(drawable.height_, 100);
}

// ResetSurface (Drawable layer) with invalid height (<=0) returns early.
HWTEST_F(RSCanvasModifiersDrawableTest, ResetSurface_InvalidHeight001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.ResetSurface(100, -1, false);
    EXPECT_EQ(drawable.width_, 100);
    EXPECT_EQ(drawable.height_, 100);
}

// ResetSurface (Draw layer) with sizeOutOfGpuLimit=false also triggers
// CleanFreeBuffers(true) now (behavior changed from conditional to unconditional).
HWTEST_F(RSCanvasModifiersDrawTest, ResetSurface_NonSizeOutOfGpuLimitTriggersClean001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->StartThread();
    NodeId nodeId = 900;
    auto& drawable = canvasModifiersDraw->drawableMap_[nodeId];
    drawable.nodeId_ = nodeId;
    drawable.producerSurface_ = RSCanvasModifiersDrawableTest::CreateSurface();
    drawable.lastBufferExpiredTime_ = 1;
    drawable.width_ = 50;
    drawable.height_ = 50;
    // sizeOutOfGpuLimit=false still triggers CleanFreeBuffers(true)
    canvasModifiersDraw->ResetSurface(nodeId, 100, 100, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    auto future = canvasModifiersDraw->ScheduleTask(
        [canvasModifiersDraw, nodeId]() { return canvasModifiersDraw->drawableMap_[nodeId].lastBufferExpiredTime_; });
    ASSERT_EQ(future.wait_for(std::chrono::milliseconds(200)), std::future_status::ready);
    EXPECT_EQ(future.get(), 0);
}

// Mock surface whose RequestFrame always returns a frame without a Drawing::Surface,
// used to cover the null drawingSurface branch of RequestBufferAndDrawHistory.
class TestRSSurfaceOhosVulkanNullFrame : public RSSurfaceOhosVulkan {
public:
    explicit TestRSSurfaceOhosVulkanNullFrame(const sptr<Surface>& producer) : RSSurfaceOhosVulkan(producer) {}

    std::unique_ptr<RSSurfaceFrame> RequestFrame(int32_t width, int32_t height,
        uint64_t uiTimestamp = 0, bool useAFBC = true, bool isProtected = false) override
    {
        return std::make_unique<RSSurfaceFrameOhosVulkan>(nullptr, width, height, 1);
    }
};

HWTEST_F(RSCanvasModifiersDrawableTest, RequestBufferAndDrawHistory_NullDrawingSurface001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestNullDrawingSurface");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkanNullFrame>(Surface::CreateSurfaceAsProducer(bp));
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    EXPECT_FALSE(drawable.RequestBufferAndDrawHistory(100, 100, false));
    EXPECT_EQ(drawable.drawingSurface_, nullptr);
    EXPECT_FALSE(drawable.bufferRequested_);
}

HWTEST_F(RSCanvasModifiersDrawableTest, RequestBufferAndDrawHistory_NullCanvas001, TestSize.Level1)
{
    // Default-constructed Drawing::Surface has no backing SkSurface, so GetCanvas() returns nullptr.
    auto nullCanvasSurface = std::make_shared<Drawing::Surface>();
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestNullCanvas");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(
        Surface::CreateSurfaceAsProducer(bp), nullCanvasSurface);
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    EXPECT_FALSE(drawable.RequestBufferAndDrawHistory(100, 100, false));
    EXPECT_FALSE(drawable.bufferRequested_);
}

HWTEST_F(RSCanvasModifiersDrawableTest, RequestBufferAndDrawHistory_SameSurface001, TestSize.Level1)
{
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto sameSurface = Drawing::Surface::MakeRaster(imageInfo);
    ASSERT_NE(sameSurface, nullptr);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestSameSurface");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(
        Surface::CreateSurfaceAsProducer(bp), sameSurface);
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    // The newly requested surface is the same object as the current one, so inherit is skipped.
    drawable.drawingSurface_ = sameSurface;
    EXPECT_TRUE(drawable.RequestBufferAndDrawHistory(100, 100, false));
    EXPECT_EQ(drawable.drawingSurface_, sameSurface);
    EXPECT_TRUE(drawable.bufferRequested_);
}

HWTEST_F(RSCanvasModifiersDrawableTest, RequestBufferAndDrawHistory_LastCanvasNull001, TestSize.Level1)
{
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto newSurface = Drawing::Surface::MakeRaster(imageInfo);
    ASSERT_NE(newSurface, nullptr);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestLastCanvasNull");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(Surface::CreateSurfaceAsProducer(bp), newSurface);
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    // Previous surface differs from the new one but has no canvas, so inherit is skipped.
    drawable.drawingSurface_ = std::make_shared<Drawing::Surface>();
    EXPECT_TRUE(drawable.RequestBufferAndDrawHistory(100, 100, false));
    EXPECT_EQ(drawable.drawingSurface_, newSurface);
    EXPECT_TRUE(drawable.bufferRequested_);
}

HWTEST_F(RSCanvasModifiersDrawableTest, ResetSurface_BufferRequested001, TestSize.Level1)
{
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto rasterSurface = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestResetSurfaceCancel");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(Surface::CreateSurfaceAsProducer(bp), rasterSurface);
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.width_ = 10;
    drawable.height_ = 10;
    drawable.bufferRequested_ = true;
    drawable.ResetSurface(20, 20, false);
    EXPECT_FALSE(drawable.bufferRequested_);
    EXPECT_EQ(drawable.width_, 20);
    EXPECT_EQ(drawable.height_, 20);
}

HWTEST_F(RSCanvasModifiersDrawableTest, Draw_NonEmptyCacheForceFlush001, TestSize.Level1)
{
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto rasterSurface = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestDrawNonEmptyForceFlush");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(Surface::CreateSurfaceAsProducer(bp), rasterSurface);
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.drawCmdListCache_->emplace_back(std::make_shared<Drawing::DrawCmdList>());
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.Draw(true);
    EXPECT_TRUE(drawable.bufferDirty_);
    EXPECT_TRUE(drawable.hasDrawn_);
}

HWTEST_F(RSCanvasModifiersDrawableTest, Draw_NonEmptyCacheNoForceFlush001, TestSize.Level1)
{
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto rasterSurface = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestDrawNonEmptyNoForceFlush");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(Surface::CreateSurfaceAsProducer(bp), rasterSurface);
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.drawCmdListCache_->emplace_back(std::make_shared<Drawing::DrawCmdList>());
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.Draw(false);
    EXPECT_TRUE(drawable.bufferDirty_);
    EXPECT_TRUE(drawable.hasDrawn_);
}

HWTEST_F(RSCanvasModifiersDrawableTest, Draw_BufferAlreadyRequested001, TestSize.Level1)
{
    auto imageInfo = Drawing::ImageInfo::MakeN32Premul(100, 100);
    auto rasterSurface = Drawing::Surface::MakeRaster(imageInfo);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("TestDrawBufferRequested");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    auto mockSurface = std::make_shared<TestRSSurfaceOhosVulkan>(Surface::CreateSurfaceAsProducer(bp), rasterSurface);
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = mockSurface;
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.drawCmdListCache_->emplace_back(std::make_shared<Drawing::DrawCmdList>());
    drawable.drawingSurface_ = rasterSurface;
    drawable.bufferRequested_ = true;
    drawable.width_ = 100;
    drawable.height_ = 100;
    drawable.Draw(false);
    EXPECT_TRUE(drawable.bufferDirty_);
    EXPECT_TRUE(drawable.hasDrawn_);
}

HWTEST_F(RSCanvasModifiersDrawableTest, CreateProducerSurface_InvalidColorGamut001, TestSize.Level1)
{
    constexpr int invalidColorGamutValue = 0xFFFF;
    auto screenId = RSInterfaces::GetInstance().GetDefaultScreenId();
    auto connectToRender = RSInterfaces::GetInstance().GetConnectToRenderToken(screenId);
    ASSERT_NE(connectToRender, nullptr);
    auto renderInterface = std::make_shared<RSRenderInterface>(connectToRender);
    auto context = std::make_shared<RSCanvasModifiersContext>(renderInterface);
    context->cacheDir_ = "/cache";
    context->GetGpuContext();
    RSCanvasModifiersDrawable drawable;
    drawable.nodeId_ = RSNode::GenerateId();
    drawable.CreateProducerSurface(context, static_cast<GraphicColorGamut>(invalidColorGamutValue));
    // Color gamut conversion fails, but the producer surface is still created when the
    // render service provides one; producerSurface_ and drawCmdListCache_ are set together.
    if (drawable.producerSurface_ != nullptr) {
        drawable.ReleaseProducerSurface(context);
    }
    EXPECT_EQ(drawable.producerSurface_ != nullptr, drawable.drawCmdListCache_ != nullptr);
}

// UpdateContent with a non-null drawCmdList appends it to the cache before drawing.
HWTEST_F(RSCanvasModifiersDrawableTest, UpdateContent_NonNullDrawCmdList001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    drawable.producerSurface_ = CreateSurface();
    drawable.drawCmdListCache_ = std::make_unique<std::vector<Drawing::DrawCmdListPtr>>();
    drawable.UpdateContent(std::make_shared<Drawing::DrawCmdList>(), false);
    EXPECT_EQ(drawable.drawCmdListCache_->size(), 1u);
}

// GetPixelMap with a null drawCmdList uses the image directly (no intermediate render target).
HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_NullDrawCmdListWithGpuImage001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    auto context = CreateCanvasModifiersContext();
    ASSERT_TRUE(PrepareGpuDrawable(drawable, context, GPU_TEST_SIZE, GPU_TEST_SIZE));
    auto pixelMap = CreateTestPixelMap(GPU_TEST_SIZE, GPU_TEST_SIZE);
    ASSERT_NE(pixelMap, nullptr);
    Drawing::Rect rect(0, 0, GPU_TEST_SIZE, GPU_TEST_SIZE);
    EXPECT_TRUE(drawable.GetPixelMap(pixelMap, &rect, nullptr, context));
}

// GetPixelMap with an empty drawCmdList skips the render-target composition block.
HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_EmptyDrawCmdList001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    auto context = CreateCanvasModifiersContext();
    ASSERT_TRUE(PrepareGpuDrawable(drawable, context, GPU_TEST_SIZE, GPU_TEST_SIZE));
    auto pixelMap = CreateTestPixelMap(GPU_TEST_SIZE, GPU_TEST_SIZE);
    ASSERT_NE(pixelMap, nullptr);
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    ASSERT_TRUE(drawCmdList->IsEmpty());
    Drawing::Rect rect(0, 0, GPU_TEST_SIZE, GPU_TEST_SIZE);
    EXPECT_TRUE(drawable.GetPixelMap(pixelMap, &rect, drawCmdList, context));
}

// A render target that cannot be created (size exceeds the GPU texture limit) leaves the
// composed image untouched, so the direct image is still read back successfully.
HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_RenderTargetCreationFails001, TestSize.Level1)
{
    constexpr int outOfGpuLimitSize = 100000;
    RSCanvasModifiersDrawable drawable;
    auto context = CreateCanvasModifiersContext();
    ASSERT_TRUE(PrepareGpuDrawable(drawable, context, GPU_TEST_SIZE, GPU_TEST_SIZE));
    drawable.width_ = outOfGpuLimitSize;
    drawable.height_ = outOfGpuLimitSize;
    auto pixelMap = CreateTestPixelMap(GPU_TEST_SIZE, GPU_TEST_SIZE);
    ASSERT_NE(pixelMap, nullptr);
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    drawCmdList->AddOp<Drawing::DrawColorOpItem>(0xFF000000, Drawing::BlendMode::SRC);
    Drawing::Rect rect(0, 0, GPU_TEST_SIZE, GPU_TEST_SIZE);
    EXPECT_TRUE(drawable.GetPixelMap(pixelMap, &rect, drawCmdList, context));
}

// ReadPixels fails when the source rectangle lies outside the image bounds.
HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_ReadPixelsOutOfBounds001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    auto context = CreateCanvasModifiersContext();
    ASSERT_TRUE(PrepareGpuDrawable(drawable, context, GPU_TEST_SIZE, GPU_TEST_SIZE));
    auto pixelMap = CreateTestPixelMap(GPU_TEST_SIZE, GPU_TEST_SIZE);
    ASSERT_NE(pixelMap, nullptr);
    Drawing::Rect rect(OUT_OF_BOUNDS_COORD, OUT_OF_BOUNDS_COORD, OUT_OF_BOUNDS_COORD, OUT_OF_BOUNDS_COORD);
    EXPECT_FALSE(drawable.GetPixelMap(pixelMap, &rect, nullptr, context));
}

// DMA-backed pixel maps flush the cache when the buffer carries the MMZ cache usage flag.
HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_DmaAllocWithMmzCache001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    auto context = CreateCanvasModifiersContext();
    ASSERT_TRUE(PrepareGpuDrawable(drawable, context, GPU_TEST_SIZE, GPU_TEST_SIZE));
    Media::InitializationOptions opts;
    opts.size.width = GPU_TEST_SIZE;
    opts.size.height = GPU_TEST_SIZE;
    opts.allocatorType = Media::AllocatorType::DMA_ALLOC;
    std::shared_ptr<Media::PixelMap> pixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(pixelMap, nullptr);
    ASSERT_EQ(pixelMap->GetAllocatorType(), Media::AllocatorType::DMA_ALLOC);
    Drawing::Rect rect(0, 0, GPU_TEST_SIZE, GPU_TEST_SIZE);
    EXPECT_TRUE(drawable.GetPixelMap(pixelMap, &rect, nullptr, context));
}

// A DMA-backed pixel map whose buffer descriptor is missing still returns the read-back result.
HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_DmaAllocNullSurfaceBuffer001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    auto context = CreateCanvasModifiersContext();
    ASSERT_TRUE(PrepareGpuDrawable(drawable, context, GPU_TEST_SIZE, GPU_TEST_SIZE));
    Media::InitializationOptions opts;
    opts.size.width = GPU_TEST_SIZE;
    opts.size.height = GPU_TEST_SIZE;
    opts.allocatorType = Media::AllocatorType::DMA_ALLOC;
    std::shared_ptr<Media::PixelMap> pixelMap(Media::PixelMap::Create(opts));
    ASSERT_NE(pixelMap, nullptr);
    void* savedContext = pixelMap->context_;
    pixelMap->context_ = nullptr;
    Drawing::Rect rect(0, 0, GPU_TEST_SIZE, GPU_TEST_SIZE);
    bool result = drawable.GetPixelMap(pixelMap, &rect, nullptr, context);
    pixelMap->context_ = savedContext;
    EXPECT_TRUE(result);
}

// A DMA-backed pixel map built from a buffer without the MMZ cache flag skips the flush.
HWTEST_F(RSCanvasModifiersDrawableTest, GetPixelMap_DmaAllocWithoutMmzCache001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    auto context = CreateCanvasModifiersContext();
    ASSERT_TRUE(PrepareGpuDrawable(drawable, context, GPU_TEST_SIZE, GPU_TEST_SIZE));
    sptr<SurfaceBuffer> surfaceBuffer = SurfaceBuffer::Create();
    ASSERT_NE(surfaceBuffer, nullptr);
    BufferRequestConfig config = { .width = GPU_TEST_SIZE, .height = GPU_TEST_SIZE, .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, .timeout = 0 };
    ASSERT_EQ(surfaceBuffer->Alloc(config), GSERROR_OK);
    auto pixelMap = std::make_shared<Media::PixelMap>();
    Media::ImageInfo imageInfo;
    imageInfo.size.width = GPU_TEST_SIZE;
    imageInfo.size.height = GPU_TEST_SIZE;
    imageInfo.pixelFormat = Media::PixelFormat::RGBA_8888;
    imageInfo.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    ASSERT_EQ(pixelMap->SetImageInfo(imageInfo), 0u);
    surfaceBuffer->IncStrongRef(surfaceBuffer.GetRefPtr());
    pixelMap->SetPixelsAddr(surfaceBuffer->GetVirAddr(), surfaceBuffer.GetRefPtr(), surfaceBuffer->GetSize(),
        Media::AllocatorType::DMA_ALLOC, nullptr);
    Drawing::Rect rect(0, 0, GPU_TEST_SIZE, GPU_TEST_SIZE);
    EXPECT_TRUE(drawable.GetPixelMap(pixelMap, &rect, nullptr, context));
}

// GetBitmap succeeds for a GPU-backed image and returns a legacy bitmap copy.
HWTEST_F(RSCanvasModifiersDrawableTest, GetBitmap_WithGpuImage001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    auto context = CreateCanvasModifiersContext();
    ASSERT_TRUE(PrepareGpuDrawable(drawable, context, GPU_TEST_SIZE, GPU_TEST_SIZE));
    Drawing::Bitmap bitmap;
    EXPECT_TRUE(drawable.GetBitmap(bitmap, context));
}

// GetImage fails when the GPU context has no underlying GrDirectContext.
HWTEST_F(RSCanvasModifiersDrawableTest, GetImage_BuildFromTextureFails001, TestSize.Level1)
{
    RSCanvasModifiersDrawable drawable;
    auto context = CreateCanvasModifiersContext();
    drawable.drawingSurface_ = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo::MakeN32Premul(GPU_TEST_SIZE, GPU_TEST_SIZE));
    context->gpuContext_ = std::make_shared<Drawing::GPUContext>();
    auto bitmapFormat = Drawing::BitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    EXPECT_EQ(drawable.GetImage(bitmapFormat, context), nullptr);
}

// PostTask drops the task when the handler is unavailable even though the thread is marked started.
HWTEST_F(RSCanvasModifiersDrawTest, PostTask_NullHandler001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->threadStarted_ = true;
    canvasModifiersDraw->handler_ = nullptr;
    bool taskExecuted = false;
    canvasModifiersDraw->PostTask([&taskExecuted]() { taskExecuted = true; }, "NullHandlerTask", 0);
    EXPECT_FALSE(taskExecuted);
}

// PostSyncTask drops the task when the handler is unavailable even though the thread is marked started.
HWTEST_F(RSCanvasModifiersDrawTest, PostSyncTask_NullHandler001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->threadStarted_ = true;
    canvasModifiersDraw->handler_ = nullptr;
    bool taskExecuted = false;
    canvasModifiersDraw->PostSyncTask([&taskExecuted]() { taskExecuted = true; });
    EXPECT_FALSE(taskExecuted);
}

// FlushSurfaceWithSemaphore returns null when Vulkan cannot create the semaphore.
HWTEST_F(RSCanvasModifiersDrawTest, FlushSurfaceWithSemaphore_CreateSemaphoreFails001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
    ASSERT_NE(canvasModifiersDraw->canvasModifiersContext_->renderContext_, nullptr);
    auto& vkInterface = RsVulkanContext::Get(
        canvasModifiersDraw->canvasModifiersContext_->renderContext_->GetType()).GetRsVulkanInterface();
    ASSERT_NE(vkInterface, nullptr);
    PFN_vkCreateSemaphore savedCreateSemaphore = vkInterface->vkCreateSemaphore;
    vkInterface->vkCreateSemaphore = StubCreateSemaphoreFail;
    VkSemaphore semaphore = VK_NULL_HANDLE;
    auto drawingSurface = Drawing::Surface::MakeRaster(
        Drawing::ImageInfo::MakeN32Premul(GPU_TEST_SIZE, GPU_TEST_SIZE));
    auto* result = canvasModifiersDraw->FlushSurfaceWithSemaphore(semaphore, drawingSurface);
    vkInterface->vkCreateSemaphore = savedCreateSemaphore;
    EXPECT_EQ(result, nullptr);
}

// GetFenceFd returns an invalid fd when the semaphore handle is null.
HWTEST_F(RSCanvasModifiersDrawTest, GetFenceFd_NullSemaphore001, TestSize.Level1)
{
    auto canvasModifiersDraw = CreateCanvasModifiersDraw();
    canvasModifiersDraw->canvasModifiersContext_->GetGpuContext();
    ASSERT_NE(canvasModifiersDraw->canvasModifiersContext_->renderContext_, nullptr);
    VkSemaphore semaphore = VK_NULL_HANDLE;
    EXPECT_EQ(canvasModifiersDraw->GetFenceFd(semaphore), INVALID_FD);
}
} // namespace Rosen
} // namespace OHOS
