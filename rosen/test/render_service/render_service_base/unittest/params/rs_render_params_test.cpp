/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
class RSRenderParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static void DisplayTestInfo();
};

void RSRenderParamsTest::SetUpTestCase() {}
void RSRenderParamsTest::TearDownTestCase() {}
void RSRenderParamsTest::SetUp() {}
void RSRenderParamsTest::TearDown() {}
void RSRenderParamsTest::DisplayTestInfo()
{
    return;
}

/**
 * @tc.name: OnSync001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsTest, OnSync001, TestSize.Level1)
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
 * @tc.name: SetAlpha001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsTest, SetAlpha001, TestSize.Level1)
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
 * @tc.name: SetAlphaOffScreen001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsTest, SetAlphaOffScreen001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSRenderParams params(id);
    bool alphaOffScreen = false;
    params.SetAlphaOffScreen(alphaOffScreen);
    EXPECT_TRUE(!params.GetAlphaOffScreen());
}

/**
 * @tc.name: ApplyAlphaAndMatrixToCanvas001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderParamsTest, ApplyAlphaAndMatrixToCanvas001, TestSize.Level1)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[0];
    RSRenderParams params(id);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    params.ApplyAlphaAndMatrixToCanvas(paintFilterCanvas, true);
    EXPECT_TRUE(id != -1);
}

/**
 * @tc.name: SetAlpha_002
 * @tc.desc: Test function SetAlpha，alpha != renderParams->alpha_
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, SetAlpha_002, TestSize.Level2)
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
 * @tc.name: SetAlphaOffScreen_002
 * @tc.desc: Test function SetAlphaOffScreen, alphaOffScreen != params.alphaOffScreen_
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, SetAlphaOffScreen_002, TestSize.Level2)
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
 * @tc.name: ApplyAlphaAndMatrixToCanvas_002
 * @tc.desc: Test function ApplyAlphaAndMatrixToCanvas, UNLIKELY(HasSandBox()) is true and applyMatrix is false
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, ApplyAlphaAndMatrixToCanvas_002, TestSize.Level2)
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
 * @tc.name: ApplyAlphaAndMatrixToCanvas_003
 * @tc.desc: Test function ApplyAlphaAndMatrixToCanvas, UNLIKELY(HasSandBox()) and applyMatrix is true
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, ApplyAlphaAndMatrixToCanvas_003, TestSize.Level2)
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
 * @tc.name: ApplyAlphaAndMatrixToCanvas_004
 * @tc.desc: Test function ApplyAlphaAndMatrixToCanvas， not HasSandBox and not applyMatrix
 * and alpha_ < 1.0f and (drawingCacheType_ == RSDrawingCacheType::FORCED_CACHE) and alphaOffScreen_ is false
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, ApplyAlphaAndMatrixToCanvas_004, TestSize.Level2)
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
 * @tc.name: ApplyAlphaAndMatrixToCanvas_005
 * @tc.desc: Test function ApplyAlphaAndMatrixToCanvas， not HasSandBox and not applyMatrix
 * and alpha_ < 1.0f and (drawingCacheType_ != RSDrawingCacheType::FORCED_CACHE) and alphaOffScreen_ is true
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, ApplyAlphaAndMatrixToCanvas_005, TestSize.Level2)
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
 * @tc.name: SetBoundsRect_001
 * @tc.desc: Test function SetBoundsRect
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, SetBoundsRect_001, TestSize.Level2)
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
 * @tc.name: SetFrameRect_001
 * @tc.desc: Test function SetFrameRect
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, SetFrameRect_001, TestSize.Level2)
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
 * @tc.name: SetHasSandBox_001
 * @tc.desc: Test function SetHasSandBox
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, SetHasSandBox_001, TestSize.Level2)
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
 * @tc.name: SetLocalDrawRect_001
 * @tc.desc: Test function SetLocalDrawRect
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, SetLocalDrawRect_001, TestSize.Level2)
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
 * @tc.name: GetLocalDrawRect_001
 * @tc.desc: Test function GetLocalDrawRect
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, GetLocalDrawRect_001, TestSize.Level2)
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
 * @tc.name: SetContentEmpty_001
 * @tc.desc: Test function SetContentEmpty
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, SetContentEmpty_001, TestSize.Level2)
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
 * @tc.name: SetChildHasVisibleFilter_001
 * @tc.desc: Test function SetChildHasVisibleFilter
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, SetChildHasVisibleFilter_001, TestSize.Level2)
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
 * @tc.name: SetChildHasVisibleEffect_001
 * @tc.desc: Test function SetChildHasVisibleEffect
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, SetChildHasVisibleEffect_001, TestSize.Level2)
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
 * @tc.name: GetNeedUpdateCache_001
 * @tc.desc: Test function GetNeedUpdateCache
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, GetNeedUpdateCache_001, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->isNeedUpdateCache_ = false;

    EXPECT_FALSE(renderParams->GetNeedUpdateCache());
}

/**
 * @tc.name: SetDrawingCacheIncludeProperty_001
 * @tc.desc: Test function SetDrawingCacheIncludeProperty
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, SetDrawingCacheIncludeProperty_001, TestSize.Level2)
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
 * @tc.name: OpincSetIsSuggest
 * @tc.desc: Test function OpincSetIsSuggest
 * @tc.type:FUNC
 * @tc.require:issueIC87OJ
 */
HWTEST_F(RSRenderParamsTest, OpincSetIsSuggest, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->isOpincSuggestFlag_ = false;
    renderParams->needSync_  = false;

    renderParams->OpincSetIsSuggest(true);
    EXPECT_TRUE(renderParams->needSync_);
    EXPECT_TRUE(renderParams->OpincIsSuggest());
}

/**
 * @tc.name: OpincUpdateSupportFlag
 * @tc.desc: Test function OpincUpdateSupportFlag
 * @tc.type:FUNC
 * @tc.require:issueIC87OJ
 */
HWTEST_F(RSRenderParamsTest, OpincUpdateSupportFlag, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->isOpincSupportFlag_ = false;
    renderParams->needSync_  = false;

    renderParams->OpincUpdateSupportFlag(true);
    EXPECT_TRUE(renderParams->needSync_);
    EXPECT_TRUE(renderParams->OpincGetSupportFlag());
}

/**
 * @tc.name: OpincSetCacheChangeFlag_001
 * @tc.desc: Test function OpincSetCacheChangeFlag, not lastFrameSynced
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, OpincSetCacheChangeFlag_001, TestSize.Level2)
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
 * @tc.name: SetNeedFilter_001
 * @tc.desc: Test function SetNeedFilter
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, SetNeedFilter_001, TestSize.Level2)
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
 * @tc.name: SetHDRBrightness
 * @tc.desc: Test function SetHDRBrightness
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, SetHDRBrightnessTest, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->hdrBrightness_ = 0.0f;
    float hdrBrightness = 1.0f;
    renderParams->needSync_ = false;

    renderParams->SetHDRBrightness(hdrBrightness);
    EXPECT_TRUE(renderParams->needSync_);
    EXPECT_EQ(renderParams->hdrBrightness_, hdrBrightness);
}

/**
 * @tc.name: OnCanvasDrawingSurfaceChange_001
 * @tc.desc: Test function OnCanvasDrawingSurfaceChange
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, OnCanvasDrawingSurfaceChange_001, TestSize.Level2)
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
 * @tc.name: GetCanvasDrawingSurfaceChanged_001
 * @tc.desc: Test function GetCanvasDrawingSurfaceChanged
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, GetCanvasDrawingSurfaceChanged_001, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());
    renderParams->canvasDrawingNodeSurfaceChanged_ = false;

    EXPECT_FALSE(renderParams->GetCanvasDrawingSurfaceChanged());
}

/**
 * @tc.name: SetForegroundFilterCache_001
 * @tc.desc: Test function SetForegroundFilterCache
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, SetForegroundFilterCache_001, TestSize.Level2)
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
    EXPECT_TRUE(renderParams->needSync_);
}

/**
 * @tc.name: GetCanvasDrawingSurfaceParams_001
 * @tc.desc: Test function GetCanvasDrawingSurfaceParams
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, GetCanvasDrawingSurfaceParams_001, TestSize.Level2)
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
 * @tc.name: OnSync_002
 * @tc.desc: Test function OnSync
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, OnSync_002, TestSize.Level2)
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
 * @tc.name: RepaintBoundary
 * @tc.desc: Test function MarkRepaintBoundary and IsRepaintBoundary
 * @tc.type:FUNC
 * @tc.require: issuesIC50OX
 */
HWTEST_F(RSRenderParamsTest, RepaintBoundary, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> renderParams = std::make_unique<RSRenderParams>(id);
    renderParams->MarkRepaintBoundary(true);
    EXPECT_TRUE(renderParams->IsRepaintBoundary());
}

/**
 * @tc.name: SetUiFirstRootNode_001
 * @tc.desc: Test function SetUiFirstRootNode
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, SetUiFirstRootNode_001, TestSize.Level2)
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
 * @tc.name: GetLayerInfo_001
 * @tc.desc: Test function GetLayerInfo
 * @tc.type:FUNC
 * @tc.require:issueIB1KXV
 */
HWTEST_F(RSRenderParamsTest, GetLayerInfo_001, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> target = std::make_unique<RSRenderParams>(id);
    RSRenderParams params(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());

    RSLayerInfo defaultLayerInfo = {};
    EXPECT_EQ(defaultLayerInfo, renderParams->GetLayerInfo());
}

/**
 * @tc.name: EnableWindowKeyFrame
 * @tc.desc: Test EnableWindowKeyFrame
 * @tc.type: FUNC
 * @tc.require:#IBPVN9
 */
HWTEST_F(RSRenderParamsTest, EnableWindowKeyFrame, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> renderParams = std::make_unique<RSRenderParams>(id);

    renderParams->EnableWindowKeyFrame(true);
    EXPECT_TRUE(renderParams->IsWindowKeyFrameEnabled());
    EXPECT_TRUE(renderParams->needSync_);
}

/**
 * @tc.name: SetNeedSwapBuffer
 * @tc.desc: Test SetNeedSwapBuffer
 * @tc.type: FUNC
 * @tc.require:#IBPVN9
 */
HWTEST_F(RSRenderParamsTest, SetNeedSwapBuffer, TestSize.Level2)
{
    constexpr NodeId id = TestSrc::limitNumber::Uint64[4];
    std::unique_ptr<RSRenderParams> renderParams = std::make_unique<RSRenderParams>(id);

    renderParams->SetNeedSwapBuffer(true);
    EXPECT_TRUE(renderParams->GetNeedSwapBuffer());
    EXPECT_TRUE(renderParams->needSync_);
}
} // namespace OHOS::Rosen