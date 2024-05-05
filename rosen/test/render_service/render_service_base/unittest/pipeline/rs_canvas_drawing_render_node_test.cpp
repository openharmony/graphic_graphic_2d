/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "draw/canvas.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasDrawingRenderNodeTest : public testing::Test {
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

void RSCanvasDrawingRenderNodeTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSCanvasDrawingRenderNodeTest::TearDownTestCase()
{
    if (canvas_) {
        delete canvas_;
        canvas_ = nullptr;
    }
}
void RSCanvasDrawingRenderNodeTest::SetUp() {}
void RSCanvasDrawingRenderNodeTest::TearDown() {}

/**
 * @tc.name: ProcessRenderContentsTest
 * @tc.desc: test results of ProcessRenderContents
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ProcessRenderContentsTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    rsCanvasDrawingRenderNode.ProcessRenderContents(*canvas_);

    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
        std::make_shared<RSGeometryTransRenderModifier>(property);
    std::shared_ptr<RSRenderModifier> modifier = modifierCast;
    modifierCast->drawStyle_ = RSModifierType::BOUNDS;
    rsCanvasDrawingRenderNode.boundsModifier_ = modifier;
    rsCanvasDrawingRenderNode.AddGeometryModifier(modifier);
    rsCanvasDrawingRenderNode.surface_ = std::make_shared<Drawing::Surface>();
    rsCanvasDrawingRenderNode.image_ = std::make_shared<Drawing::Image>();
    rsCanvasDrawingRenderNode.ProcessRenderContents(*canvas_);
    ASSERT_FALSE(rsCanvasDrawingRenderNode.isNeedProcess_);
}

/**
 * @tc.name: ResetSurfaceTest
 * @tc.desc: test results of ResetSurface
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ResetSurfaceTest, TestSize.Level1)
{
    int width = 0;
    int height = 0;
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    auto res = rsCanvasDrawingRenderNode.ResetSurface(width, height, *canvas_);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: GetSizeFromDrawCmdModifiersTest001
 * @tc.desc: test results of GetSizeFromDrawCmdModifiers
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetSizeFromDrawCmdModifiersTest001, TestSize.Level1)
{
    int width = 1;
    int height = 1;
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    ASSERT_FALSE(rsCanvasDrawingRenderNode.GetSizeFromDrawCmdModifiers(width, height));
}

/**
 * @tc.name: GetSizeFromDrawCmdModifiersTest002
 * @tc.desc: test results of GetSizeFromDrawCmdModifiers
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetSizeFromDrawCmdModifiersTest002, TestSize.Level1)
{
    int width = -1;
    int height = -1;
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);

    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
        std::make_shared<RSGeometryTransRenderModifier>(property);
    modifierCast->SetType(RSModifierType::CONTENT_STYLE);
    std::shared_ptr<RSRenderModifier> modifier = modifierCast;
    rsCanvasDrawingRenderNode.modifiers_.emplace(modifier->GetPropertyId(), modifier);
    auto res = rsCanvasDrawingRenderNode.GetSizeFromDrawCmdModifiers(width, height);
    ASSERT_FALSE(res);

    width = 1;
    height = 1;
    ASSERT_FALSE(rsCanvasDrawingRenderNode.GetSizeFromDrawCmdModifiers(width, height));
}

/**
 * @tc.name: IsNeedResetSurfaceTest
 * @tc.desc: test results of IsNeedResetSurface
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, IsNeedResetSurfaceTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    rsCanvasDrawingRenderNode.surface_ = std::make_shared<Drawing::Surface>();
    ASSERT_TRUE(rsCanvasDrawingRenderNode.IsNeedResetSurface());
}

/**
 * @tc.name: PlaybackInCorrespondThread
 * @tc.desc: test results of PlaybackInCorrespondThread
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, PlaybackInCorrespondThread, TestSize.Level1)
{
    NodeId nodeId = 1;
    RSContext* rsContext = new RSContext();
    std::shared_ptr<RSContext> sharedContext(rsContext);
    std::weak_ptr<RSContext> context = sharedContext;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    rsCanvasDrawingRenderNode.PlaybackInCorrespondThread();
    int32_t width = 1;
    int32_t height = 1;
    Drawing::Canvas canvas(width, height);
    rsCanvasDrawingRenderNode.canvas_ = std::make_unique<RSPaintFilterCanvas>(&canvas);
    rsCanvasDrawingRenderNode.PlaybackInCorrespondThread();
    rsCanvasDrawingRenderNode.surface_ = std::make_shared<Drawing::Surface>();
    rsCanvasDrawingRenderNode.PlaybackInCorrespondThread();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ProcessCPURenderInBackgroundThread
 * @tc.desc: test results of ProcessCPURenderInBackgroundThread
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ProcessCPURenderInBackgroundThread, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    int32_t width = 1;
    int32_t height = 1;
    std::shared_ptr<Drawing::DrawCmdList> cmds = std::make_shared<Drawing::DrawCmdList>(width, height);
    rsCanvasDrawingRenderNode.ProcessCPURenderInBackgroundThread(cmds);

    rsCanvasDrawingRenderNode.surface_ = std::make_shared<Drawing::Surface>();
    rsCanvasDrawingRenderNode.ProcessCPURenderInBackgroundThread(cmds);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ApplyDrawCmdModifier
 * @tc.desc: test results of ApplyDrawCmdModifier
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ApplyDrawCmdModifier, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);

    RSProperties properties;
    RSModifierContext contextArgs(properties);
    rsCanvasDrawingRenderNode.ApplyDrawCmdModifier(contextArgs, RSModifierType::CONTENT_STYLE);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetBitmap
 * @tc.desc: test results of GetBitmap
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetBitmap, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    uint64_t tid = 1;
    rsCanvasDrawingRenderNode.GetBitmap(tid);

    rsCanvasDrawingRenderNode.image_ = std::make_shared<Drawing::Image>();
    ASSERT_NE(rsCanvasDrawingRenderNode.GetBitmap(tid).bmpImplPtr, nullptr);
}

/**
 * @tc.name: GetPixelmap
 * @tc.desc: test results of GetPixelmap
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetPixelmap, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    uint64_t tid = 1;
    int32_t width = 1;
    int32_t height = 1;
    {
        std::shared_ptr<Media::PixelMap> pixelmap = std::make_shared<Media::PixelMap>();
        Drawing::Rect rect(1, 1, 1, 1);
        std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>(width, height);
        ASSERT_FALSE(rsCanvasDrawingRenderNode.GetPixelmap(pixelmap, &rect, tid, drawCmdList));
    }
    {
        std::shared_ptr<Media::PixelMap> pixelmap;
        Drawing::Rect* rect = nullptr;
        std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>(width, height);
        ASSERT_FALSE(rsCanvasDrawingRenderNode.GetPixelmap(pixelmap, rect, tid, drawCmdList));
    }
    {
        std::shared_ptr<Media::PixelMap> pixelmap = std::make_shared<Media::PixelMap>();
        Drawing::Rect rect(1, 1, 1, 1);
        std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>(width, height);
        rsCanvasDrawingRenderNode.surface_ = std::make_shared<Drawing::Surface>();
        uint64_t tidTwo = OHOS::Rosen::UNI_MAIN_THREAD_INDEX;
        ASSERT_FALSE(rsCanvasDrawingRenderNode.GetPixelmap(pixelmap, &rect, tid, drawCmdList));
        ASSERT_FALSE(rsCanvasDrawingRenderNode.GetPixelmap(pixelmap, &rect, tidTwo, drawCmdList));
    }
}

/**
 * @tc.name: AddDirtyType
 * @tc.desc: test results of AddDirtyType
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, AddDirtyType, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
        std::make_shared<RSGeometryTransRenderModifier>(property);
    std::shared_ptr<RSRenderModifier> modifier = modifierCast;
    modifierCast->drawStyle_ = RSModifierType::BOUNDS;
    rsCanvasDrawingRenderNode.boundsModifier_ = modifier;
    rsCanvasDrawingRenderNode.AddGeometryModifier(modifier);
    rsCanvasDrawingRenderNode.AddDirtyType(RSModifierType::BLOOM);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: ResetSurface
 * @tc.desc: test results of ResetSurface
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ResetSurface, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    rsCanvasDrawingRenderNode.stagingRenderParams_ = std::make_unique<RSRenderParams>(nodeId);
    rsCanvasDrawingRenderNode.ResetSurface();
    ASSERT_EQ(rsCanvasDrawingRenderNode.surface_, nullptr);
    
    rsCanvasDrawingRenderNode.surface_ = std::make_shared<Drawing::Surface>();
    rsCanvasDrawingRenderNode.ResetSurface();
    ASSERT_EQ(rsCanvasDrawingRenderNode.surface_, nullptr);
}
} // namespace OHOS::Rosen