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
#include "modifier/rs_extended_modifier.h"
#include <memory>
#include "modifier/rs_render_modifier.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_recording_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSExtendedModifierUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSExtendedModifierUnitTest::SetUpTestCase() {}
void RSExtendedModifierUnitTest::TearDownTestCase() {}
void RSExtendedModifierUnitTest::SetUp() {}
void RSExtendedModifierUnitTest::TearDown() {}

class ExtendedModifierUnitTest : public RSExtendedModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        return;
    };
};

class GeometryTransModifierUnitTest : public RSGeometryTransModifier {
public:
    Drawing::Matrix GeometryEffect(float width, float height) const override
    {
        Drawing::Matrix matrix;
        return matrix;
    }
};

class TransitionModifierUnitTest : public RSTransitionModifier {
public:
    void Active() override
    {
        return;
    }

    void Identity() override
    {
        return;
    }
    void Draw(RSDrawingContext& context) const override
    {
        return;
    }
};

class BackgroundStyleModifierUnitTest : public RSBackgroundStyleModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        return;
    }
};
class ContentStyleModifierUnitTest : public RSContentStyleModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        return;
    }
};
class ForegroundStyleModifierUnitTest : public RSForegroundStyleModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        return;
    }
};
class OverlayStyleModifierUnitTest : public RSOverlayStyleModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        return;
    }
};
class NodeModifierUnitTest : public RSNodeModifier {
public:
    void Modify(RSNode& target) const override
    {
        return;
    }
};
/**
 * @tc.name: CreateDrawingContextTest
 * @tc.desc: test results of CreateDrawingContext
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSExtendedModifierUnitTest, CreateDrawingContextTest001, TestSize.Level1)
{
    NodeId nodeId = -1;
    RSExtendedModifierHelper::CreateDrawingContext(nodeId);
    EXPECT_EQ(RSNodeMap::Instance().GetNode<RSCanvasNode>(nodeId), nullptr);

    nodeId = 0;
    RSExtendedModifierHelper::CreateDrawingContext(nodeId);
    EXPECT_NE(RSNodeMap::Instance().GetNode<RSCanvasNode>(nodeId), nullptr);
}

/**
 * @tc.name: CreateRenderModifierTest
 * @tc.desc: test results of CreateRenderModifier
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSExtendedModifierUnitTest, CreateRenderModifierTest001, TestSize.Level1)
{
    RSDrawingContext ctx;
    PropertyId id = 1;
    RSModifierType type = RSModifierType::EXTENDED;
    std::shared_ptr<RSRenderModifier> ptr = RSExtendedModifierHelper::CreateRenderModifier(ctx, id, type);
    EXPECT_TRUE(ptr != nullptr);
}

/**
 * @tc.name: FinishDrawingTest
 * @tc.desc: test results of FinishDrawing
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSExtendedModifierUnitTest, FinishDrawingTest001, TestSize.Level1)
{
    RSDrawingContext ctx;
    std::shared_ptr<Drawing::DrawCmdList> ptr = RSExtendedModifierHelper::FinishDrawing(ctx);
    EXPECT_TRUE(ptr == nullptr);

    ctx.canvas = new Drawing::Canvas();
    ptr = RSExtendedModifierHelper::FinishDrawing(ctx);
    EXPECT_TRUE(ptr == nullptr);

    auto recordingCanvas = new ExtendRecordingCanvas(10, 10);
    ctx.canvas = recordingCanvas;
    RSSystemProperties::SetDrawTextAsBitmap(true);
    ptr = RSExtendedModifierHelper::FinishDrawing(ctx);
    EXPECT_TRUE(ptr != nullptr);
    RSSystemProperties::SetDrawTextAsBitmap(false);
}

/**
 * @tc.name: FinishDrawingTest001
 * @tc.desc: long text FinishDrawingTest
 * @tc.type: FUNC
 * @tc.require: issueI9P3X4
 */
HWTEST_F(RSExtendedModifierUnitTest, FinishDrawingTest001, TestSize.Level1)
{
    auto recordingCanvas = new ExtendRecordingCanvas(1000, 3000);
    Drawing::Font font = Drawing::Font();
    font.SetSize(10);
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromString("hello", font, Drawing::TextEncoding::UTF8);
    int y = 0;
    int x = 0;
    for (int i = 0; i < 20; i++) {
        recordingCanvas->DrawTextBlob(textBlob.get(), x, y);
        y += 100;
    }
    RSDrawingContext ctx = { recordingCanvas, 1000, 3000 };
    std::shared_ptr<Drawing::DrawCmdList> cmdList = RSExtendedModifierHelper::FinishDrawing(ctx);
    ASSERT_NE(cmdList, nullptr);
}

/**
 * @tc.name: GetModifierTypeTest001
 * @tc.desc: GetModifierType Test
 * @tc.type: FUNC
 * @tc.require: issueI9N22N
 */
HWTEST_F(RSExtendedModifierUnitTest, GetModifierTypeTest002, TestSize.Level1)
{
    ExtendedModifierUnitTest extendedModifier;
    GeometryTransModifierUnitTest geometryTransModifier;
    TransitionModifierUnitTest transitionModifier;
    BackgroundStyleModifierUnitTest backgroundStyleModifier;
    ContentStyleModifierUnitTest contentStyleModifier;
    ForegroundStyleModifierUnitTest foregroundStyleModifier;
    OverlayStyleModifierUnitTest overlayStyleModifier;
    NodeModifierUnitTest nodeModifier;

    ASSERT_EQ(extendedModifier.GetModifierType(), RSModifierType::EXTENDED);
    ASSERT_EQ(geometryTransModifier.GetModifierType(), RSModifierType::GEOMETRYTRANS);
    ASSERT_EQ(transitionModifier.GetModifierType(), RSModifierType::TRANSITION);
    ASSERT_EQ(backgroundStyleModifier.GetModifierType(), RSModifierType::BACKGROUND_STYLE);
    ASSERT_EQ(contentStyleModifier.GetModifierType(), RSModifierType::CONTENT_STYLE);
    ASSERT_EQ(foregroundStyleModifier.GetModifierType(), RSModifierType::FOREGROUND_STYLE);
    ASSERT_EQ(overlayStyleModifier.GetModifierType(), RSModifierType::OVERLAY_STYLE);
    ASSERT_EQ(nodeModifier.GetModifierType(), RSModifierType::NODE_MODIFIER);
}

/**
 * @tc.name: DrawTest001
 * @tc.desc: Draw Test
 * @tc.type: FUNC
 * @tc.require: issueI9N22N
 */
HWTEST_F(RSExtendedModifierUnitTest, DrawTest002, TestSize.Level1)
{
    GeometryTransModifierUnitTest geometryTransModifier;
    Drawing::Canvas canvas;
    RSDrawingContext rsDrawingContext = { &canvas, 1, 1 };
    geometryTransModifier.Draw(rsDrawingContext);
}
} // namespace OHOS::Rosen