/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "parameters.h"
#include "pipeline/rs_surface_render_node.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_render_node.h"
#include "skia_adapter/skia_surface.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;
class RSSubThreadCacheTest : public testing::Test {
public:
    std::shared_ptr<RSSurfaceRenderNode> renderNode_;
    std::shared_ptr<RSScreenRenderNode> displayRenderNode_;
    std::shared_ptr<RSSurfaceRenderNodeDrawable> surfaceDrawable_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
    std::shared_ptr<Drawing::Canvas> drawingCanvas_;

    static inline RSUifirstManager& uifirstManager_ = RSUifirstManager::Instance();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSubThreadCacheTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void RSSubThreadCacheTest::TearDownTestCase() {}
void RSSubThreadCacheTest::SetUp()
{
    renderNode_ = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID);
    auto context = std::make_shared<RSContext>();
    displayRenderNode_ = std::make_shared<RSScreenRenderNode>(DEFAULT_ID, 0, context);
    if (!renderNode_) {
        RS_LOGE("RSSubThreadCacheTest: failed to create surface node.");
        return;
    }
    surfaceDrawable_ = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode_));
    if (!surfaceDrawable_ || !surfaceDrawable_->renderParams_) {
        RS_LOGE("RSSubThreadCacheTest: failed to init render params.");
        return;
    }
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
}
void RSSubThreadCacheTest::TearDown() {}

/**
 * @tc.name: CreateUIFirstSurfaceRenderNodeDrawableTest
 * @tc.desc: Test If UIFirstSurfaceRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSubThreadCacheTest, CreateUIFirstSurfaceRenderNodeDrawableTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
}

/**
 * @tc.name: GetCacheSurface
 * @tc.desc: Test If GetCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSubThreadCacheTest, GetCacheSurfaceTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);

    surfaceDrawable_->GetRsSubThreadCache().cacheSurface_ = std::make_shared<Drawing::Surface>();
    uint32_t threadIndex = 0;
    surfaceDrawable_->GetRsSubThreadCache().cacheSurfaceThreadIndex_ = 1;
    EXPECT_EQ(surfaceDrawable_->GetRsSubThreadCache().GetCacheSurface(threadIndex), nullptr);

    surfaceDrawable_->GetRsSubThreadCache().cacheSurfaceThreadIndex_ = threadIndex;
    EXPECT_NE(surfaceDrawable_->GetRsSubThreadCache().GetCacheSurface(threadIndex), nullptr);
}

/**
 * @tc.name: GetCompletedImage
 * @tc.desc: Test If GetCompletedImage Can Run
 * @tc.type: FUNC
 * @tc.require: #IB1MHP
 */
HWTEST_F(RSSubThreadCacheTest, GetCompletedImageTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);

    uint32_t threadIndex = 1;
    bool isUIFirst = true;
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto paintFilterCanvas = RSPaintFilterCanvas(drawingCanvas_.get());
    auto result = surfaceDrawable_->GetRsSubThreadCache().GetCompletedImage(paintFilterCanvas, threadIndex, isUIFirst);
    EXPECT_EQ(result, nullptr);

    paintFilterCanvas.canvas_->gpuContext_ = std::make_shared<Drawing::GPUContext>();
    result = surfaceDrawable_->GetRsSubThreadCache().GetCompletedImage(paintFilterCanvas, threadIndex, isUIFirst);
    ASSERT_EQ(result, nullptr);

    surfaceDrawable_->GetRsSubThreadCache().cacheCompletedBackendTexture_.isValid_ = true;
    result = surfaceDrawable_->GetRsSubThreadCache().GetCompletedImage(paintFilterCanvas, threadIndex, isUIFirst);
    ASSERT_NE(result, nullptr);

#ifdef RS_ENABLE_VK
    surfaceDrawable_->GetRsSubThreadCache().cacheCompletedSurface_ = std::make_shared<Drawing::Surface>();
    auto cacheBackendTexture_ = NativeBufferUtils::MakeBackendTexture(
        10, 10, 0, VkFormat::VK_FORMAT_A1R5G5B5_UNORM_PACK16);
    auto vkTextureInfo = cacheBackendTexture_.GetTextureInfo().GetVkTextureInfo();
    surfaceDrawable_->GetRsSubThreadCache().cacheCompletedCleanupHelper_ = new NativeBufferUtils::VulkanCleanupHelper(
        RsVulkanContext::GetSingleton, vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
    result = surfaceDrawable_->GetRsSubThreadCache().GetCompletedImage(paintFilterCanvas, threadIndex, isUIFirst);
    ASSERT_NE(result, nullptr);
    delete surfaceDrawable_->GetRsSubThreadCache().cacheCompletedCleanupHelper_;
    surfaceDrawable_->GetRsSubThreadCache().cacheCompletedCleanupHelper_ = nullptr;
#endif

    isUIFirst = false;
    result = surfaceDrawable_->GetRsSubThreadCache().GetCompletedImage(paintFilterCanvas, threadIndex, isUIFirst);
    ASSERT_EQ(result, nullptr);

    surfaceDrawable_->GetRsSubThreadCache().cacheCompletedSurface_ = Drawing::Surface::MakeRasterN32Premul(10, 10);
    result = surfaceDrawable_->GetRsSubThreadCache().GetCompletedImage(paintFilterCanvas, threadIndex, isUIFirst);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawCacheSurface
 * @tc.desc: Test If DrawCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #IB1MHP
 */
HWTEST_F(RSSubThreadCacheTest, DrawCacheSurfaceTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Vector2f boundSize;
    uint32_t threadIndex = 1;
    bool isUIFirst = false;
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto rscanvas = RSPaintFilterCanvas(drawingCanvas_.get());
    auto result = surfaceDrawable_->GetRsSubThreadCache().DrawCacheSurface(surfaceDrawable_.get(), rscanvas, boundSize,
        threadIndex, isUIFirst);
    EXPECT_FALSE(result);

    surfaceDrawable_->boundsWidth_ = 1.f;
    result = surfaceDrawable_->GetRsSubThreadCache().DrawCacheSurface(surfaceDrawable_.get(), rscanvas, boundSize,
        threadIndex, isUIFirst);
    EXPECT_FALSE(result);

    surfaceDrawable_->boundsHeight_ = 1.f;
    result = surfaceDrawable_->GetRsSubThreadCache().DrawCacheSurface(surfaceDrawable_.get(), rscanvas, boundSize,
        threadIndex, isUIFirst);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: NeedInitCacheSurface
 * @tc.desc: Test If NeedInitCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSubThreadCacheTest, NeedInitCacheSurfaceTest, TestSize.Level1)
{
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->GetRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    auto result = surfaceDrawable_->GetRsSubThreadCache().NeedInitCacheSurface(surfaceParams);
    EXPECT_TRUE(result);
    ASSERT_FALSE(surfaceDrawable_->GetRsSubThreadCache().cacheSurface_);

    surfaceDrawable_->GetRsSubThreadCache().cacheSurface_ = std::make_shared<Drawing::Surface>();
    result = surfaceDrawable_->GetRsSubThreadCache().NeedInitCacheSurface(surfaceParams);
    EXPECT_TRUE(result);
    ASSERT_FALSE(surfaceDrawable_->GetRsSubThreadCache().cacheSurface_->GetCanvas());

    surfaceDrawable_->GetRsSubThreadCache().cacheSurface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>();
    result = surfaceDrawable_->GetRsSubThreadCache().NeedInitCacheSurface(surfaceParams);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: UpdateCompletedCacheSurface
 * @tc.desc: Test If UpdateCompletedCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSubThreadCacheTest, UpdateCompletedCacheSurfaceTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    uint32_t completedSurfaceThreadIndex = 1;
    uint32_t cacheSurfaceThreadIndex = 0;
    surfaceDrawable_->GetRsSubThreadCache().completedSurfaceThreadIndex_ = completedSurfaceThreadIndex;
    surfaceDrawable_->GetRsSubThreadCache().cacheSurfaceThreadIndex_ = cacheSurfaceThreadIndex;

    surfaceDrawable_->GetRsSubThreadCache().cacheSurface_ = nullptr;
    surfaceDrawable_->GetRsSubThreadCache().UpdateCompletedCacheSurface();
    EXPECT_EQ(surfaceDrawable_->GetRsSubThreadCache().isCacheValid_, false);
    EXPECT_NE(surfaceDrawable_->GetRsSubThreadCache().completedSurfaceThreadIndex_, cacheSurfaceThreadIndex);

    surfaceDrawable_->GetRsSubThreadCache().cacheSurface_ = std::make_shared<Drawing::Surface>();
    surfaceDrawable_->GetRsSubThreadCache().isCacheValid_ = true;
    surfaceDrawable_->GetRsSubThreadCache().UpdateCompletedCacheSurface();
    EXPECT_EQ(surfaceDrawable_->GetRsSubThreadCache().completedSurfaceThreadIndex_, cacheSurfaceThreadIndex);
}

#ifdef RS_ENABLE_VK
/**
 * @tc.name: ClearCacheSurface
 * @tc.desc: Test If ClearCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSubThreadCacheTest, ClearCacheSurfaceTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    surfaceDrawable_->GetRsSubThreadCache().ClearCacheSurface(true);
    ASSERT_EQ(surfaceDrawable_->GetRsSubThreadCache().cacheCompletedCleanupHelper_, nullptr);

    surfaceDrawable_->GetRsSubThreadCache().cacheCompletedSurface_ = std::make_shared<Drawing::Surface>();
    surfaceDrawable_->GetRsSubThreadCache().ClearCacheSurface(false);
    ASSERT_EQ(surfaceDrawable_->GetRsSubThreadCache().cacheCompletedCleanupHelper_, nullptr);
}
#endif
/**
 * @tc.name: IsCurFrameStatic
 * @tc.desc: Test If IsCurFrameStatic Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSubThreadCacheTest, IsCurFrameStaticTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->GetRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    ASSERT_EQ(surfaceParams->GetSurfaceCacheContentStatic(), false);
}

/**
 * @tc.name: UpdateUifirstDirtyManagerTest
 * @tc.desc: Test UpdateUifirstDirtyManager
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSubThreadCacheTest, UpdateUifirstDirtyManagerTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    system::SetParameter("rosen.ui.first.dirty.enabled", "0");
    surfaceDrawable_->GetRsSubThreadCache().UpdateUifirstDirtyManager(surfaceDrawable_.get());
    ASSERT_NE(surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_, nullptr);
    ASSERT_EQ(surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_->currentFrameDirtyRegion_.GetWidth(),
        0);

    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    system::SetParameter("rosen.ui.first.dirty.enabled", "1");
    surfaceDrawable_->GetRsSubThreadCache().UpdateUifirstDirtyManager(surfaceDrawable_.get());
    ASSERT_EQ(surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_->currentFrameDirtyRegion_.GetWidth(),
        0);

    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::MULTI));
    system::SetParameter("rosen.ui.first.dirty.enabled", "0");
    surfaceDrawable_->GetRsSubThreadCache().UpdateUifirstDirtyManager(surfaceDrawable_.get());
    ASSERT_EQ(surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_->currentFrameDirtyRegion_.GetWidth(),
        0);

    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::MULTI));
    system::SetParameter("rosen.ui.first.dirty.enabled", "1");
    surfaceDrawable_->syncDirtyManager_->dirtyRegion_ = {0, 0, 10, 10};
    surfaceDrawable_->GetRsSubThreadCache().isCacheValid_ = true;
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->GetRenderParams().get());
    surfaceParams->allSubSurfaceNodeIds_.insert(renderNode_->GetId());
    surfaceDrawable_->GetRsSubThreadCache().UpdateUifirstDirtyManager(surfaceDrawable_.get());
    ASSERT_EQ(surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_->currentFrameDirtyRegion_.GetWidth(),
        10);

    surfaceDrawable_->GetRsSubThreadCache().isCacheValid_ = false;
    surfaceParams->absDrawRect_ = {0, 0, 15, 15};
    surfaceDrawable_->GetRsSubThreadCache().UpdateUifirstDirtyManager(surfaceDrawable_.get());
    ASSERT_EQ(surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_->currentFrameDirtyRegion_.GetWidth(),
        15);
    surfaceDrawable_->syncDirtyManager_->Clear();
    surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_->Clear();
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
}

/**
 * @tc.name: CalculateUifirstDirtyRegionTest
 * @tc.desc: Test CalculateUifirstDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSubThreadCacheTest, CalculateUifirstDirtyRegionTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    RectI dirtyRegion = {0, 0, 10, 10};
    surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_->historyHead_ = 0;
    surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_->dirtyHistory_[0]= dirtyRegion;
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->GetRenderParams().get());
    surfaceParams->absDrawRect_ = {0, 0, 15, 15};
    Drawing::RectI dirtyRect = {};
    bool isCalculateSucc = surfaceDrawable_->GetRsSubThreadCache().CalculateUifirstDirtyRegion(surfaceDrawable_.get(),
        dirtyRect);
    ASSERT_EQ(isCalculateSucc, false);
    surfaceDrawable_->syncDirtyManager_->Clear();
    surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_->Clear();
}

/**
 @tc.name: UpadteAllSurfaceUifirstDirtyEnableState
 @tc.desc: Test UpadteAllSurfaceUifirstDirtyEnableState
 @tc.type: FUNC
 @tc.require: #ICEFXX
 */
HWTEST_F(RSSubThreadCacheTest, UpadteAllSurfaceUifirstDirtyEnableState, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto dirtyEnableFlag = false;
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    system::SetParameter("rosen.ui.first.dirty.enabled", "0");
    surfaceDrawable_->GetRsSubThreadCache().UpadteAllSurfaceUifirstDirtyEnableState(
    surfaceDrawable_.get(), dirtyEnableFlag);
    ASSERT_EQ(surfaceDrawable_->GetRsSubThreadCache().GetUifrstDirtyEnableFlag(), false);

    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    system::SetParameter("rosen.ui.first.dirty.enabled", "1");
    surfaceDrawable_->GetRsSubThreadCache().UpadteAllSurfaceUifirstDirtyEnableState(
    surfaceDrawable_.get(), dirtyEnableFlag);
    ASSERT_EQ(surfaceDrawable_->GetRsSubThreadCache().GetUifrstDirtyEnableFlag(), false);

    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::MULTI));
    system::SetParameter("rosen.ui.first.dirty.enabled", "0");
    surfaceDrawable_->GetRsSubThreadCache().UpadteAllSurfaceUifirstDirtyEnableState(
    surfaceDrawable_.get(), dirtyEnableFlag);
    ASSERT_EQ(surfaceDrawable_->GetRsSubThreadCache().GetUifrstDirtyEnableFlag(), false);

    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::MULTI));
    system::SetParameter("rosen.ui.first.dirty.enabled", "1");
    surfaceDrawable_->GetRsSubThreadCache().UpadteAllSurfaceUifirstDirtyEnableState(
    surfaceDrawable_.get(), dirtyEnableFlag);
    ASSERT_EQ(surfaceDrawable_->GetRsSubThreadCache().GetUifrstDirtyEnableFlag(), false);
}

/**
 * @tc.name: MergeUifirstAllSurfaceDirtyRegionTest
 * @tc.desc: Test MergeUifirstAllSurfaceDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSubThreadCacheTest, MergeUifirstAllSurfaceDirtyRegionTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    system::SetParameter("rosen.ui.first.dirty.enabled", "0");
    Drawing::RectI dirtyRect = {};
    bool dirtyEnableFlag = surfaceDrawable_->GetRsSubThreadCache().MergeUifirstAllSurfaceDirtyRegion(
        surfaceDrawable_.get(), dirtyRect);
    ASSERT_EQ(dirtyEnableFlag, false);

    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    system::SetParameter("rosen.ui.first.dirty.enabled", "1");
    dirtyEnableFlag = surfaceDrawable_->GetRsSubThreadCache().MergeUifirstAllSurfaceDirtyRegion(
        surfaceDrawable_.get(), dirtyRect);
    ASSERT_EQ(dirtyEnableFlag, false);

    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::MULTI));
    system::SetParameter("rosen.ui.first.dirty.enabled", "0");
    dirtyEnableFlag = surfaceDrawable_->GetRsSubThreadCache().MergeUifirstAllSurfaceDirtyRegion(
        surfaceDrawable_.get(), dirtyRect);
    ASSERT_EQ(dirtyEnableFlag, false);

    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::MULTI));
    system::SetParameter("rosen.ui.first.dirty.enabled", "1");
    RectI dirtyRegion = {0, 0, 10, 10};
    surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_->historyHead_ = 0;
    surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_->dirtyHistory_[0]= dirtyRegion;
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->GetRenderParams().get());
    surfaceParams->absDrawRect_ = {0, 0, 15, 15};
    surfaceParams->allSubSurfaceNodeIds_.insert(renderNode_->GetId());
    Drawing::RectF bounds = {1, 1, 2, 2};
    surfaceParams->boundsRect_ = bounds;
    dirtyEnableFlag = surfaceDrawable_->GetRsSubThreadCache().MergeUifirstAllSurfaceDirtyRegion(
        surfaceDrawable_.get(), dirtyRect);
    ASSERT_EQ(dirtyEnableFlag, false);
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
}

/**
 * @tc.name: SubDraw
 * @tc.desc: Test If SubDraw Can Run
 * @tc.type: FUNC
 * @tc.require: #IB1MHP
 */
HWTEST_F(RSSubThreadCacheTest, SubDrawTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto rscanvas = RSPaintFilterCanvas(drawingCanvas_.get());
    surfaceDrawable_->GetRsSubThreadCache().SubDraw(nullptr, rscanvas);
    EXPECT_TRUE(surfaceDrawable_->uifirstDrawCmdList_.empty());
    surfaceDrawable_->GetRsSubThreadCache().SubDraw(surfaceDrawable_.get(), rscanvas);
    EXPECT_TRUE(surfaceDrawable_->uifirstDrawCmdList_.empty());
}

/**
 * @tc.name: DrawUIFirstCache
 * @tc.desc: Test If DrawUIFirstCache Can Run
 * @tc.type: FUNC
 * @tc.require: #IB1MHP
 */
HWTEST_F(RSSubThreadCacheTest, DrawUIFirstCacheTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subCache = surfaceDrawable_->GetRsSubThreadCache();
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto rscanvas = RSPaintFilterCanvas(drawingCanvas_.get());
    // drawable is null
    ASSERT_FALSE(subCache.DrawUIFirstCache(nullptr, rscanvas, false));

    NodeId id = 100;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    surfaceDrawable->renderParams_ = nullptr;
    // render param is null, draw cache failed
    ASSERT_FALSE(subCache.DrawUIFirstCache(nullptr, rscanvas, false));
    surfaceDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(id);
    subCache.isCacheCompletedValid_ = false;
    ASSERT_FALSE(subCache.HasCachedTexture());
    // no valid cache and skip wait sub thread, draw cache failed
    ASSERT_FALSE(subCache.DrawUIFirstCache(surfaceDrawable.get(), rscanvas, true));
    ASSERT_FALSE(subCache.HasCachedTexture());

    subCache.isCacheValid_ = true;
    subCache.cacheSurface_ = std::make_shared<Drawing::Surface>();
    // no cache, wait sub thread
    subCache.DrawUIFirstCache(surfaceDrawable.get(), rscanvas, false);
    ASSERT_TRUE(subCache.HasCachedTexture());
}

/**
 * @tc.name: DrawUIFirstCacheWithStarting
 * @tc.desc: Test If DrawUIFirstCacheWithStarting Can Run
 * @tc.type: FUNC
 * @tc.require: #IB1MHP
 */
HWTEST_F(RSSubThreadCacheTest, DrawUIFirstCacheWithStartingTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto rscanvas = static_cast<RSPaintFilterCanvas*>(drawingCanvas_.get());
    NodeId id = 0;
    auto result = surfaceDrawable_->GetRsSubThreadCache().DrawUIFirstCacheWithStarting(surfaceDrawable_.get(),
        *rscanvas, id);
    ASSERT_TRUE(result);

    id = 65535; // for test
    surfaceDrawable_->GetRsSubThreadCache().isCacheCompletedValid_ = true;
    result = surfaceDrawable_->GetRsSubThreadCache().DrawUIFirstCacheWithStarting(surfaceDrawable_.get(),
        *rscanvas, id);
    ASSERT_FALSE(result);
    ASSERT_FALSE(RSRenderNodeDrawableAdapter::RenderNodeDrawableCache_.empty());
}

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
/**
 * @tc.name: UpdateBackendTexture
 * @tc.desc: Test If UpdateBackendTexture Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSSubThreadCacheTest, UpdateBackendTextureTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    surfaceDrawable_->GetRsSubThreadCache().UpdateBackendTexture();
    ASSERT_FALSE(surfaceDrawable_->GetRsSubThreadCache().cacheSurface_);

    surfaceDrawable_->GetRsSubThreadCache().cacheSurface_ = std::make_shared<Drawing::Surface>();
    surfaceDrawable_->GetRsSubThreadCache().UpdateBackendTexture();
    ASSERT_TRUE(surfaceDrawable_->GetRsSubThreadCache().cacheSurface_);
}
#endif

/**
 * @tc.name: InitCacheSurface
 * @tc.desc: Test If InitCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSSubThreadCacheTest, InitCacheSurfaceTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext();
    DrawableV2::RsSubThreadCache::ClearCacheSurfaceFunc func;
    uint32_t threadIndex = 0;
    surfaceDrawable_->GetRsSubThreadCache().InitCacheSurface(gpuContext, surfaceDrawable_, func, threadIndex);
    ASSERT_FALSE(surfaceDrawable_->GetRsSubThreadCache().clearCacheSurfaceFunc_);

    func = [](std::shared_ptr<Drawing::Surface>&& oldSurface, std::shared_ptr<Drawing::Surface>&& newSurface,
               uint32_t width, uint32_t height) {};
    surfaceDrawable_->GetRsSubThreadCache().InitCacheSurface(gpuContext, surfaceDrawable_, func, threadIndex);
    ASSERT_TRUE(surfaceDrawable_->GetRsSubThreadCache().clearCacheSurfaceFunc_);

    surfaceDrawable_->GetRsSubThreadCache().cacheSurface_ = std::make_shared<Drawing::Surface>();
    surfaceDrawable_->GetRsSubThreadCache().InitCacheSurface(gpuContext, surfaceDrawable_, func, threadIndex);
    delete gpuContext;
    gpuContext = nullptr;
    surfaceDrawable_->GetRsSubThreadCache().InitCacheSurface(gpuContext, surfaceDrawable_, func, threadIndex);
    ASSERT_FALSE(surfaceDrawable_->GetRsSubThreadCache().cacheSurface_);
}

/**
 * @tc.name: ClearCacheSurfaceOnly
 * @tc.desc: Test If ClearCacheSurfaceOnly Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSSubThreadCacheTest, ClearCacheSurfaceOnlyTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    DrawableV2::RsSubThreadCache::ClearCacheSurfaceFunc func;
    surfaceDrawable_->GetRsSubThreadCache().ClearCacheSurfaceOnly();
    ASSERT_FALSE(surfaceDrawable_->GetRsSubThreadCache().cacheSurface_);

    surfaceDrawable_->GetRsSubThreadCache().cacheSurface_ = std::make_shared<Drawing::Surface>();
    ASSERT_TRUE(surfaceDrawable_->GetRsSubThreadCache().cacheSurface_);
    surfaceDrawable_->GetRsSubThreadCache().ClearCacheSurfaceOnly();
    ASSERT_FALSE(surfaceDrawable_->GetRsSubThreadCache().clearCacheSurfaceFunc_);

    func = [](std::shared_ptr<Drawing::Surface>&& oldSurface, std::shared_ptr<Drawing::Surface>&& newSurface,
               uint32_t width, uint32_t height) {};
    surfaceDrawable_->GetRsSubThreadCache().clearCacheSurfaceFunc_ = func;
    surfaceDrawable_->GetRsSubThreadCache().ClearCacheSurfaceOnly();
    ASSERT_TRUE(surfaceDrawable_->GetRsSubThreadCache().clearCacheSurfaceFunc_);
}

/**
 * @tc.name: ClearCacheSurfaceInThread
 * @tc.desc: Test If ClearCacheSurfaceInThread Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSSubThreadCacheTest, ClearCacheSurfaceInThreadTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    DrawableV2::RsSubThreadCache::ClearCacheSurfaceFunc func;
    surfaceDrawable_->GetRsSubThreadCache().ClearCacheSurfaceInThread();
    ASSERT_FALSE(surfaceDrawable_->GetRsSubThreadCache().clearCacheSurfaceFunc_);

    func = [](std::shared_ptr<Drawing::Surface>&& oldSurface, std::shared_ptr<Drawing::Surface>&& newSurface,
               uint32_t width, uint32_t height) {};
    surfaceDrawable_->GetRsSubThreadCache().clearCacheSurfaceFunc_ = func;
    surfaceDrawable_->GetRsSubThreadCache().ClearCacheSurfaceInThread();
    ASSERT_TRUE(surfaceDrawable_->GetRsSubThreadCache().clearCacheSurfaceFunc_);
}

/**
 * @tc.name: BufferFormatNeedUpdate
 * @tc.desc: Test BufferFormatNeedUpdate
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSubThreadCacheTest, BufferFormatNeedUpdateTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    std::shared_ptr<Drawing::Surface> surface = Drawing::Surface::MakeRasterN32Premul(100, 100);
    ASSERT_NE(surface, nullptr);
    RSPaintFilterCanvas paintFilterCanvas(surface.get());
    surfaceDrawable_->curCanvas_ = &paintFilterCanvas;
    EXPECT_TRUE(surfaceDrawable_->GetRsSubThreadCache().BufferFormatNeedUpdate(surface, true));
    EXPECT_FALSE(surfaceDrawable_->GetRsSubThreadCache().BufferFormatNeedUpdate(surface, false));
}

/**
 * @tc.name: DrawUIFirstDfx
 * @tc.desc: Test DrawUIFirstDfx
 * @tc.type: FUNC
 * @tc.require: #IA940V
 */
HWTEST_F(RSSubThreadCacheTest, DrawUIFirstDfx, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);

    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);

    surfaceDrawable_->GetRsSubThreadCache().DrawUIFirstDfx(paintFilterCanvas, MultiThreadCacheType::ARKTS_CARD,
        *surfaceParams, true);
    surfaceDrawable_->GetRsSubThreadCache().DrawUIFirstDfx(paintFilterCanvas, MultiThreadCacheType::LEASH_WINDOW,
        *surfaceParams, true);
    surfaceDrawable_->GetRsSubThreadCache().DrawUIFirstDfx(paintFilterCanvas, MultiThreadCacheType::LEASH_WINDOW,
        *surfaceParams, false);
}

/**
 * @tc.name: DrawUIFirstDfx
 * @tc.desc: Test DrawUIFirstDfx
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSubThreadCacheTest, DrawUIFirstDfxTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    MultiThreadCacheType enableType = MultiThreadCacheType::ARKTS_CARD;
    surfaceDrawable_->GetRsSubThreadCache().DrawUIFirstDfx(canvas, enableType, *surfaceParams, true);

    enableType = MultiThreadCacheType::LEASH_WINDOW;
    surfaceDrawable_->GetRsSubThreadCache().DrawUIFirstDfx(canvas, enableType, *surfaceParams, true);
    surfaceDrawable_->GetRsSubThreadCache().DrawUIFirstDfx(canvas, enableType, *surfaceParams, false);
}

/**
 * @tc.name: DealWithUIFirstCache
 * @tc.desc: Test not use uifirst cache
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSubThreadCacheTest, DealWithUIFirstCacheTest001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subThreadCache = surfaceDrawable_->GetRsSubThreadCache();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    // drawable is null
    ASSERT_FALSE(subThreadCache.DealWithUIFirstCache(nullptr, *canvas_, *surfaceParams, *uniParams));

    RSUniRenderThread::GetCaptureParam().isMirror_ = false;
    RSUniRenderThread::GetCaptureParam().isSnapshot_ = false;
    surfaceParams->SetUifirstNodeEnableParam(MultiThreadCacheType::NONE);
    subThreadCache.SetCacheSurfaceProcessedStatus(CacheProcessStatus::SKIPPED);
    // uifirst disabled and no sub thread task
    ASSERT_FALSE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));

    RSUniRenderThread::GetCaptureParam().isSnapshot_ = true;
    subThreadCache.isCacheCompletedValid_ = false;
    // no uifirst cache
    ASSERT_FALSE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));
}

/**
 * @tc.name: DealWithUIFirstCacheTest002
 * @tc.desc: Test skip drawing special nodes in the virtual screen
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSubThreadCacheTest, DealWithUIFirstCacheTest002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subThreadCache = surfaceDrawable_->GetRsSubThreadCache();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();

    RSUniRenderThread::GetCaptureParam().isSnapshot_ = false;
    surfaceParams->SetUifirstNodeEnableParam(MultiThreadCacheType::LEASH_WINDOW);
    RSUniRenderThread::GetCaptureParam().isMirror_ = true;

    uint64_t virtualScreenId = 1;
    RSUniRenderThread::GetCaptureParam().virtualScreenId_ = virtualScreenId;
    surfaceParams->blackListIds_[virtualScreenId].insert(surfaceParams->GetId());
    surfaceParams->blackListIds_[0].insert(surfaceParams->GetId());
    // node in black list
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));

    RSUniRenderThread::GetCaptureParam().virtualScreenId_ = -1;
    // node in black list
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));
    surfaceParams->blackListIds_.clear();
}

/**
 * @tc.name: DealWithUIFirstCacheTest003
 * @tc.desc: Test reuse mainthread cache
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSubThreadCacheTest, DealWithUIFirstCacheTest003, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();

    auto& subThreadCache = surfaceDrawable_->GetRsSubThreadCache();
    subThreadCache.isCacheCompletedValid_ = false;
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    int32_t width = 100;
    int32_t height = 100;
    bmp.Build(width, height, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_RED);
    subThreadCache.drawWindowCache_.image_ = bmp.MakeImage();
    surfaceParams->SetUifirstNodeEnableParam(MultiThreadCacheType::LEASH_WINDOW);
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));
}

/**
 * @tc.name: DealWithUIFirstCacheTest004
 * @tc.desc: Test reuse subthread cache
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSubThreadCacheTest, DealWithUIFirstCacheTest004, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subThreadCache = surfaceDrawable_->GetRsSubThreadCache();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();
    RSUniRenderThread::GetCaptureParam().isMirror_ = false;

    RSUniRenderThread::GetCaptureParam().isSnapshot_ = false;
    surfaceParams->SetUifirstNodeEnableParam(MultiThreadCacheType::LEASH_WINDOW);
    // uifirst enabled, draw uifirst cache
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));

    RSUniRenderThread::GetCaptureParam().isSnapshot_ = false;
    surfaceParams->SetUifirstNodeEnableParam(MultiThreadCacheType::NONE);
    subThreadCache.SetCacheSurfaceProcessedStatus(CacheProcessStatus::WAITING);
    // uifirst disabled, but we have task in waitting state, still use uifirst cache
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));

    RSUniRenderThread::GetCaptureParam().isSnapshot_ = false;
    surfaceParams->SetUifirstNodeEnableParam(MultiThreadCacheType::NONE);
    subThreadCache.SetCacheSurfaceProcessedStatus(CacheProcessStatus::DOING);
    // uifirst disabled, but we have task in doing state, still use uifirst cache
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));

    RSUniRenderThread::GetCaptureParam().isSnapshot_ = true;
    subThreadCache.isCacheCompletedValid_ = true;
    // snapshot reuse uifirst cache
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));
}

/**
 * @tc.name: DealWithUIFirstCacheTest005
 * @tc.desc: Test reuse subthread cache
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSubThreadCacheTest, DealWithUIFirstCacheTest005, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subThreadCache = surfaceDrawable_->GetRsSubThreadCache();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    auto uniParams = std::make_shared<RSRenderThreadParams>();

    RSUniRenderThread::GetCaptureParam().isMirror_ = false;
    RSUniRenderThread::GetCaptureParam().isSnapshot_ = false;
    surfaceParams->SetUifirstNodeEnableParam(MultiThreadCacheType::LEASH_WINDOW);

    surfaceParams->SetGlobalPositionEnabled(true);
    surfaceParams->SetUifirstUseStarting(0);
    surfaceParams->isLeashWindow_ = true;
    uniParams->isUIFirstDebugEnable_ = true;
    // irrevertible matrix
    surfaceParams->matrix_.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));
    // invertible matrix
    surfaceParams->matrix_.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));

    surfaceParams->SetGlobalPositionEnabled(true);
    surfaceParams->SetUifirstUseStarting(1);
    surfaceParams->isLeashWindow_ = false;
    uniParams->isUIFirstDebugEnable_ = false;
    // draw starting window
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));

    surfaceParams->SetGlobalPositionEnabled(false);
    // global position disabled
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));
}


/**
 * @tc.name: SetSubThreadSkip001
 * @tc.desc: Test SetSubThreadSkip
 * @tc.type: FUNC
 * @tc.require: issueIANDBE
 */
HWTEST_F(RSSubThreadCacheTest, SetSubThreadSkip001, TestSize.Level2)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    
    surfaceDrawable_->GetRsSubThreadCache().SetSubThreadSkip(true);
    ASSERT_TRUE(surfaceDrawable_->GetRsSubThreadCache().IsSubThreadSkip());
}

/**
 * @tc.name: CacheBehindWindowDataTest
 * @tc.desc: Test CacheBehindWindowData
 * @tc.type: FUNC
 * @tc.require: issuesIC0HM8
 */
HWTEST_F(RSSubThreadCacheTest, CacheBehindWindowDataTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto data = std::make_shared<RSPaintFilterCanvas::CacheBehindWindowData>();
    surfaceDrawable_->GetRsSubThreadCache().SetCacheBehindWindowData(data);
    ASSERT_NE(surfaceDrawable_->GetRsSubThreadCache().cacheBehindWindowData_, nullptr);
    surfaceDrawable_->GetRsSubThreadCache().ResetCacheBehindWindowData();
    ASSERT_EQ(surfaceDrawable_->GetRsSubThreadCache().cacheBehindWindowData_, nullptr);
    surfaceDrawable_->GetRsSubThreadCache().SetCacheCompletedBehindWindowData(data);
    ASSERT_NE(surfaceDrawable_->GetRsSubThreadCache().cacheCompletedBehindWindowData_, nullptr);
    surfaceDrawable_->GetRsSubThreadCache().ResetCacheCompletedBehindWindowData();
    ASSERT_EQ(surfaceDrawable_->GetRsSubThreadCache().cacheCompletedBehindWindowData_, nullptr);
}

/**
 * @tc.name: DrawBehindWindowBeforeCacheTest
 * @tc.desc: Test DrawBehindWindowBeforeCache
 * @tc.type: FUNC
 * @tc.require: issuesICFWAC
 */
HWTEST_F(RSSubThreadCacheTest, DrawBehindWindowBeforeCacheTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subThreadCache = surfaceDrawable_->GetRsSubThreadCache();
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    float scaleFactor = 2;
    canvas.Scale(scaleFactor, scaleFactor);
    // CacheBehindWindowData is null
    subThreadCache.DrawBehindWindowBeforeCache(canvas);
    ASSERT_EQ(canvas.GetTotalMatrix().Get(Drawing::Matrix::SCALE_X), scaleFactor);

    auto data = std::make_shared<RSPaintFilterCanvas::CacheBehindWindowData>();
    subThreadCache.cacheCompletedBehindWindowData_ = data;
    // filter invalid
    subThreadCache.DrawBehindWindowBeforeCache(canvas);
    ASSERT_EQ(canvas.GetTotalMatrix().Get(Drawing::Matrix::SCALE_X), scaleFactor);

    data->filter_ = RSFilter::CreateMaterialFilter(80.0f, 1.9f, 1.0f, 0xFFFFFFE5);
    data->rect_ = {200, 0, 100, 100};
    // rect invalid
    subThreadCache.DrawBehindWindowBeforeCache(canvas);
    ASSERT_EQ(canvas.GetTotalMatrix().Get(Drawing::Matrix::SCALE_X), scaleFactor);

    data->rect_ = {0, 0, 100, 100};
    // surface is null
    subThreadCache.DrawBehindWindowBeforeCache(canvas);
    ASSERT_EQ(canvas.GetTotalMatrix().Get(Drawing::Matrix::SCALE_X), scaleFactor);
}

/**
 * @tc.name: SetNodeIdTest
 * @tc.desc: Test SetNodeId and GetNodeId
 * @tc.type: FUNC
 * @tc.require: issueIC3DK9
 */
HWTEST_F(RSSubThreadCacheTest, SetNodeIdTest, TestSize.Level1)
{
    RsSubThreadCache subCache;
    NodeId id = 100;
    subCache.SetNodeId(id);
    ASSERT_EQ(subCache.GetNodeId(), id);
}

/**
 * @tc.name: ResetUifirstTest
 * @tc.desc: Test ResetUifirstTest
 * @tc.type: FUNC
 * @tc.require: issueIC3DK9
 */
HWTEST_F(RSSubThreadCacheTest, ResetUifirstTest, TestSize.Level1)
{
    RsSubThreadCache subCache;
    subCache.cacheSurface_ = std::make_shared<Drawing::Surface>();
    subCache.cacheCompletedSurface_ = std::make_shared<Drawing::Surface>();
    // clear cache only
    subCache.ResetUifirst(true);
    ASSERT_EQ(subCache.cacheSurface_, nullptr);
    ASSERT_NE(subCache.cacheCompletedSurface_, nullptr);

    subCache.cacheSurface_ = std::make_shared<Drawing::Surface>();
    // clear cache and complete cache
    subCache.ResetUifirst(false);
    ASSERT_EQ(subCache.cacheSurface_, nullptr);
    ASSERT_EQ(subCache.cacheCompletedSurface_, nullptr);
}

/**
 * @tc.name: PushDirtyRegionToStack
 * @tc.desc: Test push dirty region
 * @tc.type: FUNC
 * @tc.require: issuesICFWAC
 */
HWTEST_F(RSSubThreadCacheTest, PushDirtyRegionToStack, TestSize.Level1)
{
    RsSubThreadCache subCache;
    Drawing::Region region;
    Drawing::Region dirtyRegion;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    canvas.SetIsParallelCanvas(true);
    subCache.SetUifrstDirtyEnableFlag(false);
    // push nothing
    subCache.PushDirtyRegionToStack(canvas, region);
    ASSERT_TRUE(canvas.IsDirtyRegionStackEmpty());

    canvas.SetIsParallelCanvas(true);
    subCache.SetUifrstDirtyEnableFlag(true);
    subCache.SetUifirstDirtyRegion(dirtyRegion);
    // push uifirst dirty region
    subCache.PushDirtyRegionToStack(canvas, region);
    ASSERT_FALSE(canvas.IsDirtyRegionStackEmpty());
    canvas.PopDirtyRegion();
    ASSERT_TRUE(canvas.IsDirtyRegionStackEmpty());

    canvas.SetIsParallelCanvas(false);
    // push region
    subCache.PushDirtyRegionToStack(canvas, region);
    ASSERT_FALSE(canvas.IsDirtyRegionStackEmpty());
}

/**
 * @tc.name: UpdateCacheSurfaceDirtyManagerTest
 * @tc.desc: Test update cache surface dirty manager failed
 * @tc.type: FUNC
 * @tc.require: issuesICFWAC
 */
HWTEST_F(RSSubThreadCacheTest, UpdateCacheSurfaceDirtyManagerTest001, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(surfaceDrawable, nullptr);

    RsSubThreadCache subCache;
    bool hasCompleteCache = true;
    bool isLastFrameSkip = true;
    // drawable is null
    ASSERT_FALSE(subCache.UpdateCacheSurfaceDirtyManager(nullptr, hasCompleteCache, isLastFrameSkip));

    subCache.syncUifirstDirtyManager_ = nullptr;
    // dirty manager is null
    ASSERT_FALSE(subCache.UpdateCacheSurfaceDirtyManager(surfaceDrawable.get(), hasCompleteCache, isLastFrameSkip));

    subCache.syncUifirstDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    surfaceDrawable->syncDirtyManager_ = nullptr;
    // dirty manager is null
    ASSERT_FALSE(subCache.UpdateCacheSurfaceDirtyManager(surfaceDrawable.get(), hasCompleteCache, isLastFrameSkip));

    surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    surfaceDrawable->renderParams_ = nullptr;
    // param is null
    ASSERT_FALSE(subCache.UpdateCacheSurfaceDirtyManager(surfaceDrawable.get(), hasCompleteCache, isLastFrameSkip));
}
}
