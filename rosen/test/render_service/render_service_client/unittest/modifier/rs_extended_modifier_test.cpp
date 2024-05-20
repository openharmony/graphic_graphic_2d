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
class RSExtendedModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSExtendedModifierTest::SetUpTestCase() {}
void RSExtendedModifierTest::TearDownTestCase() {}
void RSExtendedModifierTest::SetUp() {}
void RSExtendedModifierTest::TearDown() {}

class ExtendedModifierTest : public RSExtendedModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        return;
    };
};

class GeometryTransModifierTest : public RSGeometryTransModifier {
public:
    Drawing::Matrix GeometryEffect(float width, float height) const override
    {
        Drawing::Matrix matrix;
        return matrix;
    }
};

class TransitionModifierTest : public RSTransitionModifier {
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

class BackgroundStyleModifierTest : public RSBackgroundStyleModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        return;
    }
};
class ContentStyleModifierTest : public RSContentStyleModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        return;
    }
};
class ForegroundStyleModifierTest : public RSForegroundStyleModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        return;
    }
};
class OverlayStyleModifierTest : public RSOverlayStyleModifier {
public:
    void Draw(RSDrawingContext& context) const override
    {
        return;
    }
};
class NodeModifierTest : public RSNodeModifier {
public:
    void Modify(RSNode& target) const override
    {
        return;
    }
};
/**
 * @tc.name: CreateDrawingContextTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSExtendedModifierTest, CreateDrawingContextTest, TestSize.Level1)
{
    NodeId nodeId = -1;
    RSExtendedModifierHelper::CreateDrawingContext(nodeId);
    auto node = RSNodeMap::Instance().GetNode<RSCanvasNode>(nodeId);
    ASSERT_EQ(node, nullptr);
}

/**
 * @tc.name: CreateRenderModifierTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSExtendedModifierTest, CreateRenderModifierTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto ctx = RSExtendedModifierHelper::CreateDrawingContext(nodeId);
    PropertyId id = 1;
    RSModifierType type = RSModifierType::EXTENDED;
    RSExtendedModifierHelper::CreateRenderModifier(ctx, id, type);
}

/**
 * @tc.name: FinishDrawingTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSExtendedModifierTest, FinishDrawingTest, TestSize.Level1)
{
    NodeId nodeId = 0;
    auto ctx = RSExtendedModifierHelper::CreateDrawingContext(nodeId);
    RSExtendedModifierHelper::FinishDrawing(ctx);
    ASSERT_EQ(ctx.canvas, nullptr);
}

/**
 * @tc.name: FinishDrawingTest001
 * @tc.desc: long text FinishDrawingTest
 * @tc.type: FUNC
 * @tc.require: issueI9P3X4
 */
HWTEST_F(RSExtendedModifierTest, FinishDrawingTest1, TestSize.Level1)
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
    ASSERT_EQ(cmdList, nullptr);
}

/**
 * @tc.name: GetModifierTypeTest001
 * @tc.desc: GetModifierType Test
 * @tc.type: FUNC
 * @tc.require: issueI9N22N
 */
HWTEST_F(RSExtendedModifierTest, GetModifierTypeTest001, TestSize.Level1)
{
    ExtendedModifierTest extendedModifier;
    GeometryTransModifierTest geometryTransModifier;
    TransitionModifierTest transitionModifier;
    BackgroundStyleModifierTest backgroundStyleModifier;
    ContentStyleModifierTest contentStyleModifier;
    ForegroundStyleModifierTest foregroundStyleModifier;
    OverlayStyleModifierTest overlayStyleModifier;
    NodeModifierTest nodeModifier;

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
HWTEST_F(RSExtendedModifierTest, DrawTest001, TestSize.Level1)
{
    GeometryTransModifierTest geometryTransModifier;
    Drawing::Canvas canvas;
    RSDrawingContext rsDrawingContext = { &canvas, 1, 1 };
    geometryTransModifier.Draw(rsDrawingContext);
}
} // namespace OHOS::Rosen