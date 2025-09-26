/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <ostream>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-ext.h"
#include "gtest/hwext/gtest-tag.h"
#include "recording/draw_cmd_list.h"
#include "rs_profiler.h"
#include "rs_profiler_json.h"
#include "rs_profiler_log.h"

#include "animation/rs_animation_manager.h"
#include "common/rs_color.h"
#include "common/rs_vector4.h"
#include "effect/rs_render_filter_base.h"
#include "modifier/rs_property.h"
#include "modifier_ng/appearance/rs_hdr_brightness_render_modifier.h"
#include "modifier_ng/foreground/rs_env_foreground_color_render_modifier.h"
#include "modifier_ng/geometry/rs_frame_clip_render_modifier.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "property/rs_properties.h"
#include "property/rs_properties_def.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

HWTEST(RSProfilerDumpTest, DumpOffScreen, TestSize.Level1)
{
    const NodeId nodeId = 42;
    RSRenderNode node(nodeId);
    JsonWriter out;
    RSContext context;
    RSProfiler::DumpOffscreen(context, out, false, 0);
    std::string expected {
        "{\"type\":\"OFF_SCREEN\",\"children\":[{\"type\":\"RS_NODE\",\"id\":0,\"instanceRootNodeId\":0,"
        "\"firstLevelNodeId\":0,\"subclass\":{},\"Properties\":{\"Bounds\":[null,null,null,null],"
        "\"Frame\":[null,null,null,null]},\"IsPureContainer\":1,\"DrawCmdModifiers\":[],\"children\":[]}]}"
    };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpEmptyNode, TestSize.Level1)
{
    const NodeId nodeId = 42;
    RSRenderNode node(nodeId);
    JsonWriter out;
    RSProfiler::DumpNode(node, out, false, false, false);
    std::string expected {
        "{\"type\":\"RS_NODE\",\"id\":42,\"instanceRootNodeId\":0,\"firstLevelNodeId\":0,"
        "\"subclass\":{},\"Properties\":{\"Bounds\":[null,null,null,null],\"Frame\":[null,null,null,null]},"
        "\"IsPureContainer\":1,\"DrawCmdModifiers\":[],\"children\":[]}"
    };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpEmptyNodeAsRoot, TestSize.Level1)
{
    JsonWriter out;

    const NodeId nodeId = 42;
    RSRenderNode node(nodeId);
    const bool clearMockFlag = false;
    const bool absRoot = true;
    const bool isSorted = false;
    RSProfiler::DumpNode(node, out, clearMockFlag, absRoot, isSorted);
    std::string expected {
        "{\"type\":\"RS_NODE\",\"id\":42,\"instanceRootNodeId\":0,\"firstLevelNodeId\":0,"
        "\"subclass\":{},\"Properties\":{\"Bounds\":[null,null,null,null],\"Frame\":[null,null,null,null]"
        ",\"ScaleX\":1,\"ScaleY\":1},"
        "\"IsPureContainer\":1,\"DrawCmdModifiers\":[],\"children\":[]}"
    };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodeWithChildren, TestSize.Level1)
{
    JsonWriter out;

    const NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    EXPECT_TRUE(node->GetIsFullChildrenListValid());
    auto child1 = std::make_shared<RSRenderNode>(1);
    auto child2 = std::make_shared<RSRenderNode>(2);
    node->AddChild(child1);
    EXPECT_FALSE(node->GetIsFullChildrenListValid());
    node->AddChild(child2);
    EXPECT_FALSE(node->GetIsFullChildrenListValid());
    // Child Node disappeared because no strong links after call
    node->AddChild(std::make_shared<RSRenderNode>(3));
    EXPECT_EQ(node->GetChildrenCount(), 3); // But weak link will be saved

    const bool clearMockFlag = false;
    const bool absRoot = true;
    const bool isSorted = false;
    EXPECT_FALSE(node->GetIsFullChildrenListValid());
    RSProfiler::DumpNode(*node, out, clearMockFlag, absRoot, isSorted);
    std::string expected {
        "{\"type\":\"RS_NODE\",\"id\":42,\"instanceRootNodeId\":0,\"firstLevelNodeId\":0,\"subclass\":{},"
        "\"Properties\":{\"Bounds\":[null,null,null,null],\"Frame\":[null,null,null,null],\"ScaleX\":1,\"ScaleY\":1},"
        "\"isNodeDirty\":1,\"IsPureContainer\":1,\"DrawCmdModifiers\":[],\"children update\":{\"current "
        "count\":0,\"expected "
        "count\":\"0\"},\"children\":[{\"type\":\"RS_NODE\",\"id\":1,\"instanceRootNodeId\":0,\"firstLevelNodeId\":0,"
        "\"subclass\":{},\"Properties\":{\"Bounds\":[null,null,null,null],\"Frame\":[null,null,null,null]},"
        "\"IsPureContainer\":1,\"DrawCmdModifiers\":[],\"children\":[]},{\"type\":\"RS_NODE\",\"id\":2,"
        "\"instanceRootNodeId\":0,\"firstLevelNodeId\":0,\"subclass\":{},\"Properties\":{\"Bounds\":[null,null,null,"
        "null],\"Frame\":[null,null,null,null]},\"IsPureContainer\":1,\"DrawCmdModifiers\":[],\"children\":[]}]}"
    };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodeWithChildrenSorted, Level1)
{
    JsonWriter out;

    const NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    auto child1 = std::make_shared<RSRenderNode>(1);
    child1->GetMutableRenderProperties().SetPositionZ(0.2);
    auto child2 = std::make_shared<RSRenderNode>(2);
    child2->GetMutableRenderProperties().SetPositionZ(0.1);
    node->AddChild(child1);
    node->AddChild(child2);
    node->GenerateFullChildrenList();

    const bool clearMockFlag = false;
    const bool absRoot = true;
    const bool isSorted = true;

    RSProfiler::DumpNode(*node, out, clearMockFlag, absRoot, isSorted);
    std::string expected {
        "{\"type\":\"RS_NODE\",\"id\":42,\"instanceRootNodeId\":0,\"firstLevelNodeId\":0,\"subclass\":{},"
        "\"Properties\":{\"Bounds\":[null,null,null,null],\"Frame\":[null,null,null,null],\"ScaleX\":1,\"ScaleY\":1},"
        "\"isNodeDirty\":1,\"IsPureContainer\":1,\"DrawCmdModifiers\":[],\"children\":[{\"type\":\"RS_NODE\",\"id\":2,"
        "\"instanceRootNodeId\":0,\"firstLevelNodeId\":0,\"subclass\":{},\"Properties\":{"
        "\"Bounds\":[null,null,null,null],\"Frame\":[null,null,null,null],\"PositionZ\":0.1},"
        "\"isPropertyDirty\":1,\"IsPureContainer\":1,"
        "\"DrawCmdModifiers\":[],\"children\":[]},{\"type\":\"RS_NODE\",\"id\":1,\"instanceRootNodeId\":0,"
        "\"firstLevelNodeId\":0,\"subclass\":{},\"Properties\":{\"Bounds\":[null,null,null,null],"
        "\"Frame\":[null,null,null,null],\"PositionZ\":0.2},\"isPropertyDirty\":1,\"IsPureContainer\":1,"
        "\"DrawCmdModifiers\":[],\"children\":[]}]}"
    };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, AdjustNodeId, TestSize.Level1)
{
    const NodeId simple = 420;
    const NodeId mask = (NodeId(1) << (30 + 32));
    const NodeId masked = simple | mask;
    const bool clearMockFlag = true;

    EXPECT_EQ(RSProfiler::AdjustNodeId(simple, false), simple);
    EXPECT_EQ(RSProfiler::AdjustNodeId(masked, false), masked);
    EXPECT_EQ(RSProfiler::AdjustNodeId(simple, clearMockFlag), simple);
    EXPECT_EQ(RSProfiler::AdjustNodeId(masked, clearMockFlag), simple);
}

HWTEST(RSProfilerDumpTest, DumpNodeAbsoluteProperties, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto upperNode = std::make_shared<RSRenderNode>(nodeId++);
    upperNode->GetMutableRenderProperties().SetScale({ 3.14, 2.7 });                // Unused in node
    upperNode->GetMutableRenderProperties().SetBounds({ 5000, 15, 10100, 20200 });  // Unused in node
    upperNode->GetMutableRenderProperties().SetFrame({ 33000, 44000, 5500, 6600 }); // Unused in node

    auto screenNode = std::make_shared<RSScreenRenderNode>(1, 1);
    screenNode->GetMutableRenderProperties().SetScale({ 2, 3 });
    screenNode->GetMutableRenderProperties().SetBounds({ 1, 2, 101, 202 });          // used only x, y
    screenNode->GetMutableRenderProperties().SetFrame({ 33000, 44000, 5500, 6600 }); // Unused in node
    upperNode->AddChild(screenNode);

    auto parent = std::make_shared<RSRenderNode>(nodeId++);
    parent->GetMutableRenderProperties().SetScale({ 5, 7 });
    parent->GetMutableRenderProperties().SetBounds({ 10, 20, 101, 202 });
    parent->GetMutableRenderProperties().SetFrame({ 33, 44, 55, 66 }); // Unused in node
    screenNode->AddChild(parent);
    auto node = std::make_shared<RSRenderNode>(nodeId++);
    node->GetMutableRenderProperties().SetScale({ 11, 13 });
    node->GetMutableRenderProperties().SetBounds({ 100, 200, 123, 234 });
    node->GetMutableRenderProperties().SetFrame({ 30, 40, 50, 60 });
    parent->AddChild(node);

    auto saved_parent = node->GetParent().lock();
    EXPECT_EQ(saved_parent, parent);

    RSProfiler::DumpNodeAbsoluteProperties(*node, out);

    std::string expected { "\"Properties\":{\"Bounds\":[111,222,123,234],\"Frame\":[30,40,50,60],"
                           "\"ScaleX\":110,\"ScaleY\":273}" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);

    JsonWriter out2;
    auto invisible = std::make_shared<RSRenderNode>(nodeId++);
    invisible->GetMutableRenderProperties().SetScale({ 0.1, 0.1 });
    invisible->GetMutableRenderProperties().SetBounds({ 0, 0, 345, 456 });
    invisible->GetMutableRenderProperties().SetFrame({ 31, 41, 51, 61 });
    invisible->GetMutableRenderProperties().SetVisible(false);
    node->AddChild(invisible);

    RSProfiler::DumpNodeAbsoluteProperties(*invisible, out2);

    std::string expected2 { "\"Properties\":{\"Bounds\":[111,222,345,456],\"Frame\":[31,41,51,61],"
                            "\"IsVisible\":0,\"ScaleX\":11,\"ScaleY\":27.3}" };
    expected2.append(1, '\0');
    EXPECT_EQ(out2.GetDumpString(), expected2);
}

HWTEST(RSProfilerDumpTest, DumpNodeBaseInfo, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 142;
    auto rootNode = std::make_shared<RSRenderNode>(nodeId++);
    auto node = std::make_shared<RSRenderNode>(nodeId++);
    rootNode->AddChild(node);

    auto inNode = std::make_shared<RSRenderNode>(nodeId++);
    auto outNode = std::make_shared<RSRenderNode>(nodeId++);

    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    node->SetSharedTransitionParam(sharedTransitionParam);

    node->MarkNodeGroup(RSRenderNode::GROUPED_BY_ANIM, true, false);
    node->UpdateTreeUifirstRootNodeId(rootNode->GetId());

    RSProfiler::DumpNodeBaseInfo(*node, out, false);
    std::string expected { "\"type\":\"RS_NODE\",\"id\":143,\"instanceRootNodeId\":0,\"firstLevelNodeId\":0,"
                           "\"SharedTransitionParam\":\"144 -> 145\",\"nodeGroup\":1,\"nodeGroupReuseCache\":0,"
                           "\"uifirstRootNodeId\":142,\"subclass\":{}" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodeSubsurfaces, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 142;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId++);
    auto subsurface1 = std::make_shared<RSSurfaceRenderNode>(nodeId++);
    auto subsurface2 = std::make_shared<RSSurfaceRenderNode>(nodeId++);
    node->UpdateChildSubSurfaceNodes(subsurface1, true);
    node->UpdateChildSubSurfaceNodes(subsurface2, true);

    RSProfiler::DumpNodeSubsurfaces(*node, out);
    std::string expected { "\"subsurface\":[143,144]" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodeSubsurfacesSurface, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto node = std::make_shared<RSSurfaceRenderNode>(nodeId++);

    RSProfiler::DumpNodeSubClassNode(*node, out);
    std::string expected { "\"subclass\":{\"Parent\":0,\"Name\":\"SurfaceNode\",\"hasConsumer\":0,"
                           "\"Alpha\":\"1.000000 (include ContextAlpha: 1.000000)\",\"Visible\":\"1 Region [Empty]\","
                           "\"Opaque\":\"Region [Empty]\",\"OcclusionBg\":\"0\",\"SpecialLayer\":\"0\"}" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodeSubsurfacesRoot, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRootRenderNode>(nodeId++);

    RSProfiler::DumpNodeSubClassNode(*node, out);
    std::string expected { "\"subclass\":{\"Visible\":1,\"Size\":[null,null],\"EnableRender\":1}" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodeSubsurfacesLogDisp, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(nodeId++, RSDisplayNodeConfig {});

    RSProfiler::DumpNodeSubClassNode(*node, out);
    std::string expected { "\"subclass\":{\"skipLayer\":0}" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodeOptionalFlags, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);
    node->SetBootAnimation(true);
    node->SetContainBootAnimation(true);
    node->SetSubTreeDirty(true);

    RSProfiler::DumpNodeOptionalFlags(*node, out);
    std::string expected {
        "\"GetBootAnimation\":1,\"isContainBootAnimation_\":1,\"isSubTreeDirty\":1,\"IsPureContainer\":1"
    };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}
HWTEST(RSProfilerDumpTest, DumpNodeDrawCmdModifiers, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);
    auto modifier = std::make_shared<ModifierNG::RSFrameClipRenderModifier>();

    node->AddModifier(modifier);
    RSProfiler::DumpNodeDrawCmdModifiers(*node, out);
    std::string expected { "\"DrawCmdModifiers\":[{\"type\":12,\"modifiers\":[]}]" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodeDrawCmdModifiersWithCustomDrawCmdList, TestSize.Level1)
{
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);
    const auto property = std::make_shared<RSRenderProperty<int>>(1, 0);
    auto modifier =
        ModifierNG::RSRenderModifier::MakeRenderModifier(ModifierNG::RSModifierType::TRANSITION_STYLE, property);
    Drawing::DrawCmdListPtr cmdList = Drawing::DrawCmdList::CreateFromData(Drawing::CmdListData { "1234", 4 }, true);

    auto opItem1 = std::make_shared<Drawing::HybridRenderPixelMapSizeOpItem>(100, 200);
    auto opItem2 = std::make_shared<Drawing::RotateOpItem>(90, 10, 20);
    cmdList->drawOpItems_.emplace_back(opItem1);
    cmdList->drawOpItems_.emplace_back(opItem2);

    std::shared_ptr<RSRenderProperty<Drawing::DrawCmdListPtr>> property2 =
        std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>(cmdList, 0);
    node->AddModifier(modifier);

    JsonWriter out1;
    RSProfiler::DumpNodeDrawCmdModifiers(*node, out1);
    std::string expected1 { "\"DrawCmdModifiers\":[{\"type\":25,\"modifiers\":[{\"drawCmdList\":[]}]}]" };
    expected1.append(1, '\0');
    EXPECT_EQ(out1.GetDumpString(), expected1);

    modifier->AttachProperty(ModifierNG::RSPropertyType::TRANSITION_STYLE, property2);

    JsonWriter out2;
    RSProfiler::DumpNodeDrawCmdModifiers(*node, out2);
    std::string expected2 { "\"DrawCmdModifiers\":[{\"type\":25,\"modifiers\":[{\"drawCmdList\":"
                            "[\"HYBRID_RENDER_PIXELMAP_SIZE_OPITEM\",\"ROTATE_OPITEM\"]}]}]" };
    expected2.append(1, '\0');
    EXPECT_EQ(out2.GetDumpString(), expected2);
}

HWTEST(RSProfilerDumpTest, DumpNodeDrawCmdModifiersWithPropertyForegroundColor, TestSize.Level1)
{
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);
    auto modifier = std::make_shared<ModifierNG::RSEnvForegroundColorRenderModifier>();
    node->AddModifier(modifier);

    {
        JsonWriter out1;
        RSProfiler::DumpNodeDrawCmdModifiers(*node, out1);
        std::string expected1 { "\"DrawCmdModifiers\":[{\"type\":31,\"modifiers\":[]}]" };
        expected1.append(1, '\0');
        EXPECT_EQ(out1.GetDumpString(), expected1);
    }

    std::shared_ptr<RSRenderPropertyBase> property1 =
        std::make_shared<RSRenderProperty<RSColor>>(RSColor(1, 2, 3, 0xa), 0);
    modifier->AttachProperty(ModifierNG::RSPropertyType::ENV_FOREGROUND_COLOR, property1);

    {
        JsonWriter out2;
        RSProfiler::DumpNodeDrawCmdModifiers(*node, out2);
        std::string expected2 {
            "\"DrawCmdModifiers\":[{\"type\":31,\"modifiers\":[{\"ENV_FOREGROUND_COLOR\":\"#102030a (RGBA)\"}]}]"
        };
        expected2.append(1, '\0');
        EXPECT_EQ(out2.GetDumpString(), expected2);
    }

    std::shared_ptr<RSRenderPropertyBase> property2 = std::make_shared<RSRenderProperty<int>>(42, 0);
    modifier->AttachProperty(ModifierNG::RSPropertyType::ENV_FOREGROUND_COLOR_STRATEGY, property2);

    {
        JsonWriter out3;
        RSProfiler::DumpNodeDrawCmdModifiers(*node, out3);
        std::string expected3 {
            "\"DrawCmdModifiers\":[{\"type\":31,\"modifiers\":[{\"ENV_FOREGROUND_COLOR\":\"#102030a (RGBA)\"},"
            "{\"ENV_FOREGROUND_COLOR_STRATEGY\":42}]}]"
        };
        expected3.append(1, '\0');
        EXPECT_EQ(out3.GetDumpString(), expected3);
    }
}

HWTEST(RSProfilerDumpTest, DumpNodeDrawCmdModifiersWithPropertyCustomClipFrame, TestSize.Level1)
{
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);
    auto modifier = std::make_shared<ModifierNG::RSFrameClipRenderModifier>();
    std::shared_ptr<RSRenderPropertyBase> property = std::make_shared<RSRenderProperty<int>>(1, 101);
    node->AddModifier(modifier);

    JsonWriter out1;
    RSProfiler::DumpNodeDrawCmdModifiers(*node, out1);
    std::string expected1 { "\"DrawCmdModifiers\":[{\"type\":12,\"modifiers\":[]}]" };
    expected1.append(1, '\0');
    EXPECT_EQ(out1.GetDumpString(), expected1);

    modifier->AttachProperty(ModifierNG::RSPropertyType::CUSTOM_CLIP_TO_FRAME, property);

    JsonWriter out2;
    RSProfiler::DumpNodeDrawCmdModifiers(*node, out2);
    std::string expected2 { "\"DrawCmdModifiers\":[{\"type\":12,\"modifiers\":[{\"CUSTOM_CLIP_TO_FRAME\":\"[1]\"}]}]" };
    expected2.append(1, '\0');
    EXPECT_EQ(out2.GetDumpString(), expected2);
}

HWTEST(RSProfilerDumpTest, DumpNodeDrawCmdModifiersWithPropertyHdrBrightness, TestSize.Level1)
{
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);
    auto modifier = std::make_shared<ModifierNG::RSHDRBrightnessRenderModifier>();
    std::shared_ptr<RSRenderPropertyBase> property = std::make_shared<RSRenderProperty<float>>(3.14, 0);
    node->AddModifier(modifier);

    JsonWriter out1;
    RSProfiler::DumpNodeDrawCmdModifiers(*node, out1);
    std::string expected1 { "\"DrawCmdModifiers\":[{\"type\":32,\"modifiers\":[]}]" };
    expected1.append(1, '\0');
    EXPECT_EQ(out1.GetDumpString(), expected1);

    modifier->AttachProperty(ModifierNG::RSPropertyType::HDR_BRIGHTNESS, property);

    JsonWriter out2;
    RSProfiler::DumpNodeDrawCmdModifiers(*node, out2);
    std::string expected2 { "\"DrawCmdModifiers\":[{\"type\":32,\"modifiers\":[{\"HDR_BRIGHTNESS\":3.14}]}]" };
    expected2.append(1, '\0');
    EXPECT_EQ(out2.GetDumpString(), expected2);
}

HWTEST(RSProfilerDumpTest, DumpNodeProperties, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto invisible = std::make_shared<RSRenderNode>(nodeId++);
    invisible->GetMutableRenderProperties().SetScale({ 0.125, 0.5 });
    invisible->GetMutableRenderProperties().SetBounds({ 0, 0, 345, 456 });
    invisible->GetMutableRenderProperties().SetFrame({ 31, 41, 51, 61 });
    invisible->GetMutableRenderProperties().SetVisible(false);

    RSProfiler::DumpNodeProperties(invisible->GetRenderProperties(), out);
    std::string expected { "\"Properties\":{\"Bounds\":[0,0,345,456],\"Frame\":[31,41,51,61],"
                           "\"IsVisible\":0,\"ScaleX\":0.125,\"ScaleY\":0.5}" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodePropertiesClip, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);
    node->GetMutableRenderProperties().SetClipToBounds(true);
    node->GetMutableRenderProperties().SetClipToFrame(true);

    RSProfiler::DumpNodePropertiesClip(node->GetRenderProperties(), out);
    std::string expected { "\"ClipToBounds\":1,\"ClipToFrame\":1" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodePropertiesTransform, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);
    node->GetMutableRenderProperties().SetPivot({ 2.5, 100 });
    node->GetMutableRenderProperties().SetRotation(30);
    node->GetMutableRenderProperties().SetRotationX(45);
    node->GetMutableRenderProperties().SetRotationY(60);
    node->GetMutableRenderProperties().SetTranslate({ 10, 20 });
    node->GetMutableRenderProperties().SetTranslateZ(30);
    node->GetMutableRenderProperties().SetScale({ 3, 5 });

    RSProfiler::DumpNodePropertiesTransform(node->GetRenderProperties(), out);
    std::string expected { "\"Pivot\":[2.5,100],"
                           "\"Rotation\":30,"
                           "\"RotationX\":45,"
                           "\"RotationY\":60,"
                           "\"TranslateX\":10,"
                           "\"TranslateY\":20,"
                           "\"TranslateZ\":30,"
                           "\"ScaleX\":3,"
                           "\"ScaleY\":5" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodePropertiesDecoration, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);

    node->GetMutableRenderProperties().SetCornerRadius({ 1, 2, 3, 4 });
    node->GetMutableRenderProperties().SetPixelStretch(Vector4f { 5, 6, 7, 8 });
    node->GetMutableRenderProperties().SetAlpha(0.5);
    node->GetMutableRenderProperties().SetSpherize(0.25);
    node->GetMutableRenderProperties().SetForegroundColor(Color(2, 3, 4, 5));
    node->GetMutableRenderProperties().SetBackgroundColor(Color(0x20, 0x30, 0x40, 0x50));
    node->GetMutableRenderProperties().SetBgImageDstRect({ 10, 20, 30, 40 });

    RSProfiler::DumpNodePropertiesDecoration(node->GetRenderProperties(), out);

    std::string expected { "\"CornerRadius\":[1,2,3,4],"
                           "\"PixelStretch\":{\"left\":5,\"top\":6,\"right\":7,\"bottom\":8},"
                           "\"Alpha\":0.5,"
                           "\"Spherize\":0.25,"
                           "\"ForegroundColor\":\"#5020304 (ARGB)\","
                           "\"BackgroundColor\":\"#50203040 (ARGB)\","
                           "\"BgImage\":[10,20,30,40]" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodePropertiesShadow, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);

    node->GetMutableRenderProperties().SetShadowColor(Color(2, 3, 4, 5));
    node->GetMutableRenderProperties().SetShadowOffsetX(12);
    node->GetMutableRenderProperties().SetShadowOffsetY(14);
    node->GetMutableRenderProperties().SetShadowElevation(16);
    node->GetMutableRenderProperties().SetShadowRadius(18);
    node->GetMutableRenderProperties().SetShadowIsFilled(true);

    RSProfiler::DumpNodePropertiesShadow(node->GetRenderProperties(), out);

    std::string expected { "\"ShadowColor\":\"#ff020304 (ARGB)\","
                           "\"ShadowOffsetX\":12,"
                           "\"ShadowOffsetY\":14,"
                           "\"ShadowElevation\":16,"
                           "\"ShadowRadius\":18,"
                           "\"ShadowIsFilled\":1" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodePropertiesEffectsBorder, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);

    node->GetMutableRenderProperties().SetBorderColor(
        { { 1, 2, 3, 4 }, { 5, 6, 7, 8 }, { 9, 10, 11, 12 }, { 13, 14, 15, 16 } }); // +
    node->GetMutableRenderProperties().SetBorderDashGap({ 17, 18, 19, 20 });        // -
    node->GetMutableRenderProperties().SetBorderDashWidth({ 21, 22, 23, 24 });      // -
    node->GetMutableRenderProperties().SetBorderStyle({ 0, 1, 2, 3 });              // +
    node->GetMutableRenderProperties().SetBorderWidth({ 25, 26, 27, 28 });          // +

    RSProfiler::DumpNodePropertiesEffects(node->GetRenderProperties(), out);

    std::string expected {
        "\"Border\":\"colors: 67174915, 134546951, 201918987, 269291023, widths: 25, 26, 27, 28, styles: 0, 1, 2, 3, \""
    };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodePropertiesEffectsFilter, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);

    RSProperties& prop = node->GetMutableRenderProperties();
    prop.SetForegroundBlurRadiusX(5);
    prop.GenerateForegroundBlurFilter();

    RSProfiler::DumpNodePropertiesEffects(node->GetRenderProperties(), out);

    std::string expected { "\"Filter\":\"RSBlurFilterBlur, radius: 5 sigma\"" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodePropertiesEffectsOtherFilter, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);
    node->GetMutableRenderProperties().SetBackgroundBlurRadiusX(1);
    node->GetMutableRenderProperties().SetBackgroundBlurRadiusY(2);
    node->GetMutableRenderProperties().GenerateBackgroundBlurFilter();

    auto foregroundFilterCache = RSFilter::CreateBlurFilter(3, 4);
    node->GetMutableRenderProperties().SetForegroundFilterCache(foregroundFilterCache);

    RSProfiler::DumpNodePropertiesEffects(node->GetRenderProperties(), out);

    std::string expected { "\"BackgroundFilter\":\"RSBlurFilterBlur, radius: 1 "
                           "sigma\",\"ForegroundFilter\":\"RSBlurFilter blur radius is 3.000000 sigma\"" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodePropertiesEffectsOutline, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);
    node->GetMutableRenderProperties().SetOutlineColor(
        { { 0, 0, 1, 1 }, { 0, 0, 2, 1 }, { 0, 0, 3, 1 }, { 0, 0, 4, 1 } });   // +
    node->GetMutableRenderProperties().SetOutlineDashGap({ 5, 6, 7, 8 });      // -
    node->GetMutableRenderProperties().SetOutlineDashWidth({ 9, 10, 11, 12 }); // -
    node->GetMutableRenderProperties().SetOutlineRadius({ 17, 18, 19, 20 });   // -
    node->GetMutableRenderProperties().SetOutlineStyle({ 0, 1, 2, 3 });        // +
    node->GetMutableRenderProperties().SetOutlineWidth({ 13, 14, 15, 16 });    // +

    RSProfiler::DumpNodePropertiesEffects(node->GetRenderProperties(), out);

    std::string expected {
        "\"Outline\":\"colors: 16777217, 16777218, 16777219, 16777220, widths: 13, 14, 15, 16, styles: 0, 1, 2, 3, \""
    };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodePropertiesEffectsGravityEtc, TestSize.Level1)
{
    JsonWriter out;
    NodeId nodeId = 42;
    auto node = std::make_shared<RSRenderNode>(nodeId++);
    node->GetMutableRenderProperties().SetFrameGravity(Gravity::RESIZE_ASPECT_BOTTOM_RIGHT);
    node->GetMutableRenderProperties().SetUseEffect(true);
    node->GetMutableRenderProperties().SetGrayScale(0.5);
    node->GetMutableRenderProperties().SetLightUpEffect(30);
    node->GetMutableRenderProperties().SetDynamicLightUpRate(0.25f);
    node->GetMutableRenderProperties().SetDynamicLightUpDegree(45.f);

    RSProfiler::DumpNodePropertiesEffects(node->GetRenderProperties(), out);

    std::string expected { "\"FrameGravity\":12,"
                           "\"GetUseEffect\":1,"
                           "\"GrayScale\":0.5,"
                           "\"LightUpEffect\":30,"
                           "\"DynamicLightUpRate\":0.25,"
                           "\"DynamicLightUpDegree\":45" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodePropertiesColorDefault, TestSize.Level1)
{
    JsonWriter out;
    RSProperties properties;
    std::string expected {};

    RSProfiler::DumpNodePropertiesColor(properties, out);
    EXPECT_EQ(out.GetDumpString(), expected);

    properties.SetBrightness(1.f);
    properties.SetContrast(1.f);
    properties.SetSaturate(1.f);
    properties.SetSepia(0.f);
    properties.SetInvert(0.f);
    properties.SetHueRotate(0.f);
    properties.SetColorBlend(RSColor { 0, 0, 0, 0 });
    properties.SetColorBlendMode(0);

    RSProfiler::DumpNodePropertiesColor(properties, out);
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodePropertiesColorValues, TestSize.Level1)
{
    JsonWriter out;
    RSProperties properties;

    properties.SetBrightness(2.f);
    properties.SetContrast(3.f);
    properties.SetSaturate(4.f);
    properties.SetSepia(0.5f);
    properties.SetInvert(0.25f);
    properties.SetHueRotate(0.125f);
    properties.SetColorBlend(RSColor { 10, 20, 30, 40 });
    properties.SetColorBlendMode(static_cast<int>(RSColorBlendMode::DARKEN));
    properties.SetColorBlendApplyType(static_cast<int>(RSColorBlendApplyType::SAVE_LAYER_ALPHA));

    RSProfiler::DumpNodePropertiesColor(properties, out);

    std::string expected { "\"Brightness\":2,"
                           "\"Contrast\":3,"
                           "\"Saturate\":4,"
                           "\"Sepia\":0.5,"
                           "\"Invert\":0.25,"
                           "\"HueRotate\":0.125,"
                           "\"ColorBlend\":\"#280a141e (ARGB)\","
                           "\"skblendmode\":16," // RSColorBlendMode::DARKEN - 1;
                           "\"blendType\":2" };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodeAnimations, TestSize.Level1)
{
    JsonWriter out;
    RSAnimationManager anims;
    AnimationId id = 123;
    PropertyId propertyId = 1;
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.5f);
    auto anim1 = std::make_shared<RSRenderKeyframeAnimation>(id, propertyId, property);

    anims.AddAnimation(anim1);

    RSProfiler::DumpNodeAnimations(anims, out);

    std::string expected {
        "\"RSAnimationManager\":[{\"id\":123,\"type\":\"Type:RSRenderKeyframeAnimation, "
        "ModifierType: INVALID\",\"AnimationState\":0,\"Duration\":300,\"StartDelay\":0,\"Speed\":1,"
        "\"RepeatCount\":1,\"AutoReverse\":0,\"Direction\":1,\"FillMode\":1,\"RepeatCallbackEnable\":0,"
        "\"FrameRateRange_min\":0,\"FrameRateRange_max\":0,\"FrameRateRange_prefered\":0}]"
    };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}

HWTEST(RSProfilerDumpTest, DumpNodeChildrenListUpdate, TestSize.Level1)
{
    JsonWriter out;
    const NodeId nodeId = 42;

    auto node = std::make_shared<RSRenderNode>(nodeId);
    EXPECT_TRUE(node->GetIsFullChildrenListValid());
    auto child1 = std::make_shared<RSRenderNode>(1);
    auto child2 = std::make_shared<RSRenderNode>(2);
    node->AddChild(child1);
    node->AddChild(child2);
    node->disappearingChildren_.emplace_back(child1, 1);
    node->disappearingChildren_.emplace_back(child2, 2);

    RSProfiler::DumpNodeChildrenListUpdate(*node, out);

    std::string expected {
        "\"children update\":{\"current count\":0,\"disappearing count\":2,\"expected count\":\"0 + 2\"}"
    };
    expected.append(1, '\0');
    EXPECT_EQ(out.GetDumpString(), expected);
}
} // namespace OHOS::Rosen
