/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_canvas_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline RSPaintFilterCanvas* canvas_;
#ifndef USE_ROSEN_DRAWING
    static inline SkCanvas skCanvas_;
#else
    static inline Drawing::Canvas drawingCanvas_;
#endif
};

void RSCanvasRenderNodeTest::SetUpTestCase()
{
#ifndef USE_ROSEN_DRAWING
    canvas_ = new RSPaintFilterCanvas(&skCanvas_);
#else
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
#endif
}
void RSCanvasRenderNodeTest::TearDownTestCase()
{
    delete canvas_;
    canvas_ = nullptr;
}
void RSCanvasRenderNodeTest::SetUp() {}
void RSCanvasRenderNodeTest::TearDown() {}

/**
 * @tc.name: UpdateRecording001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, UpdateRecording001, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    canvasRenderNode->UpdateRecording(nullptr, RSModifierType::INVALID);
}

/**
 * @tc.name: UpdateRecording002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, UpdateRecording002, TestSize.Level1)
{
    int32_t w;
    int32_t h;
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id + 1);
#ifndef USE_ROSEN_DRAWING
    auto drawCmds = std::make_shared<DrawCmdList>(w, h);
#else
    auto drawCmds = std::make_shared<Drawing::DrawCmdList>(w, h);
#endif
    canvasRenderNode->UpdateRecording(drawCmds, RSModifierType::INVALID);
}

/**
 * @tc.name: PrepareTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, PrepareTest, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.Prepare(visitor);
}

/**
 * @tc.name: ProcessTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessTest, TestSize.Level1)
{
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.Process(visitor);
}

/**
 * @tc.name: ProcessAnimatePropertyAfterChildrenTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessAnimatePropertyAfterChildrenTest001, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    canvasRenderNode->ProcessRenderAfterChildren(*canvas_);
}

/**
 * @tc.name: ProcessDrivenBackgroundRenderTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessDrivenBackgroundRenderTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.ProcessDrivenBackgroundRender(*canvas_);
#if defined(RS_ENABLE_DRIVEN_RENDER)
    EXPECT_EQ(rsCanvasRenderNode.GetChildrenCount(), 0);
#endif
}

/**
 * @tc.name: ColorBlendModeTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ColorBlendModeTest, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
#ifndef USE_ROSEN_DRAWING
    canvas_->saveLayer(nullptr, nullptr);

    int blendMode = 0;
    auto convertToSkBlendMode = [&blendMode]() {
        static const std::unordered_map<int, SkBlendMode> skBlendModeLUT = {
            {static_cast<int>(RSColorBlendMode::DST_IN), SkBlendMode::kDstIn},
            {static_cast<int>(RSColorBlendMode::SRC_IN), SkBlendMode::kSrcIn}
        };

        auto iter = skBlendModeLUT.find(blendMode);
        if (iter == skBlendModeLUT.end()) {
            ROSEN_LOGE("The desired color_blend_mode is undefined, and the SkBlendMode::kSrc is used.");
            return SkBlendMode::kSrc;
        }

        return skBlendModeLUT.at(blendMode);
    };
    SkBlendMode skBlendMode = convertToSkBlendMode();
    SkPaint maskPaint;
    maskPaint.setBlendMode(skBlendMode);
    SkCanvas::SaveLayerRec maskLayerRec(nullptr, &maskPaint, nullptr, 0);
    canvas_->saveLayer(maskLayerRec);

    canvas_->restore();
    canvas_->restore();
#else
    canvas_->SaveLayer({nullptr, nullptr});

    int blendMode = 0;
    auto convertToBlendMode = [&blendMode]() {
        static const std::unordered_map<int, Drawing::BlendMode> blendModeLUT = {
            {static_cast<int>(RSColorBlendMode::DST_IN), Drawing::BlendMode::DST_IN},
            {static_cast<int>(RSColorBlendMode::SRC_IN), Drawing::BlendMode::SRC_IN}
        };

        auto iter = blendModeLUT.find(blendMode);
        if (iter == blendModeLUT.end()) {
            ROSEN_LOGE("The desired color_blend_mode is undefined, and the Drawing::BlendMode::SRC is used.");
            return Drawing::BlendMode::SRC;
        }

        return blendModeLUT.at(blendMode);
    };
    Drawing::BlendMode drawingBlendMode = convertToBlendMode();
    Drawing::Brush maskBrush;
    maskBrush.SetBlendMode(drawingBlendMode);
    Drawing::SaveLayerOps maskLayerRec(nullptr, &maskBrush, 0);
    canvas_->SaveLayer(maskLayerRec);

    canvas_->Restore();
    canvas_->Restore();
#endif
}

/**
 * @tc.name: DangerousFastBlendModeTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, DangerousFastBlendModeTest, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    RSProperties properties;
    properties.SetColorBlendMode(static_cast<int>(RSColorBlendMode::NONE));
    properties.SetColorBlendApplyType(static_cast<int>(RSColorBlendApplyType::FAST));
    int initCnt = canvas_->GetBlendOffscreenLayerCnt();
    RSPropertiesPainter::BeginBlendMode(*canvas_, properties);
    EXPECT_EQ(canvas_->GetBlendOffscreenLayerCnt(), initCnt);
    RSPropertiesPainter::EndBlendMode(*canvas_, properties);
}

/**
 * @tc.name: DangerousSavelayerBlendModeTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, DangerousSavelayerBlendModeTest, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    RSProperties properties;
    properties.SetColorBlendMode(static_cast<int>(RSColorBlendMode::NONE));
    properties.SetColorBlendApplyType(static_cast<int>(RSColorBlendApplyType::SAVE_LAYER));
    int initCnt = canvas_->GetBlendOffscreenLayerCnt();
    RSPropertiesPainter::BeginBlendMode(*canvas_, properties);
    EXPECT_EQ(canvas_->GetBlendOffscreenLayerCnt(), initCnt);
    RSPropertiesPainter::EndBlendMode(*canvas_, properties);
    properties.SetColorBlendMode(static_cast<int>(RSColorBlendMode::CLEAR));
    RSPropertiesPainter::BeginBlendMode(*canvas_, properties);
    EXPECT_EQ(canvas_->GetBlendOffscreenLayerCnt(), initCnt + 2);
    RSPropertiesPainter::EndBlendMode(*canvas_, properties);
}
/**
 * @tc.name: ProcessShadowBatchingTest
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessShadowBatchingTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.ProcessShadowBatching(*canvas_);
}
} // namespace OHOS::Rosen