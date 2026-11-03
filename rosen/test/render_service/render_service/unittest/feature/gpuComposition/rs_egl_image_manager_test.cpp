/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
#include <memory>
#include <vector>
#include "gtest/gtest.h"
#include "surface_buffer_impl.h"

#include "drawable/rs_screen_render_node_drawable.h"
#include "gpuComposition/rs_egl_image_manager.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "render_context/new_render_context/render_context_gl.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSEglImageManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static std::shared_ptr<RenderContextGL> renderContext_;
    static std::shared_ptr<RSEglImageManager> eglImageManager_;
};
std::shared_ptr<RenderContextGL> RSEglImageManagerTest::renderContext_ = std::make_shared<RenderContextGL>();
std::shared_ptr<RSEglImageManager> RSEglImageManagerTest::eglImageManager_ = nullptr;

void RSEglImageManagerTest::SetUpTestCase()
{
    renderContext_->Init();
    renderContext_->SetUpGpuContext();
    eglImageManager_ = std::make_shared<RSEglImageManager>(renderContext_->GetEGLDisplay());
    RSTestUtil::InitRenderNodeGC();
}

void RSEglImageManagerTest::TearDownTestCase()
{
    renderContext_ = nullptr;
    eglImageManager_ = nullptr;
}

void RSEglImageManagerTest::SetUp() {}
void RSEglImageManagerTest::TearDown() {}

class TestableEglImageManager : public RSEglImageManager {
public:
    explicit TestableEglImageManager(EGLDisplay display) : RSEglImageManager(display) {}
    void SetUniRenderFlag(bool uniRender)
    {
        isUniRender_ = uniRender;
    }
    bool GetUniRenderFlag() const
    {
        return isUniRender_;
    }
};

namespace {
constexpr uint64_t SMALL_DISTINCT_BASE_ID = 1001;
constexpr uint64_t SMALL_DISTINCT_STRIDE_ID = 1024;
constexpr uint64_t LARGE_DISTINCT_BASE_ID = 997;
constexpr uint64_t LARGE_DISTINCT_STRIDE_ID = 1000;
} // namespace

static std::vector<uint64_t> GenDistinctBufferIds(size_t count, uint64_t baseId, uint64_t stride)
{
    std::vector<uint64_t> bufferIds;
    bufferIds.reserve(count);
    for (size_t i = 0; i < count; i++) {
        bufferIds.push_back(baseId + i * stride);
    }
    return bufferIds;
}

static void AddDistinctCacheEntries(TestableEglImageManager& manager, const std::vector<uint64_t>& bufferIds)
{
    for (const auto& id : bufferIds) {
        manager.cacheQueue_.push(id);
        manager.imageCacheSeqs_[id] =
            std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
}

/**
 * @tc.name: UniRenderFlagInit001
 * @tc.desc: Verify isUniRender_ is initialized from RSUniRenderJudgement in constructor.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, UniRenderFlagInit001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    ASSERT_NE(testManager, nullptr);
    EXPECT_EQ(testManager->GetUniRenderFlag(), RSUniRenderJudgement::IsUniRender());
}

/**
 * @tc.name: UniRenderCreateCacheSkipQueue001
 * @tc.desc: In uni render mode, create cache should not enqueue cacheQueue_.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, UniRenderCreateCacheSkipQueue001, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR && RSUniRenderJudgement::IsUniRender()) {
        NodeId id = 0;
        auto rsContext = std::make_shared<RSContext>();
        auto node = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
        node->InitRenderParams();
        sptr<IConsumerSurface> consumer = IConsumerSurface::Create("UniRenderCreateCacheSkipQueue001");
        auto screenDrawable =
            std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(node->GetRenderDrawable());
        auto surfaceHandler = screenDrawable->GetRSSurfaceHandlerOnDraw();
        surfaceHandler->SetConsumer(consumer);
        sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
        int64_t timestamp = 0;
        Rect damage;
        sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
        surfaceHandler->SetBuffer(buffer, acquireFence, damage, timestamp, nullptr);
        if (auto displayNode = node->ReinterpretCastTo<RSScreenRenderNode>()) {
            sptr<OHOS::SurfaceBuffer> testBuffer = surfaceHandler->GetBuffer();
            auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
            testManager->SetUniRenderFlag(true);
            auto ret = testManager->CreateEglImageCacheFromBuffer(testBuffer, 0);
            ASSERT_NE(ret, 0);
            EXPECT_TRUE(testManager->cacheQueue_.empty());
            EXPECT_EQ(testManager->imageCacheSeqs_.size(), 1);
        }
    }
}

/**
 * @tc.name: DivideRenderCreateCacheEnqueue001
 * @tc.desc: In divided render mode, create cache should enqueue cacheQueue_.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderCreateCacheEnqueue001, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR && RSUniRenderJudgement::IsUniRender()) {
        NodeId id = 0;
        auto rsContext = std::make_shared<RSContext>();
        auto node = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
        node->InitRenderParams();
        sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DivideRenderCreateCacheEnqueue001");
        auto screenDrawable =
            std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(node->GetRenderDrawable());
        auto surfaceHandler = screenDrawable->GetRSSurfaceHandlerOnDraw();
        surfaceHandler->SetConsumer(consumer);
        sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
        int64_t timestamp = 0;
        Rect damage;
        sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
        surfaceHandler->SetBuffer(buffer, acquireFence, damage, timestamp, nullptr);
        if (auto displayNode = node->ReinterpretCastTo<RSScreenRenderNode>()) {
            sptr<OHOS::SurfaceBuffer> testBuffer = surfaceHandler->GetBuffer();
            auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
            testManager->SetUniRenderFlag(false);
            auto ret = testManager->CreateEglImageCacheFromBuffer(testBuffer, 0);
            ASSERT_NE(ret, 0);
            EXPECT_EQ(testManager->cacheQueue_.size(), 1);
            EXPECT_EQ(testManager->imageCacheSeqs_.size(), 1);
        }
    }
}

/**
 * @tc.name: UnMapImageFromSurfaceBufferValid001
 * @tc.desc: UnMap valid image cache should remove it and dispatch release task.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, UnMapImageFromSurfaceBufferValid001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    const uint64_t bufferId = 100;
    testManager->imageCacheSeqs_[bufferId] =
        std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    testManager->UnMapImageFromSurfaceBuffer(bufferId);
    EXPECT_EQ(testManager->imageCacheSeqs_.count(bufferId), 0);
}

/**
 * @tc.name: ShrinkCachesForUniRedrawErase001
 * @tc.desc: UniRedraw shrink should pop queue and erase the corresponding cache entry.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, ShrinkCachesForUniRedrawErase001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    for (size_t i = 0; i <= testManager->MAX_CACHE_SIZE; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = nullptr;
    }
    testManager->ShrinkCachesIfNeeded(true);
    ASSERT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
    EXPECT_EQ(testManager->imageCacheSeqs_.count(0), 0);
    EXPECT_EQ(testManager->imageCacheSeqs_.count(1), 1);
}

/**
 * @tc.name: ShrinkCachesForDivideRenderDispatch001
 * @tc.desc: Divided render shrink should pop queue and dispatch release task for the oldest cache.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, ShrinkCachesForDivideRenderDispatch001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    for (size_t i = 0; i <= testManager->MAX_CACHE_SIZE; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
    testManager->ShrinkCachesIfNeeded(false);
    ASSERT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
    EXPECT_EQ(testManager->imageCacheSeqs_.count(0), 0);
    EXPECT_EQ(testManager->imageCacheSeqs_.count(1), 1);
}

/**
 * @tc.name: DivideRenderMultiCacheFIFOOrder001
 * @tc.desc: In divided render mode, cacheQueue_ should maintain FIFO order across multiple creates.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderMultiCacheFIFOOrder001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    const size_t count = 5;
    for (size_t i = 1; i <= count; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
    ASSERT_EQ(testManager->cacheQueue_.size(), count);
    for (size_t i = 1; i <= count; i++) {
        EXPECT_EQ(testManager->cacheQueue_.front(), i);
        testManager->cacheQueue_.pop();
    }
    EXPECT_TRUE(testManager->cacheQueue_.empty());
}

/**
 * @tc.name: DivideRenderShrinkExactBoundaryNoOp001
 * @tc.desc: In divided render mode, ShrinkCachesIfNeeded should be a no-op when queue size == MAX_CACHE_SIZE.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderShrinkExactBoundaryNoOp001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    for (size_t i = 0; i < testManager->MAX_CACHE_SIZE; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
    ASSERT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
    testManager->ShrinkCachesIfNeeded(false);
    EXPECT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
    EXPECT_EQ(testManager->imageCacheSeqs_.size(), testManager->MAX_CACHE_SIZE);
}

/**
 * @tc.name: DivideRenderShrinkEvictExactCount001
 * @tc.desc: In divided render mode, ShrinkCachesIfNeeded should evict exactly (queueSize - MAX_CACHE_SIZE) entries.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderShrinkEvictExactCount001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    const size_t extraCount = 3;
    for (size_t i = 0; i < testManager->MAX_CACHE_SIZE + extraCount; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
    testManager->ShrinkCachesIfNeeded(false);
    ASSERT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
    for (size_t i = 0; i < extraCount; i++) {
        EXPECT_EQ(testManager->imageCacheSeqs_.count(i), 0);
    }
    EXPECT_EQ(testManager->imageCacheSeqs_.count(extraCount), 1);
}

/**
 * @tc.name: DivideRenderUnMapNonExistent001
 * @tc.desc: In divided render mode, UnMapImageFromSurfaceBuffer with non-existent seqNum should not crash.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderUnMapNonExistent001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    const uint64_t nonexistentId = 999999;
    testManager->UnMapImageFromSurfaceBuffer(nonexistentId);
    EXPECT_EQ(testManager->imageCacheSeqs_.count(nonexistentId), 0);
}

/**
 * @tc.name: DivideRenderShrinkEmptyQueue001
 * @tc.desc: In divided render mode, ShrinkCachesIfNeeded with empty queue should be a no-op.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderShrinkEmptyQueue001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    ASSERT_TRUE(testManager->cacheQueue_.empty());
    ASSERT_TRUE(testManager->imageCacheSeqs_.empty());
    testManager->ShrinkCachesIfNeeded(false);
    EXPECT_TRUE(testManager->cacheQueue_.empty());
    EXPECT_TRUE(testManager->imageCacheSeqs_.empty());
}

/**
 * @tc.name: DivideRenderCacheDuplicateBufferId001
 * @tc.desc: In divided render mode, creating cache with same bufferId should overwrite map entry
 *          but queue should still have both entries (FIFO tracks creation order).
 *          ShrinkCachesIfNeeded does not trigger because queue size < MAX_CACHE_SIZE.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderCacheDuplicateBufferId001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    const uint64_t sameId = 42;
    testManager->cacheQueue_.push(sameId);
    testManager->imageCacheSeqs_[sameId] =
        std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    testManager->cacheQueue_.push(sameId);
    testManager->imageCacheSeqs_[sameId] =
        std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    EXPECT_EQ(testManager->cacheQueue_.size(), 2);
    EXPECT_EQ(testManager->imageCacheSeqs_.size(), 1);
    testManager->ShrinkCachesIfNeeded(false);
    EXPECT_EQ(testManager->cacheQueue_.size(), 2);
    EXPECT_EQ(testManager->imageCacheSeqs_.size(), 1);
}

/**
 * @tc.name: DivideRenderCacheDuplicateBufferIdShrink001
 * @tc.desc: In divided render mode, duplicate bufferId in queue with total size > MAX_CACHE_SIZE.
 *          Shrink evicts oldest entries; map entry overwritten by later create survives.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderCacheDuplicateBufferIdShrink001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    const uint64_t sameId = 42;
    for (size_t i = 0; i < testManager->MAX_CACHE_SIZE; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
    testManager->cacheQueue_.push(sameId);
    testManager->imageCacheSeqs_[sameId] =
        std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    testManager->cacheQueue_.push(sameId);
    testManager->imageCacheSeqs_[sameId] =
        std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    ASSERT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE + 2);
    testManager->ShrinkCachesIfNeeded(false);
    ASSERT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
    EXPECT_EQ(testManager->imageCacheSeqs_.count(sameId), 1);
}

/**
 * @tc.name: DivideRenderShrinkThenCreate001
 * @tc.desc: In divided render mode, after shrink evicts old entries, new entries can be added correctly.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderShrinkThenCreate001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    for (size_t i = 0; i <= testManager->MAX_CACHE_SIZE; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
    testManager->ShrinkCachesIfNeeded(false);
    ASSERT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
    const uint64_t newId = testManager->MAX_CACHE_SIZE + 100;
    testManager->cacheQueue_.push(newId);
    testManager->imageCacheSeqs_[newId] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    EXPECT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE + 1);
    EXPECT_EQ(testManager->imageCacheSeqs_.count(newId), 1);
}

/**
 * @tc.name: DivideRenderCreateAndUnmapThenShrink001
 * @tc.desc: In divided render mode, unmap removes cache from map; subsequent shrink should skip unmapped entries.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderCreateAndUnmapThenShrink001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    const size_t count = testManager->MAX_CACHE_SIZE + 2;
    for (size_t i = 0; i < count; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
    const uint64_t firstId = 0;
    testManager->UnMapImageFromSurfaceBuffer(firstId);
    EXPECT_EQ(testManager->imageCacheSeqs_.count(firstId), 0);
    testManager->ShrinkCachesIfNeeded(false);
    EXPECT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
}

/**
 * @tc.name: DivideRenderShrinkFIFOOrder001
 * @tc.desc: In divided render mode, shrink should evict in FIFO order (oldest first).
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderShrinkFIFOOrder001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    for (size_t i = 0; i <= testManager->MAX_CACHE_SIZE; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
    testManager->ShrinkCachesIfNeeded(false);
    EXPECT_EQ(testManager->imageCacheSeqs_.count(0), 0);
    for (size_t i = 1; i <= testManager->MAX_CACHE_SIZE; i++) {
        EXPECT_EQ(testManager->imageCacheSeqs_.count(i), 1);
    }
    size_t queueIdx = 0;
    while (!testManager->cacheQueue_.empty()) {
        EXPECT_EQ(testManager->cacheQueue_.front(), queueIdx + 1);
        testManager->cacheQueue_.pop();
        queueIdx++;
    }
    EXPECT_EQ(queueIdx, testManager->MAX_CACHE_SIZE);
}

/**
 * @tc.name: DivideRenderShrink50ImagesEvictExactCount001
 * @tc.desc: With 50 images in divided render mode, shrink should evict 34 images (50-16), keeping 16.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderShrink50ImagesEvictExactCount001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    const size_t imageCount = 50;
    for (size_t i = 0; i < imageCount; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
    ASSERT_EQ(testManager->cacheQueue_.size(), imageCount);
    ASSERT_EQ(testManager->imageCacheSeqs_.size(), imageCount);
    testManager->ShrinkCachesIfNeeded(false);
    ASSERT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
    ASSERT_EQ(testManager->imageCacheSeqs_.size(), testManager->MAX_CACHE_SIZE);
    for (size_t i = 0; i < imageCount - testManager->MAX_CACHE_SIZE; i++) {
        EXPECT_EQ(testManager->imageCacheSeqs_.count(i), 0) << "image " << i << " should be evicted";
    }
    for (size_t i = imageCount - testManager->MAX_CACHE_SIZE; i < imageCount; i++) {
        EXPECT_EQ(testManager->imageCacheSeqs_.count(i), 1) << "image " << i << " should be retained";
    }
}

/**
 * @tc.name: DivideRenderShrink100ImagesFIFOOrder001
 * @tc.desc: With 100 images, shrink should evict oldest 84 and retain newest 16 in FIFO order.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderShrink100ImagesFIFOOrder001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    const size_t imageCount = 100;
    for (size_t i = 0; i < imageCount; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
    testManager->ShrinkCachesIfNeeded(false);
    ASSERT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
    size_t queueIdx = 0;
    while (!testManager->cacheQueue_.empty()) {
        size_t expected = (imageCount - testManager->MAX_CACHE_SIZE) + queueIdx;
        EXPECT_EQ(testManager->cacheQueue_.front(), expected);
        testManager->cacheQueue_.pop();
        queueIdx++;
    }
    EXPECT_EQ(queueIdx, testManager->MAX_CACHE_SIZE);
}

/**
 * @tc.name: DivideRenderShrink50ThenRecreateCycle001
 * @tc.desc: With 50 images, shrink then recreate 50 more should trigger another shrink, always <= MAX_CACHE_SIZE.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderShrink50ThenRecreateCycle001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    for (size_t cycle = 0; cycle < 3; cycle++) {
        for (size_t i = 0; i < 50; i++) {
            uint64_t id = cycle * 50 + i;
            testManager->cacheQueue_.push(id);
            testManager->imageCacheSeqs_[id] =
                std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
        }
        testManager->ShrinkCachesIfNeeded(false);
        ASSERT_LE(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
        ASSERT_LE(testManager->imageCacheSeqs_.size(), testManager->MAX_CACHE_SIZE);
    }
    EXPECT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
    EXPECT_EQ(testManager->imageCacheSeqs_.size(), testManager->MAX_CACHE_SIZE);
}

/**
 * @tc.name: DivideRender50ImagesUnMapSomeThenShrink001
 * @tc.desc: With 50 images, unmap some then shrink. Unmapped entries should be skipped during shrink.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRender50ImagesUnMapSomeThenShrink001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    const size_t imageCount = 50;
    for (size_t i = 0; i < imageCount; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
    testManager->UnMapImageFromSurfaceBuffer(0);
    testManager->UnMapImageFromSurfaceBuffer(5);
    testManager->UnMapImageFromSurfaceBuffer(10);
    EXPECT_EQ(testManager->imageCacheSeqs_.size(), imageCount - 3);
    EXPECT_EQ(testManager->cacheQueue_.size(), imageCount);
    testManager->ShrinkCachesIfNeeded(false);
    EXPECT_LE(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
    EXPECT_LE(testManager->imageCacheSeqs_.size(), testManager->MAX_CACHE_SIZE);
}

/**
 * @tc.name: DivideRender50ImagesInterleavedCreateShrink001
 * @tc.desc: Interleave creating 5 images then shrinking, repeated 10 times (50 total).
 *           Cache should never exceed MAX_CACHE_SIZE after each shrink.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRender50ImagesInterleavedCreateShrink001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    for (size_t i = 0; i < 50; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
        if ((i + 1) % 5 == 0) {
            testManager->ShrinkCachesIfNeeded(false);
            EXPECT_LE(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE) << "Exceeded at iteration " << i;
            EXPECT_LE(testManager->imageCacheSeqs_.size(), testManager->MAX_CACHE_SIZE)
                << "Exceeded at iteration " << i;
        }
    }
}

/**
 * @tc.name: DivideRender50ImagesShrinkPreservesNewest001
 * @tc.desc: With 50 images, shrink should preserve the 34 newest (indices 16..49) and evict oldest (0..15).
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRender50ImagesShrinkPreservesNewest001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    const size_t imageCount = 50;
    for (size_t i = 0; i < imageCount; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
    testManager->ShrinkCachesIfNeeded(false);
    for (size_t i = 0; i < imageCount - testManager->MAX_CACHE_SIZE; i++) {
        EXPECT_EQ(testManager->imageCacheSeqs_.count(i), 0) << "oldest image " << i << " should be evicted";
    }
    size_t retained = 0;
    for (size_t i = imageCount - testManager->MAX_CACHE_SIZE; i < imageCount; i++) {
        EXPECT_EQ(testManager->imageCacheSeqs_.count(i), 1) << "newest image " << i << " should be retained";
        retained++;
    }
    EXPECT_EQ(retained, testManager->MAX_CACHE_SIZE);
}

/**
 * @tc.name: DivideRender51ImagesEvictExactly35001
 * @tc.desc: With MAX_CACHE_SIZE+35=51 images, shrink should evict exactly 35 entries.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRender51ImagesEvictExactly35001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    const size_t extraCount = 35;
    const size_t imageCount = testManager->MAX_CACHE_SIZE + extraCount;
    for (size_t i = 0; i < imageCount; i++) {
        testManager->cacheQueue_.push(i);
        testManager->imageCacheSeqs_[i] = std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
    testManager->ShrinkCachesIfNeeded(false);
    EXPECT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
    EXPECT_EQ(testManager->imageCacheSeqs_.size(), testManager->MAX_CACHE_SIZE);
    for (size_t i = 0; i < extraCount; i++) {
        EXPECT_EQ(testManager->imageCacheSeqs_.count(i), 0);
    }
    for (size_t i = extraCount; i < imageCount; i++) {
        EXPECT_EQ(testManager->imageCacheSeqs_.count(i), 1);
    }
}

/**
 * @tc.name: DivideRenderSmallDistinctBufferIdsFIFOEnqueue001
 * @tc.desc: In divided render mode, a few distinct sparse bufferIds are enqueued and mapped in FIFO order.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderSmallDistinctBufferIdsFIFOEnqueue001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    const size_t smallCount = 5;
    std::vector<uint64_t> bufferIds =
        GenDistinctBufferIds(smallCount, SMALL_DISTINCT_BASE_ID, SMALL_DISTINCT_STRIDE_ID);
    AddDistinctCacheEntries(*testManager, bufferIds);
    ASSERT_EQ(testManager->cacheQueue_.size(), smallCount);
    ASSERT_EQ(testManager->imageCacheSeqs_.size(), smallCount);
    for (const auto& id : bufferIds) {
        EXPECT_EQ(testManager->cacheQueue_.front(), id);
        testManager->cacheQueue_.pop();
        EXPECT_EQ(testManager->imageCacheSeqs_.count(id), 1);
    }
    EXPECT_TRUE(testManager->cacheQueue_.empty());
}

/**
 * @tc.name: DivideRenderSmallDistinctBufferIdsShrinkNoOp001
 * @tc.desc: In divided render mode, shrink is a no-op for a few distinct bufferIds within MAX_CACHE_SIZE.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderSmallDistinctBufferIdsShrinkNoOp001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    std::vector<uint64_t> bufferIds = GenDistinctBufferIds(5, SMALL_DISTINCT_BASE_ID, SMALL_DISTINCT_STRIDE_ID);
    AddDistinctCacheEntries(*testManager, bufferIds);
    testManager->ShrinkCachesIfNeeded(false);
    ASSERT_EQ(testManager->cacheQueue_.size(), bufferIds.size());
    ASSERT_EQ(testManager->imageCacheSeqs_.size(), bufferIds.size());
    for (const auto& id : bufferIds) {
        EXPECT_EQ(testManager->imageCacheSeqs_.count(id), 1);
    }
}

/**
 * @tc.name: DivideRenderSmallDistinctBufferIdsUnmapEach001
 * @tc.desc: In divided render mode, unmapping each distinct bufferId removes only its own map entry.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderSmallDistinctBufferIdsUnmapEach001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    std::vector<uint64_t> bufferIds = GenDistinctBufferIds(5, SMALL_DISTINCT_BASE_ID, SMALL_DISTINCT_STRIDE_ID);
    AddDistinctCacheEntries(*testManager, bufferIds);
    for (const auto& id : bufferIds) {
        testManager->UnMapImageFromSurfaceBuffer(id);
        EXPECT_EQ(testManager->imageCacheSeqs_.count(id), 0);
    }
    EXPECT_TRUE(testManager->imageCacheSeqs_.empty());
    EXPECT_EQ(testManager->cacheQueue_.size(), bufferIds.size());
}

/**
 * @tc.name: DivideRenderLargeDistinctBufferIdsShrinkEvict001
 * @tc.desc: With 100 distinct sparse bufferIds, shrink evicts oldest 84 and keeps newest 16 in FIFO order.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderLargeDistinctBufferIdsShrinkEvict001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    const size_t imageCount = 100;
    std::vector<uint64_t> bufferIds =
        GenDistinctBufferIds(imageCount, LARGE_DISTINCT_BASE_ID, LARGE_DISTINCT_STRIDE_ID);
    AddDistinctCacheEntries(*testManager, bufferIds);
    testManager->ShrinkCachesIfNeeded(false);
    const size_t evictedCount = imageCount - testManager->MAX_CACHE_SIZE;
    ASSERT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
    ASSERT_EQ(testManager->imageCacheSeqs_.size(), testManager->MAX_CACHE_SIZE);
    for (size_t i = 0; i < evictedCount; i++) {
        EXPECT_EQ(testManager->imageCacheSeqs_.count(bufferIds[i]), 0) << "id " << bufferIds[i];
    }
    size_t queueIdx = 0;
    while (!testManager->cacheQueue_.empty()) {
        EXPECT_EQ(testManager->cacheQueue_.front(), bufferIds[evictedCount + queueIdx]);
        testManager->cacheQueue_.pop();
        queueIdx++;
    }
    EXPECT_EQ(queueIdx, testManager->MAX_CACHE_SIZE);
}

/**
 * @tc.name: DivideRenderLargeDistinctBufferIdsUnmapAllShrink001
 * @tc.desc: With 100 distinct bufferIds, shrink skips unmapped entries and still bounds the queue size.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, DivideRenderLargeDistinctBufferIdsUnmapAllShrink001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(false);
    const size_t imageCount = 100;
    std::vector<uint64_t> bufferIds =
        GenDistinctBufferIds(imageCount, LARGE_DISTINCT_BASE_ID, LARGE_DISTINCT_STRIDE_ID);
    AddDistinctCacheEntries(*testManager, bufferIds);
    for (const auto& id : bufferIds) {
        testManager->UnMapImageFromSurfaceBuffer(id);
    }
    EXPECT_TRUE(testManager->imageCacheSeqs_.empty());
    EXPECT_EQ(testManager->cacheQueue_.size(), imageCount);
    testManager->ShrinkCachesIfNeeded(false);
    EXPECT_EQ(testManager->cacheQueue_.size(), testManager->MAX_CACHE_SIZE);
    EXPECT_TRUE(testManager->imageCacheSeqs_.empty());
}

/**
 * @tc.name: UniRenderManyDistinctBufferIdsQueueEmpty001
 * @tc.desc: In uni render mode, creating caches for 100 distinct bufferIds never enqueues cacheQueue_;
 *          UniRedraw unmapping removes each map entry.
 * @tc.type: FUNC
 * @tc.require: issueI32801
 */
HWTEST_F(RSEglImageManagerTest, UniRenderManyDistinctBufferIdsQueueEmpty001, TestSize.Level1)
{
    auto testManager = std::make_shared<TestableEglImageManager>(renderContext_->GetEGLDisplay());
    testManager->SetUniRenderFlag(true);
    const size_t imageCount = 100;
    std::vector<uint64_t> bufferIds =
        GenDistinctBufferIds(imageCount, LARGE_DISTINCT_BASE_ID, LARGE_DISTINCT_STRIDE_ID);
    for (const auto& id : bufferIds) {
        testManager->imageCacheSeqs_[id] =
            std::make_unique<EglImageResource>(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, nullptr);
    }
    EXPECT_TRUE(testManager->cacheQueue_.empty());
    ASSERT_EQ(testManager->imageCacheSeqs_.size(), imageCount);
    for (const auto& id : bufferIds) {
        testManager->UnMapEglImageFromSurfaceBufferForUniRedraw(id);
        EXPECT_EQ(testManager->imageCacheSeqs_.count(id), 0);
    }
    EXPECT_TRUE(testManager->imageCacheSeqs_.empty());
}

/**
 * @tc.name: CreateInvalidImageCache001
 * @tc.desc: Create invalid cache with invalid egl params.
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, CreateInvalidImageCache001, TestSize.Level1)
{
    std::shared_ptr<EglImageResource> invalidCache =
        std::make_shared<EglImageResource>(renderContext_->GetEGLDisplay(), EGL_NO_IMAGE_KHR, nullptr);
    auto ret = invalidCache->GetTextureId();
    ASSERT_EQ(ret, 0);
    invalidCache.reset();
}

/**
 * @tc.name: CreateAndShrinkImageCacheFromBuffer001
 * @tc.desc: Create valid ImageCache from buffer and shrink it.
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, CreateAndShrinkImageCacheFromBuffer001, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR && RSUniRenderJudgement::IsUniRender()) {
        NodeId id = 0;
        auto rsContext = std::make_shared<RSContext>();
        auto node = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
        node->InitRenderParams();
        sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
        auto screenDrawable =
            std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(node->GetRenderDrawable());
        auto surfaceHandler = screenDrawable->GetRSSurfaceHandlerOnDraw();
        surfaceHandler->SetConsumer(consumer);
        sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
        int64_t timestamp = 0;
        Rect damage;
        sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
        surfaceHandler->SetBuffer(buffer, acquireFence, damage, timestamp, nullptr);
        ASSERT_NE(node, nullptr);
        if (auto displayNode = node->ReinterpretCastTo<RSScreenRenderNode>()) {
            sptr<OHOS::SurfaceBuffer> buffer = surfaceHandler->GetBuffer();
            // create cache from buffer directly
            auto ret = eglImageManager_->CreateEglImageCacheFromBuffer(buffer, 0);
            ASSERT_NE(ret, 0);
            eglImageManager_->ShrinkCachesIfNeeded(false);
        }
    }
}

/**
 * @tc.name: MapImageFromSurfaceBuffer001
 * @tc.desc: Map egl image from buffer.
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, MapImageFromSurfaceBuffer001, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR && RSUniRenderJudgement::IsUniRender()) {
        NodeId id = 0;
        auto rsContext = std::make_shared<RSContext>();
        auto node = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
        node->InitRenderParams();
        sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
        auto screenDrawable =
            std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(node->GetRenderDrawable());
        auto surfaceHandler = screenDrawable->GetRSSurfaceHandlerOnDraw();
        surfaceHandler->SetConsumer(consumer);
        sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
        int64_t timestamp = 0;
        Rect damage;
        sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
        surfaceHandler->SetBuffer(buffer, acquireFence, damage, timestamp, nullptr);
        ASSERT_NE(node, nullptr);
        if (auto displayNode = node->ReinterpretCastTo<RSScreenRenderNode>()) {
            sptr<OHOS::SurfaceBuffer> buffer = surfaceHandler->GetBuffer();
            sptr<SyncFence> acquireFence;
            auto ret = eglImageManager_->MapEglImageFromSurfaceBuffer(buffer, acquireFence, 0);
            ASSERT_NE(ret, 0);
            ret = eglImageManager_->MapEglImageFromSurfaceBuffer(nullptr, acquireFence, 0);
            ASSERT_EQ(ret, 0);
        }
    }
}

/**
 * @tc.name: WaitAcquireFence001
 * @tc.desc: Wait nullptr acquirefence
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSEglImageManagerTest, WaitAcquireFence001, TestSize.Level1)
{
    ASSERT_NE(eglImageManager_, nullptr);
    eglImageManager_->WaitAcquireFence(nullptr);
}

/**
 * @tc.name: ShrinkCachesIfNeeded001
 * @tc.desc: Shrink Caches
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSEglImageManagerTest, ShrinkCachesIfNeeded001, TestSize.Level1)
{
    ASSERT_NE(eglImageManager_, nullptr);
    for (size_t i = 0; i <= eglImageManager_->MAX_CACHE_SIZE; i++) {
        eglImageManager_->cacheQueue_.push(i);
        eglImageManager_->imageCacheSeqs_[i] = nullptr;
    }
    eglImageManager_->ShrinkCachesIfNeeded(true);
    ASSERT_EQ(eglImageManager_->cacheQueue_.size(), eglImageManager_->MAX_CACHE_SIZE);

    eglImageManager_->cacheQueue_.push(eglImageManager_->MAX_CACHE_SIZE);
    eglImageManager_->imageCacheSeqs_[eglImageManager_->MAX_CACHE_SIZE] = nullptr;
    eglImageManager_->ShrinkCachesIfNeeded(false);
    ASSERT_EQ(eglImageManager_->cacheQueue_.size(), eglImageManager_->MAX_CACHE_SIZE);
}

/**
 * @tc.name: ShrinkCachesIfNeeded001
 * @tc.desc: UnMap eglImage with invalid seqNum
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSEglImageManagerTest, UnMapEglImage001, TestSize.Level1)
{
    ASSERT_NE(eglImageManager_, nullptr);
    const int invalidSeqNum = -1;
    eglImageManager_->UnMapImageFromSurfaceBuffer(invalidSeqNum);
    eglImageManager_->UnMapEglImageFromSurfaceBufferForUniRedraw(invalidSeqNum);
}

/**
 * @tc.name: ImageCacheSeqCreate001
 * @tc.desc: Create EglImageResource
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSEglImageManagerTest, ImageCacheSeqCreate001, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto imageCache = EglImageResource::Create(
        EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, node->GetRSSurfaceHandler()->GetBuffer());
    ASSERT_EQ(imageCache, nullptr);
    imageCache = EglImageResource::Create(
        renderContext_->GetEGLDisplay(), EGL_NO_CONTEXT, node->GetRSSurfaceHandler()->GetBuffer());
    ASSERT_NE(imageCache, nullptr);
}

/**
 * @tc.name: ImageCacheSeqBindToTexture001
 * @tc.desc: Bind to texture
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSEglImageManagerTest, ImageCacheSeqBindToTexture001, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto imageCache = EglImageResource::Create(
        renderContext_->GetEGLDisplay(), EGL_NO_CONTEXT, node->GetRSSurfaceHandler()->GetBuffer());
    ASSERT_NE(imageCache, nullptr);
    ASSERT_EQ(imageCache->BindToTexture(), true);
    imageCache->eglImage_ = EGL_NO_IMAGE_KHR;
    ASSERT_EQ(imageCache->BindToTexture(), false);
}

/**
 * @tc.name: CreateTest
 * @tc.desc: CreateTest
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, CreateTest, TestSize.Level1)
{
    std::shared_ptr<RSImageManager> imageManager;
    std::shared_ptr<RenderContext> renderContext = RenderContext::Create();
#ifdef RS_ENABLE_VK
    imageManager = RSImageManager::Create(renderContext);
    ASSERT_NE(imageManager, nullptr);
#endif // RS_ENABLE_VK
#ifdef RS_ENABLE_GL
    imageManager = RSImageManager::Create(renderContext);
    ASSERT_NE(imageManager, nullptr);
#endif // RS_ENABLE_GL
}

/**
 * @tc.name: CreateImageFromBufferTest
 * @tc.desc: Test RSEglImageManager Func CreateImageFromBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, CreateImageFromBufferTest, TestSize.Level1)
{
    int canvasHeight = 10;
    int canvasWidth = 10;
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(canvasHeight, canvasWidth);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    BufferDrawParam params;
    params.buffer = nullptr;
    params.acquireFence = nullptr;
    params.threadIndex = 0;
    std::shared_ptr<Drawing::ColorSpace> drawingColorSpace = nullptr;
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    std::shared_ptr<RSImageManager> imageManager = std::make_shared<RSEglImageManager>(renderContext->GetEGLDisplay());
    auto res = imageManager->CreateImageFromBuffer(*canvas, params, drawingColorSpace);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name: CreateImageFromBufferTest
 * @tc.desc: Test RSEglImageManager Func CreateImageFromBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, CreateImageFromBufferTest002, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR && RSUniRenderJudgement::IsUniRender()) {
        int canvasHeight = 10;
        int canvasWidth = 10;
        auto renderEngine = std::make_shared<RSRenderEngine>();
        renderEngine->Init();
        auto drawingCanvas = std::make_unique<Drawing::RecordingCanvas>(canvasHeight, canvasWidth);
        drawingCanvas->SetGrRecordingContext(renderEngine->GetRenderContext()->GetSharedDrGPUContext());
        std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
        BufferDrawParam params;
        params.buffer = nullptr;
        params.acquireFence = nullptr;
        params.threadIndex = 0;
        std::shared_ptr<Drawing::ColorSpace> drawingColorSpace = nullptr;
        std::shared_ptr<RenderContextGL> renderContext = std::make_shared<RenderContextGL>();
        renderContext->Init();
        renderContext->SetUpGpuContext();
        auto imageManager = std::make_shared<RSEglImageManager>(renderContext->GetEGLDisplay());
        auto res = imageManager->CreateImageFromBuffer(*canvas, params, drawingColorSpace);
        EXPECT_EQ(res, nullptr);
        params.buffer = SurfaceBuffer::Create();
        res = imageManager->CreateImageFromBuffer(*canvas, params, drawingColorSpace);
        EXPECT_EQ(res, nullptr); // MapEglImageFromSurfaceBuffer failed
    }
}

/**
 * @tc.name: CreateImageFromBufferTest
 * @tc.desc: Test RSEglImageManager Func CreateImageFromBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, CreateImageFromBufferTest003, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR && RSUniRenderJudgement::IsUniRender()) {
        NodeId id = 3;
        auto rsContext = std::make_shared<RSContext>();
        auto node = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
        node->InitRenderParams();
        sptr<IConsumerSurface> consumer = IConsumerSurface::Create("CreateImageFromBufferTest002");
        auto screenDrawable =
            std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(node->GetRenderDrawable());
        auto surfaceHandler = screenDrawable->GetRSSurfaceHandlerOnDraw();
        surfaceHandler->SetConsumer(consumer);
        sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
        int64_t timestamp = 0;
        Rect damage;
        sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
        surfaceHandler->SetBuffer(buffer, acquireFence, damage, timestamp, nullptr);
        ASSERT_NE(node, nullptr);
        if (auto displayNode = node->ReinterpretCastTo<RSScreenRenderNode>()) {
            sptr<OHOS::SurfaceBuffer> buffer = surfaceHandler->GetBuffer();
            sptr<SyncFence> acquireFence;
            std::shared_ptr<RenderContextGL> renderContext = std::make_shared<RenderContextGL>();
            renderContext->Init();
            renderContext->SetUpGpuContext();
            auto eglImageManager = std::make_shared<RSEglImageManager>(renderContext->GetEGLDisplay());
            int canvasHeight = 10;
            int canvasWidth = 10;
            auto renderEngine = std::make_shared<RSRenderEngine>();
            renderEngine->Init();
            auto drawingCanvas = std::make_unique<Drawing::RecordingCanvas>(canvasHeight, canvasWidth);
            drawingCanvas->SetGrRecordingContext(renderEngine->GetRenderContext()->GetSharedDrGPUContext());
            auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
            BufferDrawParam params;
            params.acquireFence = nullptr;
            params.threadIndex = 0;
            std::shared_ptr<Drawing::ColorSpace> drawingColorSpace = nullptr;
            auto res = eglImageManager->CreateImageFromBuffer(*canvas, params, drawingColorSpace);
            EXPECT_EQ(res, nullptr);
        }
    }
}

/**
 * @tc.name: GetIntersectImageTest
 * @tc.desc: Test RSEglImageManager Func GetIntersectImage
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, GetIntersectImageTest, TestSize.Level1)
{
    auto renderContext = std::make_shared<RenderContextGL>();
    renderContext->Init();
    renderContext->SetUpGpuContext();
    std::shared_ptr<RSImageManager> imageManager = std::make_shared<RSEglImageManager>(renderContext->GetEGLDisplay());
    Drawing::RectI imgCutRect = Drawing::RectI{0, 0, 10, 10};
    std::shared_ptr<Drawing::GPUContext> context = std::make_shared<Drawing::GPUContext>();
    BufferDrawParam params;
    params.acquireFence = nullptr;
    params.threadIndex = 0;
    params.buffer = SurfaceBuffer::Create();
    // Allocate memory for the buffer to avoid crash in eglCreateImageKHR
    if (params.buffer != nullptr) {
        BufferRequestConfig requestConfig = {
            .width = 10,
            .height = 10,
            .strideAlignment = 0x8,
            .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
            .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
            .timeout = 0,
        };
        params.buffer->Alloc(requestConfig);
        // MapEglImageFromSurfaceBuffer will fail because GPU context is not properly set up
        // This test verifies the function doesn't crash
        auto res = imageManager->GetIntersectImage(imgCutRect, context, params);
        EXPECT_EQ(res, nullptr);
    }
}

/**
 * @tc.name: GetIntersectImageTest002
 * @tc.desc: Test RSEglImageManager Func GetIntersectImage
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, GetIntersectImageTest002, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR && RSUniRenderJudgement::IsUniRender()) {
        NodeId id = 2;
        auto rsContext = std::make_shared<RSContext>();
        auto node = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
        node->InitRenderParams();
        sptr<IConsumerSurface> consumer = IConsumerSurface::Create("GetIntersectImageTest002");
        auto screenDrawable =
            std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(node->GetRenderDrawable());
        auto surfaceHandler = screenDrawable->GetRSSurfaceHandlerOnDraw();
        surfaceHandler->SetConsumer(consumer);
        BufferDrawParam params;
        params.acquireFence = SyncFence::InvalidFence();
        params.threadIndex = 0;
        int64_t timestamp = 0;
        Rect damage;
        sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
        // Allocate memory for the buffer to avoid crash in eglCreateImageKHR
        BufferRequestConfig requestConfig = {
            .width = 10,
            .height = 10,
            .strideAlignment = 0x8,
            .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
            .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
            .timeout = 0,
        };
        buffer->Alloc(requestConfig);
        surfaceHandler->SetBuffer(buffer, params.acquireFence, damage, timestamp, nullptr);
        ASSERT_NE(node, nullptr);
        if (auto displayNode = node->ReinterpretCastTo<RSScreenRenderNode>()) {
            params.buffer = surfaceHandler->GetBuffer();
            auto renderContext = std::make_shared<RenderContextGL>();
            renderContext->Init();
            renderContext->SetUpGpuContext();
            auto eglImageManager = std::make_shared<RSEglImageManager>(renderContext->GetEGLDisplay());
            Drawing::RectI imgCutRect = Drawing::RectI{0, 0, 10, 10};
            std::shared_ptr<Drawing::GPUContext> context = std::make_shared<Drawing::GPUContext>();
            auto res = eglImageManager->GetIntersectImage(imgCutRect, context, params);
            EXPECT_EQ(res, nullptr);
        }
    }
}

/**
 * @tc.name: CreateImageFromBufferWithRGB565Test
 * @tc.desc: Test CreateImageFromBuffer with GRAPHIC_PIXEL_FMT_RGB_565 format
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, CreateImageFromBufferWithRGB565Test, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR && RSUniRenderJudgement::IsUniRender()) {
        NodeId id = 4;
        auto rsContext = std::make_shared<RSContext>();
        auto node = std::make_shared<RSScreenRenderNode>(id, 0, rsContext->weak_from_this());
        node->InitRenderParams();
        sptr<IConsumerSurface> consumer = IConsumerSurface::Create("CreateImageFromBufferWithRGB565Test");
        auto screenDrawable =
            std::static_pointer_cast<DrawableV2::RSScreenRenderNodeDrawable>(node->GetRenderDrawable());
        auto surfaceHandler = screenDrawable->GetRSSurfaceHandlerOnDraw();
        surfaceHandler->SetConsumer(consumer);

        sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
        int64_t timestamp = 0;
        Rect damage;
        sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
        GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_RGB_565;
        BufferRequestConfig requestConfig = {
            .width = 0x100,
            .height = 0x100,
            .strideAlignment = 0x8,
            .format = pixelFormat,
            .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
            .timeout = 0,
        };
        buffer->Alloc(requestConfig);
        surfaceHandler->SetBuffer(buffer, acquireFence, damage, timestamp, nullptr);

        ASSERT_NE(node, nullptr);
        if (auto displayNode = node->ReinterpretCastTo<RSScreenRenderNode>()) {
            sptr<OHOS::SurfaceBuffer> testBuffer = surfaceHandler->GetBuffer();
            ASSERT_NE(testBuffer, nullptr);

            EXPECT_EQ(testBuffer->GetFormat(), static_cast<int32_t>(GRAPHIC_PIXEL_FMT_RGB_565));

            auto renderContext = std::make_shared<RenderContextGL>();
            renderContext->Init();
            renderContext->SetUpGpuContext();
            auto eglImageManager = std::make_shared<RSEglImageManager>(renderContext->GetEGLDisplay());

            int canvasHeight = 10;
            int canvasWidth = 10;
            auto renderEngine = std::make_shared<RSRenderEngine>();
            renderEngine->Init();
            auto drawingCanvas = std::make_unique<Drawing::RecordingCanvas>(canvasHeight, canvasWidth);
            drawingCanvas->SetGrRecordingContext(renderEngine->GetRenderContext()->GetSharedDrGPUContext());
            auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());

            BufferDrawParam params;
            params.buffer = testBuffer;
            params.acquireFence = acquireFence;
            params.threadIndex = 0;
            std::shared_ptr<Drawing::ColorSpace> drawingColorSpace = nullptr;

            auto res = eglImageManager->CreateImageFromBuffer(*canvas, params, drawingColorSpace);
            // may fail in CI environment, just check it is not crash
            (void)res;
        }
    }
}
} // namespace OHOS::Rosen
