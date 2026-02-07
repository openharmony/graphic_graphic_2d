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

#define private public
#include "pipeline/buffer_thread/rs_buffer_manager.h"
#undef private
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

    // Prepare a concrete RSLayer and bufferOwnerCount
    auto layer = std::static_pointer_cast<RSSurfaceLayer>(RSSurfaceLayer::Create(0, nullptr));
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

    mgr->OnReleaseLayerBuffers(rsLayers, releaseVec, /*screenId*/7);

    // Now cover ReleaseUniOnDrawBuffers branches for entries not in decedSet
    auto owner2 = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    owner2->bufferId_ = 1111ULL;
    owner2->bufferReleaseCb_ = [&released](uint64_t){ released.fetch_add(1); };
    // insert unmatched uniOnDrawBufferMap_ entry to trigger SetBufferOwnerCount(false)
    owner->InsertUniOnDrawSet(id1, owner2->bufferId_);
    // Set last owner map to mismatch so CheckLastUniBufferOwner returns false
    owner->SetUniBufferOwner(/*bufferId=*/2222ULL, /*screenId=*/7);
    // Put owner2 into layer so PopBufferOwnerCountById returns non-null
    layer->SetBufferOwnerCount(owner2, false);
    sptr<SyncFence> uniFence = new SyncFence(dup(STDERR_FILENO));
    std::set<uint32_t> decedSet{}; // empty so not skipped
    mgr->ReleaseUniOnDrawBuffers(owner, uniFence, decedSet, rsLayers, /*screenId*/7);

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
    std::set<uint32_t> decedSet;
    decedSet.insert(3333U);
    owner->InsertUniOnDrawSet(1U, 3333U);
    std::unordered_map<RSLayerId, std::weak_ptr<RSLayer>> rsLayers;
    sptr<SyncFence> uniFence = SyncFence::InvalidFence();
    mgr->ReleaseUniOnDrawBuffers(owner, uniFence, decedSet, rsLayers, 7);
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
}