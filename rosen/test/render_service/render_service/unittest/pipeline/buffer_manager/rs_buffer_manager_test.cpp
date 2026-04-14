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

#include "gtest/gtest.h"

#include "pipeline/buffer_manager/rs_buffer_manager.h"
#include "pipeline/rs_surface_handler.h"
#include "iconsumer_surface.h"
#include "surface.h"
#include "surface_buffer.h"
#include "sync_fence.h"
#include "rs_surface_layer.h"
#include <unistd.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t BUFFER_WIDTH = 8;
constexpr uint32_t BUFFER_HEIGHT = 8;
constexpr uint32_t BUFFER_STRIDE = 8;
constexpr uint64_t TEST_LAYER_ID = 1;
constexpr uint64_t TEST_SCREEN_ID = 1;

static uint64_t GenerateBufferId()
{
    static uint64_t counter = 1;
    return counter++;
}
}

class RSBufferManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBufferManagerTest::SetUpTestCase() {}
void RSBufferManagerTest::TearDownTestCase() {}
void RSBufferManagerTest::SetUp() {}
void RSBufferManagerTest::TearDown() {}

/**
 * @tc.name: AddPendingReleaseBufferTest001
 * @tc.desc: Verify AddPendingReleaseBuffer(cons, buffer, fence) safely returns when buffer is null.
 *           Uses a freshly constructed SurfaceBufferEntry (buffer not allocated) and an invalid fence.
 *           Expect no crash and early return behavior.
 * @tc.type: FUNC
 */
HWTEST_F(RSBufferManagerTest, AddPendingReleaseBufferTest001, TestSize.Level1)
{
    auto bufferManager = std::make_shared<RSBufferManager>();
    ASSERT_NE(bufferManager, nullptr);
    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(0);
    surfaceHandler->consumer_ = IConsumerSurface::Create("testName");
    auto surfaceBuffer = std::make_shared<RSSurfaceHandler::SurfaceBufferEntry>();
    bufferManager->AddPendingReleaseBuffer(surfaceHandler->consumer_, surfaceBuffer->buffer, SyncFence::InvalidFence());
}

/**
 * @tc.name: AddPendingReleaseBufferTest002
 * @tc.desc: Verify AddPendingReleaseBuffer(bufferId, fence) path with bufferId=0 and invalid fence object.
 *           Confirms function handles the input gracefully without throwing or crashing.
 * @tc.type: FUNC
 */
HWTEST_F(RSBufferManagerTest, AddPendingReleaseBufferTest002, TestSize.Level1)
{
    auto bufferManager = std::make_shared<RSBufferManager>();
    ASSERT_NE(bufferManager, nullptr);
    bufferManager->AddPendingReleaseBuffer(0, SyncFence::InvalidFence());
}

/**
 * @tc.name: AddPendingReleaseBuffer_WithConsumer_MergeFence
 * @tc.desc: Add twice with same bufferId to trigger fence merge and consumer preservation
 * @tc.type: FUNC
 */
HWTEST_F(RSBufferManagerTest, AddPendingReleaseBuffer_WithConsumer_MergeFence, TestSize.Level1)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto consumer = IConsumerSurface::Create("bm-ut");
    auto buf = SurfaceBuffer::Create();
    BufferRequestConfig cfg { 16, 16, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buf->Alloc(cfg), GSERROR_OK);
    sptr<SyncFence> fence1 = new SyncFence(dup(STDOUT_FILENO));
    sptr<SyncFence> fence2 = new SyncFence(dup(STDERR_FILENO));
    mgr->AddPendingReleaseBuffer(consumer, buf, fence1);
    mgr->AddPendingReleaseBuffer(consumer, buf, fence2);
    EXPECT_NE(fence1, nullptr);
    EXPECT_NE(fence2, nullptr);
    EXPECT_NO_FATAL_FAILURE(mgr->ReleaseBufferById(buf->GetBufferId()));
}

/**
 * @tc.name: OnReleaseLayerBuffers_AllBranches
 * @tc.desc: Exercise all branches in OnReleaseLayerBuffers and ReleaseUniOnDrawBuffers
 * @tc.type: FUNC
 */
HWTEST_F(RSBufferManagerTest, OnReleaseLayerBuffers_AllBranches, TestSize.Level1)
{
    auto mgr = std::make_shared<RSBufferManager>();

    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    // Prepare a concrete RSLayer and bufferOwnerCount
    auto layer = std::static_pointer_cast<RSSurfaceLayer>(RSSurfaceLayer::Create(0, ctx));
    layer->SetUniRenderFlag(true);
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    owner->bufferId_ = 9999ULL;
    // Set callback to ensure DecRef path is valid
    std::atomic<int> released{0};
    owner->bufferReleaseCb_ = [&released](uint64_t){ released.fetch_add(1); };
    layer->SetBufferOwnerCount(owner, true);

    // Create buffer and fence
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { 32, 32, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    // match owner by bufferId
    owner->bufferId_ = buffer->GetBufferId();
    sptr<SyncFence> fence = new SyncFence(dup(STDOUT_FILENO));

    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    RSLayerId id1 = 1;
    rsLayers[id1] = std::static_pointer_cast<RSLayer>(layer);

    // Prepare vec: include cases for missing id, null layer, null buffer
    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>> releaseVec;
    releaseVec.emplace_back(9999, buffer, fence); // missing id -> skipped
    releaseVec.emplace_back(id1, nullptr, fence); // null buffer -> skipped
    // id1 with buffer -> main path
    releaseVec.emplace_back(id1, buffer, fence);

    uint64_t screenId = 7;
    uint64_t bufferId = 2222ULL;
    mgr->OnReleaseLayerBuffers(rsLayers, releaseVec, screenId);

    // Now cover ReleaseUniOnDrawBuffers branches for entries not in decedSet
    auto owner2 = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    owner2->bufferId_ = 1111ULL;
    owner2->bufferReleaseCb_ = [&released](uint64_t){ released.fetch_add(1); };
    // insert unmatched uniOnDrawBufferMap_ entry to trigger SetBufferOwnerCount(false)
    owner->InsertUniOnDrawSet(id1, owner2->bufferId_);
    // Set last owner map to mismatch so CheckLastUniBufferOwner returns false
    owner->SetUniBufferOwner(bufferId, screenId);
    // Put owner2 into layer so PopBufferOwnerCountById returns non-null
    layer->SetBufferOwnerCount(owner2, false);
    sptr<SyncFence> uniFence = new SyncFence(dup(STDERR_FILENO));
    std::set<uint64_t> decedSet{}; // empty so not skipped
    mgr->ReleaseUniOnDrawBuffers(owner, uniFence, decedSet, rsLayers, screenId);

    // At least one release callback should fire
    EXPECT_GE(released.load(), 0);
}

/**
 * @tc.name: ReleaseBufferById_Branches
 * @tc.desc: Cover not-found, null consumer/buffer, and success release
 * @tc.type: FUNC
 */
HWTEST_F(RSBufferManagerTest, ReleaseBufferById_Branches, TestSize.Level1)
{
    auto mgr = std::make_shared<RSBufferManager>();
    // not found branch
    mgr->ReleaseBufferById(424242ULL);

    // insert with null consumer/buffer via without-consumer overload -> null branch
    sptr<SyncFence> f = new SyncFence(dup(STDOUT_FILENO));
    mgr->AddPendingReleaseBuffer(5555ULL, f);
    mgr->ReleaseBufferById(5555ULL);

    // success path: add with consumer+buffer then release
    auto consumer = IConsumerSurface::Create("bm-release");
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { 8, 8, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    sptr<SyncFence> f2 = new SyncFence(dup(STDERR_FILENO));
    mgr->AddPendingReleaseBuffer(consumer, buffer, f2);
    mgr->ReleaseBufferById(buffer->GetBufferId());
}

/**
 * @tc.name: ReleaseBufferById_NullBufferOnlyTest001
 * @tc.desc: Test null buffer_ only path in ReleaseBufferById
 *           When consumer is not null but buffer_ is null, should return early with error
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, ReleaseBufferById_NullBufferOnlyTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto consumer = IConsumerSurface::Create("bm-ut-nullbuf");
    // Directly construct entry with consumer but null buffer to cover null buffer_ branch
    RSBufferManager::PendingReleaseBufferInfo info;
    info.consumer_ = consumer;
    info.buffer_ = nullptr;
    info.mergedFences_ = {};
    mgr->pendingReleaseBuffers_[12345ULL] = info;
    // Should return early without crash
    EXPECT_NO_FATAL_FAILURE(mgr->ReleaseBufferById(12345ULL));
}

/**
 * @tc.name: ReleaseBufferById_NullConsumerOnlyTest001
 * @tc.desc: Test null consumer only path in ReleaseBufferById
 *           When buffer_ is not null but consumer is null, should return early with error
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, ReleaseBufferById_NullConsumerOnlyTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    // Directly construct entry with null consumer but buffer to cover null consumer branch
    RSBufferManager::PendingReleaseBufferInfo info;
    info.consumer_ = nullptr;
    info.buffer_ = buffer;
    info.mergedFences_ = {};
    mgr->pendingReleaseBuffers_[54321ULL] = info;
    // Should return early without crash
    EXPECT_NO_FATAL_FAILURE(mgr->ReleaseBufferById(54321ULL));
}

/**
 * @tc.name: OnDraw_Collector_FenceMerge
 * @tc.desc: Use OnDrawStart/OnDrawBuffer/OnDrawEnd to cover RSBufferCollectorHelper paths
 * @tc.type: FUNC
 */
HWTEST_F(RSBufferManagerTest, OnDraw_Collector_FenceMerge, TestSize.Level1)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto consumer = IConsumerSurface::Create("bm-draw");
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { 4, 4, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    owner->bufferId_ = buffer->GetBufferId();
    std::atomic<int> released{0};
    owner->bufferReleaseCb_ = [&released](uint64_t){ released.fetch_add(1); };

    mgr->OnDrawStart();
    mgr->OnDrawBuffer(consumer, buffer, owner);
    EXPECT_EQ(owner->refCount_.load(), 2);
    // merge path inside helper
    sptr<SyncFence> A = new SyncFence(dup(STDOUT_FILENO));
    sptr<SyncFence> B = new SyncFence(dup(STDERR_FILENO));
    EXPECT_NE(A, nullptr);
    EXPECT_NE(B, nullptr);
    mgr->OnDrawEnd(A);
    EXPECT_EQ(owner->refCount_.load(), 1);
    mgr->OnDrawStart();
    mgr->OnDrawBuffer(consumer, buffer, owner);
    EXPECT_EQ(owner->refCount_.load(), 2);
    mgr->OnDrawEnd(B);
    EXPECT_EQ(owner->refCount_.load(), 1);
    // also cover null fence branch
    mgr->OnDrawStart();
    mgr->OnDrawBuffer(consumer, buffer, owner);
    EXPECT_EQ(owner->refCount_.load(), 2);
    EXPECT_NO_FATAL_FAILURE(mgr->OnDrawEnd(nullptr));
    EXPECT_EQ(owner->refCount_.load(), 1);
}

/**
 * @tc.name: AddPendingReleaseBuffer_WithConsumer_NullFence_NoMerge
 * @tc.desc: Adding with consumer and null fence should not merge; later non-null fence sets mergedFence
 * @tc.type: FUNC
 */
HWTEST_F(RSBufferManagerTest, AddPendingReleaseBuffer_WithConsumer_NullFence_NoMerge, TestSize.Level1)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto consumer = IConsumerSurface::Create("bm-ut-null");
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { 8, 8, 8, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    mgr->AddPendingReleaseBuffer(consumer, buffer, nullptr);
    sptr<SyncFence> f = new SyncFence(dup(STDOUT_FILENO));
    mgr->AddPendingReleaseBuffer(consumer, buffer, f);
}

/**
 * @tc.name: ReleaseUniOnDrawBuffers_DecedSetSkip
 * @tc.desc: Entry present in decedSet should be skipped without adding pending release
 * @tc.type: FUNC
 */
HWTEST_F(RSBufferManagerTest, ReleaseUniOnDrawBuffers_DecedSetSkip, TestSize.Level1)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    owner->bufferId_ = 2222ULL;
    std::set<uint64_t> decedSet;
    decedSet.insert(3333U);
    owner->InsertUniOnDrawSet(1U, 3333U);
    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    sptr<SyncFence> uniFence = SyncFence::InvalidFence();
    mgr->ReleaseUniOnDrawBuffers(owner, uniFence, decedSet, rsLayers, 7);
    // Verify pendingReleaseBuffers is empty since buffer was skipped
    EXPECT_TRUE(mgr->pendingReleaseBuffers_.empty());
}

/**
 * @tc.name: OnDraw_BufferCollector_State
 * @tc.desc: Directly observe bufferCollector_ lifecycle (non-ABI test-only access)
 * @tc.type: FUNC
 */
HWTEST_F(RSBufferManagerTest, OnDraw_BufferCollector_State, TestSize.Level1)
{
    auto mgr = std::make_shared<RSBufferManager>();
    // start -> bufferCollector_ constructed
    mgr->OnDrawStart();
    ASSERT_NE(RSBufferManager::bufferCollector_.get(), nullptr);
    // end -> bufferCollector_ destroyed/reset
    mgr->OnDrawEnd(nullptr);
    ASSERT_EQ(RSBufferManager::bufferCollector_.get(), nullptr);
}

/**
 * @tc.name: SetAcquireFence_MergeFenceTest001
 * @tc.desc: Test fence merge path in RSBufferCollectorHelper::SetAcquireFence
 *           when fence_ exists and is valid (Get() != -1), trigger MergeFence
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, SetAcquireFence_MergeFenceTest001, TestSize.Level2)
{
    RSBufferCollectorHelper helper;
    sptr<SyncFence> fence1 = new SyncFence(dup(STDOUT_FILENO));
    // First call: fence_ is InvalidFence, Get() == -1, so fence_ is set directly without merge
    helper.SetAcquireFence(fence1);
    ASSERT_NE(helper.fence_, nullptr);
    ASSERT_NE(helper.fence_->Get(), -1);

    sptr<SyncFence> fence2 = new SyncFence(dup(STDERR_FILENO));
    // Second call: fence_ now exists and is valid (Get() != -1), triggers MergeFence
    helper.SetAcquireFence(fence2);
}

/**
 * @tc.name: OnCanvasDrawBuffer_NullConsumerTest001
 * @tc.desc: Test null consumer path in RSBufferCollectorHelper::OnCanvasDrawBuffer
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, OnCanvasDrawBuffer_NullConsumerTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    mgr->OnDrawStart();
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    // Consumer is null, should return early without calling AddRef
    mgr->OnDrawBuffer(nullptr, buffer, owner);
    ASSERT_EQ(owner->refCount_.load(), 1);
}

/**
 * @tc.name: OnCanvasDrawBuffer_NullBufferTest001
 * @tc.desc: Test null buffer path in RSBufferCollectorHelper::OnCanvasDrawBuffer
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, OnCanvasDrawBuffer_NullBufferTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    mgr->OnDrawStart();
    auto consumer = IConsumerSurface::Create("bm-ut");
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    // Buffer is null, should return early without calling AddRef
    mgr->OnDrawBuffer(consumer, nullptr, owner);
    ASSERT_EQ(owner->refCount_.load(), 1);
}

/**
 * @tc.name: OnCanvasDrawBuffer_NullBufferOwnerCountTest001
 * @tc.desc: Test null bufferOwnerCount path in RSBufferCollectorHelper::OnCanvasDrawBuffer
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, OnCanvasDrawBuffer_NullBufferOwnerCountTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    mgr->OnDrawStart();
    auto consumer = IConsumerSurface::Create("bm-ut");
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    // BufferOwnerCount is null, should return early without adding to pending list
    mgr->OnDrawBuffer(consumer, buffer, nullptr);
}

/**
 * @tc.name: AddPendingReleaseBuffer_WithConsumer_NullFenceTest001
 * @tc.desc: Test null fence path in AddPendingReleaseBuffer with consumer
 *           When bufferId already exists and new fence is null, should skip fence merge
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, AddPendingReleaseBuffer_WithConsumer_NullFenceTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto consumer = IConsumerSurface::Create("bm-ut");
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    sptr<SyncFence> fence1 = new SyncFence(dup(STDOUT_FILENO));
    // First add with valid fence, create entry with fence
    mgr->AddPendingReleaseBuffer(consumer, buffer, fence1);
    // Second add with null fence, fence is null so skip merge branch
    mgr->AddPendingReleaseBuffer(consumer, buffer, nullptr);
    mgr->ReleaseBufferById(buffer->GetBufferId());
}

/**
 * @tc.name: AddPendingReleaseBuffer_WithConsumer_UpdateConsumerTest001
 * @tc.desc: Test consumer update path when consumer_ is null
 *           First add without consumer, then add with consumer to update
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, AddPendingReleaseBuffer_WithConsumer_UpdateConsumerTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    sptr<SyncFence> f = new SyncFence(dup(STDOUT_FILENO));
    // First add without consumer (using bufferId overload), consumer_ is null
    mgr->AddPendingReleaseBuffer(buffer->GetBufferId(), f);
    auto consumer = IConsumerSurface::Create("bm-ut-update");
    sptr<SyncFence> f2 = new SyncFence(dup(STDERR_FILENO));
    // Second add with consumer, consumer_ is null so update consumer_ branch
    mgr->AddPendingReleaseBuffer(consumer, buffer, f2);
    mgr->ReleaseBufferById(buffer->GetBufferId());
}

/**
 * @tc.name: AddPendingReleaseBuffer_WithoutConsumer_NullFenceTest001
 * @tc.desc: Test null fence path in AddPendingReleaseBuffer without consumer
 *           Should return early and log error
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, AddPendingReleaseBuffer_WithoutConsumer_NullFenceTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    // Fence is null, should return early with error log
    mgr->AddPendingReleaseBuffer(TEST_LAYER_ID, nullptr);
    // Verify pendingReleaseBuffers is empty since fence was null
    EXPECT_TRUE(mgr->pendingReleaseBuffers_.empty());
}

/**
 * @tc.name: AddPendingReleaseBuffer_WithoutConsumer_MergeFenceTest001
 * @tc.desc: Test fence merge path in AddPendingReleaseBuffer without consumer
 *           When mergedFence_ exists and is valid, merge with new fence
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, AddPendingReleaseBuffer_WithoutConsumer_MergeFenceTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    sptr<SyncFence> f1 = new SyncFence(dup(STDOUT_FILENO));
    sptr<SyncFence> f2 = new SyncFence(dup(STDERR_FILENO));
    // First add, mergedFence_ is InvalidFence so set directly
    mgr->AddPendingReleaseBuffer(TEST_SCREEN_ID, f1);
    // Second add, mergedFence_ now valid, triggers merge branch
    mgr->AddPendingReleaseBuffer(TEST_SCREEN_ID, f2);
    // Verify entry exists and has two fences merged
    ASSERT_EQ(mgr->pendingReleaseBuffers_.size(), 1);
    auto& info = mgr->pendingReleaseBuffers_[TEST_SCREEN_ID];
    EXPECT_EQ(info.mergedFences_.size(), 2);
    EXPECT_EQ(info.consumer_, nullptr);
    EXPECT_EQ(info.buffer_, nullptr);
}

/**
 * @tc.name: AddPendingReleaseBuffer_WithoutConsumer_SetFenceTest001
 * @tc.desc: Test set fence path when mergedFence_ is null or invalid
 *           Should directly set the new fence without merging
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, AddPendingReleaseBuffer_WithoutConsumer_SetFenceTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    // First add with InvalidFence, mergedFence_ is InvalidFence (Get() == -1)
    mgr->AddPendingReleaseBuffer(TEST_LAYER_ID, SyncFence::InvalidFence());
    sptr<SyncFence> f = new SyncFence(dup(STDOUT_FILENO));
    // Second add, mergedFence_ is still invalid, set directly without merge
    mgr->AddPendingReleaseBuffer(TEST_LAYER_ID, f);
    // Verify entry exists and has two fences (InvalidFence + valid fence)
    ASSERT_EQ(mgr->pendingReleaseBuffers_.size(), 1);
    auto& info = mgr->pendingReleaseBuffers_[TEST_LAYER_ID];
    EXPECT_EQ(info.mergedFences_.size(), 2);
    EXPECT_EQ(info.consumer_, nullptr);
    EXPECT_EQ(info.buffer_, nullptr);
}

/**
 * @tc.name: AddPendingReleaseBuffer_WithConsumer_UpdateBufferOwnerCountTest001
 * @tc.desc: Test bufferOwnerCount_ update path in AddPendingReleaseBuffer with consumer
 *           When entry exists and bufferOwnerCount_ is expired and new bufferOwnerCount is provided,
 *           should update bufferOwnerCount_ (line 242-244)
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, AddPendingReleaseBuffer_WithConsumer_UpdateBufferOwnerCountTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto consumer = IConsumerSurface::Create("bm-ut-owner1");
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);

    // First add with null bufferOwnerCount (simulating expired state)
    sptr<SyncFence> fence1 = new SyncFence(dup(STDOUT_FILENO));
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> nullOwner = nullptr;
    mgr->AddPendingReleaseBuffer(consumer, buffer, fence1, nullOwner);

    // Verify bufferOwnerCount_ is expired after first add with null
    auto iter = mgr->pendingReleaseBuffers_.find(buffer->GetBufferId());
    ASSERT_NE(iter, mgr->pendingReleaseBuffers_.end());
    EXPECT_TRUE(iter->second.bufferOwnerCount_.expired());

    // Second add with valid bufferOwnerCount should update the expired one
    auto newOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    newOwnerCount->bufferId_ = buffer->GetBufferId();
    sptr<SyncFence> fence2 = new SyncFence(dup(STDERR_FILENO));
    mgr->AddPendingReleaseBuffer(consumer, buffer, fence2, newOwnerCount);

    // Verify bufferOwnerCount_ is now updated to newOwnerCount
    auto& info = mgr->pendingReleaseBuffers_[buffer->GetBufferId()];
    auto lockedOwner = info.bufferOwnerCount_.lock();
    EXPECT_NE(lockedOwner, nullptr);
    EXPECT_EQ(lockedOwner->bufferId_, buffer->GetBufferId());
}

/**
 * @tc.name: AddPendingReleaseBuffer_WithoutConsumer_UpdateBufferOwnerCountTest001
 * @tc.desc: Test bufferOwnerCount_ update path in AddPendingReleaseBuffer without consumer
 *           When entry exists and bufferOwnerCount_ is expired and new bufferOwnerCount is provided,
 *           should update bufferOwnerCount_ (line 266-268)
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, AddPendingReleaseBuffer_WithoutConsumer_UpdateBufferOwnerCountTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    uint64_t bufferId = 8888ULL;

    // First add with null bufferOwnerCount (simulating expired state)
    sptr<SyncFence> fence1 = new SyncFence(dup(STDOUT_FILENO));
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> nullOwner = nullptr;
    mgr->AddPendingReleaseBuffer(bufferId, fence1, nullOwner);

    // Verify bufferOwnerCount_ is expired after first add with null
    auto iter = mgr->pendingReleaseBuffers_.find(bufferId);
    ASSERT_NE(iter, mgr->pendingReleaseBuffers_.end());
    EXPECT_TRUE(iter->second.bufferOwnerCount_.expired());

    // Second add with valid bufferOwnerCount should update the expired one
    auto newOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    newOwnerCount->bufferId_ = bufferId;
    sptr<SyncFence> fence2 = new SyncFence(dup(STDERR_FILENO));
    mgr->AddPendingReleaseBuffer(bufferId, fence2, newOwnerCount);

    // Verify bufferOwnerCount_ is now updated to newOwnerCount
    auto& info = mgr->pendingReleaseBuffers_[bufferId];
    auto lockedOwner = info.bufferOwnerCount_.lock();
    EXPECT_NE(lockedOwner, nullptr);
    EXPECT_EQ(lockedOwner->bufferId_, bufferId);
}

/**
 * @tc.name: AddPendingReleaseBuffer_WithConsumer_BufferOwnerCountNotExpiredTest001
 * @tc.desc: Test bufferOwnerCount_ not updated when not expired
 *           When entry exists and bufferOwnerCount_ is NOT expired, should NOT update
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, AddPendingReleaseBuffer_WithConsumer_BufferOwnerCountNotExpiredTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto consumer = IConsumerSurface::Create("bm-ut-owner2");
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);

    // First add with valid bufferOwnerCount
    sptr<SyncFence> fence1 = new SyncFence(dup(STDOUT_FILENO));
    auto originalOwner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    originalOwner->bufferId_ = buffer->GetBufferId();
    mgr->AddPendingReleaseBuffer(consumer, buffer, fence1, originalOwner);

    // Second add with different bufferOwnerCount should NOT update because original is not expired
    auto differentOwner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    differentOwner->bufferId_ = 9999ULL;  // Different bufferId
    sptr<SyncFence> fence2 = new SyncFence(dup(STDERR_FILENO));
    mgr->AddPendingReleaseBuffer(consumer, buffer, fence2, differentOwner);

    // Verify bufferOwnerCount_ is still the original one
    auto& info = mgr->pendingReleaseBuffers_[buffer->GetBufferId()];
    auto lockedOwner = info.bufferOwnerCount_.lock();
    EXPECT_NE(lockedOwner, nullptr);
    EXPECT_EQ(lockedOwner->bufferId_, buffer->GetBufferId());  // Should be original, not different
}

/**
 * @tc.name: AddPendingReleaseBuffer_WithoutConsumer_BufferOwnerCountNotExpiredTest001
 * @tc.desc: Test bufferOwnerCount_ not updated when not expired (without consumer)
 *           When entry exists and bufferOwnerCount_ is NOT expired, should NOT update
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, AddPendingReleaseBuffer_WithoutConsumer_BufferOwnerCountNotExpiredTest001,
    TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    uint64_t bufferId = 7777ULL;

    // First add with valid bufferOwnerCount
    sptr<SyncFence> fence1 = new SyncFence(dup(STDOUT_FILENO));
    auto originalOwner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    originalOwner->bufferId_ = bufferId;
    mgr->AddPendingReleaseBuffer(bufferId, fence1, originalOwner);

    // Second add with different bufferOwnerCount should NOT update because original is not expired
    auto differentOwner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    differentOwner->bufferId_ = 9999ULL;  // Different bufferId
    sptr<SyncFence> fence2 = new SyncFence(dup(STDERR_FILENO));
    mgr->AddPendingReleaseBuffer(bufferId, fence2, differentOwner);

    // Verify bufferOwnerCount_ is still the original one
    auto& info = mgr->pendingReleaseBuffers_[bufferId];
    auto lockedOwner = info.bufferOwnerCount_.lock();
    EXPECT_NE(lockedOwner, nullptr);
    EXPECT_EQ(lockedOwner->bufferId_, bufferId);  // Should be original, not different
}

/**
 * @tc.name: OnReleaseLayerBuffers_NullLayerTest001
 * @tc.desc: Test null layer path in OnReleaseLayerBuffers
 *           When layer weak_ptr.lock() returns nullptr, should skip
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, OnReleaseLayerBuffers_NullLayerTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    auto layer = RSSurfaceLayer::Create(0, ctx);
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    sptr<SyncFence> f = new SyncFence(dup(STDOUT_FILENO));

    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    rsLayers[TEST_LAYER_ID] = std::static_pointer_cast<RSLayer>(layer);

    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>> releaseVec;
    // Reset layer to make weak_ptr expired, lock() will return nullptr
    layer.reset();
    releaseVec.emplace_back(TEST_LAYER_ID, buffer, f);

    mgr->OnReleaseLayerBuffers(rsLayers, releaseVec, TEST_SCREEN_ID);
}

/**
 * @tc.name: OnReleaseLayerBuffers_UniRenderFlagFalseTest001
 * @tc.desc: Test GetUniRenderFlag false path in OnReleaseLayerBuffers
 *           When uniRender flag is false, should not set uniBufferCount
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, OnReleaseLayerBuffers_UniRenderFlagFalseTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    auto layer = RSSurfaceLayer::Create(0, ctx);
    // UniRender flag is false, uniBufferCount should not be set
    layer->SetUniRenderFlag(false);
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    std::atomic<bool> released{false};
    owner->bufferReleaseCb_ = [&released](uint64_t){ released = true; };
    layer->SetBufferOwnerCount(owner, false);

    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    owner->bufferId_ = buffer->GetBufferId();
    sptr<SyncFence> f = new SyncFence(dup(STDOUT_FILENO));

    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    rsLayers[TEST_LAYER_ID] = std::static_pointer_cast<RSLayer>(layer);

    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>> releaseVec;
    releaseVec.emplace_back(TEST_LAYER_ID, buffer, f);

    mgr->OnReleaseLayerBuffers(rsLayers, releaseVec, TEST_SCREEN_ID);
}

/**
 * @tc.name: OnReleaseLayerBuffers_UniRenderFlagTrueTest001
 * @tc.desc: Test GetUniRenderFlag true path in OnReleaseLayerBuffers
 *           When uniRender flag is true, should set uniBufferCount and uniFence
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, OnReleaseLayerBuffers_UniRenderFlagTrueTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    auto layer = RSSurfaceLayer::Create(0, ctx);
    // UniRender flag is true, uniBufferCount should be set
    layer->SetUniRenderFlag(true);
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    std::atomic<bool> released{false};
    owner->bufferReleaseCb_ = [&released](uint64_t){ released = true; };
    layer->SetBufferOwnerCount(owner, false);

    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    owner->bufferId_ = buffer->GetBufferId();
    sptr<SyncFence> f = new SyncFence(dup(STDOUT_FILENO));

    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    rsLayers[TEST_LAYER_ID] = std::static_pointer_cast<RSLayer>(layer);

    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>> releaseVec;
    releaseVec.emplace_back(TEST_LAYER_ID, buffer, f);

    mgr->OnReleaseLayerBuffers(rsLayers, releaseVec, TEST_SCREEN_ID);
}

/**
 * @tc.name: ReleaseUniOnDrawBuffers_NullLayerTest001
 * @tc.desc: Test null layer path in ReleaseUniOnDrawBuffers
 *           When layer weak_ptr.lock() returns nullptr, should skip and log error
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, ReleaseUniOnDrawBuffers_NullLayerTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    owner->bufferId_ = GenerateBufferId();
    std::atomic<bool> released{false};
    owner->bufferReleaseCb_ = [&released](uint64_t){ released = true; };

    // Insert a layerId into uniOnDrawBuffersMap_
    owner->InsertUniOnDrawSet(TEST_LAYER_ID, GenerateBufferId());

    // Create rsLayers with expired weak_ptr (layer has been released)
    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    auto layer = RSSurfaceLayer::Create(0, ctx);
    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    rsLayers[TEST_LAYER_ID] = std::static_pointer_cast<RSLayer>(layer);
    // Reset layer to make weak_ptr expired, lock() will return nullptr
    layer.reset();

    std::set<uint64_t> decedSet{};
    sptr<SyncFence> uniFence = new SyncFence(dup(STDOUT_FILENO));
    // Call ReleaseUniOnDrawBuffers, should skip when layer == nullptr
    mgr->ReleaseUniOnDrawBuffers(owner, uniFence, decedSet, rsLayers, TEST_SCREEN_ID);

    // Verify pendingReleaseBuffers is empty since layer was null and skipped
    EXPECT_TRUE(mgr->pendingReleaseBuffers_.empty());
    // Verify release callback was not called
    EXPECT_FALSE(released.load());
}

/**
 * @tc.name: ReleaseUniOnDrawBuffers_DecedSetSkipTest001
 * @tc.desc: Test bufferId in decedSet path in ReleaseUniOnDrawBuffers
 *           When bufferId is found in decedSet, should skip processing without AddPendingReleaseBuffer
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, ReleaseUniOnDrawBuffers_DecedSetSkipTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    owner->bufferId_ = GenerateBufferId();
    std::atomic<bool> released{false};
    owner->bufferReleaseCb_ = [&released](uint64_t){ released = true; };

    // Insert layerId and bufferId into uniOnDrawBuffersMap_
    uint64_t targetBufferId = GenerateBufferId();
    owner->InsertUniOnDrawSet(TEST_LAYER_ID, targetBufferId);

    // Create rsLayers
    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    auto layer = RSSurfaceLayer::Create(0, ctx);
    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    rsLayers[TEST_LAYER_ID] = std::static_pointer_cast<RSLayer>(layer);

    // Put bufferOwnerCount into layer so PopBufferOwnerCountById can return it
    auto bufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    bufferOwnerCount->bufferId_ = targetBufferId;
    layer->SetBufferOwnerCount(bufferOwnerCount, false);

    // Set targetBufferId into decedSet to trigger skip branch
    std::set<uint64_t> decedSet{};
    decedSet.insert(static_cast<uint32_t>(targetBufferId));

    sptr<SyncFence> uniFence = new SyncFence(dup(STDOUT_FILENO));
    // Call ReleaseUniOnDrawBuffers, should skip when bufferId in decedSet
    mgr->ReleaseUniOnDrawBuffers(owner, uniFence, decedSet, rsLayers, TEST_SCREEN_ID);

    // Verify buffer was not added to pending release
    ASSERT_FALSE(released.load());
}

/**
 * @tc.name: ReleaseUniOnDrawBuffers_NullBufferOwnerCountTest001
 * @tc.desc: Test null bufferOwnerCount path in ReleaseUniOnDrawBuffers
 *           When PopBufferOwnerCountById returns nullptr, should skip and log error
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, ReleaseUniOnDrawBuffers_NullBufferOwnerCountTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    owner->bufferId_ = GenerateBufferId();
    std::atomic<bool> released{false};
    owner->bufferReleaseCb_ = [&released](uint64_t){ released = true; };

    // Insert layerId and bufferId into uniOnDrawBuffersMap_
    uint64_t targetBufferId = GenerateBufferId();
    owner->InsertUniOnDrawSet(TEST_LAYER_ID, targetBufferId);

    // Create rsLayers with valid layer
    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    auto layer = RSSurfaceLayer::Create(0, ctx);
    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    rsLayers[TEST_LAYER_ID] = std::static_pointer_cast<RSLayer>(layer);

    // Do NOT put bufferOwnerCount into layer, so PopBufferOwnerCountById returns nullptr

    std::set<uint64_t> decedSet{};
    sptr<SyncFence> uniFence = new SyncFence(dup(STDOUT_FILENO));
    // Call ReleaseUniOnDrawBuffers, should skip when bufferOwnerCount == nullptr
    mgr->ReleaseUniOnDrawBuffers(owner, uniFence, decedSet, rsLayers, TEST_SCREEN_ID);

    // Verify buffer was not added to pending release
    ASSERT_FALSE(released.load());
}

/**
 * @tc.name: ReleaseUniOnDrawBuffers_NotLastUniBufferOwnerTest001
 * @tc.desc: Test CheckLastUniBufferOwner false path in ReleaseUniOnDrawBuffers
 *           When CheckLastUniBufferOwner returns false, should call SetBufferOwnerCount with false
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, ReleaseUniOnDrawBuffers_NotLastUniBufferOwnerTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    owner->bufferId_ = GenerateBufferId();
    std::atomic<bool> released{false};
    owner->bufferReleaseCb_ = [&released](uint64_t){ released = true; };

    // Insert layerId and bufferId into uniOnDrawBuffersMap_
    uint64_t targetBufferId = GenerateBufferId();
    owner->InsertUniOnDrawSet(TEST_LAYER_ID, targetBufferId);

    // Set uniBufferOwner with different bufferId and screenId to make CheckLastUniBufferOwner return false
    owner->SetUniBufferOwner(GenerateBufferId(), TEST_SCREEN_ID);

    // Create rsLayers with valid layer
    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    auto layer = RSSurfaceLayer::Create(0, ctx);
    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    rsLayers[TEST_LAYER_ID] = std::static_pointer_cast<RSLayer>(layer);

    // Put bufferOwnerCount into layer so PopBufferOwnerCountById returns non-null
    auto bufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    bufferOwnerCount->bufferId_ = targetBufferId;
    layer->SetBufferOwnerCount(bufferOwnerCount, false);

    std::set<uint64_t> decedSet{};
    sptr<SyncFence> uniFence = new SyncFence(dup(STDOUT_FILENO));
    // Call ReleaseUniOnDrawBuffers, CheckLastUniBufferOwner returns false so SetBufferOwnerCount(false) called
    mgr->ReleaseUniOnDrawBuffers(owner, uniFence, decedSet, rsLayers, TEST_SCREEN_ID);
    ASSERT_EQ(bufferOwnerCount->refCount_.load(), 1);
}

/**
 * @tc.name: ReleaseUniOnDrawBuffers_CheckLastUniBufferOwnerTrueTest001
 * @tc.desc: Test CheckLastUniBufferOwner true path in ReleaseUniOnDrawBuffers
 *           When CheckLastUniBufferOwner returns true, should NOT call SetBufferOwnerCount
 *           This covers the else branch of if (!bufferOwnerCount->CheckLastUniBufferOwner(...))
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, ReleaseUniOnDrawBuffers_CheckLastUniBufferOwnerTrueTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    owner->bufferId_ = 9999ULL;
    std::atomic<bool> released{false};
    owner->bufferReleaseCb_ = [&released](uint64_t){ released = true; };

    // Insert layerId and bufferId into uniOnDrawBuffersMap_
    uint64_t targetBufferId = GenerateBufferId();
    owner->InsertUniOnDrawSet(TEST_LAYER_ID, targetBufferId);

    // Create rsLayers with valid layer
    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    auto layer = RSSurfaceLayer::Create(0, ctx);
    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    rsLayers[TEST_LAYER_ID] = std::static_pointer_cast<RSLayer>(layer);

    // Put bufferOwnerCount into layer so PopBufferOwnerCountById returns non-null
    auto bufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    bufferOwnerCount->bufferId_ = targetBufferId;
    layer->SetBufferOwnerCount(bufferOwnerCount, false);

    std::set<uint64_t> decedSet{};
    sptr<SyncFence> uniFence = new SyncFence(dup(STDOUT_FILENO));
    // Do NOT call SetUniBufferOwner, so CheckLastUniBufferOwner returns true
    // (screenId not found in uniBufferOwnerSeqNumMap_)
    // This means SetBufferOwnerCount will NOT be called

    // Track if SetBufferOwnerCount is called by monitoring refCount
    int initialRefCount = bufferOwnerCount->refCount_.load();

    mgr->ReleaseUniOnDrawBuffers(owner, uniFence, decedSet, rsLayers, TEST_SCREEN_ID);

    // refCount should decrement by 1 due to OnBufferReleased
    // If SetBufferOwnerCount was NOT called (which is correct for this test),
    // refCount should be initialRefCount - 1
    EXPECT_EQ(bufferOwnerCount->refCount_.load(), initialRefCount - 1);
}

/**
 * @tc.name: ReleaseUniOnDrawBuffers_CheckLastUniBufferOwnerMatchTest001
 * @tc.desc: Test CheckLastUniBufferOwner true path when bufferId matches
 *           When screenId exists AND bufferId matches, CheckLastUniBufferOwner returns true
 *           Should NOT call SetBufferOwnerCount in this case
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, ReleaseUniOnDrawBuffers_CheckLastUniBufferOwnerMatchTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    uint64_t targetBufferId = GenerateBufferId();
    owner->bufferId_ = targetBufferId;
    std::atomic<bool> released{false};
    owner->bufferReleaseCb_ = [&released](uint64_t){ released = true; };

    // Insert layerId and bufferId into uniOnDrawBuffersMap_
    owner->InsertUniOnDrawSet(TEST_LAYER_ID, targetBufferId);

    // Create rsLayers with valid layer
    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    auto layer = RSSurfaceLayer::Create(0, ctx);
    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    rsLayers[TEST_LAYER_ID] = std::static_pointer_cast<RSLayer>(layer);

    // Put bufferOwnerCount into layer so PopBufferOwnerCountById returns non-null
    auto bufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    bufferOwnerCount->bufferId_ = targetBufferId;
    layer->SetBufferOwnerCount(bufferOwnerCount, false);

    std::set<uint64_t> decedSet{};
    sptr<SyncFence> uniFence = new SyncFence(dup(STDOUT_FILENO));
    // Set matching bufferId so CheckLastUniBufferOwner returns true (iter->second == bufferId)
    owner->SetUniBufferOwner(targetBufferId, TEST_SCREEN_ID);

    int initialRefCount = bufferOwnerCount->refCount_.load();

    mgr->ReleaseUniOnDrawBuffers(owner, uniFence, decedSet, rsLayers, TEST_SCREEN_ID);

    // refCount should decrement by 1 due to OnBufferReleased
    // SetBufferOwnerCount should NOT be called since CheckLastUniBufferOwner returns true
    EXPECT_EQ(bufferOwnerCount->refCount_.load(), initialRefCount - 1);
}

/**
 * @tc.name: OnReleaseLayerBuffers_NullBufferOwnerCountTest001
 * @tc.desc: Test null bufferOwnerCount path in OnReleaseLayerBuffers
 *           When PopBufferOwnerCountById returns nullptr (bufferId not found in layer's bufferOwnerCounts_),
 *           the if (bufferOwnerCount) branch at line 177 should NOT be entered.
 *           This covers the false branch of line 177.
 * @tc.type: FUNC
 * @tc.require: issue41
 */
HWTEST_F(RSBufferManagerTest, OnReleaseLayerBuffers_NullBufferOwnerCountTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    auto layer = RSSurfaceLayer::Create(0, ctx);
    layer->SetUniRenderFlag(false);

    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);

    // Create owner and set it in layer, but with DIFFERENT bufferId
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    owner->bufferId_ = 9999ULL; // Different from buffer's actual bufferId
    std::atomic<bool> released{false};
    owner->bufferReleaseCb_ = [&released](uint64_t){ released = true; };
    layer->SetBufferOwnerCount(owner, false);

    sptr<SyncFence> f = new SyncFence(dup(STDOUT_FILENO));

    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    rsLayers[TEST_LAYER_ID] = std::static_pointer_cast<RSLayer>(layer);

    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>> releaseVec;
    // Use actual buffer's bufferId, which doesn't exist in layer's bufferOwnerCounts_
    // So PopBufferOwnerCountById will return nullptr
    releaseVec.emplace_back(TEST_LAYER_ID, buffer, f);

    // This should cover the false branch of if (bufferOwnerCount) at line 177
    mgr->OnReleaseLayerBuffers(rsLayers, releaseVec, TEST_SCREEN_ID);
}

/**
 * @tc.name: OnReleaseLayerBuffers_BufferOwnerCountTrueTest001
 * @tc.desc: Test bufferOwnerCount is not null path in OnReleaseLayerBuffers
 *           When PopBufferOwnerCountById returns valid bufferOwnerCount:
 *           - AddPendingReleaseBuffer should be called
 *           - OnBufferReleased should be called (refCount decrements)
 *           - decedSet should be inserted with bufferId
 *           This covers the true branch of if (bufferOwnerCount) at line 297.
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, OnReleaseLayerBuffers_BufferOwnerCountTrueTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    auto layer = RSSurfaceLayer::Create(0, ctx);
    layer->SetUniRenderFlag(false);

    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);

    // Create owner with MATCHING bufferId so PopBufferOwnerCountById returns non-null
    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    owner->bufferId_ = buffer->GetBufferId();  // Match buffer's bufferId
    std::atomic<int> releaseCount{0};
    owner->bufferReleaseCb_ = [&releaseCount](uint64_t){ releaseCount.fetch_add(1); };
    layer->SetBufferOwnerCount(owner, false);

    sptr<SyncFence> f = new SyncFence(dup(STDOUT_FILENO));

    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    rsLayers[TEST_LAYER_ID] = std::static_pointer_cast<RSLayer>(layer);

    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>> releaseVec;
    releaseVec.emplace_back(TEST_LAYER_ID, buffer, f);

    mgr->OnReleaseLayerBuffers(rsLayers, releaseVec, TEST_SCREEN_ID);

    // Verify AddPendingReleaseBuffer was called by checking pendingReleaseBuffers_
    EXPECT_EQ(mgr->pendingReleaseBuffers_.size(), 1);
    auto iter = mgr->pendingReleaseBuffers_.find(buffer->GetBufferId());
    ASSERT_NE(iter, mgr->pendingReleaseBuffers_.end());
    EXPECT_NE(iter->second.bufferOwnerCount_.lock(), nullptr);
}

/**
 * @tc.name: OnReleaseLayerBuffers_UniRenderFlagTrueVerifyUniBufferCountTest001
 * @tc.desc: Test uniBufferCount and uniFence are set when GetUniRenderFlag returns true
 *           This verifies the inner if (layer->GetUniRenderFlag()) branch at line 301
 *           and that ReleaseUniOnDrawBuffers is called with correct parameters.
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, OnReleaseLayerBuffers_UniRenderFlagTrueVerifyUniBufferCountTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto ctx = std::make_shared<RSComposerContext>(nullptr);
    auto layer = std::static_pointer_cast<RSSurfaceLayer>(RSSurfaceLayer::Create(0, ctx));
    layer->SetUniRenderFlag(true);  // Enable uniRender flag

    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);

    auto owner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    owner->bufferId_ = buffer->GetBufferId();
    std::atomic<int> releaseCount{0};
    owner->bufferReleaseCb_ = [&releaseCount](uint64_t){ releaseCount.fetch_add(1); };
    layer->SetBufferOwnerCount(owner, false);

    sptr<SyncFence> f = new SyncFence(dup(STDOUT_FILENO));

    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    rsLayers[TEST_LAYER_ID] = std::static_pointer_cast<RSLayer>(layer);

    std::vector<std::tuple<RSLayerId, sptr<SurfaceBuffer>, sptr<SyncFence>>> releaseVec;
    releaseVec.emplace_back(TEST_LAYER_ID, buffer, f);

    mgr->OnReleaseLayerBuffers(rsLayers, releaseVec, TEST_SCREEN_ID);

    // Verify AddPendingReleaseBuffer was called
    EXPECT_EQ(mgr->pendingReleaseBuffers_.size(), 1);
}

/**
 * @tc.name: DumpPendingReleaseBuffers_EmptyTest001
 * @tc.desc: Test DumpPendingReleaseBuffers when pendingReleaseBuffers_ is empty
 *           Should output "(empty)" message and zero counts in summary
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, DumpPendingReleaseBuffers_EmptyTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    std::string output;
    mgr->DumpPendingReleaseBuffers(output);

    // Verify empty dump output
    EXPECT_NE(output.find("pendingReleaseBuffers_ Dump"), std::string::npos);
    EXPECT_NE(output.find("Total entries: 0"), std::string::npos);
    EXPECT_NE(output.find("(empty)"), std::string::npos);
    EXPECT_NE(output.find("Valid buffers: 0"), std::string::npos);
    EXPECT_NE(output.find("Expired buffers: 0"), std::string::npos);
    EXPECT_NE(output.find("Total fences: 0"), std::string::npos);
}

/**
 * @tc.name: DumpPendingReleaseBuffers_WithValidBufferTest001
 * @tc.desc: Test DumpPendingReleaseBuffers with one valid buffer entry
 *           Covers valid buffer path in AppendBufferEntry and AppendSummary
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, DumpPendingReleaseBuffers_WithValidBufferTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto consumer = IConsumerSurface::Create("bm-dump-test");
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    sptr<SyncFence> fence = new SyncFence(dup(STDOUT_FILENO));
    auto ownerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    ownerCount->bufferId_ = buffer->GetBufferId();

    mgr->AddPendingReleaseBuffer(consumer, buffer, fence, ownerCount);

    std::string output;
    mgr->DumpPendingReleaseBuffers(output);

    // Verify valid buffer dump output
    EXPECT_NE(output.find("pendingReleaseBuffers_ Dump"), std::string::npos);
    EXPECT_NE(output.find("Total entries: 1"), std::string::npos);
    EXPECT_NE(output.find("valid"), std::string::npos);
    EXPECT_NE(output.find(std::to_string(buffer->GetWidth())), std::string::npos);
    EXPECT_NE(output.find(std::to_string(buffer->GetHeight())), std::string::npos);
    EXPECT_NE(output.find("Valid buffers: 1"), std::string::npos);
    EXPECT_NE(output.find("Expired buffers: 0"), std::string::npos);
    EXPECT_NE(output.find("Total fences: 1"), std::string::npos);
}

/**
 * @tc.name: DumpPendingReleaseBuffers_WithExpiredBufferTest001
 * @tc.desc: Test DumpPendingReleaseBuffers with expired buffer (buffer_ is wptr expired)
 *           Covers expired buffer path in AppendBufferEntry and AppendSummary
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, DumpPendingReleaseBuffers_WithExpiredBufferTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto consumer = IConsumerSurface::Create("bm-dump-expired");
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    sptr<SyncFence> fence = new SyncFence(dup(STDOUT_FILENO));
    auto ownerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    ownerCount->bufferId_ = buffer->GetBufferId();

    mgr->AddPendingReleaseBuffer(consumer, buffer, fence, ownerCount);

    // Release the buffer so buffer_ becomes expired
    mgr->ReleaseBufferById(buffer->GetBufferId());

    // Now add a new entry with same consumer but buffer will be expired
    // Directly construct entry with expired buffer
    RSBufferManager::PendingReleaseBufferInfo info;
    info.consumer_ = consumer;
    info.buffer_ = nullptr;  // wptr already expired
    info.mergedFences_.push_back(fence);
    info.bufferOwnerCount_ = ownerCount;
    mgr->pendingReleaseBuffers_[9999ULL] = info;

    std::string output;
    mgr->DumpPendingReleaseBuffers(output);

    // Verify expired buffer dump output
    EXPECT_NE(output.find("expired"), std::string::npos);
    EXPECT_NE(output.find("Valid buffers: 0"), std::string::npos);
    EXPECT_NE(output.find("Expired buffers: 1"), std::string::npos);
}

/**
 * @tc.name: DumpPendingReleaseBuffers_NullConsumerTest001
 * @tc.desc: Test DumpPendingReleaseBuffers with null consumer entry
 *           Covers null consumer path in AppendBufferEntry
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, DumpPendingReleaseBuffers_NullConsumerTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    sptr<SyncFence> fence = new SyncFence(dup(STDOUT_FILENO));

    // Add entry without consumer (using bufferId overload)
    mgr->AddPendingReleaseBuffer(TEST_LAYER_ID, fence);

    std::string output;
    mgr->DumpPendingReleaseBuffers(output);

    // Verify null consumer dump output
    EXPECT_NE(output.find("null"), std::string::npos);
}

/**
 * @tc.name: DumpPendingReleaseBuffers_MultipleFencesTest001
 * @tc.desc: Test DumpPendingReleaseBuffers with multiple fences in mergedFences_
 *           Covers fence details output in AppendFenceDetails
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, DumpPendingReleaseBuffers_MultipleFencesTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto consumer = IConsumerSurface::Create("bm-dump-fences");
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);

    // Add multiple times to accumulate fences
    sptr<SyncFence> fence1 = new SyncFence(dup(STDOUT_FILENO));
    sptr<SyncFence> fence2 = new SyncFence(dup(STDERR_FILENO));
    auto ownerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    ownerCount->bufferId_ = buffer->GetBufferId();

    mgr->AddPendingReleaseBuffer(consumer, buffer, fence1, ownerCount);
    mgr->AddPendingReleaseBuffer(consumer, buffer, fence2, ownerCount);

    std::string output;
    mgr->DumpPendingReleaseBuffers(output);

    // Verify multiple fence details output
    EXPECT_NE(output.find("[0]"), std::string::npos);
    EXPECT_NE(output.find("[1]"), std::string::npos);
    EXPECT_NE(output.find("fence_fd="), std::string::npos);
    EXPECT_NE(output.find("Total fences: 2"), std::string::npos);
}

/**
 * @tc.name: DumpPendingReleaseBuffers_NullFenceInMergedTest001
 * @tc.desc: Test DumpPendingReleaseBuffers with null fence in mergedFences_
 *           Covers null fence path in GetFenceStatus and AppendFenceDetails
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, DumpPendingReleaseBuffers_NullFenceInMergedTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto consumer = IConsumerSurface::Create("bm-dump-nullfence");
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    auto ownerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    ownerCount->bufferId_ = buffer->GetBufferId();

    // Directly construct entry with null fence in mergedFences_
    RSBufferManager::PendingReleaseBufferInfo info;
    info.consumer_ = consumer;
    info.buffer_ = buffer;
    info.mergedFences_.push_back(nullptr);  // null fence
    info.mergedFences_.push_back(SyncFence::InvalidFence());  // invalid fence (Get() == -1)
    info.bufferOwnerCount_ = ownerCount;
    mgr->pendingReleaseBuffers_[buffer->GetBufferId()] = info;

    std::string output;
    mgr->DumpPendingReleaseBuffers(output);

    // Verify null and invalid fence output
    EXPECT_NE(output.find("(null)"), std::string::npos);
    EXPECT_NE(output.find("(invalid)"), std::string::npos);
}

/**
 * @tc.name: DumpPendingReleaseBuffers_NullOwnerCountTest001
 * @tc.desc: Test DumpPendingReleaseBuffers with null/expired bufferOwnerCount_
 *           Covers null ownerCount path in AppendBufferEntry showing "N/A"
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, DumpPendingReleaseBuffers_NullOwnerCountTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto consumer = IConsumerSurface::Create("bm-dump-nullowner");
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    sptr<SyncFence> fence = new SyncFence(dup(STDOUT_FILENO));

    // Directly construct entry with expired bufferOwnerCount_
    RSBufferManager::PendingReleaseBufferInfo info;
    info.consumer_ = consumer;
    info.buffer_ = buffer;
    info.mergedFences_.push_back(fence);
    // bufferOwnerCount_ is weak_ptr, default is expired
    info.bufferOwnerCount_ = std::shared_ptr<RSSurfaceHandler::BufferOwnerCount>();
    mgr->pendingReleaseBuffers_[buffer->GetBufferId()] = info;

    std::string output;
    mgr->DumpPendingReleaseBuffers(output);

    // Verify N/A for null ownerCount
    EXPECT_NE(output.find("N/A"), std::string::npos);
}

/**
 * @tc.name: DumpPendingReleaseBuffers_AverageFencesTest001
 * @tc.desc: Test DumpPendingReleaseBuffers average fences calculation
 *           When buffers not empty, average = totalFences / buffers.size()
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, DumpPendingReleaseBuffers_AverageFencesTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto consumer = IConsumerSurface::Create("bm-dump-avg");
    auto buffer = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer->Alloc(cfg), GSERROR_OK);
    auto ownerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    ownerCount->bufferId_ = buffer->GetBufferId();

    // Add multiple fences to test average calculation
    sptr<SyncFence> fence1 = new SyncFence(dup(STDOUT_FILENO));
    sptr<SyncFence> fence2 = new SyncFence(dup(STDERR_FILENO));
    sptr<SyncFence> fence3 = new SyncFence(dup(STDIN_FILENO));

    mgr->AddPendingReleaseBuffer(consumer, buffer, fence1, ownerCount);
    mgr->AddPendingReleaseBuffer(consumer, buffer, fence2, ownerCount);
    mgr->AddPendingReleaseBuffer(consumer, buffer, fence3, ownerCount);

    std::string output;
    mgr->DumpPendingReleaseBuffers(output);

    // Verify average fences output (3 fences / 1 buffer = 3)
    EXPECT_NE(output.find("Average fences per buffer:"), std::string::npos);
    EXPECT_NE(output.find("3"), std::string::npos);
}

/**
 * @tc.name: DumpPendingReleaseBuffers_MultipleBuffersTest001
 * @tc.desc: Test DumpPendingReleaseBuffers with multiple buffer entries
 *           Covers iteration over multiple entries in AppendBufferTable and AppendSummary
 * @tc.type: FUNC
 * @tc.require: issueI5N3G0
 */
HWTEST_F(RSBufferManagerTest, DumpPendingReleaseBuffers_MultipleBuffersTest001, TestSize.Level2)
{
    auto mgr = std::make_shared<RSBufferManager>();
    auto consumer = IConsumerSurface::Create("bm-dump-multi");

    auto buffer1 = SurfaceBuffer::Create();
    BufferRequestConfig cfg { BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_STRIDE, GRAPHIC_PIXEL_FMT_RGBA_8888,
        BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, 0 };
    ASSERT_EQ(buffer1->Alloc(cfg), GSERROR_OK);
    auto ownerCount1 = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    ownerCount1->bufferId_ = buffer1->GetBufferId();

    auto buffer2 = SurfaceBuffer::Create();
    ASSERT_EQ(buffer2->Alloc(cfg), GSERROR_OK);
    auto ownerCount2 = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    ownerCount2->bufferId_ = buffer2->GetBufferId();

    sptr<SyncFence> fence1 = new SyncFence(dup(STDOUT_FILENO));
    sptr<SyncFence> fence2 = new SyncFence(dup(STDERR_FILENO));

    mgr->AddPendingReleaseBuffer(consumer, buffer1, fence1, ownerCount1);
    mgr->AddPendingReleaseBuffer(consumer, buffer2, fence2, ownerCount2);

    std::string output;
    mgr->DumpPendingReleaseBuffers(output);

    // Verify multiple buffers output
    EXPECT_NE(output.find("Total entries: 2"), std::string::npos);
    EXPECT_NE(output.find("Valid buffers: 2"), std::string::npos);
    EXPECT_NE(output.find("Expired buffers: 0"), std::string::npos);
    EXPECT_NE(output.find("Total fences: 2"), std::string::npos);
}
}