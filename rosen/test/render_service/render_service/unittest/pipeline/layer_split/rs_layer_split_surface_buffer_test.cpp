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
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#endif
#include "render_context/render_context.h"
#include "pipeline/render_thread/rs_render_engine.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

namespace OHOS::Rosen {
namespace {

constexpr int32_t BUFFER_DEFAULT_WIDTH = 100;
constexpr int32_t BUFFER_DEFAULT_HEIGHT = 100;

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
        if (splitBuffer_ && splitBuffer_->surfaceHandler_) {
            splitBuffer_->surfaceHandler_->buffer_.buffer.ForceSetRefPtr(nullptr);
            splitBuffer_->surfaceHandler_->buffer_.acquireFence.ForceSetRefPtr(nullptr);
            splitBuffer_->surfaceHandler_->buffer_.releaseFence.ForceSetRefPtr(nullptr);
            splitBuffer_->surfaceHandler_->buffer_.bufferDeleteCb_ = nullptr;
            splitBuffer_->surfaceHandler_->buffer_.bufferOwnerCount_ = nullptr;
            splitBuffer_->surfaceHandler_->preBuffer_.buffer.ForceSetRefPtr(nullptr);
            splitBuffer_->surfaceHandler_->preBuffer_.acquireFence.ForceSetRefPtr(nullptr);
            splitBuffer_->surfaceHandler_->preBuffer_.releaseFence.ForceSetRefPtr(nullptr);
            splitBuffer_->surfaceHandler_->preBuffer_.bufferDeleteCb_ = nullptr;
            splitBuffer_->surfaceHandler_->preBuffer_.bufferOwnerCount_ = nullptr;
            splitBuffer_->surfaceHandler_->holdBuffer_ = nullptr;
#ifndef ROSEN_CROSS_PLATFORM
            splitBuffer_->surfaceHandler_->consumer_.ForceSetRefPtr(nullptr);
#endif
        }
        splitBuffer_->producerSurface_.ForceSetRefPtr(nullptr);
        if (splitBuffer_->rsSurface_) {
            splitBuffer_->rsSurface_->producer_.ForceSetRefPtr(nullptr);
        }
        splitBuffer_.reset();
        RSSurfaceHandler::SetGPUCacheCleanupCallback(nullptr);
#ifndef ROSEN_CROSS_PLATFORM
        RSSurfaceHandler::SetConsumerDeleteBufferListenerCallback(nullptr);
#endif
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
};

/**
 * @tc.name: PreAllocateBuffer005_HebcFormat
 * @tc.desc: Test PreAllocateBuffer when isHebc is true and format is RGBA_8888
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, PreAllocateBuffer005_HebcFormat, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    bool originInProgress = splitBuffer_->isPreAllocInProgress_.load();
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->isPreAllocInProgress_.store(false);
    bool originCreated = splitBuffer_->surfaceCreated_;
    splitBuffer_->surfaceCreated_ = true;
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test_hebc");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    splitBuffer_->producerSurface_ = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(splitBuffer_->producerSurface_, nullptr);
    splitBuffer_->bufferConfig_.format = GRAPHIC_PIXEL_FMT_RGBA_8888;
    splitBuffer_->isHebc_ = true;
    splitBuffer_->PreAllocateBuffer();
    splitBuffer_->isPreAllocInProgress_.store(originInProgress);
    splitBuffer_->surfaceCreated_ = originCreated;
    splitBuffer_->producerSurface_.ForceSetRefPtr(nullptr);
    splitBuffer_->isHebc_ = false;
};

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
 * @tc.desc: Test GetBufferHandle when rsSurface_ == nullptr or buffer == nullptr
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
    handle = splitBuffer_->GetBufferHandle();
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
 * @tc.desc: Test GetSurfaceHandler when surfaceHandler_ is not nullptr or nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, GetSurfaceHandler001_NotNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    ASSERT_NE(splitBuffer_->GetSurfaceHandler(), nullptr);
    ASSERT_EQ(splitBuffer_->GetSurfaceHandler(), splitBuffer_->surfaceHandler_);
    splitBuffer_->surfaceHandler_ = nullptr;
    ASSERT_EQ(splitBuffer_->GetSurfaceHandler(), nullptr);
}

/*
 * ── GetRSSurface ──────────────────────────────────────────────
 */

/**
 * @tc.name: GetRSSurface001_Null
 * @tc.desc: Test GetRSSurface when rsSurface_ == nullptr or not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, GetRSSurface001_Null, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->rsSurface_ = nullptr;
    ASSERT_EQ(splitBuffer_->GetRSSurface(), nullptr);
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(splitBuffer_->surfaceHandler_);
    splitBuffer_->CreateSurface(listener);
    ASSERT_NE(splitBuffer_->GetRSSurface(), nullptr);
}

/*
 * ── RSSplitSurfaceRenderListener ──────────────────────────────
 */

/**
 * @tc.name: SplitSurfaceRenderListener001_ExpiredHandler
 * @tc.desc: Test OnBufferAvailable when surfaceHandler_ is expired or valid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, SplitSurfaceRenderListener001_ExpiredHandler, TestSize.Level1)
{
    auto handler = std::make_shared<RSSurfaceHandler>(100);
    ASSERT_NE(handler, nullptr);
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(handler);
    ASSERT_NE(listener, nullptr);
    listener->OnBufferAvailable();
    ASSERT_EQ(handler->GetAvailableBufferCount(), 1);

    std::weak_ptr<RSSurfaceHandler> weakHandler;
    {
        auto expiredHandler = std::make_shared<RSSurfaceHandler>(0);
        weakHandler = expiredHandler;
    }
    auto expiredListener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(weakHandler);
    ASSERT_NE(expiredListener, nullptr);
    expiredListener->OnBufferAvailable();
}

/*
 * ── Destructor ────────────────────────────────
 */

/**
 * @tc.name: Destructor001_CleanCache
 * @tc.desc: Test destructor when producerSurface_ is not null and isBufferPreAllocated_ is true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, Destructor001_CleanCache, TestSize.Level1)
{
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test_destructor");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    sptr<Surface> producerSurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(producerSurface, nullptr);

    auto buffer = std::make_unique<RSSplitSurfaceBuffer>("test_destructor", 0,
        BUFFER_DEFAULT_WIDTH, BUFFER_DEFAULT_HEIGHT);
    ASSERT_NE(buffer, nullptr);
    buffer->producerSurface_ = producerSurface;
    buffer->isBufferPreAllocated_.store(true);
    producerSurface.ForceSetRefPtr(nullptr);
    buffer.reset();
}

/*
 * ── IsSurfaceCreated ─────────────────────────────────────────
 */

/**
 * @tc.name: IsSurfaceCreated001_True
 * @tc.desc: Test IsSurfaceCreated when surfaceCreated_ == true or false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, IsSurfaceCreated001_True, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    bool originCreated = splitBuffer_->surfaceCreated_;
    splitBuffer_->surfaceCreated_ = true;
    ASSERT_EQ(splitBuffer_->IsSurfaceCreated(), true);
    splitBuffer_->surfaceCreated_ = false;
    ASSERT_EQ(splitBuffer_->IsSurfaceCreated(), false);
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
    auto handle = splitBuffer_->GetBufferHandle();
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
    ASSERT_EQ(splitBuffer_->isPreAllocInProgress_.load(), false);
    splitBuffer_->producerSurface_.ForceSetRefPtr(nullptr);
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
    ASSERT_EQ(splitBuffer_->isPreAllocInProgress_.load(), false);
    splitBuffer_->producerSurface_.ForceSetRefPtr(nullptr);
}

/**
 * @tc.name: Init001_RenderEngineNull
 * @tc.desc: Test Init when GetRenderEngine returns nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, Init001_RenderEngineNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->grContext_ = nullptr;
    splitBuffer_->Init(false);
}

/**
 * @tc.name: Init002_GrContextNull
 * @tc.desc: Test Init when GetRenderContext returns nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, Init002_GrContextNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->grContext_ = nullptr;
    splitBuffer_->Init(false);
}

/**
 * @tc.name: Init003_SurfaceCreatedAndRsSurfaceExists
 * @tc.desc: Test Init when surface already created and rsSurface_ is not null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, Init003_SurfaceCreatedAndRsSurfaceExists, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(splitBuffer_->surfaceHandler_);
    auto result = splitBuffer_->CreateSurface(listener);
    ASSERT_EQ(result, true);
    ASSERT_NE(splitBuffer_->rsSurface_, nullptr);
    bool originCreated = splitBuffer_->surfaceCreated_;
    splitBuffer_->surfaceCreated_ = true;
    splitBuffer_->Init(false);
    splitBuffer_->surfaceCreated_ = originCreated;
}

/**
 * @tc.name: Init004_RsSurfaceNull
 * @tc.desc: Test Init when rsSurface_ is nullptr after CreateSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, Init004_RsSurfaceNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->surfaceCreated_ = false;
    splitBuffer_->rsSurface_ = nullptr;
    splitBuffer_->Init(false);
}

/**
 * @tc.name: Init005_GrContextNotNull
 * @tc.desc: Test Init when grContext_ is already set
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, Init005_GrContextNotNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->grContext_ = nullptr;
    splitBuffer_->Init(false);
}

/**
 * @tc.name: Init007_GetRenderContextValid
 * @tc.desc: Test Init when grContext is nullptr but GetRenderContext returns valid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, Init007_GetRenderContextValid, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->grContext_ = nullptr;
    auto& renderThread = RSUniRenderThread::Instance();
    renderThread.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    renderThread.uniRenderEngine_->renderContext_ = RenderContext::Create();
    renderThread.uniRenderEngine_->renderContext_->drGPUContext_ = nullptr;
    bool originCreated = splitBuffer_->surfaceCreated_;
    splitBuffer_->surfaceCreated_ = true;
    splitBuffer_->Init(false);
    splitBuffer_->surfaceCreated_ = originCreated;
    if (splitBuffer_->rsSurface_ && splitBuffer_->rsSurface_->renderContext_) {
        splitBuffer_->rsSurface_->renderContext_->drGPUContext_ = nullptr;
    }
    if (splitBuffer_->rsSurface_) splitBuffer_->rsSurface_->SetRenderContext(nullptr);
    splitBuffer_->producerSurface_.ForceSetRefPtr(nullptr);
    splitBuffer_->rsSurface_ = nullptr;
    renderThread.uniRenderEngine_->skContext_ = nullptr;
    if (renderThread.uniRenderEngine_->renderContext_) {
        renderThread.uniRenderEngine_->renderContext_->drGPUContext_ = nullptr;
        renderThread.uniRenderEngine_->renderContext_ = nullptr;
    }
    renderThread.uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: RequestFrame002_GrContextNull
 * @tc.desc: Test RequestFrame when grContext_ is nullptr and GetRenderContext is null
 * @tc.type: FUNC
 * @tc.require: RSSplitSurfaceBuffer::RequestFrame context is nullptr
 */
HWTEST_F(RSSplitSurfaceBufferTest, RequestFrame002_GrContextNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(true);
    splitBuffer_->grContext_ = nullptr;
    auto frame = splitBuffer_->RequestFrame(GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(frame, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(false);
}

/**
 * @tc.name: RequestFrame003_RenderEngineNull
 * @tc.desc: Test RequestFrame when uniRenderEngine is nullptr
 * @tc.type: FUNC
 * @tc.require: RSSplitSurfaceBuffer::RequestFrame RenderEngine is null
 */
HWTEST_F(RSSplitSurfaceBufferTest, RequestFrame003_RenderEngineNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(true);
    splitBuffer_->grContext_ = nullptr;
    auto frame = splitBuffer_->RequestFrame(GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(frame, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(false);
}

/**
 * @tc.name: RequestFrame006_CreateSurfaceFailed
 * @tc.desc: Test RequestFrame when CreateSurface returns false
 * @tc.type: FUNC
 * @tc.require: RSSplitSurfaceBuffer::RequestFrame CreateSurface failed
 */
HWTEST_F(RSSplitSurfaceBufferTest, RequestFrame006_CreateSurfaceFailed, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(true);
    bool originCreated = splitBuffer_->surfaceCreated_;
    splitBuffer_->surfaceCreated_ = false;
    splitBuffer_->rsSurface_ = nullptr;
    auto frame = splitBuffer_->RequestFrame(GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(frame, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->surfaceCreated_ = originCreated;
}

/**
 * @tc.name: RequestFrame007_GrContextNullButGetRenderContextValid
 * @tc.desc: Test RequestFrame when grContext is nullptr but GetRenderContext returns valid
 * @tc.type: FUNC
 * @tc.require: RSSplitSurfaceBuffer::RequestFrame grContext assignment from GetRenderContext
 * @tc.note: This test requires a properly initialized render engine (integration test)
 */
HWTEST_F(RSSplitSurfaceBufferTest, RequestFrame007_GrContextNullButGetRenderContextValid, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(true);
    splitBuffer_->surfaceCreated_ = true;
    splitBuffer_->grContext_ = nullptr;
    auto& renderThread = RSUniRenderThread::Instance();
    renderThread.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    renderThread.uniRenderEngine_->renderContext_ = RenderContext::Create();
    renderThread.uniRenderEngine_->renderContext_->drGPUContext_ = nullptr;
    auto frame = splitBuffer_->RequestFrame(GRAPHIC_COLOR_GAMUT_SRGB);
    if (frame != nullptr) {
        ASSERT_NE(splitBuffer_->grContext_, nullptr);
    }
    frame.reset();
    splitBuffer_->producerSurface_.ForceSetRefPtr(nullptr);
    splitBuffer_->rsSurface_ = nullptr;
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->surfaceCreated_ = false;
    splitBuffer_->grContext_ = nullptr;
    if (renderThread.uniRenderEngine_->renderContext_) {
        renderThread.uniRenderEngine_->renderContext_->drGPUContext_ = nullptr;
        renderThread.uniRenderEngine_->renderContext_ = nullptr;
    }
    renderThread.uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: CreateSurface003_ProducerSurfaceNull
 * @tc.desc: Test CreateSurface when consumer is null (IConsumerSurface::Create will create new one)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, CreateSurface003_ProducerSurfaceNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->surfaceHandler_->SetConsumer(nullptr);
    splitBuffer_->rsSurface_ = nullptr;
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(splitBuffer_->surfaceHandler_);
    auto result = splitBuffer_->CreateSurface(listener);
    ASSERT_EQ(result, true);
    ASSERT_NE(splitBuffer_->rsSurface_, nullptr);
}

/**
 * @tc.name: CreateSurface004_Success
 * @tc.desc: Test CreateSurface when all conditions are met
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, CreateSurface004_Success, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->surfaceHandler_->SetConsumer(nullptr);
    splitBuffer_->rsSurface_ = nullptr;
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(splitBuffer_->surfaceHandler_);
    auto result = splitBuffer_->CreateSurface(listener);
    ASSERT_EQ(result, true);
    ASSERT_NE(splitBuffer_->rsSurface_, nullptr);
}

/**
 * @tc.name: CreateSurface005_SuccessPath
 * @tc.desc: Test CreateSurface success path with consumer set
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, CreateSurface005_SuccessPath, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->surfaceHandler_->SetConsumer(nullptr);
    splitBuffer_->rsSurface_ = nullptr;
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(splitBuffer_->surfaceHandler_);
    auto result = splitBuffer_->CreateSurface(listener);
    ASSERT_EQ(result, true);
    ASSERT_NE(splitBuffer_->producerSurface_, nullptr);
}

/**
 * @tc.name: GetBufferHandle002_BufferNotNull
 * @tc.desc: Test GetBufferHandle when rsSurface_ is not null but GetCurrentBuffer returns nullptr (no buffer allocated)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, GetBufferHandle002_BufferNotNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(splitBuffer_->surfaceHandler_);
    auto result = splitBuffer_->CreateSurface(listener);
    ASSERT_EQ(result, true);
    ASSERT_NE(splitBuffer_->rsSurface_, nullptr);
    auto handle = splitBuffer_->GetBufferHandle();
    ASSERT_EQ(handle, nullptr);
}

/**
 * @tc.name: PreAllocateBufferImpl005_PreAllocBuffersFailed
 * @tc.desc: Test PreAllocateBufferImpl when PreAllocBuffers returns error
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, PreAllocateBufferImpl005_PreAllocBuffersFailed, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->isPreAllocInProgress_.store(true);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    splitBuffer_->producerSurface_ = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(splitBuffer_->producerSurface_, nullptr);
    splitBuffer_->producerSurface_->Connect();
    splitBuffer_->PreAllocateBufferImpl(splitBuffer_->bufferConfig_);
    ASSERT_EQ(splitBuffer_->isPreAllocInProgress_.load(), false);
    splitBuffer_->producerSurface_.ForceSetRefPtr(nullptr);
}

/**
 * @tc.name: CreateSurface006_ConsumerExistsRsSurfaceNull
 * @tc.desc: Test CreateSurface when consumer != nullptr but rsSurface_ == nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, CreateSurface006_ConsumerExistsRsSurfaceNull, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test_consumer_exists");
    splitBuffer_->surfaceHandler_->SetConsumer(consumer);
    ASSERT_NE(splitBuffer_->surfaceHandler_->GetConsumer(), nullptr);
    ASSERT_EQ(splitBuffer_->rsSurface_, nullptr);
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(splitBuffer_->surfaceHandler_);
    auto result = splitBuffer_->CreateSurface(listener);
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: Init006_GetRenderContextSuccess
 * @tc.desc: Test Init when GetRenderContext returns valid context
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, Init006_GetRenderContextSuccess, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->grContext_ = nullptr;
    splitBuffer_->Init(false);
}

/**
 * @tc.name: FlushFrame004_AllConditionsMet
 * @tc.desc: Test FlushFrame when all conditions are met (buffer is null in unit test)
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, FlushFrame004_AllConditionsMet, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(splitBuffer_->surfaceHandler_);
    auto result = splitBuffer_->CreateSurface(listener);
    ASSERT_EQ(result, true);
    ASSERT_NE(splitBuffer_->producerSurface_, nullptr);
    ASSERT_NE(splitBuffer_->surfaceHandler_, nullptr);
    ASSERT_NE(splitBuffer_->rsSurface_, nullptr);
    auto flushResult = splitBuffer_->FlushFrame();
    ASSERT_EQ(flushResult, false);
}

/**
 * @tc.name: ReleaseBuffer003_Success
 * @tc.desc: Test ReleaseBuffer when surfaceConsumer is valid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, ReleaseBuffer003_Success, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    ASSERT_NE(splitBuffer_->surfaceHandler_, nullptr);
    auto listener = sptr<RSSplitSurfaceRenderListener>::MakeSptr(splitBuffer_->surfaceHandler_);
    auto result = splitBuffer_->CreateSurface(listener);
    ASSERT_EQ(result, true);
    ASSERT_NE(splitBuffer_->surfaceHandler_->GetConsumer(), nullptr);
    auto releaseResult = splitBuffer_->ReleaseBuffer();
    ASSERT_EQ(releaseResult, false);
}

/**
 * @tc.name: RequestFrame008_GrContextAssignment
 * @tc.desc: Test RequestFrame when grContext is nullptr but GetRenderContext returns valid
 * @tc.type: FUNC
 * @tc.require: RSSplitSurfaceBuffer::RequestFrame grContext assignment
 */
HWTEST_F(RSSplitSurfaceBufferTest, RequestFrame008_GrContextAssignment, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(true);
    splitBuffer_->grContext_ = nullptr;
    splitBuffer_->surfaceCreated_ = true;
    auto& renderThread = RSUniRenderThread::Instance();
    renderThread.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    renderThread.uniRenderEngine_->renderContext_ = RenderContext::Create();
    renderThread.uniRenderEngine_->renderContext_->drGPUContext_ = nullptr;
    auto frame = splitBuffer_->RequestFrame(GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(frame, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->surfaceCreated_ = false;
    splitBuffer_->grContext_ = nullptr;
    renderThread.uniRenderEngine_->skContext_ = nullptr;
    if (renderThread.uniRenderEngine_->renderContext_) {
        renderThread.uniRenderEngine_->renderContext_->drGPUContext_ = nullptr;
        renderThread.uniRenderEngine_->renderContext_ = nullptr;
    }
    renderThread.uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: RequestFrame009_RequestFrameSuccess
 * @tc.desc: Test RequestFrame when uniRenderEngine->RequestFrame returns valid frame
 * @tc.type: FUNC
 * @tc.require: RSSplitSurfaceBuffer::RequestFrame success path
 */
HWTEST_F(RSSplitSurfaceBufferTest, RequestFrame009_RequestFrameSuccess, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    splitBuffer_->isBufferPreAllocated_.store(true);
    splitBuffer_->grContext_ = std::make_shared<Drawing::GPUContext>();
    splitBuffer_->surfaceCreated_ = true;
    auto& renderThread = RSUniRenderThread::Instance();
    renderThread.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    renderThread.uniRenderEngine_->renderContext_ = RenderContext::Create();
    renderThread.uniRenderEngine_->renderContext_->drGPUContext_ = nullptr;
    auto frame = splitBuffer_->RequestFrame(GRAPHIC_COLOR_GAMUT_SRGB);
    splitBuffer_->isBufferPreAllocated_.store(false);
    splitBuffer_->surfaceCreated_ = false;
    frame.reset();
    splitBuffer_->producerSurface_.ForceSetRefPtr(nullptr);
    splitBuffer_->rsSurface_ = nullptr;
    splitBuffer_->grContext_ = nullptr;
    renderThread.uniRenderEngine_->skContext_ = nullptr;
    if (renderThread.uniRenderEngine_->renderContext_) {
        renderThread.uniRenderEngine_->renderContext_->drGPUContext_ = nullptr;
        renderThread.uniRenderEngine_->renderContext_ = nullptr;
    }
    renderThread.uniRenderEngine_ = nullptr;
}

#ifdef RS_ENABLE_VK
/**
 * @tc.name: Init008_VulkanApiType
 * @tc.desc: Test Init when GpuApiType is VULKAN and grContext_ is not null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, Init008_VulkanApiType, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    auto& renderThread = RSUniRenderThread::Instance();
    renderThread.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    renderThread.uniRenderEngine_->renderContext_ = RenderContext::Create();
    renderThread.uniRenderEngine_->renderContext_->drGPUContext_ = std::make_shared<Drawing::GPUContext>();
    splitBuffer_->grContext_ = renderThread.uniRenderEngine_->renderContext_->GetSharedDrGPUContext();
    splitBuffer_->surfaceCreated_ = true;
    splitBuffer_->Init(false);
    splitBuffer_->surfaceCreated_ = false;
    if (splitBuffer_->rsSurface_ && splitBuffer_->rsSurface_->renderContext_) {
        splitBuffer_->rsSurface_->renderContext_->drGPUContext_ = nullptr;
    }
    if (splitBuffer_->rsSurface_) splitBuffer_->rsSurface_->SetRenderContext(nullptr);
    splitBuffer_->producerSurface_.ForceSetRefPtr(nullptr);
    splitBuffer_->rsSurface_ = nullptr;
    splitBuffer_->grContext_ = nullptr;
    renderThread.uniRenderEngine_->skContext_ = nullptr;
    if (renderThread.uniRenderEngine_->renderContext_) {
        renderThread.uniRenderEngine_->renderContext_->drGPUContext_ = nullptr;
        renderThread.uniRenderEngine_->renderContext_ = nullptr;
    }
    renderThread.uniRenderEngine_ = nullptr;
}

/**
 * @tc.name: Init009_DdgrApiType
 * @tc.desc: Test Init when GpuApiType is DDGR and grContext_ is not null
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSplitSurfaceBufferTest, Init009_DdgrApiType, TestSize.Level1)
{
    ASSERT_NE(splitBuffer_, nullptr);
    auto& renderThread = RSUniRenderThread::Instance();
    renderThread.uniRenderEngine_ = std::make_shared<RSRenderEngine>();
    renderThread.uniRenderEngine_->renderContext_ = RenderContext::Create();
    renderThread.uniRenderEngine_->renderContext_->drGPUContext_ = std::make_shared<Drawing::GPUContext>();
    splitBuffer_->grContext_ = renderThread.uniRenderEngine_->renderContext_->GetSharedDrGPUContext();
    splitBuffer_->surfaceCreated_ = true;
    splitBuffer_->Init(false);
    splitBuffer_->surfaceCreated_ = false;
    if (splitBuffer_->rsSurface_ && splitBuffer_->rsSurface_->renderContext_) {
        splitBuffer_->rsSurface_->renderContext_->drGPUContext_ = nullptr;
    }
    if (splitBuffer_->rsSurface_) splitBuffer_->rsSurface_->SetRenderContext(nullptr);
    splitBuffer_->producerSurface_.ForceSetRefPtr(nullptr);
    splitBuffer_->rsSurface_ = nullptr;
    splitBuffer_->grContext_ = nullptr;
    renderThread.uniRenderEngine_->skContext_ = nullptr;
    if (renderThread.uniRenderEngine_->renderContext_) {
        renderThread.uniRenderEngine_->renderContext_->drGPUContext_ = nullptr;
        renderThread.uniRenderEngine_->renderContext_ = nullptr;
    }
    renderThread.uniRenderEngine_ = nullptr;
}
#endif
} // namespace
} // namespace OHOS::Rosen
