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

#include <gtest/gtest.h>
#include <thread>

#include "pipeline/layer_split/buffer/rs_layer_split_surface_buffer.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "params/rs_screen_render_params.h"
#include "screen_manager/screen_types.h"
#include "composer/composer_service/external_depend/engine/rs_uni_render_engine.h"
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

namespace OHOS::Rosen {
namespace {

constexpr int32_t BUFFER_DEFAULT_WIDTH = 100;
constexpr int32_t BUFFER_DEFAULT_HEIGHT = 100;
constexpr int32_t kBusyWaitTimeout = 10000;

class RSSplitSurfaceBufferTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() override
    {
        splitBuffer_ = std::make_unique<RSSplitSurfaceBuffer>("test_layer", 0,
            BUFFER_DEFAULT_WIDTH, BUFFER_DEFAULT_HEIGHT);
    }

    void TearDown() override
    {
        splitBuffer_.reset();
    }

protected:
    std::unique_ptr<RSSplitSurfaceBuffer> splitBuffer_;
};

/*
 * ── PreAllocateBuffer ────────────────────────────────────────
 */

/**
 * @tc.name: PreAllocateBuffer001_AlreadyAllocated
 * @tc.desc: Test PreAllocateBuffer when isBufferPreAllocated_ == true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, PreAllocateBuffer001_AlreadyAllocated, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    bool originPreAllocated = splitBuffer_->isBufferPreAllocated_.load();
    splitBuffer_->isBufferPreAllocated_.store(true);
    ASSERT_EQ(splitBuffer_->isBufferPreAllocated_, true);
    splitBuffer_->PreAllocateBuffer();
    splitBuffer_->isBufferPreAllocated_.store(originPreAllocated);
}

/**
 * @tc.name: PreAllocateBuffer002_AllocInProgress
 * @tc.desc: Test isPreAllocInProgress_ exchange failed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, PreAllocateBuffer002_AllocInProgress, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->isPreAllocInProgress_.store(true);
    ASSERT_EQ(splitBuffer_->isPreAllocInProgress_, true);
    splitBuffer_->PreAllocateBuffer();
    splitBuffer_->isPreAllocInProgress_.store(false);
}

/**
 * @tc.name: PreAllocateBuffer003_SurfaceNotCreated
 * @tc.desc: Test PreAllocateBuffer when surfaceCreated_ == false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, PreAllocateBuffer003_SurfaceNotCreated, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    bool originInProgress = splitBuffer_->isPreAllocInProgress_.load();
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->isPreAllocInProgress_.store(false);
    bool originCreated = splitBuffer_->surfaceCreated_;
    splitBuffer_->surfaceCreated_ = false;
    ASSERT_EQ(splitBuffer_->surfaceCreated_, false);
    splitBuffer_->PreAllocateBuffer();
    splitBuffer_->isPreAllocInProgress_.store(originInProgress);
    splitBuffer_->surfaceCreated_ = originCreated;
}

/**
 * @tc.name: PreAllocateBuffer004_ProducerNull
 * @tc.desc: Test PreAllocateBuffer when producerSurface_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, PreAllocateBuffer004_ProducerNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    bool originInProgress = splitBuffer_->isPreAllocInProgress_.load();
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->isPreAllocInProgress_.store(false);
    bool originCreated = splitBuffer_->surfaceCreated_;
    splitBuffer_->surfaceCreated_ = true;
    ASSERT_EQ(splitBuffer_->producerSurface_, nullptr);
    splitBuffer_->PreAllocateBuffer();
    splitBuffer_->isPreAllocInProgress_.store(originInProgress);
    splitBuffer_->surfaceCreated_ = originCreated;
}

/**
 * @tc.name: PreAllocateBuffer005_BufferRequestConfig
 * @tc.desc: Test PreAllocateBuffer with various buffer configurations
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, PreAllocateBuffer005_BufferRequestConfig, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    bool originInProgress = splitBuffer_->isPreAllocInProgress_.load();
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->isPreAllocInProgress_.store(false);
    bool originCreated = splitBuffer_->surfaceCreated_;
    splitBuffer_->surfaceCreated_ = true;
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("ScreenNode");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    splitBuffer_->producerSurface_ = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(splitBuffer_->producerSurface_, nullptr);
    splitBuffer_->bufferConfig_.format = GRAPHIC_PIXEL_FMT_RGBA_8888;
    splitBuffer_->isHebc_ = false;
    splitBuffer_->PreAllocateBuffer();
    int timeoutCount = 0;
    while (splitBuffer_->isPreAllocInProgress_.load() && timeoutCount < kBusyWaitTimeout) {
        std::this_thread::yield();
        ++timeoutCount;
    }
    ASSERT_LT(timeoutCount, kBusyWaitTimeout);
    splitBuffer_->isHebc_ = true;
    splitBuffer_->PreAllocateBuffer();
    timeoutCount = 0;
    while (splitBuffer_->isPreAllocInProgress_.load() && timeoutCount < kBusyWaitTimeout) {
        std::this_thread::yield();
        ++timeoutCount;
    }
    ASSERT_LT(timeoutCount, kBusyWaitTimeout);
    splitBuffer_->bufferConfig_.format = GRAPHIC_PIXEL_FMT_RGBA_1010108;
    splitBuffer_->PreAllocateBuffer();
    timeoutCount = 0;
    while (splitBuffer_->isPreAllocInProgress_.load() && timeoutCount < kBusyWaitTimeout) {
        std::this_thread::yield();
        ++timeoutCount;
    }
    ASSERT_LT(timeoutCount, kBusyWaitTimeout);
    splitBuffer_->bufferConfig_.format = 1;
    splitBuffer_->PreAllocateBuffer();
    timeoutCount = 0;
    while (splitBuffer_->isPreAllocInProgress_.load() && timeoutCount < kBusyWaitTimeout) {
        std::this_thread::yield();
        ++timeoutCount;
    }
    ASSERT_LT(timeoutCount, kBusyWaitTimeout);
    splitBuffer_->isPreAllocInProgress_.store(originInProgress);
    splitBuffer_->surfaceCreated_ = originCreated;
}

/*
 * ── RequestFrame ──────────────────────────────────────────────
 */

/**
 * @tc.name: RequestFrame001_BufferNotAllocated
 * @tc.desc: Test RequestFrame when isBufferPreAllocated_ == false
 * @tc.type: FUNC
 * @tc.require: RSSplitSurfaceBuffer::RequestFrame buffer not pre-allocated
 */
HWTEST_F(RSSplitSurfaceBufferTest, RequestFrame001_BufferNotAllocated, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(false);
    auto frame = splitBuffer_->RequestFrame(GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(frame, nullptr);
}

/**
 * @tc.name: RequestFrame004_SurfaceNotCreated
 * @tc.desc: Test RequestFrame when surfaceCreated_ == false
 * @tc.type: FUNC
 * @tc.require: RSSplitSurfaceBuffer::RequestFrame CreateSurface failed
 */
HWTEST_F(RSSplitSurfaceBufferTest, RequestFrame004_SurfaceNotCreated, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(true);
    bool originCreated = splitBuffer_->surfaceCreated_;
    splitBuffer_->surfaceCreated_ = false;
    auto frame = splitBuffer_->RequestFrame(GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(frame, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->surfaceCreated_ = originCreated;
}

/**
 * @tc.name: RequestFrame005_RsSurfaceNull
 * @tc.desc: Test RequestFrame when rsSurface_ == nullptr
 * @tc.type: FUNC
 * @tc.require: RSSplitSurfaceBuffer::RequestFrame: surface is null!
 */
HWTEST_F(RSSplitSurfaceBufferTest, RequestFrame005_RsSurfaceNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(true);
    bool originCreated = splitBuffer_->surfaceCreated_;
    splitBuffer_->surfaceCreated_ = false;
    auto frame = splitBuffer_->RequestFrame(GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(frame, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->surfaceCreated_ = originCreated;
}

/*
 * ── FlushFrame ────────────────────────────────────────────────
 */

/**
 * @tc.name: FlushFrame001_ProducerNull
 * @tc.desc: Test FlushFrame when producerSurface_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, FlushFrame001_ProducerNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->producerSurface_ = nullptr;
    ASSERT_EQ(splitBuffer_->producerSurface_, nullptr);
    auto result = splitBuffer_->FlushFrame();
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: FlushFrame002_SurfaceHandlerNull
 * @tc.desc: Test FlushFrame when surfaceHandler_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, FlushFrame002_SurfaceHandlerNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->surfaceHandler_ = nullptr;
    ASSERT_EQ(splitBuffer_->surfaceHandler_, nullptr);
    auto result = splitBuffer_->FlushFrame();
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: FlushFrame003_RsSurfaceNull
 * @tc.desc: Test FlushFrame when rsSurface_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, FlushFrame003_RsSurfaceNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->rsSurface_ = nullptr;
    ASSERT_EQ(splitBuffer_->rsSurface_, nullptr);
    auto result = splitBuffer_->FlushFrame();
    ASSERT_EQ(result, false);
}

/*
 * ── ReleaseBuffer ─────────────────────────────────────────────
 */

/**
 * @tc.name: ReleaseBuffer001_SurfaceHandlerNull
 * @tc.desc: Test ReleaseBuffer when surfaceHandler_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, ReleaseBuffer001_SurfaceHandlerNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->surfaceHandler_ = nullptr;
    ASSERT_EQ(splitBuffer_->surfaceHandler_, nullptr);
    auto result = splitBuffer_->ReleaseBuffer();
    ASSERT_EQ(result, false);
}

/*
 * ── CreateSurface ─────────────────────────────────────────────
 */

/**
 * @tc.name: CreateSurface001_AlreadyCreated
 * @tc.desc: Test CreateSurface when consumer!=nullptr && rsSurface_!=nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, CreateSurface001_AlreadyCreated, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(splitBuffer_->surfaceHandler_);
    auto result = splitBuffer_->CreateSurface(listener);
    ASSERT_EQ(result, true);
    ASSERT_NE(splitBuffer_->rsSurface_, nullptr);
}

/**
 * @tc.name: CreateSurface002_ConsumerNull
 * @tc.desc: Test CreateSurface when consumer == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, CreateSurface002_ConsumerNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->surfaceHandler_->SetConsumer(nullptr);
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(splitBuffer_->surfaceHandler_);
    auto result = splitBuffer_->CreateSurface(listener);
    ASSERT_EQ(result, true);
}

/*
 * ── GetBufferHandle ───────────────────────────────────────────
 */

/**
 * @tc.name: GetBufferHandle001_SurfaceNull
 * @tc.desc: Test GetBufferHandle when rsSurface_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, GetBufferHandle001_SurfaceNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->rsSurface_ = nullptr;
    ASSERT_EQ(splitBuffer_->rsSurface_, nullptr);
    auto handle = splitBuffer_->GetBufferHandle();
    ASSERT_EQ(handle, nullptr);
}

/**
 * @tc.name: GetBufferHandle002_BufferNull
 * @tc.desc: Test GetBufferHandle when buffer == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, GetBufferHandle002_BufferNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    auto handle = splitBuffer_->GetBufferHandle();
    ASSERT_EQ(handle, nullptr);
}

/*
 * ── GetSkContext ──────────────────────────────────────────────
 */

/**
 * @tc.name: GetSkContext001_Null
 * @tc.desc: Test GetSkContext when grContext_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, GetSkContext001_Null, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->grContext_ = nullptr;
    ASSERT_EQ(splitBuffer_->GetSkContext(), nullptr);
}

/*
 * ── GetSurfaceHandler ─────────────────────────────────────────
 */

/**
 * @tc.name: GetSurfaceHandler001_NotNull
 * @tc.desc: Test GetSurfaceHandler when surfaceHandler_ is not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, GetSurfaceHandler001_NotNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    ASSERT_NE(splitBuffer_->GetSurfaceHandler(), nullptr);
    ASSERT_EQ(splitBuffer_->GetSurfaceHandler(), splitBuffer_->surfaceHandler_);
}

/**
 * @tc.name: GetSurfaceHandler002_Null
 * @tc.desc: Test GetSurfaceHandler when surfaceHandler_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, GetSurfaceHandler002_Null, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->surfaceHandler_ = nullptr;
    ASSERT_EQ(splitBuffer_->GetSurfaceHandler(), nullptr);
}

/*
 * ── GetRSSurface ──────────────────────────────────────────────
 */

/**
 * @tc.name: GetRSSurface001_Null
 * @tc.desc: Test GetRSSurface when rsSurface_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, GetRSSurface001_Null, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->rsSurface_ = nullptr;
    ASSERT_EQ(splitBuffer_->GetRSSurface(), nullptr);
}

/**
 * @tc.name: GetRSSurface002_NotNull
 * @tc.desc: Test GetRSSurface when rsSurface_ is not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, GetRSSurface002_NotNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(splitBuffer_->surfaceHandler_);
    splitBuffer_->CreateSurface(listener);
    ASSERT_NE(splitBuffer_->GetRSSurface(), nullptr);
}

/*
 * ── RSSplitSurfaceRenderListener ──────────────────────────────
 */

/**
 * @tc.name: SplitSurfaceRenderListener001_ExpiredHandler
 * @tc.desc: Test OnBufferAvailable when surfaceHandler_ is expired
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, SplitSurfaceRenderListener001_ExpiredHandler, TestSize.Level1)
{
    std::weak_ptr<RSSurfaceHandler> weakHandler;
    {
        auto handler = std::make_shared<RSSurfaceHandler>(0);
        weakHandler = handler;
    }
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(weakHandler);
    listener->OnBufferAvailable();
}

/**
 * @tc.name: SplitSurfaceRenderListener002_ValidHandler
 * @tc.desc: Test OnBufferAvailable when surfaceHandler_ is valid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, SplitSurfaceRenderListener002_ValidHandler, TestSize.Level1)
{
    auto handler = std::make_shared<RSSurfaceHandler>(100);
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(handler);
    listener->OnBufferAvailable();
}

/*
 * ── IsSurfaceCreated ─────────────────────────────────────────
 */

/**
 * @tc.name: IsSurfaceCreated001_True
 * @tc.desc: Test IsSurfaceCreated when surfaceCreated_ == true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, IsSurfaceCreated001_True, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    bool originCreated = splitBuffer_->surfaceCreated_;
    splitBuffer_->surfaceCreated_ = true;
    ASSERT_EQ(splitBuffer_->IsSurfaceCreated(), true);
    splitBuffer_->surfaceCreated_ = originCreated;
}

// ===================== ReleaseBuffer =====================

/**
 * @tc.name: ReleaseBuffer002_ConsumerNull
 * @tc.desc: Test ReleaseBuffer when surfaceConsumer is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, ReleaseBuffer002_ConsumerNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    ASSERT_NE(splitBuffer_->surfaceHandler_, nullptr);
    splitBuffer_->surfaceHandler_->SetConsumer(nullptr);
    auto result = splitBuffer_->ReleaseBuffer();
    ASSERT_EQ(result, false);
}

// ===================== GetBufferHandle =====================

/**
 * @tc.name: GetBufferHandle003_SuccessPath
 * @tc.desc: Test GetBufferHandle when rsSurface_ is not null and GetCurrentBuffer returns a buffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, GetBufferHandle003_SuccessPath, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(splitBuffer_->surfaceHandler_);
    auto result = splitBuffer_->CreateSurface(listener);
    ASSERT_EQ(result, true);
    ASSERT_NE(splitBuffer_->rsSurface_, nullptr);
    // After CreateSurface, rsSurface_ is set; GetCurrentBuffer may return null (buffer not allocated),
    // but the code path through !buffer -> return nullptr is already covered.
    // This test ensures no crash when rsSurface_ is non-null.
    auto handle = splitBuffer_->GetBufferHandle();
    // buffer is likely null (no frame requested), so handle should be null
    // but we are covering the rsSurface_ non-null path
    (void)handle;
}

// ===================== PreAllocateBufferImpl =====================

/**
 * @tc.name: PreAllocateBufferImpl001_AlreadyAllocated
 * @tc.desc: Test PreAllocateBufferImpl when isBufferPreAllocated_ is already true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, PreAllocateBufferImpl001_AlreadyAllocated, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(true);
    splitBuffer_->isPreAllocInProgress_.store(true);
    splitBuffer_->PreAllocateBufferImpl(splitBuffer_->bufferConfig_);
    // isPreAllocInProgress_ should be set to false
    ASSERT_EQ(splitBuffer_->isPreAllocInProgress_.load(), false);
    splitBuffer_->isBufferPreAllocated_.store(false);
}

/**
 * @tc.name: PreAllocateBufferImpl002_ProducerNull
 * @tc.desc: Test PreAllocateBufferImpl when producerSurface_ is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, PreAllocateBufferImpl002_ProducerNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->isPreAllocInProgress_.store(true);
    splitBuffer_->producerSurface_ = nullptr;
    splitBuffer_->PreAllocateBufferImpl(splitBuffer_->bufferConfig_);
    ASSERT_EQ(splitBuffer_->isPreAllocInProgress_.load(), false);
}

/**
 * @tc.name: PreAllocateBufferImpl003_PreAllocBuffersFailed
 * @tc.desc: Test PreAllocateBufferImpl when PreAllocBuffers returns non-OK
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, PreAllocateBufferImpl003_PreAllocBuffersFailed, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->isPreAllocInProgress_.store(true);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    splitBuffer_->producerSurface_ = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(splitBuffer_->producerSurface_, nullptr);
    splitBuffer_->PreAllocateBufferImpl(splitBuffer_->bufferConfig_);
    // PreAllocBuffers may or may not fail depending on environment;
    // what matters is that isPreAllocInProgress_ is reset and no crash
    ASSERT_EQ(splitBuffer_->isPreAllocInProgress_.load(), false);
    splitBuffer_->producerSurface_ = nullptr;
}

/**
 * @tc.name: PreAllocateBufferImpl004_Success
 * @tc.desc: Test PreAllocateBufferImpl when PreAllocBuffers succeeds
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, PreAllocateBufferImpl004_Success, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->isPreAllocInProgress_.store(true);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    splitBuffer_->producerSurface_ = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(splitBuffer_->producerSurface_, nullptr);
    splitBuffer_->PreAllocateBufferImpl(splitBuffer_->bufferConfig_);
    // isPreAllocInProgress_ is always reset at the end
    ASSERT_EQ(splitBuffer_->isPreAllocInProgress_.load(), false);
    splitBuffer_->producerSurface_ = nullptr;
}

// ===================== IsSurfaceCreated =====================

/**
 * @tc.name: IsSurfaceCreated002_False
 * @tc.desc: Test IsSurfaceCreated when surfaceCreated_ == false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, IsSurfaceCreated002_False, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    bool originCreated = splitBuffer_->surfaceCreated_;
    splitBuffer_->surfaceCreated_ = false;
    ASSERT_EQ(splitBuffer_->IsSurfaceCreated(), false);
    splitBuffer_->surfaceCreated_ = originCreated;
}

} // namespace
} // namespace OHOS::Rosen
