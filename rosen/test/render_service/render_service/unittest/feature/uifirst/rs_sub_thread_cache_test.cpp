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
#include "params/rs_render_thread_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
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
    RSRenderNodeDrawableAdapter::SharedPtr CreateStartingNodeDrawable(NodeId startingWindowId);
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

RSRenderNodeDrawableAdapter::SharedPtr RSSubThreadCacheTest::CreateStartingNodeDrawable(NodeId startingWindowId)
{
    auto startingWindow = std::make_shared<RSCanvasRenderNode>(startingWindowId);
    auto startingNodeDrawable = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(startingWindow);
    return startingNodeDrawable;
}

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
    auto vkTextureInfo = cacheBackendTexture_.GetTextureInfo().GetVKTextureInfo();
    surfaceDrawable_->GetRsSubThreadCache().cacheCompletedCleanupHelper_ = new NativeBufferUtils::VulkanCleanupHelper(
        RsVulkanContext::GetSingleton(), vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory);
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

#ifdef RS_ENABLE_VK
    auto context = RsVulkanContext::GetSingleton().GetRsVulkanInterface().CreateDrawingContext();
    rscanvas.canvas_->gpuContext_ = context;
    Drawing::ImageInfo info = Drawing::ImageInfo {
                                100, 100,
                                Drawing::ColorType::COLORTYPE_RGBA_8888,
                                Drawing::AlphaType::ALPHATYPE_PREMUL };
    surfaceDrawable_->GetRsSubThreadCache().cacheCompletedSurface_ =
        Drawing::Surface::MakeRenderTarget(context.get(), false, info);
    result = surfaceDrawable_->GetRsSubThreadCache().DrawCacheSurface(surfaceDrawable_.get(), rscanvas, boundSize,
        threadIndex, isUIFirst);
    EXPECT_TRUE(result);

    boundSize.x_ = 10;
    boundSize.y_ = 50;
    auto recordingEnabled = system::GetParameter("debug.graphic.recording.enabled", "0");
    system::SetParameter("debug.graphic.recording.enabled", "1");
    result = surfaceDrawable_->GetRsSubThreadCache().DrawCacheSurface(surfaceDrawable_.get(), rscanvas, boundSize,
        threadIndex, isUIFirst);
    EXPECT_TRUE(result);
    system::SetParameter("debug.graphic.recording.enabled", recordingEnabled);
#endif
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
 * @tc.name: CalculateUifirstDirtyRegionTest001
 * @tc.desc: Test CalculateUifirstDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSubThreadCacheTest, CalculateUifirstDirtyRegionTest001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Drawing::RectI dirtyRect = {};
    Drawing::RectI visibleFilterRect = {};
    bool isCalculateSucc = surfaceDrawable_->GetRsSubThreadCache().CalculateUifirstDirtyRegion(surfaceDrawable_.get(),
        dirtyRect, false, visibleFilterRect);
    ASSERT_EQ(isCalculateSucc, true);
    RectI dirtyRegion = {0, 0, 10, 10};
    surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_->historyHead_ = 0;
    surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_->dirtyHistory_[0]= dirtyRegion;
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->GetRenderParams().get());
    surfaceParams->absDrawRect_ = {0, 0, 15, 15};
    isCalculateSucc = surfaceDrawable_->GetRsSubThreadCache().CalculateUifirstDirtyRegion(surfaceDrawable_.get(),
        dirtyRect, false, visibleFilterRect);
    ASSERT_EQ(isCalculateSucc, false);

    surfaceParams->screenRect_ = {0, 0, 100, 100};
    isCalculateSucc = surfaceDrawable_->GetRsSubThreadCache().CalculateUifirstDirtyRegion(surfaceDrawable_.get(),
        dirtyRect, false, visibleFilterRect);
    ASSERT_EQ(isCalculateSucc, true);

    visibleFilterRect = {0, 0, 10, 10};
    isCalculateSucc = surfaceDrawable_->GetRsSubThreadCache().CalculateUifirstDirtyRegion(surfaceDrawable_.get(),
        dirtyRect, false, visibleFilterRect);
    ASSERT_EQ(isCalculateSucc, true);
    surfaceDrawable_->syncDirtyManager_->Clear();
    surfaceDrawable_->GetRsSubThreadCache().syncUifirstDirtyManager_->Clear();
}

/**
 * @tc.name: CalculateUifirstDirtyRegionTest002
 * @tc.desc: Test calculate dirty region when drawable is null
 * @tc.type: FUNC
 * @tc.require: issue21071
 */
HWTEST_F(RSSubThreadCacheTest, CalculateUifirstDirtyRegionTest002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subCache = surfaceDrawable_->GetRsSubThreadCache();
    Drawing::RectI dirtyRect = {};
    Drawing::RectI visibleFilterRect = {};
    ASSERT_FALSE(subCache.CalculateUifirstDirtyRegion(nullptr, dirtyRect, false, visibleFilterRect));
}

/**
 * @tc.name: CalculateUifirstDirtyRegionTest003
 * @tc.desc: Test calculate dirty region when dirtymanager is null
 * @tc.type: FUNC
 * @tc.require: issue21071
 */
HWTEST_F(RSSubThreadCacheTest, CalculateUifirstDirtyRegionTest003, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subCache = surfaceDrawable_->GetRsSubThreadCache();
    Drawing::RectI dirtyRect = {};
    Drawing::RectI visibleFilterRect = {};
    subCache.syncUifirstDirtyManager_ = nullptr;
    ASSERT_FALSE(subCache.CalculateUifirstDirtyRegion(surfaceDrawable_.get(), dirtyRect, false, visibleFilterRect));
}

/**
 * @tc.name: CalculateUifirstDirtyRegionTest004
 * @tc.desc: Test calculate dirty region when render param is null
 * @tc.type: FUNC
 * @tc.require: issue21071
 */
HWTEST_F(RSSubThreadCacheTest, CalculateUifirstDirtyRegionTest004, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subCache = surfaceDrawable_->GetRsSubThreadCache();
    Drawing::RectI dirtyRect = {};
    Drawing::RectI visibleFilterRect = {};
    subCache.syncUifirstDirtyManager_ = std::make_shared<RSDirtyRegionManager>();

    bool isUifirstRootNode = false;
    surfaceDrawable_->renderParams_ = nullptr;
    // render params is null
    ASSERT_FALSE(
        subCache.CalculateUifirstDirtyRegion(surfaceDrawable_.get(), dirtyRect, isUifirstRootNode, visibleFilterRect));
    surfaceDrawable_->renderParams_ = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable_->GetId());

    isUifirstRootNode = true;
    surfaceDrawable_->uifirstRenderParams_ = nullptr;
    // uifirst render params is null
    ASSERT_FALSE(
        subCache.CalculateUifirstDirtyRegion(surfaceDrawable_.get(), dirtyRect, isUifirstRootNode, visibleFilterRect));
    surfaceDrawable_->uifirstRenderParams_ = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable_->GetId());
}

/**
 * @tc.name: CalculateUifirstDirtyRegionTest005
 * @tc.desc: Test calculate dirty region when latest dirty rect is empty
 * @tc.type: FUNC
 * @tc.require: issue21071
 */
HWTEST_F(RSSubThreadCacheTest, CalculateUifirstDirtyRegionTest005, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    RectI dirtyRegion = { 0, 0, 10, 10 };
    auto& subCache = surfaceDrawable_->GetRsSubThreadCache();
    Drawing::RectI dirtyRect = {};
    Drawing::RectI visibleFilterRect = {};
    subCache.syncUifirstDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    subCache.syncUifirstDirtyManager_->historyHead_ = -1;
    ASSERT_TRUE(
        subCache.CalculateUifirstDirtyRegion(surfaceDrawable_.get(), dirtyRect, false, visibleFilterRect));
}

/**
 * @tc.name: UpadteAllSurfaceUifirstDirtyEnableState
 * @tc.desc: Test UpadteAllSurfaceUifirstDirtyEnableState
 * @tc.type: FUNC
 * @tc.require: issueI9NVOG
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
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
}

/**
 * @tc.name: MergeUifirstAllSurfaceDirtyRegionTest001
 * @tc.desc: Test MergeUifirstAllSurfaceDirtyRegion
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSubThreadCacheTest, MergeUifirstAllSurfaceDirtyRegionTest001, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto uifirstDirtyEnabled = system::GetParameter("rosen.ui.first.dirty.enabled", "1");
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    system::SetParameter("rosen.ui.first.dirty.enabled", "0");
    Drawing::RectI dirtyRect = {};
    bool dirtyEnableFlag = surfaceDrawable_->GetRsSubThreadCache().MergeUifirstAllSurfaceDirtyRegion(
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
    RectI filterRect = { 0, 0, 5, 5 };
    surfaceParams->SetUIFirstVisibleFilterRect(filterRect);
    dirtyEnableFlag = surfaceDrawable_->GetRsSubThreadCache().MergeUifirstAllSurfaceDirtyRegion(
        surfaceDrawable_.get(), dirtyRect);
    ASSERT_EQ(dirtyEnableFlag, false);

    auto uifirstParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->GetUifirstRenderParams().get());
    uifirstParams->windowInfo_.isLeashWindow_ = true;
    uifirstParams->absDrawRect_ = {0, 0, 15, 15};
    uifirstParams->screenRect_ = {0, 0, 100, 100};
    surfaceDrawable_->GetRsSubThreadCache().isDirtyRecordCompletated_ = true;
    dirtyEnableFlag = surfaceDrawable_->GetRsSubThreadCache().MergeUifirstAllSurfaceDirtyRegion(
        surfaceDrawable_.get(), dirtyRect);
    ASSERT_EQ(dirtyEnableFlag, true);
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::SINGLE));
    system::SetParameter("rosen.ui.first.dirty.enabled", uifirstDirtyEnabled);
}

/**
 * @tc.name: MergeUifirstAllSurfaceDirtyRegionTest002
 * @tc.desc: Test merge dirty region when surface drawable is null
 * @tc.type: FUNC
 * @tc.require: issue21071
 */
HWTEST_F(RSSubThreadCacheTest, MergeUifirstAllSurfaceDirtyRegionTest002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subCache = surfaceDrawable_->GetRsSubThreadCache();
    Drawing::RectI dirtyRect = {};
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::MULTI));
    ASSERT_FALSE(subCache.MergeUifirstAllSurfaceDirtyRegion(nullptr, dirtyRect));
}

/**
 * @tc.name: MergeUifirstAllSurfaceDirtyRegionTest003
 * @tc.desc: Test merge dirty region when surface drawable is null
 * @tc.type: FUNC
 * @tc.require: issue21071
 */
HWTEST_F(RSSubThreadCacheTest, MergeUifirstAllSurfaceDirtyRegionTest003, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subCache = surfaceDrawable_->GetRsSubThreadCache();
    Drawing::RectI dirtyRect = {};
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::MULTI));
    ASSERT_FALSE(subCache.MergeUifirstAllSurfaceDirtyRegion(nullptr, dirtyRect));
}

/**
 * @tc.name: MergeUifirstAllSurfaceDirtyRegionTest004
 * @tc.desc: Test merge dirty region when render param is null
 * @tc.type: FUNC
 * @tc.require: issue21071
 */
HWTEST_F(RSSubThreadCacheTest, MergeUifirstAllSurfaceDirtyRegionTest004, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subCache = surfaceDrawable_->GetRsSubThreadCache();
    Drawing::RectI dirtyRect = {};
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::MULTI));
    surfaceDrawable_->uifirstRenderParams_ = nullptr;
    ASSERT_FALSE(subCache.MergeUifirstAllSurfaceDirtyRegion(surfaceDrawable_.get(), dirtyRect));
}

/**
 * @tc.name: MergeUifirstAllSurfaceDirtyRegionTest005
 * @tc.desc: Test merge dirty region when render param is null
 * @tc.type: FUNC
 * @tc.require: issue21071
 */
HWTEST_F(RSSubThreadCacheTest, MergeUifirstAllSurfaceDirtyRegionTest005, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subCache = surfaceDrawable_->GetRsSubThreadCache();
    Drawing::RectI dirtyRect = {};
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::MULTI));

    auto surfaceParam = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable_->GetId());
    // app window
    surfaceParam->SetWindowInfo(false, true, false);
    surfaceDrawable_->uifirstRenderParams_ = std::move(surfaceParam);
    ASSERT_FALSE(subCache.MergeUifirstAllSurfaceDirtyRegion(surfaceDrawable_.get(), dirtyRect));
}

/**
 * @tc.name: MergeUifirstAllSurfaceDirtyRegionTest006
 * @tc.desc: Test merge dirty region
 * @tc.type: FUNC
 * @tc.require: issue21071
 */
HWTEST_F(RSSubThreadCacheTest, MergeUifirstAllSurfaceDirtyRegionTest006, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subCache = surfaceDrawable_->GetRsSubThreadCache();
    Drawing::RectI dirtyRect = {};
    uifirstManager_.SetUiFirstType(static_cast<int>(UiFirstCcmType::MULTI));

    auto surfaceParam = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable_->GetId());
    // leash window
    surfaceParam->SetWindowInfo(false, true, false);
    surfaceDrawable_->uifirstRenderParams_ = std::move(surfaceParam);
    subCache.isDirtyRecordCompletated_ = false;
    ASSERT_FALSE(subCache.MergeUifirstAllSurfaceDirtyRegion(surfaceDrawable_.get(), dirtyRect));

    subCache.isDirtyRecordCompletated_ = true;
    ASSERT_TRUE(subCache.MergeUifirstAllSurfaceDirtyRegion(surfaceDrawable_.get(), dirtyRect));
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
 * @tc.name: DrawUIFirstCacheWithStartingTest001
 * @tc.desc: no cache and no starting window, should wait subthread
  * @tc.type: FUNC
  * @tc.require: #ICN6UK
  */
HWTEST_F(RSSubThreadCacheTest, DrawUIFirstCacheWithStartingTest001, TestSize.Level1)
{
    ASSERT_NE(canvas_, nullptr);
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subThreadCache = surfaceDrawable_->GetRsSubThreadCache();
    NodeId startingWindowId = 100;
    // surface drawable is null
    ASSERT_FALSE(subThreadCache.DrawUIFirstCacheWithStarting(nullptr, *canvas_, startingWindowId));

    // ensure no starting drawable
    RSRenderNodeDrawableAdapter::RemoveDrawableFromCache(startingWindowId);
    subThreadCache.isCacheCompletedValid_ = false;
    // no cache, no starting window
    ASSERT_FALSE(subThreadCache.DrawUIFirstCacheWithStarting(surfaceDrawable_.get(), *canvas_, startingWindowId));
}

/**
 * @tc.name: DrawUIFirstCacheWithStartingTest002
 * @tc.desc: has cache and no starting window, only draw cache
 * @tc.type: FUNC
 * @tc.require: #ICN6UK
 */
HWTEST_F(RSSubThreadCacheTest, DrawUIFirstCacheWithStartingTest002, TestSize.Level1)
{
    ASSERT_NE(canvas_, nullptr);
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subThreadCache = surfaceDrawable_->GetRsSubThreadCache();
    NodeId startingWindowId = 100;
    // ensure no starting drawable
    RSRenderNodeDrawableAdapter::RemoveDrawableFromCache(startingWindowId);
    subThreadCache.isCacheCompletedValid_ = true;
    ASSERT_FALSE(subThreadCache.DrawUIFirstCacheWithStarting(surfaceDrawable_.get(), *canvas_, startingWindowId));
}

/**
 * @tc.name: DrawUIFirstCacheWithStartingTest003
 * @tc.desc: no cache, and starting window has alpha, should wait subthread
 * @tc.type: FUNC
 * @tc.require: #ICN6UK
 */
HWTEST_F(RSSubThreadCacheTest, DrawUIFirstCacheWithStartingTest003, TestSize.Level1)
{
    ASSERT_NE(canvas_, nullptr);
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subThreadCache = surfaceDrawable_->GetRsSubThreadCache();
    NodeId startingWindowId = 100;
    auto startingNodeDrawable = CreateStartingNodeDrawable(startingWindowId);
    ASSERT_NE(startingNodeDrawable, nullptr);
    auto& startingParams = startingNodeDrawable->GetRenderParams();
    startingParams->SetAlpha(0.5f);
    subThreadCache.isCacheCompletedValid_ = false;
    // no cache, and starting window has alpha, should wait subthread
    ASSERT_FALSE(subThreadCache.DrawUIFirstCacheWithStarting(surfaceDrawable_.get(), *canvas_, startingWindowId));

    startingNodeDrawable->renderParams_ = nullptr;
    // no cache, and starting window render params is null
    ASSERT_TRUE(subThreadCache.DrawUIFirstCacheWithStarting(surfaceDrawable_.get(), *canvas_, startingWindowId));
    // restore render params
    startingNodeDrawable->renderParams_ = std::make_unique<RSRenderParams>(startingWindowId);
}

/**
 * @tc.name: DrawUIFirstCacheWithStartingTest004
 * @tc.desc: has starting window, test has cache or not
 * @tc.type: FUNC
 * @tc.require: #ICN6UK
 */
HWTEST_F(RSSubThreadCacheTest, DrawUIFirstCacheWithStartingTest004, TestSize.Level1)
{
    ASSERT_NE(canvas_, nullptr);
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subThreadCache = surfaceDrawable_->GetRsSubThreadCache();
    NodeId startingWindowId = 100;
    auto startingNodeDrawable = CreateStartingNodeDrawable(startingWindowId);
    ASSERT_NE(startingNodeDrawable, nullptr);
    auto& startingParams = startingNodeDrawable->GetRenderParams();
    startingParams->SetAlpha(1.0f);
    subThreadCache.isCacheCompletedValid_ = false;
    // no cache, only draw starting window
    ASSERT_TRUE(subThreadCache.DrawUIFirstCacheWithStarting(surfaceDrawable_.get(), *canvas_, startingWindowId));

    subThreadCache.isCacheCompletedValid_ = true;
    // draw cache and starting window
    ASSERT_FALSE(subThreadCache.DrawUIFirstCacheWithStarting(surfaceDrawable_.get(), *canvas_, startingWindowId));
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
    auto& subCache = surfaceDrawable_->GetRsSubThreadCache();
    subCache.InitCacheSurface(gpuContext, surfaceDrawable_, func, threadIndex);
    ASSERT_FALSE(subCache.clearCacheSurfaceFunc_);

    func = [](std::shared_ptr<Drawing::Surface>&& oldSurface, std::shared_ptr<Drawing::Surface>&& newSurface,
               uint32_t width, uint32_t height) {};
    surfaceDrawable_->uifirstRenderParams_ = nullptr;
    subCache.InitCacheSurface(gpuContext, surfaceDrawable_, func, threadIndex);
    ASSERT_TRUE(subCache.clearCacheSurfaceFunc_);

    subCache.cacheSurface_ = std::make_shared<Drawing::Surface>();
    surfaceDrawable_->uifirstRenderParams_ = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable_->GetId());
    subCache.InitCacheSurface(gpuContext, surfaceDrawable_, func, threadIndex);
    delete gpuContext;
    gpuContext = nullptr;
    subCache.InitCacheSurface(gpuContext, surfaceDrawable_, func, threadIndex);
    ASSERT_FALSE(subCache.cacheSurface_);
}

/**
 * @tc.name: ClearCacheSurfaceOnlyTest001
 * @tc.desc: Test If ClearCacheSurfaceOnly Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSSubThreadCacheTest, ClearCacheSurfaceOnlyTest001, TestSize.Level1)
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
 * @tc.name: ClearCacheSurfaceOnlyTest002
 * @tc.desc: Test ClearCacheSurfaceOnly with clear func
 * @tc.type: FUNC
 * @tc.require: issue21071
 */
HWTEST_F(RSSubThreadCacheTest, ClearCacheSurfaceOnlyTest002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    DrawableV2::RsSubThreadCache::ClearCacheSurfaceFunc func;
    func = [](std::shared_ptr<Drawing::Surface>&& oldSurface, std::shared_ptr<Drawing::Surface>&& newSurface,
               uint32_t width, uint32_t height) {};
    auto& subCache = surfaceDrawable_->GetRsSubThreadCache();
    subCache.cacheSurface_ = std::make_shared<Drawing::Surface>();
    subCache.clearCacheSurfaceFunc_ = func;
    subCache.ClearCacheSurfaceOnly();
    ASSERT_FALSE(surfaceDrawable_->GetRsSubThreadCache().cacheSurface_);

    subCache.clearCacheSurfaceFunc_ = nullptr;
    subCache.cacheSurface_ = std::make_shared<Drawing::Surface>();
    subCache.ClearCacheSurfaceOnly();
    ASSERT_FALSE(surfaceDrawable_->GetRsSubThreadCache().cacheSurface_);
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
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));
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
    surfaceParams->SetUifirstStartingWindowId(0);
    surfaceParams->SetWindowInfo(false, true, false);
    uniParams->isUIFirstDebugEnable_ = true;
    // irrevertible matrix
    surfaceParams->matrix_.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));
    // invertible matrix
    surfaceParams->matrix_.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));

    surfaceParams->SetGlobalPositionEnabled(true);
    surfaceParams->SetUifirstStartingWindowId(1);
    surfaceParams->SetWindowInfo(false, false, false);
    uniParams->isUIFirstDebugEnable_ = false;
    // draw starting window
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));

    surfaceParams->SetGlobalPositionEnabled(false);
    // global position disabled
    ASSERT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));
}

/**
 * @tc.name: DealWithUIFirstCacheTest006
 * @tc.desc: Test skip drawing special nodes in the virtual screen, isSnapshot_ = true
 * @tc.type: FUNC
 * @tc.require: issueIAEDYI
 */
HWTEST_F(RSSubThreadCacheTest, DealWithUIFirstCacheTest006, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    auto& subThreadCache = surfaceDrawable_->GetRsSubThreadCache();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(surfaceDrawable_->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetUifirstNodeEnableParam(MultiThreadCacheType::LEASH_WINDOW);
    auto uniParams = std::make_shared<RSRenderThreadParams>();

    subThreadCache.isCacheCompletedValid_ = true;
    RSUniRenderThread::GetCaptureParam().isSnapshot_ = false;
    EXPECT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));

    RSUniRenderThread::GetCaptureParam().isSnapshot_ = true;
    EXPECT_TRUE(subThreadCache.DealWithUIFirstCache(surfaceDrawable_.get(), *canvas_, *surfaceParams, *uniParams));
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
 * @tc.name: GetCacheSurfaceAlphaInfo
 * @tc.desc: Test Get Cache Surface Alpha Info
 * @tc.type: FUNC
 * @tc.require: issueICS5NW
 */
HWTEST_F(RSSubThreadCacheTest, GetCacheSurfaceAlphaInfo, TestSize.Level1)
{
    RsSubThreadCache subCache;
    ASSERT_EQ(subCache.GetCacheSurfaceAlphaInfo(), -1.f);
}

/**
 * @tc.name: GetCacheSurfaceProcessedNodes
 * @tc.desc: Test Get CacheSurface Processed Nodes
 * @tc.type: FUNC
 * @tc.require: issueICS5NW
 */
HWTEST_F(RSSubThreadCacheTest, GetCacheSurfaceProcessedNodes, TestSize.Level1)
{
    RsSubThreadCache subCache;
    ASSERT_EQ(subCache.GetCacheSurfaceProcessedNodes(), -1);
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

/**
 * @tc.name: UpdateCacheSurfaceDirtyManagerTest
 * @tc.desc: Test update cache surface dirty manager success
 * @tc.type: FUNC
 * @tc.require: issuesICFWAC
 */
HWTEST_F(RSSubThreadCacheTest, UpdateCacheSurfaceDirtyManagerTest002, TestSize.Level1)
{
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(1);
    auto surfaceDrawable = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(surfaceDrawable, nullptr);
    RsSubThreadCache subCache;

    subCache.syncUifirstDirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    surfaceDrawable->syncDirtyManager_ = std::make_shared<RSDirtyRegionManager>();

    ASSERT_TRUE(subCache.UpdateCacheSurfaceDirtyManager(surfaceDrawable.get(), true, true));
    ASSERT_TRUE(subCache.UpdateCacheSurfaceDirtyManager(surfaceDrawable.get(), false, false));
}

/**
 * @tc.name: IsDirtyRecordCompletatedTest
 * @tc.desc: Test IsDirtyRecordCompletated
 * @tc.type: FUNC
 * @tc.require: issuesICFWAC
 */
HWTEST_F(RSSubThreadCacheTest, IsDirtyRecordCompletatedTest, TestSize.Level1)
{
    RsSubThreadCache subCache;
    subCache.UpdateDirtyRecordCompletatedState(true);
    ASSERT_TRUE(subCache.IsDirtyRecordCompletated());
    subCache.UpdateDirtyRecordCompletatedState(false);
    ASSERT_FALSE(subCache.IsDirtyRecordCompletated());
}

/**
 * @tc.name: GetUifirstDirtyRegionTest
 * @tc.desc: Test GetUifirstDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issuesICFWAC
 */
HWTEST_F(RSSubThreadCacheTest, GetUifirstDirtyRegionTest, TestSize.Level1)
{
    RsSubThreadCache subCache;
    Drawing::Region dirtyRegion;
    Drawing::RectI rectI = {0, 0, 200, 200};
    dirtyRegion.SetRect(rectI);
    ASSERT_TRUE(subCache.GetUifirstDirtyRegion().IsEmpty());
    subCache.SetUifirstDirtyRegion(dirtyRegion);
    ASSERT_FALSE(subCache.GetUifirstDirtyRegion().IsEmpty());
}

/**
 * @tc.name: GetUifrstDirtyEnableFlagTest
 * @tc.desc: Test GetUifrstDirtyEnableFlag
 * @tc.type: FUNC
 * @tc.require: issuesICFWAC
 */
HWTEST_F(RSSubThreadCacheTest, GetUifrstDirtyEnableFlagTest, TestSize.Level1)
{
    RsSubThreadCache subCache;
    subCache.SetUifrstDirtyEnableFlag(true);
    ASSERT_TRUE(subCache.GetUifrstDirtyEnableFlag());
    subCache.SetUifrstDirtyEnableFlag(false);
    ASSERT_FALSE(subCache.GetUifrstDirtyEnableFlag());
}

/**
 * @tc.name: GetCurDirtyRegionWithMatrixTest
 * @tc.desc: Test current dirty region
 * @tc.type: FUNC
 * @tc.require: issuesICFWAC
 */
HWTEST_F(RSSubThreadCacheTest, GetCurDirtyRegionWithMatrixTest, TestSize.Level1)
{
    RsSubThreadCache subCache;
    Drawing::Matrix matrix;
    Drawing::RectF latestDirtyRect;
    Drawing::RectF absDrawRect;

    // irrevertible matrix
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    ASSERT_FALSE(subCache.GetCurDirtyRegionWithMatrix(matrix, latestDirtyRect, absDrawRect));
    // invertible matrix
    matrix.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    ASSERT_TRUE(subCache.GetCurDirtyRegionWithMatrix(matrix, latestDirtyRect, absDrawRect));
}

/**
 * @tc.name: UifirstDirtyRegionDfxTest
 * @tc.desc: Test dfx
 * @tc.type: FUNC
 * @tc.require: issuesICFWAC
 */
HWTEST_F(RSSubThreadCacheTest, UifirstDirtyRegionDfxTest, TestSize.Level1)
{
    RsSubThreadCache subCache;
    Drawing::Canvas drawingCanvas;
    Drawing::RectI rect;
    auto param = OHOS::system::GetParameter("rosen.ui.first.dirty.dfx.enabled", "0");
    OHOS::system::SetParameter("rosen.ui.first.dirty.dfx.enabled", "1");
    ASSERT_TRUE(RSSystemProperties::GetUIFirstDirtyDebugEnabled());
    subCache.UifirstDirtyRegionDfx(drawingCanvas, rect);
    OHOS::system::SetParameter("rosen.ui.first.dirty.dfx.enabled", "0");
    subCache.UifirstDirtyRegionDfx(drawingCanvas, rect);
    ASSERT_FALSE(RSSystemProperties::GetUIFirstDirtyDebugEnabled());
    OHOS::system::SetParameter("rosen.ui.first.dirty.dfx.enabled", param);
}

/**
 * @tc.name: TotalProcessedSurfaceCountIncTest
 * @tc.desc: Test processed surface node count
 * @tc.type: FUNC
 * @tc.require: issuesICFWAC
 */
HWTEST_F(RSSubThreadCacheTest, TotalProcessedSurfaceCountIncTest, TestSize.Level1)
{
    RsSubThreadCache subCache;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    subCache.totalProcessedSurfaceCount_ = 0;
    canvas.SetIsParallelCanvas(false);
    subCache.TotalProcessedSurfaceCountInc(canvas);
    ASSERT_EQ(subCache.GetTotalProcessedSurfaceCount(), 0);
    canvas.SetIsParallelCanvas(true);
    subCache.TotalProcessedSurfaceCountInc(canvas);
    ASSERT_EQ(subCache.GetTotalProcessedSurfaceCount(), 1);
}

/**
 * @tc.name: GetSurfaceSkipPriorityTest
 * @tc.desc: Test surface skip priority
 * @tc.type: FUNC
 * @tc.require: issuesICFWAC
 */
HWTEST_F(RSSubThreadCacheTest, GetSurfaceSkipPriorityTest, TestSize.Level1)
{
    RsSubThreadCache subCache;
    subCache.ResetSurfaceSkipCount();
    ASSERT_EQ(subCache.GetSurfaceSkipCount(), 0);
    ASSERT_EQ(subCache.isSurfaceSkipPriority_, 0);
    ASSERT_EQ(subCache.GetSurfaceSkipPriority(), 1);
}

/**
 * @tc.name: CacheReuseCountTest
 * @tc.desc: Test uifirst cache reuse count
 * @tc.type: FUNC
 * @tc.require: issues20692
 */
HWTEST_F(RSSubThreadCacheTest, CacheReuseCountTest, TestSize.Level1)
{
    RsSubThreadCache subCache;
    subCache.ResetCacheReuseCount();
    subCache.AddCacheReuseCount();
    subCache.AddCacheReuseCount();
    ASSERT_EQ(subCache.GetCacheReuseCount(), 2);

    subCache.ResetCacheReuseCount();
    ASSERT_EQ(subCache.GetCacheReuseCount(), 0);
}

/**
 * @tc.name: UpdateCacheSurfaceInfo001
 * @tc.desc: Test update cache info failed
 * @tc.type: FUNC
 * @tc.require: issue21674
 */
HWTEST_F(RSSubThreadCacheTest, UpdateCacheSurfaceInfo001, TestSize.Level1)
{
    RsSubThreadCache subCache;
    subCache.cacheSurfaceInfo_.processedSurfaceCount = 0;
    subCache.cacheSurfaceInfo_.processedNodeCount = 0;
    subCache.totalProcessedSurfaceCount_ = 3;
    subCache.UpdateCacheSurfaceInfo(nullptr, nullptr);
    ASSERT_EQ(subCache.cacheSurfaceInfo_.processedSurfaceCount, 0);
    ASSERT_EQ(subCache.cacheSurfaceInfo_.processedNodeCount, 0);

    subCache.UpdateCacheSurfaceInfo(surfaceDrawable_, nullptr);
    ASSERT_EQ(subCache.cacheSurfaceInfo_.processedSurfaceCount, 0);
    ASSERT_EQ(subCache.cacheSurfaceInfo_.processedNodeCount, 0);
}

/**
 * @tc.name: UpdateCacheSurfaceInfo002
 * @tc.desc: Test update cache info success
 * @tc.type: FUNC
 * @tc.require: issue21674
 */
HWTEST_F(RSSubThreadCacheTest, UpdateCacheSurfaceInfo002, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    RsSubThreadCache subCache;
    subCache.cacheSurfaceInfo_.processedSurfaceCount = 0;
    subCache.cacheSurfaceInfo_.processedNodeCount = 0;
    subCache.cacheSurfaceInfo_.alpha = -1.f;
    subCache.totalProcessedSurfaceCount_ = 1;
    surfaceDrawable_->renderParams_->SetGlobalAlpha(0);
    RSRenderNodeDrawable::ClearProcessedNodeCount();
    RSRenderNodeDrawable::ProcessedNodeCountInc();
    RSRenderNodeDrawable::ProcessedNodeCountInc();
    auto surfaceParams = std::make_unique<RSSurfaceRenderParams>(surfaceDrawable_->GetId());
    surfaceParams->SetGlobalAlpha(0);
    subCache.UpdateCacheSurfaceInfo(surfaceDrawable_, surfaceParams.get());
    ASSERT_EQ(subCache.cacheSurfaceInfo_.processedSurfaceCount, 1);
    ASSERT_EQ(subCache.cacheSurfaceInfo_.processedNodeCount, 2);
    ASSERT_EQ(subCache.cacheSurfaceInfo_.alpha, 0);
    ASSERT_TRUE(subCache.cacheSurfaceInfo_.processedSubSurfaceNodeIds.empty());
}
}
