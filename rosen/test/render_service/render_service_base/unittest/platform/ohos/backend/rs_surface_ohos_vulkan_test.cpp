/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
#include <parameter.h>
#include <parameters.h>
#include <unistd.h>

#include "param/sys_param.h"

#include "iconsumer_surface.h"

#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#include "render_context/render_context.h"
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "hpae_base/rs_hpae_base_types.h"
#include "hpae_base/rs_hpae_scheduler.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class BufferConsumerListener : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override
    {
    }
};

class RSSurfaceOhosVulkanTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceOhosVulkanTest::SetUpTestCase() {}
void RSSurfaceOhosVulkanTest::TearDownTestCase() {}
void RSSurfaceOhosVulkanTest::SetUp() {}
void RSSurfaceOhosVulkanTest::TearDown() {}

/**
 * @tc.name: ClearBuffer001
 * @tc.desc: test results of ClearBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, ClearBuffer001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    rsSurface.ClearBuffer();
    ASSERT_FALSE(rsSurface.IsValid());
}

/**
 * @tc.name: ClearBuffer002
 * @tc.desc: test results of ClearBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, ClearBuffer002, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface != nullptr);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosVulkan rsSurface(pSurface);
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        int32_t width = 1;
        int32_t height = 1;
        uint64_t uiTimestamp = 1;
        rsSurface.RequestFrame(width, height, uiTimestamp);
    }
#endif
    rsSurface.ClearBuffer();
}

/**
 * @tc.name: ResetBufferAge001
 * @tc.desc: test results of ResetBufferAge
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, ResetBufferAge001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    rsSurface.ResetBufferAge();
    ASSERT_FALSE(rsSurface.IsValid());
}

/**
 * @tc.name: ResetBufferAge002
 * @tc.desc: test results of ResetBufferAge
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, ResetBufferAge002, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface != nullptr);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosVulkan rsSurface(pSurface);
#ifdef RS_ENABLE_VK
    if (RSSystemProperties::IsUseVulkan()) {
        auto renderContext = RenderContext::Create();
        rsSurface.SetRenderContext(renderContext);
        int32_t width = 1;
        int32_t height = 1;
        uint64_t uiTimestamp = 1;
        rsSurface.RequestFrame(width, height, uiTimestamp);
    }
#endif
    rsSurface.ResetBufferAge();
}

/**
 * @tc.name: SetNativeWindowInfo001
 * @tc.desc: test results of SetNativeWindowInfo
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SetNativeWindowInfo001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    int32_t width = 1;
    int32_t height = -1;
    rsSurface.SetNativeWindowInfo(width, height, false, false); // true

    width = -1;
    height = 1;
    rsSurface.SetNativeWindowInfo(width, height, true, true); // true

    width = 1;
    height = 1;
    rsSurface.SetNativeWindowInfo(width, height, true, false); // true

    width = -1;
    height = -1;
    rsSurface.SetNativeWindowInfo(width, height, true, true); // false

    EXPECT_TRUE(rsSurface.mSurfaceMap.empty());
}

/**
 * @tc.name: SetNativeWindowInfo002
 * @tc.desc: test results of SetNativeWindowInfo
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SetNativeWindowInfo002, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    rsSurface.SetSurfaceBufferUsage(BUFFER_USAGE_GRAPHIC_2D_ACCEL);
    int32_t width = 1;
    int32_t height = -1;
    rsSurface.SetNativeWindowInfo(width, height, false, false); // true

    width = -1;
    height = 1;
    rsSurface.SetNativeWindowInfo(width, height, true, true); // true

    width = 1;
    height = 1;
    rsSurface.SetNativeWindowInfo(width, height, true, false); // true

    width = -1;
    height = -1;
    rsSurface.SetNativeWindowInfo(width, height, true, true); // false

    width = -1;
    height = -1;
    auto afbcSwitch = OHOS::system::GetParameter("rosen.afbc.enabled", "1");
    OHOS::system::SetParameter("rosen.afbc.enabled", "0");
    rsSurface.SetNativeWindowInfo(width, height, true, true);
    OHOS::system::SetParameter("rosen.afbc.enabled", afbcSwitch);

    EXPECT_TRUE(rsSurface.mSurfaceMap.empty());
}

/**
 * @tc.name: RequestNativeWindowBuffer001
 * @tc.desc: test results of RequestNativeWindowBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, RequestNativeWindowBuffer001, TestSize.Level1)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);

    int32_t width = 1;
    int32_t height = 5;
    int fenceFd = -1;
    bool useAFBC = true;
    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    auto res = rsSurface.RequestNativeWindowBuffer(&nativeWindowBuffer, width, height, fenceFd, useAFBC);
    EXPECT_TRUE(res != GSERROR_OK);
}

/*
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. preSetup: create cSurface
 *                  2. operation: PreAllocateProtectedBuffer
 *                  3. result: PreAllocateProtectedBuffer return false
 */
HWTEST_F(RSSurfaceOhosVulkanTest, PreAllocateProtectedBuffer001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface != nullptr);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosVulkan rsSurface(pSurface);
    int32_t width = 1;
    int32_t height = 1;
    bool ret = rsSurface.PreAllocateProtectedBuffer(width, height);
    EXPECT_TRUE(ret);
}

HWTEST_F(RSSurfaceOhosVulkanTest, PreAllocateHpaeBuffer001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface != nullptr);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosVulkan rsSurface(pSurface);
    int32_t width = 1;
    int32_t height = 1;
    rsSurface.PreAllocateHpaeBuffer(width, height, 1, true);

    NativeWindowBuffer windowBuffer;
    rsSurface.hpaeSurfaceBufferList_.emplace_back(std::make_pair(&windowBuffer, 0));
    rsSurface.hpaeSurfaceBufferList_.emplace_back(std::make_pair(nullptr, 0));
    rsSurface.PreAllocateHpaeBuffer(width, height, 1, false);
    EXPECT_TRUE(rsSurface.mNativeWindow != nullptr);
}

/**
 * @tc.name: RequestFrame001
 * @tc.desc: test results of RequestFrame
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, RequestFrame001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface != nullptr);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosVulkan rsSurface(pSurface);
    int32_t width = 1;
    int32_t height = 1;
    uint64_t uiTimestamp = 1;
    std::unique_ptr<RSSurfaceFrame> ret = rsSurface.RequestFrame(width, height, uiTimestamp, true, true);
    EXPECT_TRUE(ret == nullptr);
    rsSurface.SetRenderContext(RenderContext::Create());
    rsSurface.GetRenderContext()->SetDrGPUContext(std::make_shared<Drawing::GPUContext>());
    ret = rsSurface.RequestFrame(width, height, uiTimestamp, true, true);
    EXPECT_TRUE(ret == nullptr);

    rsSurface.SetRenderContext(RenderContext::Create());
    ret = rsSurface.RequestFrame(width, height, uiTimestamp, true, true);
    EXPECT_TRUE(ret == nullptr);
}

HWTEST_F(RSSurfaceOhosVulkanTest, RequestFrame002, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface != nullptr);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosVulkan rsSurface(pSurface);
    int32_t width = 1;
    int32_t height = 1;
    uint64_t uiTimestamp = 1;

    std::unique_ptr<RSSurfaceFrame> ret = rsSurface.RequestFrame(width, height, uiTimestamp, true, false);
    EXPECT_TRUE(ret == nullptr);

    std::shared_ptr<Drawing::GPUContext> skContext = std::make_shared<Drawing::GPUContext>();
    rsSurface.SetSkContext(skContext);
    ret = rsSurface.RequestFrame(width, height, uiTimestamp, true, false);

    rsSurface.MarkAsHpaeSurface();
    ret = rsSurface.RequestFrame(width, height, uiTimestamp, true, false);
    EXPECT_TRUE(ret == nullptr);

    rsSurface.hpaeSurfaceBufferList_.emplace_back(std::make_pair(nullptr, 0));
    rsSurface.RequestFrame(width, height, uiTimestamp, true, false);
}

/**
 * @tc.name: GetCurrentBuffer001
 * @tc.desc: test results of GetCurrentBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, GetCurrentBuffer001, TestSize.Level1)
{
    RSSurfaceOhosVulkan rsSurface(IConsumerSurface::Create());
    sptr<SurfaceBuffer> ret = rsSurface.GetCurrentBuffer();
    EXPECT_TRUE(ret == nullptr);

    sptr<OHOS::IConsumerSurface> cSurface = IConsumerSurface::Create();
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    cSurface->RegisterConsumerListener(listener);
    sptr<OHOS::IBufferProducer> producer = cSurface->GetProducer();
    sptr<OHOS::Surface> pSurface = Surface::CreateSurfaceAsProducer(producer);
    int32_t fence;
    sptr<OHOS::SurfaceBuffer> sBuffer = nullptr;
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    pSurface->RequestBuffer(sBuffer, fence, requestConfig);
    NativeWindowBuffer* nativeWindowBuffer = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&sBuffer);
    ASSERT_NE(nativeWindowBuffer, nullptr);
    rsSurface.mSurfaceList.emplace_back(nativeWindowBuffer);
    ret = rsSurface.GetCurrentBuffer();
    EXPECT_TRUE(ret != nullptr);
}

/**
 * @tc.name: SetColorSpace001
 * @tc.desc: test results of SetColorSpace
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SetColorSpace001, TestSize.Level1)
{
    RSSurfaceOhosVulkan rsSurface(IConsumerSurface::Create());
    rsSurface.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    ASSERT_EQ(rsSurface.colorSpace_, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    rsSurface.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    ASSERT_EQ(rsSurface.colorSpace_, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
}

/**
 * @tc.name: SetSurfacePixelFormat001
 * @tc.desc: test results of SetSurfacePixelFormat
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SetSurfacePixelFormat001, TestSize.Level1)
{
    RSSurfaceOhosVulkan rsSurface(IConsumerSurface::Create());
    rsSurface.SetSurfacePixelFormat(11);
    rsSurface.SetSurfacePixelFormat(11);
    ASSERT_EQ(rsSurface.pixelFormat_, GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBX_8888);
}

/**
 * @tc.name: FlushFrame001
 * @tc.desc: test results of FlushFrame
 * @tc.type:FUNC
 * @tc.require: issueI9VVLE
 */
HWTEST_F(RSSurfaceOhosVulkanTest, FlushFrame001, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface != nullptr);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosVulkan rsSurface(pSurface);
    uint64_t uiTimestamp = 1;
    std::unique_ptr<RSSurfaceFrame> frame = nullptr;
    rsSurface.SetUiTimeStamp(frame, uiTimestamp);

    int32_t width = 1;
    int32_t height = 1;
    std::unique_ptr<RSSurfaceFrame> ret = rsSurface.RequestFrame(width, height, uiTimestamp, true, true);
    EXPECT_FALSE(rsSurface.FlushFrame(frame, uiTimestamp));
    {
        sptr<Surface> producer = nullptr;
        RSSurfaceOhosGl rsSurface(producer);
        auto renderContext = RenderContext::Create();
        rsSurface.SetRenderContext(renderContext);
        ASSERT_TRUE(rsSurface.FlushFrame(frame, uiTimestamp));
    }
}

#if defined(ROSEN_OHOS) && defined(RS_GRAPHIC_MEDIACOMMON_ENABLE)
/**
 * @tc.name: SubmitHapeTaskTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require: wz
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SubmitHapeTaskTest, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface != nullptr);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosVulkan rsSurface(pSurface);

    uint64_t preFrameId = 1;
    rsSurface.SubmitHapeTask(preFrameId);
    HpaeTask hpaeTask;
    hpaeTask.taskId = 1;
    hpaeTask.taskPtr = &preFrameId;
    HpaeBackgroundCacheItem item;
    item.hpaeTask_ = hpaeTask;
    RSHpaeScheduler::GetInstance().CacheHpaeItem(item);
    rsSurface.SubmitHapeTask(preFrameId);
}

/**
 * @tc.name: SubmitGpuAndHpaeTaskTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require: wz
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SubmitGpuAndHpaeTaskTest, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface != nullptr);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosVulkan rsSurface(pSurface);

    uint64_t preFrameId = 1;
    uint64_t curFrameId = 1;
    rsSurface.SubmitGpuAndHpaeTask(preFrameId, curFrameId);

    preFrameId = 0;
    curFrameId = 0;
    rsSurface.SubmitGpuAndHpaeTask(preFrameId, curFrameId);
}

/**
 * @tc.name: SetGpuSemaphoreTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require: wz
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SetGpuSemaphoreTest, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface != nullptr);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    RSSurfaceOhosVulkan rsSurface(pSurface);

    bool submitWithFFTS = true;
    uint64_t preFrameId = 1;
    uint64_t curFrameId = 1;
    std::vector<GrBackendSemaphore> vec;
    NativeBufferUtils::NativeSurfaceInfo sur;
    rsSurface.SetGpuSemaphore(submitWithFFTS, preFrameId, curFrameId, vec, sur);

    preFrameId = 0;
    curFrameId = 0;
    rsSurface.SetGpuSemaphore(submitWithFFTS, preFrameId, curFrameId, vec, sur);
}
#endif

/**
 * @tc.name: CancelBufferForCurrentFrame
 * @tc.desc: test results of CancelBufferForCurrentFrame
 * @tc.type:FUNC
 * @tc.require: issueICWRWD
 */
HWTEST_F(RSSurfaceOhosVulkanTest, CancelBufferForCurrentFrame, TestSize.Level1)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    ASSERT_TRUE(cSurface != nullptr);
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    int32_t fence;
    sptr<OHOS::SurfaceBuffer> sBuffer = nullptr;
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    pSurface->RequestBuffer(sBuffer, fence, requestConfig);
    NativeWindowBuffer* nativeWindowBuffer = OH_NativeWindow_CreateNativeWindowBufferFromSurfaceBuffer(&sBuffer);
    ASSERT_NE(nativeWindowBuffer, nullptr);
    RSSurfaceOhosVulkan rsSurface(pSurface);
    rsSurface.mSurfaceList.emplace_back(nullptr);
    rsSurface.mSurfaceList.emplace_back(nativeWindowBuffer);
    rsSurface.CancelBufferForCurrentFrame();
    ASSERT_EQ(rsSurface.mSurfaceList.size(), 1);
    rsSurface.mSurfaceList.clear();
    rsSurface.CancelBufferForCurrentFrame();
    ASSERT_TRUE(rsSurface.mSurfaceList.empty());
}

/**
 * @tc.name: FlushGpu_NullSurface
 * @tc.desc: Test FlushGpu returns false with no surface
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, FlushGpu_NullSurface, TestSize.Level2)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    std::unique_ptr<RSSurfaceFrame> frame;
    EXPECT_FALSE(rsSurface.FlushGpu(frame));
}

/**
 * @tc.name: SubmitGpu_InvalidState
 * @tc.desc: Test SubmitGpu returns false without prior FlushGpu
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SubmitGpu_InvalidState, TestSize.Level2)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    std::unique_ptr<RSSurfaceFrame> frame;
    EXPECT_FALSE(rsSurface.SubmitGpu(frame));
}

/**
 * @tc.name: FlushBuffer_InvalidState
 * @tc.desc: Test FlushBuffer returns false without prior FlushGpu
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, FlushBuffer_InvalidState, TestSize.Level2)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    std::unique_ptr<RSSurfaceFrame> frame;
    EXPECT_FALSE(rsSurface.FlushBuffer(frame));
}

/**
 * @tc.name: CancelActiveFlush_NoActiveFlush
 * @tc.desc: Test CancelActiveFlush with no active flush (no-op)
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, CancelActiveFlush_NoActiveFlush, TestSize.Level2)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    rsSurface.CancelActiveFlush();
    EXPECT_FALSE(rsSurface.flushState_.valid);
}

/**
 * @tc.name: FlushState_Reset
 * @tc.desc: Test FlushState Reset clears all fields
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, FlushState_Reset, TestSize.Level2)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    rsSurface.flushState_.valid = true;
    rsSurface.flushState_.bufferKey = reinterpret_cast<NativeWindowBuffer*>(0x1);
    rsSurface.flushState_.Reset();
    EXPECT_FALSE(rsSurface.flushState_.valid);
    EXPECT_EQ(rsSurface.flushState_.bufferKey, nullptr);
}

/**
 * @tc.name: FlushFrame_EmptySurfaceList
 * @tc.desc: Test FlushFrame returns false with empty surface list
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, FlushFrame_EmptySurfaceList, TestSize.Level2)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("FlushFrameTest");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    ASSERT_NE(pSurface, nullptr);
    RSSurfaceOhosVulkan rsSurface(pSurface);
    std::unique_ptr<RSSurfaceFrame> frame;
    EXPECT_FALSE(rsSurface.FlushFrame(frame));
}

// ============================================================
// CancelBuffer branch coverage tests
// ============================================================

/**
 * @tc.name: CancelBuffer_NullBuffer
 * @tc.desc: CancelBuffer with nullptr returns early without crash
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, CancelBuffer_NullBuffer, TestSize.Level2)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("CancelBufNull");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    RSSurfaceOhosVulkan rsSurface(pSurface);
    rsSurface.CancelBuffer(nullptr);
    EXPECT_TRUE(rsSurface.mSurfaceList.empty());
    EXPECT_TRUE(rsSurface.mSurfaceMap.empty());
}

/**
 * @tc.name: CancelBuffer_NotInSurfaceMap
 * @tc.desc: CancelBuffer with buffer not in mSurfaceMap returns early
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, CancelBuffer_NotInSurfaceMap, TestSize.Level2)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("CancelBufNoMap");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    RSSurfaceOhosVulkan rsSurface(pSurface);
    NativeWindowBuffer dummyBuffer;
    rsSurface.CancelBuffer(&dummyBuffer);
    EXPECT_TRUE(rsSurface.mSurfaceList.empty());
}

/**
 * @tc.name: CancelBuffer_FlushStateReset
 * @tc.desc: CancelBuffer resets flushState_ when bufferKey matches
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, CancelBuffer_FlushStateReset, TestSize.Level2)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("CancelBufReset");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    RSSurfaceOhosVulkan rsSurface(pSurface);
    NativeWindowBuffer dummyBuffer;
    rsSurface.flushState_.valid = true;
    rsSurface.flushState_.bufferKey = &dummyBuffer;
    rsSurface.CancelBuffer(&dummyBuffer);
    EXPECT_FALSE(rsSurface.flushState_.valid);
    EXPECT_EQ(rsSurface.flushState_.bufferKey, nullptr);
}

/**
 * @tc.name: CancelBuffer_FlushStateNotResetOnMismatch
 * @tc.desc: CancelBuffer does not reset flushState_ when bufferKey differs
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, CancelBuffer_FlushStateNotResetOnMismatch, TestSize.Level2)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("CancelBufMismatch");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    RSSurfaceOhosVulkan rsSurface(pSurface);
    NativeWindowBuffer dummyKey;
    NativeWindowBuffer otherBuffer;
    rsSurface.flushState_.valid = true;
    rsSurface.flushState_.bufferKey = &dummyKey;
    rsSurface.CancelBuffer(&otherBuffer);
    EXPECT_TRUE(rsSurface.flushState_.valid);
    EXPECT_EQ(rsSurface.flushState_.bufferKey, &dummyKey);
    rsSurface.flushState_.Reset();
}

// ============================================================
// CancelBufferForCurrentFrame flushState_ reset test
// ============================================================

/**
 * @tc.name: CancelBufferForCurrentFrame_ResetsFlushState
 * @tc.desc: CancelBufferForCurrentFrame calls flushState_.Reset()
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, CancelBufferForCurrentFrame_ResetsFlushState, TestSize.Level2)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("CancelCurFlush");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    RSSurfaceOhosVulkan rsSurface(pSurface);
    rsSurface.flushState_.valid = true;
    rsSurface.flushState_.bufferKey = reinterpret_cast<NativeWindowBuffer*>(0x1);
    rsSurface.CancelBufferForCurrentFrame();
    EXPECT_FALSE(rsSurface.flushState_.valid);
    EXPECT_EQ(rsSurface.flushState_.bufferKey, nullptr);
}

// ============================================================
// SetColorSpace / SetSurfacePixelFormat flushState_ reset tests
// ============================================================

/**
 * @tc.name: SetColorSpace_ResetsFlushState
 * @tc.desc: SetColorSpace with different gamut resets flushState_
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SetColorSpace_ResetsFlushState, TestSize.Level2)
{
    RSSurfaceOhosVulkan rsSurface(IConsumerSurface::Create());
    rsSurface.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(rsSurface.colorSpace_, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    rsSurface.flushState_.valid = true;
    rsSurface.flushState_.bufferKey = reinterpret_cast<NativeWindowBuffer*>(0x1);
    rsSurface.SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_ADOBE_RGB);
    EXPECT_FALSE(rsSurface.flushState_.valid);
    EXPECT_EQ(rsSurface.flushState_.bufferKey, nullptr);
}

/**
 * @tc.name: SetSurfacePixelFormat_ResetsFlushState
 * @tc.desc: SetSurfacePixelFormat with different format resets flushState_
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, SetSurfacePixelFormat_ResetsFlushState, TestSize.Level2)
{
    RSSurfaceOhosVulkan rsSurface(IConsumerSurface::Create());
    constexpr int32_t initialFormat = 1;
    constexpr int32_t newFormat = 2;
    rsSurface.SetSurfacePixelFormat(initialFormat);
    ASSERT_EQ(rsSurface.pixelFormat_, initialFormat);
    rsSurface.flushState_.valid = true;
    rsSurface.flushState_.bufferKey = reinterpret_cast<NativeWindowBuffer*>(0x1);
    rsSurface.SetSurfacePixelFormat(newFormat);
    EXPECT_FALSE(rsSurface.flushState_.valid);
    EXPECT_EQ(rsSurface.flushState_.bufferKey, nullptr);
    EXPECT_EQ(rsSurface.pixelFormat_, newFormat);
}

// ============================================================
// FlushGpu branch coverage tests
// ============================================================

/**
 * @tc.name: FlushGpu_AlreadyActiveFlush
 * @tc.desc: FlushGpu returns false when previous flush state is still active
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, FlushGpu_AlreadyActiveFlush, TestSize.Level2)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("FlushGpuActive");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    RSSurfaceOhosVulkan rsSurface(pSurface);
    rsSurface.flushState_.valid = true;
    std::unique_ptr<RSSurfaceFrame> frame;
    EXPECT_FALSE(rsSurface.FlushGpu(frame));
}

/**
 * @tc.name: FlushGpu_BufferNotInSurfaceMap
 * @tc.desc: FlushGpu returns false when front buffer not in mSurfaceMap
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, FlushGpu_BufferNotInSurfaceMap, TestSize.Level2)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("FlushGpuNoMap");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    RSSurfaceOhosVulkan rsSurface(pSurface);
    NativeWindowBuffer dummyBuffer;
    rsSurface.mSurfaceList.emplace_back(&dummyBuffer);
    std::unique_ptr<RSSurfaceFrame> frame;
    EXPECT_FALSE(rsSurface.FlushGpu(frame));
    EXPECT_FALSE(rsSurface.flushState_.valid);
}

// ============================================================
// CancelActiveFlush with valid flushState_ test
// ============================================================

/**
 * @tc.name: CancelActiveFlush_WithValidState
 * @tc.desc: CancelActiveFlush calls CancelBuffer when flushState is valid
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, CancelActiveFlush_WithValidState, TestSize.Level2)
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("CancelActiveOk");
    ASSERT_NE(cSurface, nullptr);
    auto pSurface = Surface::CreateSurfaceAsProducer(cSurface->GetProducer());
    RSSurfaceOhosVulkan rsSurface(pSurface);
    NativeWindowBuffer dummyBuffer;
    rsSurface.flushState_.valid = true;
    rsSurface.flushState_.bufferKey = &dummyBuffer;
    rsSurface.CancelActiveFlush();
    EXPECT_FALSE(rsSurface.flushState_.valid);
}

/**
 * @tc.name: DupReservedFlushFd_NoFd
 * @tc.desc: DupReservedFlushFd returns -1 when mReservedFlushFd is -1
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceOhosVulkanTest, DupReservedFlushFd_NoFd, TestSize.Level2)
{
    sptr<Surface> producer = nullptr;
    RSSurfaceOhosVulkan rsSurface(producer);
    rsSurface.mReservedFlushFd = -1;
    EXPECT_EQ(rsSurface.DupReservedFlushFd(), -1);
}
} // namespace Rosen
} // namespace OHOS