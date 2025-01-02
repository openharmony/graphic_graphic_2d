/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "params/rs_render_params.h"
#include "params/rs_surface_render_params.h"
#include "limit_number.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderParamsUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSRenderParamsUnitTest::SetUpTestCase() {}
void RSRenderParamsUnitTest::TearDownTestCase() {}
void RSRenderParamsUnitTest::SetUp() {}
void RSRenderParamsUnitTest::TearDown() {}
void RSRenderParamsUnitTest::DisplayTestInfo()
{
    return;
}

/**
 * @tc.name: TestOnSync01
 * @tc.desc: Test function OnSync
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestOnSync01, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[3];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    RSRenderParams params(id);
    params.childHasVisibleEffect_ = true;
    params.OnSync(target);
    EXPECT_EQ(params.childHasVisibleEffect_, renderParams->childHasVisibleEffect_);
}

/**
 * @tc.name: TestSetAlpha01
 * @tc.desc: Test function SetAlpha
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetAlpha01, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->alpha_ = 1.0;
    float alpha = 1.0;
    params.SetAlpha(alpha);
    EXPECT_TRUE(renderParams != nullptr);
}

/**
 * @tc.name: TestSetAlphaOffScreen01
 * @tc.desc: Test function SetAlphaOffScreen and GetAlphaOffScreen
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetAlphaOffScreen01, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSRenderParams params(id);
    bool alphaOffScreen = false;
    params.SetAlphaOffScreen(alphaOffScreen);
    EXPECT_TRUE(!params.GetAlphaOffScreen());
}

/**
 * @tc.name: TestApplyAlphaAndMatrixToCanvas01
 * @tc.desc: Test function ApplyAlphaAndMatrixToCanvas
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestApplyAlphaAndMatrixToCanvas01, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSRenderParams params(id);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    params.ApplyAlphaAndMatrixToCanvas(paintFilterCanvas, true);
    EXPECT_TRUE(id != -1);
}

/**
 * @tc.name: TestSetAlpha_02
 * @tc.desc: Test function SetAlpha，alpha != renderParams->alpha_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetAlpha_02, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->alpha_ = 1.0f;
    float alpha = 0.1;
    EXPECT_TRUE(alpha != renderParams->alpha_);

    renderParams->SetAlpha(alpha);
    EXPECT_EQ(renderParams->alpha_, alpha);
    EXPECT_TRUE(renderParams->needSync_);
}

/**
 * @tc.name: TestSetAlphaOffScreen_02
 * @tc.desc: Test function SetAlphaOffScreen, alphaOffScreen != params.alphaOffScreen_
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetAlphaOffScreen_02, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    RSRenderParams params(id);
    bool alphaOffScreen = false;
    params.alphaOffScreen_ = true;
    EXPECT_TRUE(alphaOffScreen != params.alphaOffScreen_);

    params.SetAlphaOffScreen(alphaOffScreen);
    EXPECT_FALSE(params.GetAlphaOffScreen());
    EXPECT_TRUE(params.needSync_);
}

/**
 * @tc.name: TestApplyAlphaAndMatrixToCanvas_02
 * @tc.desc: Test function ApplyAlphaAndMatrixToCanvas, UNLIKELY(HasSandBox()) is true and applyMatrix is false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestApplyAlphaAndMatrixToCanvas_02, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->hasSandBox_ = true;
    EXPECT_TRUE(UNLIKELY(renderParams->HasSandBox()));
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    renderParams->alpha_ = 0.5;
    auto alphaClamped = std::clamp(renderParams->alpha_, 0.f, 1.f);
    Drawing::Matrix matrix;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    renderParams->SetParentSurfaceMatrix(matrix);
    renderParams->matrix_.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);

    Drawing::Matrix canvasMatrix;
    canvasMatrix.SetMatrix(2, 2, 3, 4, 5, 6, 7, 8, 9);
    RSPaintFilterCanvas::CanvasStatus canvasStatus = {0.1, canvasMatrix, nullptr};
    paintFilterCanvas.SetCanvasStatus(canvasStatus);

    renderParams->ApplyAlphaAndMatrixToCanvas(paintFilterCanvas, false);
    EXPECT_EQ(paintFilterCanvas.GetCanvasStatus().matrix_, canvasMatrix); // not SetMatrix
    EXPECT_EQ(paintFilterCanvas.GetAlpha(), alphaClamped);
}

/**
 * @tc.name: TestApplyAlphaAndMatrixToCanvas_03
 * @tc.desc: Test function ApplyAlphaAndMatrixToCanvas, UNLIKELY(HasSandBox()) and applyMatrix is true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestApplyAlphaAndMatrixToCanvas_03, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->hasSandBox_ = true;
    EXPECT_TRUE(UNLIKELY(renderParams->HasSandBox()));
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    renderParams->alpha_ = 0.5;
    auto alphaClamped = std::clamp(renderParams->alpha_, 0.f, 1.f);

    Drawing::Matrix matrix;
    matrix.SetMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    renderParams->SetParentSurfaceMatrix(matrix);
    renderParams->matrix_.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    renderParams->ApplyAlphaAndMatrixToCanvas(paintFilterCanvas, true);
    EXPECT_EQ(paintFilterCanvas.GetCanvasStatus().matrix_, renderParams->GetParentSurfaceMatrix()); // SetMatrix
    EXPECT_EQ(paintFilterCanvas.GetAlpha(), alphaClamped);
}

/**
 * @tc.name: TestApplyAlphaAndMatrixToCanvas_04
 * @tc.desc: Test function ApplyAlphaAndMatrixToCanvas， not HasSandBox and not applyMatrix
 * and alpha_ < 1.0f and (drawingCacheType_ == RSDrawingCacheType::FORCED_CACHE) and alphaOffScreen_ is false
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestApplyAlphaAndMatrixToCanvas_04, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->hasSandBox_ = false;
    EXPECT_FALSE(UNLIKELY(renderParams->HasSandBox()));
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    renderParams->alpha_ = 0.5;

    renderParams->drawingCacheType_ = RSDrawingCacheType::FORCED_CACHE;
    renderParams->alphaOffScreen_ = false;
    paintFilterCanvas.envStack_.top().hasOffscreenLayer_ = false;
    renderParams->ApplyAlphaAndMatrixToCanvas(paintFilterCanvas, false);
    EXPECT_TRUE(paintFilterCanvas.envStack_.top().hasOffscreenLayer_); // SaveLayer
}

/**
 * @tc.name: TestApplyAlphaAndMatrixToCanvas_05
 * @tc.desc: Test function ApplyAlphaAndMatrixToCanvas， not HasSandBox and not applyMatrix
 * and alpha_ < 1.0f and (drawingCacheType_ != RSDrawingCacheType::FORCED_CACHE) and alphaOffScreen_ is true
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestApplyAlphaAndMatrixToCanvas_05, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->hasSandBox_ = false;
    EXPECT_FALSE(UNLIKELY(renderParams->HasSandBox()));
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    renderParams->alpha_ = 0.5;

    renderParams->drawingCacheType_ = RSDrawingCacheType::TARGETED_CACHE;
    renderParams->alphaOffScreen_ = true;
    paintFilterCanvas.envStack_.top().hasOffscreenLayer_ = false;
    renderParams->ApplyAlphaAndMatrixToCanvas(paintFilterCanvas, false);
    EXPECT_TRUE(paintFilterCanvas.envStack_.top().hasOffscreenLayer_); // SaveLayer
}

/**
 * @tc.name: TestSetBoundsRect_01
 * @tc.desc: Test function SetBoundsRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetBoundsRect_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    Drawing::RectF boundsRect(0.0f, 0.0f, 1.0f, 1.0f);

    renderParams->needSync_ = false;
    renderParams->boundsRect_ = boundsRect;
    renderParams->SetBoundsRect(boundsRect);
    EXPECT_FALSE(renderParams->needSync_);
}

/**
 * @tc.name: TestSetFrameRect_01
 * @tc.desc: Test function SetFrameRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetFrameRect_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    Drawing::RectF frameRect(0.0f, 0.0f, 1.0f, 1.0f);

    renderParams->needSync_ = false;
    renderParams->frameRect_ = frameRect;
    renderParams->SetFrameRect(frameRect);
    EXPECT_FALSE(renderParams->needSync_);
}

/**
 * @tc.name: TestSetHasSandBox_01
 * @tc.desc: Test function SetHasSandBox
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetHasSandBox_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->needSync_ = false;
    renderParams->hasSandBox_ = false;
    bool hasSandbox = true;

    EXPECT_NE(renderParams->hasSandBox_, hasSandbox);
    renderParams->SetHasSandBox(hasSandbox);
    EXPECT_TRUE(renderParams->needSync_);
    EXPECT_EQ(renderParams->hasSandBox_, hasSandbox);
}

/**
 * @tc.name: TestSetLocalDrawRect_01
 * @tc.desc: Test function SetLocalDrawRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetLocalDrawRect_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    OHOS::Rosen::RectT<float> localDrawRect(0.0f, 0.0f, 1.0f, 1.0f);
    renderParams->localDrawRect_.left_ = 0.0f;
    renderParams->localDrawRect_.top_ = 0.0f;
    renderParams->localDrawRect_.width_ = 1.3;
    renderParams->localDrawRect_.height_ = 1.3;
    EXPECT_FALSE(renderParams->localDrawRect_.IsNearEqual(localDrawRect));
    renderParams->needSync_ = false;

    renderParams->SetLocalDrawRect(localDrawRect);
    EXPECT_TRUE(renderParams->needSync_);
    EXPECT_EQ(renderParams->localDrawRect_, localDrawRect);
}

/**
 * @tc.name: TestGetLocalDrawRect_01
 * @tc.desc: Test function GetLocalDrawRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestGetLocalDrawRect_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    OHOS::Rosen::RectT<float> localDrawRect(0.0f, 0.0f, 1.0f, 1.0f);
    renderParams->localDrawRect_.left_ = 0.0f;
    renderParams->localDrawRect_.top_ = 0.0f;
    renderParams->localDrawRect_.width_ = 1.0f;
    renderParams->localDrawRect_.height_ = 1.0f;
    EXPECT_EQ(renderParams->GetLocalDrawRect(), localDrawRect);
}

/**
 * @tc.name: TestSetContentEmpty_01
 * @tc.desc: Test function SetContentEmpty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetContentEmpty_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->contentEmpty_ = false;
    bool contentEmpty = true;
    EXPECT_NE(renderParams->contentEmpty_, contentEmpty);
    renderParams->needSync_ = false;

    renderParams->SetContentEmpty(contentEmpty);
    EXPECT_EQ(renderParams->contentEmpty_, contentEmpty);
    EXPECT_TRUE(renderParams->needSync_);
}

/**
 * @tc.name: TestSetChildHasVisibleFilter_01
 * @tc.desc: Test function SetChildHasVisibleFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetChildHasVisibleFilter_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    bool val = true;
    renderParams->childHasVisibleFilter_  = val;
    EXPECT_EQ(renderParams->childHasVisibleFilter_, val);
    renderParams->needSync_ = false;

    renderParams->SetChildHasVisibleFilter(val);
    EXPECT_FALSE(renderParams->needSync_);
}

/**
 * @tc.name: TestSetChildHasVisibleEffect_01
 * @tc.desc: Test function SetChildHasVisibleEffect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetChildHasVisibleEffect_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    bool val = true;
    renderParams->childHasVisibleEffect_   = val;
    EXPECT_EQ(renderParams->childHasVisibleEffect_, val);
    renderParams->needSync_ = false;

    renderParams->SetChildHasVisibleEffect(val);
    EXPECT_FALSE(renderParams->needSync_);
}

/**
 * @tc.name: TestGetNeedUpdateCache_01
 * @tc.desc: Test function GetNeedUpdateCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestGetNeedUpdateCache_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->isNeedUpdateCache_ = false;

    EXPECT_FALSE(renderParams->GetNeedUpdateCache());
}

/**
 * @tc.name: TestSetDrawingCacheIncludeProperty_01
 * @tc.desc: Test function SetDrawingCacheIncludeProperty
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetDrawingCacheIncludeProperty_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());

    bool includeProperty = false;
    renderParams->drawingCacheIncludeProperty_  = true;
    renderParams->needSync_ = false;

    EXPECT_NE(renderParams->drawingCacheIncludeProperty_, includeProperty);
    renderParams->SetDrawingCacheIncludeProperty(includeProperty);
    EXPECT_TRUE(renderParams->needSync_);
}

/**
 * @tc.name: TestOpincSetCacheChangeFlag_01
 * @tc.desc: Test function OpincSetCacheChangeFlag, not lastFrameSynced
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestOpincSetCacheChangeFlag_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->isOpincStateChanged_ = false;
    renderParams->needSync_  = false;

    bool state = true;
    bool lastFrameSynced = false;
    renderParams->OpincSetCacheChangeFlag(state, lastFrameSynced);
    EXPECT_TRUE(renderParams->needSync_);
    EXPECT_TRUE(renderParams->isOpincStateChanged_);
}

/**
 * @tc.name: TestSetNeedFilter_01
 * @tc.desc: Test function SetNeedFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetNeedFilter_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->needFilter_ = false;
    bool needFilter = true;
    renderParams->needSync_ = false;

    renderParams->SetNeedFilter(needFilter);
    EXPECT_TRUE(renderParams->needSync_);
    EXPECT_EQ(renderParams->needFilter_, needFilter);
}

/**
 * @tc.name: TestOnCanvasDrawingSurfaceChange_01
 * @tc.desc: Test function OnCanvasDrawingSurfaceChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestOnCanvasDrawingSurfaceChange_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());

    constexpr NodeId targetId = TestSrc::limitNumber::Uint64[5];
    const std::unique_ptr<RSRenderParams> targetParams = std::make_unique<RSRenderParams>(targetId);

    renderParams->canvasDrawingNodeSurfaceChanged_ = true;
    renderParams->surfaceParams_.width = 2.0;
    renderParams->surfaceParams_.height = 2.0;

    renderParams->OnCanvasDrawingSurfaceChange(targetParams);
    EXPECT_EQ(targetParams->canvasDrawingNodeSurfaceChanged_, true);
    EXPECT_EQ(targetParams->surfaceParams_.width, renderParams->surfaceParams_.width);
    EXPECT_EQ(targetParams->surfaceParams_.height, renderParams->surfaceParams_.height);
    EXPECT_FALSE(renderParams->canvasDrawingNodeSurfaceChanged_);
}

/**
 * @tc.name: TestGetCanvasDrawingSurfaceChanged_01
 * @tc.desc: Test function GetCanvasDrawingSurfaceChanged
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestGetCanvasDrawingSurfaceChanged_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->canvasDrawingNodeSurfaceChanged_ = false;

    EXPECT_FALSE(renderParams->GetCanvasDrawingSurfaceChanged());
}

/**
 * @tc.name: TestSetForegroundFilterCache_01
 * @tc.desc: Test function SetForegroundFilterCache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetForegroundFilterCache_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->needSync_ = false;
    renderParams->foregroundFilterCache_ = std::make_shared<RSFilter>(); // type_(FilterType::NONE)
    auto foregroundFilterCache = std::make_shared<RSFilter>();
    foregroundFilterCache->type_ = RSFilter::FilterType::BLUR;
    EXPECT_NE(foregroundFilterCache, renderParams->foregroundFilterCache_);

    renderParams->SetForegroundFilterCache(foregroundFilterCache);
    EXPECT_EQ(foregroundFilterCache, renderParams->foregroundFilterCache_);
    EXPECT_TRUE(renderParams->needSync_);
}

/**
 * @tc.name: TestGetCanvasDrawingSurfaceParams_01
 * @tc.desc: Test function GetCanvasDrawingSurfaceParams
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestGetCanvasDrawingSurfaceParams_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->surfaceParams_.height = 2;
    renderParams->surfaceParams_.width = 3;
    auto surfaceParams = renderParams->GetCanvasDrawingSurfaceParams();
    EXPECT_EQ(surfaceParams.height, renderParams->surfaceParams_.height);
    EXPECT_EQ(surfaceParams.width, renderParams->surfaceParams_.width);
}

/**
 * @tc.name: TestOnSync_02
 * @tc.desc: Test function OnSync
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestOnSync_02, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());

    constexpr NodeId targetId = TestSrc::limitNumber::Uint64[5];
    const std::unique_ptr<RSRenderParams> targetParams = std::make_unique<RSRenderParams>(targetId);

    targetParams->drawingCacheType_ = RSDrawingCacheType::DISABLED_CACHE;
    renderParams->drawingCacheType_ = RSDrawingCacheType::FORCED_CACHE;
    renderParams->dirtyType_.set(RSRenderParamsDirtyType::DRAWING_CACHE_TYPE_DIRTY);

    renderParams->OnSync(targetParams);
    EXPECT_EQ(targetParams->drawingCacheType_, renderParams->drawingCacheType_);
}

/**
 * @tc.name: TestSetUiFirstRootNode_01
 * @tc.desc: Test function SetUiFirstRootNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestSetUiFirstRootNode_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());

    renderParams->uifirstRootNodeId_ = 0;
    NodeId uifirstRootNodeId = 1;
    renderParams->needSync_ = false;
    renderParams->SetUiFirstRootNode(uifirstRootNodeId);
    EXPECT_EQ(uifirstRootNodeId, renderParams->uifirstRootNodeId_);
    EXPECT_TRUE(renderParams->needSync_);
}

/**
 * @tc.name: TestGetLayerInfo_01
 * @tc.desc: Test function GetLayerInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsUnitTest, TestGetLayerInfo_01, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());

    RSLayerInfo defaultLayerInfo = {};
    EXPECT_EQ(defaultLayerInfo, renderParams->GetLayerInfo());
}
} // namespace OHOS::Rosen