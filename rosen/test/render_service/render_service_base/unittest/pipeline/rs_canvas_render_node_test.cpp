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
#include "pipeline/rs_render_thread_visitor.h"
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
    static inline Drawing::Canvas drawingCanvas_;
};

void RSCanvasRenderNodeTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
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
 * @tc.desc: test results of UpdateRecording
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, UpdateRecording001, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    canvasRenderNode->UpdateRecording(nullptr, RSModifierType::INVALID);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: UpdateRecording002
 * @tc.desc: test results of UpdateRecording
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, UpdateRecording002, TestSize.Level1)
{
    int32_t w;
    int32_t h;
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id + 1);
    auto drawCmds = std::make_shared<Drawing::DrawCmdList>(w, h);
    canvasRenderNode->UpdateRecording(drawCmds, RSModifierType::INVALID);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: PrepareTest
 * @tc.desc: test results of Prepare
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

    visitor = std::make_shared<RSRenderThreadVisitor>();
    rsCanvasRenderNode.Prepare(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ProcessTest
 * @tc.desc: test results of Process
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

    visitor = std::make_shared<RSRenderThreadVisitor>();
    rsCanvasRenderNode.Process(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ProcessAnimatePropertyAfterChildrenTest001
 * @tc.desc: test results of ProcessRenderAfterChildren
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessAnimatePropertyAfterChildrenTest001, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    canvasRenderNode->ProcessRenderAfterChildren(*canvas_);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ColorBlendModeTest
 * @tc.desc: test results of ColorBlendMode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ColorBlendModeTest, TestSize.Level1)
{
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(id, context);
    canvas_->SaveLayer({ nullptr, nullptr });

    int blendMode = 0;
    auto convertToBlendMode = [&blendMode]() {
        static const std::unordered_map<int, Drawing::BlendMode> blendModeLUT = {
            { static_cast<int>(RSColorBlendMode::DST_IN), Drawing::BlendMode::DST_IN },
            { static_cast<int>(RSColorBlendMode::SRC_IN), Drawing::BlendMode::SRC_IN }
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
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ProcessShadowBatchingTest
 * @tc.desc: test results of ProcessShadowBatching
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessShadowBatchingTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.ProcessShadowBatching(*canvas_);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: OnTreeStateChanged
 * @tc.desc: test results of OnTreeStateChanged
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, OnTreeStateChanged, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.OnTreeStateChanged();
    rsCanvasRenderNode.isOnTheTree_ = true;
    rsCanvasRenderNode.OnTreeStateChanged();
    rsCanvasRenderNode.cacheType_ = CacheType::CONTENT;
    rsCanvasRenderNode.OnTreeStateChanged();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: DrawShadow
 * @tc.desc: test results of DrawShadow
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, DrawShadow, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    RSProperties property;
    RSModifierContext modifierContext(property);
    Drawing::Canvas canvasArgs(1, 1);
    RSPaintFilterCanvas canvas(&canvasArgs);
    rsCanvasRenderNode.DrawShadow(modifierContext, canvas);

    RSContext* rsContext = new RSContext();
    std::shared_ptr<RSContext> sharedContext(rsContext);
    std::weak_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(1, sharedContext);
    rsCanvasRenderNode.SetParent(parent);
    rsCanvasRenderNode.DrawShadow(modifierContext, canvas);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: PropertyDrawableRender
 * @tc.desc: test results of PropertyDrawableRender
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, PropertyDrawableRender, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    bool includeProperty = false;
    Drawing::Canvas canvasArgs(1, 1);
    RSPaintFilterCanvas canvas(&canvasArgs);
    rsCanvasRenderNode.PropertyDrawableRender(canvas, includeProperty);
    includeProperty = true;
    rsCanvasRenderNode.PropertyDrawableRender(canvas, includeProperty);

    RSContext* rsContext = new RSContext();
    std::shared_ptr<RSContext> sharedContext(rsContext);
    std::weak_ptr<RSRenderNode> parent = std::make_shared<RSRenderNode>(1, sharedContext);
    rsCanvasRenderNode.SetParent(parent);
    rsCanvasRenderNode.PropertyDrawableRender(canvas, includeProperty);
    includeProperty = false;
    rsCanvasRenderNode.PropertyDrawableRender(canvas, includeProperty);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ApplyDrawCmdModifier
 * @tc.desc: test results of ApplyDrawCmdModifier
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ApplyDrawCmdModifier, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    RSProperties property;
    RSModifierContext modifierContext(property);
    RSModifierType type = RSModifierType::INVALID;
    rsCanvasRenderNode.ApplyDrawCmdModifier(modifierContext, type);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: InternalDrawContent
 * @tc.desc: test results of InternalDrawContent
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, InternalDrawContent, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::weak_ptr<RSContext> context;
    RSCanvasRenderNode rsCanvasRenderNode(nodeId, context);
    rsCanvasRenderNode.InternalDrawContent(*canvas_);
    RSProperties* properties = const_cast<RSProperties*>(&rsCanvasRenderNode.GetRenderProperties());
    properties->SetClipToFrame(true);
    rsCanvasRenderNode.InternalDrawContent(*canvas_);
    ASSERT_TRUE(true);
}
} // namespace OHOS::Rosen