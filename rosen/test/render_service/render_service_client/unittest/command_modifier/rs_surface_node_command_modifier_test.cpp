/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "gtest/hwext/gtest-tag.h"

#include "command_modifier/rs_surface_node_command_modifier.h"
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSSurfaceNodeCommandModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceNodeCommandModifierTest::SetUpTestCase() {}
void RSSurfaceNodeCommandModifierTest::TearDownTestCase() {}
void RSSurfaceNodeCommandModifierTest::SetUp() {}
void RSSurfaceNodeCommandModifierTest::TearDown() {}

static std::weak_ptr<RSNode> MakeExpiredNode()
{
    std::weak_ptr<RSNode> weak;
    {
        auto n = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
        weak = n;
    }
    return weak;
}

/**
 * @tc.name: LeashPersistentIdTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, LeashPersistentIdTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    LeashPersistentIdCmdParam param{100};
    auto mod = std::make_shared<LeashPersistentIdCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::LEASH_PERSISTENT_ID);
    EXPECT_EQ(mod->GetParam().leashPersistentId_, 100);

    LeashPersistentIdCmdParam param2{200};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().leashPersistentId_, 200);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("leashPersistentId"), std::string::npos);
}

/**
 * @tc.name: LeashPersistentIdTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, LeashPersistentIdTest002, TestSize.Level1)
{
    LeashPersistentIdCmdParam param{0};
    auto mod = std::make_shared<LeashPersistentIdCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: SecurityLayerTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SecurityLayerTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SecurityLayerCmdParam param{true};
    auto mod = std::make_shared<SecurityLayerCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::IS_SECURITY_LAYER);
    EXPECT_TRUE(mod->GetParam().isSecurityLayer_);

    SecurityLayerCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isSecurityLayer_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isSecurityLayer"), std::string::npos);
}

/**
 * @tc.name: SecurityLayerTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SecurityLayerTest002, TestSize.Level1)
{
    SecurityLayerCmdParam param{true};
    auto mod = std::make_shared<SecurityLayerCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: SkipLayerTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SkipLayerTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SkipLayerCmdParam param{true};
    auto mod = std::make_shared<SkipLayerCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::IS_SKIP_LAYER);
    EXPECT_TRUE(mod->GetParam().isSkipLayer_);

    SkipLayerCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isSkipLayer_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isSkipLayer"), std::string::npos);
}

/**
 * @tc.name: SkipLayerTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SkipLayerTest002, TestSize.Level1)
{
    SkipLayerCmdParam param{true};
    auto mod = std::make_shared<SkipLayerCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: SnapshotSkipLayerTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SnapshotSkipLayerTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SnapshotSkipLayerCmdParam param{true};
    auto mod = std::make_shared<SnapshotSkipLayerCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::IS_SNAPSHOT_SKIP_LAYER);
    EXPECT_TRUE(mod->GetParam().isSnapshotSkipLayer_);

    SnapshotSkipLayerCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isSnapshotSkipLayer_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isSnapshotSkipLayer"), std::string::npos);
}

/**
 * @tc.name: SnapshotSkipLayerTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SnapshotSkipLayerTest002, TestSize.Level1)
{
    SnapshotSkipLayerCmdParam param{true};
    auto mod = std::make_shared<SnapshotSkipLayerCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: HasFingerprintTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HasFingerprintTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    HasFingerprintCmdParam param{true};
    auto mod = std::make_shared<HasFingerprintCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::HAS_FINGER_PRINT);
    EXPECT_TRUE(mod->GetParam().hasFingerprint_);

    HasFingerprintCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().hasFingerprint_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("hasFingerprint"), std::string::npos);
}

/**
 * @tc.name: HasFingerprintTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HasFingerprintTest002, TestSize.Level1)
{
    HasFingerprintCmdParam param{true};
    auto mod = std::make_shared<HasFingerprintCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ColorSpaceTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ColorSpaceTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ColorSpaceCmdParam param{GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB};
    auto mod = std::make_shared<ColorSpaceCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::COLOR_SPACE);
    EXPECT_EQ(mod->GetParam().colorSpace_, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);

    ColorSpaceCmdParam param2{GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().colorSpace_, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("colorSpace"), std::string::npos);
}

/**
 * @tc.name: ColorSpaceTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ColorSpaceTest002, TestSize.Level1)
{
    ColorSpaceCmdParam param{GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB};
    auto mod = std::make_shared<ColorSpaceCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: AbilityBGAlphaTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AbilityBGAlphaTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    constexpr uint8_t alphaVal = 128;
    AbilityBGAlphaCmdParam param{alphaVal};
    auto mod = std::make_shared<AbilityBGAlphaCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::ABILITY_BG_ALPHA);
    EXPECT_EQ(mod->GetParam().abilityBGAlpha_, alphaVal);

    constexpr uint8_t alphaVal2 = 255;
    AbilityBGAlphaCmdParam param2{alphaVal2};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().abilityBGAlpha_, alphaVal2);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("abilityBGAlpha"), std::string::npos);
}

/**
 * @tc.name: AbilityBGAlphaTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AbilityBGAlphaTest002, TestSize.Level1)
{
    AbilityBGAlphaCmdParam param{0};
    auto mod = std::make_shared<AbilityBGAlphaCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: NotifyUIBufferAvailableTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, NotifyUIBufferAvailableTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    NotifyUIBufferAvailableCmdParam param{true};
    auto mod = std::make_shared<NotifyUIBufferAvailableCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::NOTIFY_UI_BUFFER_AVL);
    EXPECT_TRUE(mod->GetParam().notifyUIBufferAvailable_);

    NotifyUIBufferAvailableCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().notifyUIBufferAvailable_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("notifyUIBufferAvailable"), std::string::npos);
}

/**
 * @tc.name: NotifyUIBufferAvailableTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, NotifyUIBufferAvailableTest002, TestSize.Level1)
{
    NotifyUIBufferAvailableCmdParam param{true};
    auto mod = std::make_shared<NotifyUIBufferAvailableCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ContainerWindowTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ContainerWindowTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    RRect rrect{RectF(0, 0, 100, 100), 10.f, 10.f};
    ContainerWindowCmdParam param{true, rrect};
    auto mod = std::make_shared<ContainerWindowCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::SET_CONTAINER_WINDOW);
    EXPECT_TRUE(mod->GetParam().hasContainerWindow_);

    ContainerWindowCmdParam param2{false, rrect};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().hasContainerWindow_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("hasContainerWindow"), std::string::npos);
}

/**
 * @tc.name: ContainerWindowTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ContainerWindowTest002, TestSize.Level1)
{
    RRect rrect;
    ContainerWindowCmdParam param{true, rrect};
    auto mod = std::make_shared<ContainerWindowCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: FreezeCmdTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, FreezeCmdTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    FreezeCmdParam param{true, false};
    auto mod = std::make_shared<FreezeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::SF_IS_FREEZE);
    EXPECT_TRUE(mod->GetParam().isFreeze_);
    EXPECT_FALSE(mod->GetParam().isMarkedByUI_);

    FreezeCmdParam param2{false, true};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isFreeze_);
    EXPECT_TRUE(mod->GetParam().isMarkedByUI_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isFreeze"), std::string::npos);
}

/**
 * @tc.name: FreezeCmdTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, FreezeCmdTest002, TestSize.Level1)
{
    FreezeCmdParam param{true, true};
    auto mod = std::make_shared<FreezeCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: HardwareEnableHintTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HardwareEnableHintTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    HardwareEnableHintCmdParam param{true};
    auto mod = std::make_shared<HardwareEnableHintCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::HARDWARE_ENABLE_HINT);
    EXPECT_TRUE(mod->GetParam().isHardwareEnabled_);

    HardwareEnableHintCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isHardwareEnabled_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isHardwareEnabled"), std::string::npos);
}

/**
 * @tc.name: HardwareEnableHintTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HardwareEnableHintTest002, TestSize.Level1)
{
    HardwareEnableHintCmdParam param{true};
    auto mod = std::make_shared<HardwareEnableHintCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: HardwareEnabledTest001
 * @tc.desc: Test all methods including UpdateToRenderWithResult
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HardwareEnabledTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    HardwareEnabledCmdParam param{true, SelfDrawingNodeType::DEFAULT, false};
    auto mod = std::make_shared<HardwareEnabledCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::HARDWARE_ENABLED);
    EXPECT_TRUE(mod->GetParam().isHardwareEnabled_);
    EXPECT_EQ(mod->GetParam().isSelfDrawingNodeType_, SelfDrawingNodeType::DEFAULT);
    EXPECT_FALSE(mod->GetParam().isDynamicHardwareEnable_);

    HardwareEnabledCmdParam param2{false, SelfDrawingNodeType::DEFAULT, true};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isHardwareEnabled_);
    EXPECT_EQ(mod->GetParam().isSelfDrawingNodeType_, SelfDrawingNodeType::DEFAULT);
    EXPECT_TRUE(mod->GetParam().isDynamicHardwareEnable_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isHardwareEnabled"), std::string::npos);
}

/**
 * @tc.name: HardwareEnabledTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HardwareEnabledTest002, TestSize.Level1)
{
    HardwareEnabledCmdParam param{true, SelfDrawingNodeType::DEFAULT, false};
    auto mod = std::make_shared<HardwareEnabledCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: HardwareEnabledTest003
 * @tc.desc: Test UpdateToRenderWithResult with null node returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HardwareEnabledTest003, TestSize.Level1)
{
    HardwareEnabledCmdParam param{true, SelfDrawingNodeType::DEFAULT, false};
    auto mod = std::make_shared<HardwareEnabledCmdModifier>(MakeExpiredNode(), param);
    auto result = mod->UpdateToRenderWithResult();
    bool val = std::get<bool>(result);
    EXPECT_FALSE(val);
}

/**
 * @tc.name: BootAnimationTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, BootAnimationTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    BootAnimationCmdParam param{true};
    auto mod = std::make_shared<BootAnimationCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::SF_IS_BOOT_ANIMATION);
    EXPECT_TRUE(mod->GetParam().isBootAnimation_);

    BootAnimationCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isBootAnimation_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isBootAnimation"), std::string::npos);
}

/**
 * @tc.name: BootAnimationTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, BootAnimationTest002, TestSize.Level1)
{
    BootAnimationCmdParam param{true};
    auto mod = std::make_shared<BootAnimationCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: GlobalPositionEnabledTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, GlobalPositionEnabledTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    GlobalPositionEnabledCmdParam param{true};
    auto mod = std::make_shared<GlobalPositionEnabledCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::IS_GLOBAL_POSITION_ENABLED);
    EXPECT_TRUE(mod->GetParam().isGlobalPositionEnabled_);

    GlobalPositionEnabledCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isGlobalPositionEnabled_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isGlobalPositionEnabled"), std::string::npos);
}

/**
 * @tc.name: GlobalPositionEnabledTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, GlobalPositionEnabledTest002, TestSize.Level1)
{
    GlobalPositionEnabledCmdParam param{true};
    auto mod = std::make_shared<GlobalPositionEnabledCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ClonedNodeInfoTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ClonedNodeInfoTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ClonedNodeInfoCmdParam param{100, true, false};
    auto mod = std::make_shared<ClonedNodeInfoCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::CLONED_NODE_INFO);
    EXPECT_EQ(mod->GetParam().clonedNodeId_, 100);
    EXPECT_TRUE(mod->GetParam().clonedNeedOffscreen_);
    EXPECT_FALSE(mod->GetParam().clonedIsRelated_);

    ClonedNodeInfoCmdParam param2{200, false, true};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().clonedNodeId_, 200);
    EXPECT_FALSE(mod->GetParam().clonedNeedOffscreen_);
    EXPECT_TRUE(mod->GetParam().clonedIsRelated_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("clonedNodeId"), std::string::npos);
}

/**
 * @tc.name: ClonedNodeInfoTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ClonedNodeInfoTest002, TestSize.Level1)
{
    ClonedNodeInfoCmdParam param{0, false, false};
    auto mod = std::make_shared<ClonedNodeInfoCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ForceUIFirstTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ForceUIFirstTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ForceUIFirstCmdParam param{true};
    auto mod = std::make_shared<ForceUIFirstCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::FORCE_UI_FIRST);
    EXPECT_TRUE(mod->GetParam().forceUIFirst_);

    ForceUIFirstCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().forceUIFirst_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("forceUIFirst"), std::string::npos);
}

/**
 * @tc.name: ForceUIFirstTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ForceUIFirstTest002, TestSize.Level1)
{
    ForceUIFirstCmdParam param{true};
    auto mod = std::make_shared<ForceUIFirstCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: AncoFlagsTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AncoFlagsTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    constexpr uint32_t flagsVal = 3;
    AncoFlagsCmdParam param{flagsVal};
    auto mod = std::make_shared<AncoFlagsCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::ANCO_FLAGS);
    EXPECT_EQ(mod->GetParam().ancoFlags_, flagsVal);

    constexpr uint32_t flagsVal2 = 7;
    AncoFlagsCmdParam param2{flagsVal2};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().ancoFlags_, flagsVal2);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("ancoFlags"), std::string::npos);
}

/**
 * @tc.name: AncoFlagsTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AncoFlagsTest002, TestSize.Level1)
{
    AncoFlagsCmdParam param{0};
    auto mod = std::make_shared<AncoFlagsCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: SkipDrawTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SkipDrawTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SkipDrawCmdParam param{true};
    auto mod = std::make_shared<SkipDrawCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::IS_SKIP_DRAW);
    EXPECT_TRUE(mod->GetParam().skipDraw_);

    SkipDrawCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().skipDraw_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("skipDraw"), std::string::npos);
}

/**
 * @tc.name: SkipDrawTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SkipDrawTest002, TestSize.Level1)
{
    SkipDrawCmdParam param{true};
    auto mod = std::make_shared<SkipDrawCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: WatermarkEnabledTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, WatermarkEnabledTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    WatermarkEnabledCmdParam param{"testMark", true};
    auto mod = std::make_shared<WatermarkEnabledCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::WATER_MARK_ENABLED);
    EXPECT_EQ(mod->GetParam().watermarkName_, "testMark");
    EXPECT_TRUE(mod->GetParam().watermarkEnabled_);

    WatermarkEnabledCmdParam param2{"otherMark", false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().watermarkName_, "otherMark");
    EXPECT_FALSE(mod->GetParam().watermarkEnabled_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("watermarkName"), std::string::npos);
}

/**
 * @tc.name: WatermarkEnabledTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, WatermarkEnabledTest002, TestSize.Level1)
{
    WatermarkEnabledCmdParam param{"", true};
    auto mod = std::make_shared<WatermarkEnabledCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: AbilityStateTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AbilityStateTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    AbilityStateCmdParam param{RSSurfaceNodeAbilityState::FOREGROUND};
    auto mod = std::make_shared<AbilityStateCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::ABILITY_STATE);
    EXPECT_EQ(mod->GetParam().abilityState_, RSSurfaceNodeAbilityState::FOREGROUND);

    AbilityStateCmdParam param2{RSSurfaceNodeAbilityState::BACKGROUND};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().abilityState_, RSSurfaceNodeAbilityState::BACKGROUND);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("abilityState"), std::string::npos);
}

/**
 * @tc.name: AbilityStateTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AbilityStateTest002, TestSize.Level1)
{
    AbilityStateCmdParam param{RSSurfaceNodeAbilityState::FOREGROUND};
    auto mod = std::make_shared<AbilityStateCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: HidePrivacyContentTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HidePrivacyContentTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    HidePrivacyContentCmdParam param{true};
    auto mod = std::make_shared<HidePrivacyContentCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::SET_HIDE_PRIVACY_CONTENT);
    EXPECT_TRUE(mod->GetParam().hidePrivacyContent_);

    HidePrivacyContentCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().hidePrivacyContent_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("hidePrivacyContent"), std::string::npos);
}

/**
 * @tc.name: HidePrivacyContentTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HidePrivacyContentTest002, TestSize.Level1)
{
    HidePrivacyContentCmdParam param{true};
    auto mod = std::make_shared<HidePrivacyContentCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: HidePrivacyContentTest003
 * @tc.desc: Test UpdateToRenderWithResult with null node returns UNKNOWN_ERROR
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HidePrivacyContentTest003, TestSize.Level1)
{
    HidePrivacyContentCmdParam param{true};
    auto mod = std::make_shared<HidePrivacyContentCmdModifier>(MakeExpiredNode(), param);
    auto result = mod->UpdateToRenderWithResult();
    auto* errCode = std::get_if<RSInterfaceErrorCode>(&result);
    ASSERT_TRUE(errCode);
    EXPECT_EQ(*errCode, RSInterfaceErrorCode::UNKNOWN_ERROR);
}

/**
 * @tc.name: ApiCompatibleVersionTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ApiCompatibleVersionTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    constexpr uint32_t versionVal = 10;
    ApiCompatibleVersionCmdParam param{versionVal};
    auto mod = std::make_shared<ApiCompatibleVersionCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::API_COMPAT_VERSION);
    EXPECT_EQ(mod->GetParam().apiCompatibleVersion_, versionVal);

    constexpr uint32_t versionVal2 = 20;
    ApiCompatibleVersionCmdParam param2{versionVal2};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().apiCompatibleVersion_, versionVal2);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("apiCompatibleVersion"), std::string::npos);
}

/**
 * @tc.name: ApiCompatibleVersionTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ApiCompatibleVersionTest002, TestSize.Level1)
{
    ApiCompatibleVersionCmdParam param{0};
    auto mod = std::make_shared<ApiCompatibleVersionCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: VirtualDisplayIdTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, VirtualDisplayIdTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    constexpr uint32_t displayId = 5;
    VirtualDisplayIdCmdParam param{displayId};
    auto mod = std::make_shared<VirtualDisplayIdCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::VIRTUAL_DISPLAY_ID);
    EXPECT_EQ(mod->GetParam().virtualDisplayId_, displayId);

    constexpr uint32_t displayId2 = 10;
    VirtualDisplayIdCmdParam param2{displayId2};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().virtualDisplayId_, displayId2);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("virtualDisplayId"), std::string::npos);
}

/**
 * @tc.name: VirtualDisplayIdTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, VirtualDisplayIdTest002, TestSize.Level1)
{
    VirtualDisplayIdCmdParam param{0};
    auto mod = std::make_shared<VirtualDisplayIdCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: AttachToWindowContainerTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AttachToWindowContainerTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    constexpr ScreenId screenId = 42;
    AttachToWindowContainerCmdParam param{screenId};
    auto mod = std::make_shared<AttachToWindowContainerCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::ATTACH_TO_WINDOW_CONTAINER);
    EXPECT_EQ(mod->GetParam().attachToWindowContainer_, screenId);

    constexpr ScreenId screenId2 = 99;
    AttachToWindowContainerCmdParam param2{screenId2};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().attachToWindowContainer_, screenId2);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("attachToWindowContainer"), std::string::npos);
}

/**
 * @tc.name: AttachToWindowContainerTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AttachToWindowContainerTest002, TestSize.Level1)
{
    AttachToWindowContainerCmdParam param{0};
    auto mod = std::make_shared<AttachToWindowContainerCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: RegionToBeMagnifiedTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, RegionToBeMagnifiedTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    Vector4<int> region{1, 2, 3, 4};
    RegionToBeMagnifiedCmdParam param{region};
    auto mod = std::make_shared<RegionToBeMagnifiedCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::REGION_TO_BE_MAGNIFIED);
    EXPECT_EQ(mod->GetParam().regionToBeMagnified_.x_, 1);
    EXPECT_EQ(mod->GetParam().regionToBeMagnified_.y_, 2);
    EXPECT_EQ(mod->GetParam().regionToBeMagnified_.z_, 3);
    EXPECT_EQ(mod->GetParam().regionToBeMagnified_.w_, 4);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("regionToBeMagnified"), std::string::npos);
}

/**
 * @tc.name: RegionToBeMagnifiedTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, RegionToBeMagnifiedTest002, TestSize.Level1)
{
    Vector4<int> region{0, 0, 0, 0};
    RegionToBeMagnifiedCmdParam param{region};
    auto mod = std::make_shared<RegionToBeMagnifiedCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: DetachFromWindowContainerTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, DetachFromWindowContainerTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    constexpr ScreenId screenId = 42;
    DetachFromWindowContainerCmdParam param{screenId};
    auto mod = std::make_shared<DetachFromWindowContainerCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::DETACH_TO_WINDOW_CONTAINER);
    EXPECT_EQ(mod->GetParam().detachFromWindowContainer_, screenId);

    constexpr ScreenId screenId2 = 99;
    DetachFromWindowContainerCmdParam param2{screenId2};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().detachFromWindowContainer_, screenId2);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("detachFromWindowContainer"), std::string::npos);
}

/**
 * @tc.name: DetachFromWindowContainerTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, DetachFromWindowContainerTest002, TestSize.Level1)
{
    DetachFromWindowContainerCmdParam param{0};
    auto mod = std::make_shared<DetachFromWindowContainerCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: CompositeLayerTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, CompositeLayerTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    CompositeLayerCmdParam param{true, 0};
    auto mod = std::make_shared<CompositeLayerCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::UPDATE_COMPOSITE_LAYER);
    EXPECT_TRUE(mod->GetParam().isCompositeLayer_);
    EXPECT_EQ(mod->GetParam().isMarkedByUI_, 0);

    CompositeLayerCmdParam param2{false, 1};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isCompositeLayer_);
    EXPECT_EQ(mod->GetParam().isMarkedByUI_, 1);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isCompositeLayer"), std::string::npos);
}

/**
 * @tc.name: CompositeLayerTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, CompositeLayerTest002, TestSize.Level1)
{
    CompositeLayerCmdParam param{true, 0};
    auto mod = std::make_shared<CompositeLayerCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: CompositeLayerTest003
 * @tc.desc: Test UpdateToRenderWithResult with null node returns false
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, CompositeLayerTest003, TestSize.Level1)
{
    CompositeLayerCmdParam param{true, 0};
    auto mod = std::make_shared<CompositeLayerCmdModifier>(MakeExpiredNode(), param);
    auto result = mod->UpdateToRenderWithResult();
    bool val = std::get<bool>(result);
    EXPECT_FALSE(val);
}

/**
 * @tc.name: CompositeLayerTest004
 * @tc.desc: Test UpdateToRenderWithResult with valid node returns true
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, CompositeLayerTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    CompositeLayerCmdParam param{true, 0};
    auto mod = std::make_shared<CompositeLayerCmdModifier>(node, param);
    auto result = mod->UpdateToRenderWithResult();
    bool val = std::get<bool>(result);
    EXPECT_TRUE(val);
}

/**
 * @tc.name: StaticCachedTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, StaticCachedTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    StaticCachedCmdParam param{true};
    auto mod = std::make_shared<StaticCachedCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::SET_STATIC_CACHED);
    EXPECT_TRUE(mod->GetParam().isStaticCached_);

    StaticCachedCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isStaticCached_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isStaticCached"), std::string::npos);
}

/**
 * @tc.name: StaticCachedTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, StaticCachedTest002, TestSize.Level1)
{
    StaticCachedCmdParam param{true};
    auto mod = std::make_shared<StaticCachedCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: FrameGravityNewVersionEnabledTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, FrameGravityNewVersionEnabledTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    FrameGravityNewVersionEnabledCmdParam param{true};
    auto mod = std::make_shared<FrameGravityNewVersionEnabledCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::IS_FRAME_GRAVITY_NEW_VER_ENABLED);
    EXPECT_TRUE(mod->GetParam().isFrameGravityNewVersionEnabled_);

    FrameGravityNewVersionEnabledCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isFrameGravityNewVersionEnabled_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isFrameGravityNewVersionEnabled"), std::string::npos);
}

/**
 * @tc.name: FrameGravityNewVersionEnabledTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, FrameGravityNewVersionEnabledTest002, TestSize.Level1)
{
    FrameGravityNewVersionEnabledCmdParam param{true};
    auto mod = std::make_shared<FrameGravityNewVersionEnabledCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: SurfaceBufferOpaqueTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SurfaceBufferOpaqueTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SurfaceBufferOpaqueCmdParam param{true};
    auto mod = std::make_shared<SurfaceBufferOpaqueCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::IS_SURFACE_BUFFER_OPAQUE);
    EXPECT_TRUE(mod->GetParam().isSurfaceBufferOpaque_);

    SurfaceBufferOpaqueCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isSurfaceBufferOpaque_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isSurfaceBufferOpaque"), std::string::npos);
}

/**
 * @tc.name: SurfaceBufferOpaqueTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SurfaceBufferOpaqueTest002, TestSize.Level1)
{
    SurfaceBufferOpaqueCmdParam param{true};
    auto mod = std::make_shared<SurfaceBufferOpaqueCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ContainerWindowTransparentTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ContainerWindowTransparentTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ContainerWindowTransparentCmdParam param{true};
    auto mod = std::make_shared<ContainerWindowTransparentCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::IS_CONTAINER_WINDOW_TRANSPARENT);
    EXPECT_TRUE(mod->GetParam().isContainerWindowTransparent_);

    ContainerWindowTransparentCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isContainerWindowTransparent_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isContainerWindowTransparent"), std::string::npos);
}

/**
 * @tc.name: ContainerWindowTransparentTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ContainerWindowTransparentTest002, TestSize.Level1)
{
    ContainerWindowTransparentCmdParam param{true};
    auto mod = std::make_shared<ContainerWindowTransparentCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: SurfaceDefaultSizeTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SurfaceDefaultSizeTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SurfaceDefaultSizeCmdParam param{100.0f, 200.0f};
    auto mod = std::make_shared<SurfaceDefaultSizeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::UPDATE_SURFACE_DEFAULT_SIZE);
    EXPECT_FLOAT_EQ(mod->GetParam().width_, 100.0f);
    EXPECT_FLOAT_EQ(mod->GetParam().height_, 200.0f);

    SurfaceDefaultSizeCmdParam param2{300.0f, 400.0f};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FLOAT_EQ(mod->GetParam().width_, 300.0f);
    EXPECT_FLOAT_EQ(mod->GetParam().height_, 400.0f);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("width"), std::string::npos);
    EXPECT_NE(out.find("height"), std::string::npos);
}

/**
 * @tc.name: SurfaceDefaultSizeTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SurfaceDefaultSizeTest002, TestSize.Level1)
{
    SurfaceDefaultSizeCmdParam param{0.0f, 0.0f};
    auto mod = std::make_shared<SurfaceDefaultSizeCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: RSBufferAvailableCallbackWrapperTest001
 * @tc.desc: Test callback wrapper with valid callback
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, RSBufferAvailableCallbackWrapperTest001, TestSize.Level1)
{
    bool called = false;
    auto wrapper = new RSBufferAvailableCallbackWrapper([&called]() { called = true; });
    wrapper->OnBufferAvailable();
    EXPECT_TRUE(called);
    ASSERT_TRUE(wrapper->GetCallback());
    delete wrapper;
}

/**
 * @tc.name: RSBufferAvailableCallbackWrapperTest002
 * @tc.desc: Test callback wrapper with null callback
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, RSBufferAvailableCallbackWrapperTest002, TestSize.Level1)
{
    auto wrapper = new RSBufferAvailableCallbackWrapper(nullptr);
    wrapper->OnBufferAvailable();
    ASSERT_FALSE(wrapper->GetCallback());
    delete wrapper;
}

/**
 * @tc.name: LeashPersistentIdTest003
 * @tc.desc: Test LeashPersistentIdCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, LeashPersistentIdTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    LeashPersistentIdCmdParam param{100};
    auto mod = std::make_shared<LeashPersistentIdCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().leashPersistentId_, 100);
}

/**
 * @tc.name: SecurityLayerTest003
 * @tc.desc: Test SecurityLayerCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SecurityLayerTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SecurityLayerCmdParam param{true};
    auto mod = std::make_shared<SecurityLayerCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().isSecurityLayer_);
}

/**
 * @tc.name: SkipLayerTest003
 * @tc.desc: Test SkipLayerCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SkipLayerTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SkipLayerCmdParam param{true};
    auto mod = std::make_shared<SkipLayerCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().isSkipLayer_);
}

/**
 * @tc.name: SnapshotSkipLayerTest003
 * @tc.desc: Test SnapshotSkipLayerCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SnapshotSkipLayerTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SnapshotSkipLayerCmdParam param{true};
    auto mod = std::make_shared<SnapshotSkipLayerCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().isSnapshotSkipLayer_);
}

/**
 * @tc.name: HasFingerprintTest003
 * @tc.desc: Test HasFingerprintCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HasFingerprintTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    HasFingerprintCmdParam param{true};
    auto mod = std::make_shared<HasFingerprintCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().hasFingerprint_);
}

/**
 * @tc.name: ColorSpaceTest003
 * @tc.desc: Test ColorSpaceCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ColorSpaceTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ColorSpaceCmdParam param{GRAPHIC_COLOR_GAMUT_SRGB};
    auto mod = std::make_shared<ColorSpaceCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().colorSpace_, GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: AbilityBGAlphaTest003
 * @tc.desc: Test AbilityBGAlphaCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AbilityBGAlphaTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    AbilityBGAlphaCmdParam param{128};
    auto mod = std::make_shared<AbilityBGAlphaCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().abilityBGAlpha_, 128);
}

/**
 * @tc.name: NotifyUIBufferAvailableTest003
 * @tc.desc: Test NotifyUIBufferAvailableCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, NotifyUIBufferAvailableTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    NotifyUIBufferAvailableCmdParam param{true};
    auto mod = std::make_shared<NotifyUIBufferAvailableCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().notifyUIBufferAvailable_);
}

/**
 * @tc.name: ContainerWindowTest003
 * @tc.desc: Test ContainerWindowCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ContainerWindowTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ContainerWindowCmdParam param{true, RRect()};
    auto mod = std::make_shared<ContainerWindowCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().hasContainerWindow_);
}

/**
 * @tc.name: FreezeTest003
 * @tc.desc: Test FreezeCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, FreezeTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    FreezeCmdParam param{true, false};
    auto mod = std::make_shared<FreezeCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().isFreeze_);
    EXPECT_FALSE(mod->GetParam().isMarkedByUI_);
}

/**
 * @tc.name: HardwareEnableHintTest003
 * @tc.desc: Test HardwareEnableHintCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HardwareEnableHintTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    HardwareEnableHintCmdParam param{true};
    auto mod = std::make_shared<HardwareEnableHintCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().isHardwareEnabled_);
}

/**
 * @tc.name: HardwareEnabledTest004
 * @tc.desc: Test HardwareEnabledCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HardwareEnabledTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    HardwareEnabledCmdParam param{true, SelfDrawingNodeType::DEFAULT, false};
    auto mod = std::make_shared<HardwareEnabledCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().isHardwareEnabled_);
    EXPECT_EQ(mod->GetParam().isSelfDrawingNodeType_, SelfDrawingNodeType::DEFAULT);
    EXPECT_FALSE(mod->GetParam().isDynamicHardwareEnable_);
}

/**
 * @tc.name: BootAnimationTest003
 * @tc.desc: Test BootAnimationCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, BootAnimationTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    BootAnimationCmdParam param{true};
    auto mod = std::make_shared<BootAnimationCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().isBootAnimation_);
}

/**
 * @tc.name: GlobalPositionEnabledTest003
 * @tc.desc: Test GlobalPositionEnabledCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, GlobalPositionEnabledTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    GlobalPositionEnabledCmdParam param{true};
    auto mod = std::make_shared<GlobalPositionEnabledCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().isGlobalPositionEnabled_);
}

/**
 * @tc.name: ClonedNodeInfoTest003
 * @tc.desc: Test ClonedNodeInfoCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ClonedNodeInfoTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ClonedNodeInfoCmdParam param{100, false, true};
    auto mod = std::make_shared<ClonedNodeInfoCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().clonedNodeId_, 100);
    EXPECT_FALSE(mod->GetParam().clonedNeedOffscreen_);
    EXPECT_TRUE(mod->GetParam().clonedIsRelated_);
}

/**
 * @tc.name: ForceUIFirstTest003
 * @tc.desc: Test ForceUIFirstCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ForceUIFirstTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ForceUIFirstCmdParam param{true};
    auto mod = std::make_shared<ForceUIFirstCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().forceUIFirst_);
}

/**
 * @tc.name: AncoFlagsTest003
 * @tc.desc: Test AncoFlagsCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AncoFlagsTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    AncoFlagsCmdParam param{42};
    auto mod = std::make_shared<AncoFlagsCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().ancoFlags_, 42u);
}

/**
 * @tc.name: SkipDrawTest003
 * @tc.desc: Test SkipDrawCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SkipDrawTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SkipDrawCmdParam param{true};
    auto mod = std::make_shared<SkipDrawCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().skipDraw_);
}

/**
 * @tc.name: WatermarkEnabledTest003
 * @tc.desc: Test WatermarkEnabledCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, WatermarkEnabledTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    WatermarkEnabledCmdParam param{"test", true};
    auto mod = std::make_shared<WatermarkEnabledCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().watermarkName_, "test");
    EXPECT_TRUE(mod->GetParam().watermarkEnabled_);
}

/**
 * @tc.name: AbilityStateTest003
 * @tc.desc: Test AbilityStateCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AbilityStateTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    AbilityStateCmdParam param{RSSurfaceNodeAbilityState::FOREGROUND};
    auto mod = std::make_shared<AbilityStateCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().abilityState_, RSSurfaceNodeAbilityState::FOREGROUND);
}

/**
 * @tc.name: HidePrivacyContentTest004
 * @tc.desc: Test HidePrivacyContentCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HidePrivacyContentTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    HidePrivacyContentCmdParam param{true};
    auto mod = std::make_shared<HidePrivacyContentCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().hidePrivacyContent_);
}

/**
 * @tc.name: ApiCompatibleVersionTest003
 * @tc.desc: Test ApiCompatibleVersionCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ApiCompatibleVersionTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ApiCompatibleVersionCmdParam param{12};
    auto mod = std::make_shared<ApiCompatibleVersionCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().apiCompatibleVersion_, 12u);
}

/**
 * @tc.name: VirtualDisplayIdTest003
 * @tc.desc: Test VirtualDisplayIdCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, VirtualDisplayIdTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    VirtualDisplayIdCmdParam param{99};
    auto mod = std::make_shared<VirtualDisplayIdCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().virtualDisplayId_, 99u);
}

/**
 * @tc.name: AttachToWindowContainerTest003
 * @tc.desc: Test AttachToWindowContainerCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AttachToWindowContainerTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    AttachToWindowContainerCmdParam param{100};
    auto mod = std::make_shared<AttachToWindowContainerCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().attachToWindowContainer_, 100u);
}

/**
 * @tc.name: RegionToBeMagnifiedTest003
 * @tc.desc: Test RegionToBeMagnifiedCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, RegionToBeMagnifiedTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    RegionToBeMagnifiedCmdParam param{Vector4<int>(1, 2, 3, 4)};
    auto mod = std::make_shared<RegionToBeMagnifiedCmdModifier>(node, param);
    RegionToBeMagnifiedCmdParam param2{Vector4<int>(5, 6, 7, 8)};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().regionToBeMagnified_.x_, 5);
}

/**
 * @tc.name: DetachFromWindowContainerTest003
 * @tc.desc: Test DetachFromWindowContainerCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, DetachFromWindowContainerTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    DetachFromWindowContainerCmdParam param{200};
    auto mod = std::make_shared<DetachFromWindowContainerCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().detachFromWindowContainer_, 200u);
}

/**
 * @tc.name: CompositeLayerTest005
 * @tc.desc: Test CompositeLayerCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, CompositeLayerTest005, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    CompositeLayerCmdParam param{true, 0};
    auto mod = std::make_shared<CompositeLayerCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().isCompositeLayer_);
    EXPECT_EQ(mod->GetParam().isMarkedByUI_, 0u);
}

/**
 * @tc.name: StaticCachedTest003
 * @tc.desc: Test StaticCachedCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, StaticCachedTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    StaticCachedCmdParam param{true};
    auto mod = std::make_shared<StaticCachedCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().isStaticCached_);
}

/**
 * @tc.name: FrameGravityNewVersionEnabledTest003
 * @tc.desc: Test FrameGravityNewVersionEnabledCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, FrameGravityNewVersionEnabledTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    FrameGravityNewVersionEnabledCmdParam param{true};
    auto mod = std::make_shared<FrameGravityNewVersionEnabledCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().isFrameGravityNewVersionEnabled_);
}

/**
 * @tc.name: SurfaceBufferOpaqueTest003
 * @tc.desc: Test SurfaceBufferOpaqueCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SurfaceBufferOpaqueTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SurfaceBufferOpaqueCmdParam param{true};
    auto mod = std::make_shared<SurfaceBufferOpaqueCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().isSurfaceBufferOpaque_);
}

/**
 * @tc.name: ContainerWindowTransparentTest003
 * @tc.desc: Test ContainerWindowTransparentCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ContainerWindowTransparentTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ContainerWindowTransparentCmdParam param{true};
    auto mod = std::make_shared<ContainerWindowTransparentCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(mod->GetParam().isContainerWindowTransparent_);
}

/**
 * @tc.name: SurfaceDefaultSizeTest003
 * @tc.desc: Test SurfaceDefaultSizeCmdModifier SetParam and GetParam
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SurfaceDefaultSizeTest003, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SurfaceDefaultSizeCmdParam param{100.0f, 200.0f};
    auto mod = std::make_shared<SurfaceDefaultSizeCmdModifier>(node, param);
    bool ret = mod->SetParam(param);
    EXPECT_TRUE(ret);
    EXPECT_FLOAT_EQ(mod->GetParam().width_, 100.0f);
    EXPECT_FLOAT_EQ(mod->GetParam().height_, 200.0f);
}

/**
 * @tc.name: LeashPersistentIdTest004
 * @tc.desc: Test LeashPersistentIdCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, LeashPersistentIdTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    LeashPersistentIdCmdParam param{100};
    auto mod = std::make_shared<LeashPersistentIdCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: SecurityLayerTest004
 * @tc.desc: Test SecurityLayerCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SecurityLayerTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SecurityLayerCmdParam param{true};
    auto mod = std::make_shared<SecurityLayerCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: SkipLayerTest004
 * @tc.desc: Test SkipLayerCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SkipLayerTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SkipLayerCmdParam param{true};
    auto mod = std::make_shared<SkipLayerCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: SnapshotSkipLayerTest004
 * @tc.desc: Test SnapshotSkipLayerCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SnapshotSkipLayerTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SnapshotSkipLayerCmdParam param{true};
    auto mod = std::make_shared<SnapshotSkipLayerCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: HasFingerprintTest004
 * @tc.desc: Test HasFingerprintCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HasFingerprintTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    HasFingerprintCmdParam param{true};
    auto mod = std::make_shared<HasFingerprintCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ColorSpaceTest004
 * @tc.desc: Test ColorSpaceCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ColorSpaceTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ColorSpaceCmdParam param{GRAPHIC_COLOR_GAMUT_SRGB};
    auto mod = std::make_shared<ColorSpaceCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: AbilityBGAlphaTest004
 * @tc.desc: Test AbilityBGAlphaCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AbilityBGAlphaTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    AbilityBGAlphaCmdParam param{128};
    auto mod = std::make_shared<AbilityBGAlphaCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: NotifyUIBufferAvailableTest004
 * @tc.desc: Test NotifyUIBufferAvailableCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, NotifyUIBufferAvailableTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    NotifyUIBufferAvailableCmdParam param{true};
    auto mod = std::make_shared<NotifyUIBufferAvailableCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ContainerWindowTest004
 * @tc.desc: Test ContainerWindowCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ContainerWindowTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ContainerWindowCmdParam param{true, RRect()};
    auto mod = std::make_shared<ContainerWindowCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: FreezeTest004
 * @tc.desc: Test FreezeCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, FreezeTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    FreezeCmdParam param{true, false};
    auto mod = std::make_shared<FreezeCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: HardwareEnableHintTest004
 * @tc.desc: Test HardwareEnableHintCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HardwareEnableHintTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    HardwareEnableHintCmdParam param{true};
    auto mod = std::make_shared<HardwareEnableHintCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: HardwareEnabledTest005
 * @tc.desc: Test HardwareEnabledCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HardwareEnabledTest005, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    HardwareEnabledCmdParam param{true, SelfDrawingNodeType::DEFAULT, false};
    auto mod = std::make_shared<HardwareEnabledCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: BootAnimationTest004
 * @tc.desc: Test BootAnimationCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, BootAnimationTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    BootAnimationCmdParam param{true};
    auto mod = std::make_shared<BootAnimationCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: GlobalPositionEnabledTest004
 * @tc.desc: Test GlobalPositionEnabledCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, GlobalPositionEnabledTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    GlobalPositionEnabledCmdParam param{true};
    auto mod = std::make_shared<GlobalPositionEnabledCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ClonedNodeInfoTest004
 * @tc.desc: Test ClonedNodeInfoCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ClonedNodeInfoTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ClonedNodeInfoCmdParam param{100, false, true};
    auto mod = std::make_shared<ClonedNodeInfoCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ForceUIFirstTest004
 * @tc.desc: Test ForceUIFirstCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ForceUIFirstTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ForceUIFirstCmdParam param{true};
    auto mod = std::make_shared<ForceUIFirstCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: AncoFlagsTest004
 * @tc.desc: Test AncoFlagsCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AncoFlagsTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    AncoFlagsCmdParam param{42};
    auto mod = std::make_shared<AncoFlagsCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: SkipDrawTest004
 * @tc.desc: Test SkipDrawCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SkipDrawTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SkipDrawCmdParam param{true};
    auto mod = std::make_shared<SkipDrawCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: WatermarkEnabledTest004
 * @tc.desc: Test WatermarkEnabledCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, WatermarkEnabledTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    WatermarkEnabledCmdParam param{"test", true};
    auto mod = std::make_shared<WatermarkEnabledCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: AbilityStateTest004
 * @tc.desc: Test AbilityStateCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AbilityStateTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    AbilityStateCmdParam param{RSSurfaceNodeAbilityState::FOREGROUND};
    auto mod = std::make_shared<AbilityStateCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: HidePrivacyContentTest005
 * @tc.desc: Test HidePrivacyContentCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HidePrivacyContentTest005, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    HidePrivacyContentCmdParam param{true};
    auto mod = std::make_shared<HidePrivacyContentCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ApiCompatibleVersionTest004
 * @tc.desc: Test ApiCompatibleVersionCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ApiCompatibleVersionTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ApiCompatibleVersionCmdParam param{12};
    auto mod = std::make_shared<ApiCompatibleVersionCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: VirtualDisplayIdTest004
 * @tc.desc: Test VirtualDisplayIdCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, VirtualDisplayIdTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    VirtualDisplayIdCmdParam param{99};
    auto mod = std::make_shared<VirtualDisplayIdCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: AttachToWindowContainerTest004
 * @tc.desc: Test AttachToWindowContainerCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AttachToWindowContainerTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    AttachToWindowContainerCmdParam param{100};
    auto mod = std::make_shared<AttachToWindowContainerCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: RegionToBeMagnifiedTest004
 * @tc.desc: Test RegionToBeMagnifiedCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, RegionToBeMagnifiedTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    RegionToBeMagnifiedCmdParam param{Vector4<int>(1, 2, 3, 4)};
    auto mod = std::make_shared<RegionToBeMagnifiedCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: DetachFromWindowContainerTest004
 * @tc.desc: Test DetachFromWindowContainerCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, DetachFromWindowContainerTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    DetachFromWindowContainerCmdParam param{200};
    auto mod = std::make_shared<DetachFromWindowContainerCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: CompositeLayerTest006
 * @tc.desc: Test CompositeLayerCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, CompositeLayerTest006, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    CompositeLayerCmdParam param{true, 0};
    auto mod = std::make_shared<CompositeLayerCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: StaticCachedTest004
 * @tc.desc: Test StaticCachedCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, StaticCachedTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    StaticCachedCmdParam param{true};
    auto mod = std::make_shared<StaticCachedCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: FrameGravityNewVersionEnabledTest004
 * @tc.desc: Test FrameGravityNewVersionEnabledCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, FrameGravityNewVersionEnabledTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    FrameGravityNewVersionEnabledCmdParam param{true};
    auto mod = std::make_shared<FrameGravityNewVersionEnabledCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: SurfaceBufferOpaqueTest004
 * @tc.desc: Test SurfaceBufferOpaqueCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SurfaceBufferOpaqueTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SurfaceBufferOpaqueCmdParam param{true};
    auto mod = std::make_shared<SurfaceBufferOpaqueCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ContainerWindowTransparentTest004
 * @tc.desc: Test ContainerWindowTransparentCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ContainerWindowTransparentTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ContainerWindowTransparentCmdParam param{true};
    auto mod = std::make_shared<ContainerWindowTransparentCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: SurfaceDefaultSizeTest004
 * @tc.desc: Test SurfaceDefaultSizeCmdModifier UpdateToRender
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, SurfaceDefaultSizeTest004, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    SurfaceDefaultSizeCmdParam param{100.0f, 200.0f};
    auto mod = std::make_shared<SurfaceDefaultSizeCmdModifier>(node, param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: ForceHardwareAndFixRotationTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ForceHardwareAndFixRotationTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    ForceHardwareAndFixRotationCmdParam param{true};
    auto mod = std::make_shared<ForceHardwareAndFixRotationCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::FORCE_HARDWARE_AND_FIX_ROTATION);
    EXPECT_TRUE(mod->GetParam().flag_);

    ForceHardwareAndFixRotationCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().flag_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("flag"), std::string::npos);
}

/**
 * @tc.name: ForceHardwareAndFixRotationTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, ForceHardwareAndFixRotationTest002, TestSize.Level1)
{
    ForceHardwareAndFixRotationCmdParam param{true};
    auto mod = std::make_shared<ForceHardwareAndFixRotationCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: AppRotationCorrectionTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AppRotationCorrectionTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    AppRotationCorrectionCmdParam param{ScreenRotation::ROTATION_90};
    auto mod = std::make_shared<AppRotationCorrectionCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::APP_ROTATION_CORRECTION);
    EXPECT_EQ(mod->GetParam().appRotationCorrection_, ScreenRotation::ROTATION_90);

    AppRotationCorrectionCmdParam param2{ScreenRotation::ROTATION_180};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().appRotationCorrection_, ScreenRotation::ROTATION_180);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("appRotationCorrection"), std::string::npos);
}

/**
 * @tc.name: AppRotationCorrectionTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, AppRotationCorrectionTest002, TestSize.Level1)
{
    AppRotationCorrectionCmdParam param{ScreenRotation::ROTATION_0};
    auto mod = std::make_shared<AppRotationCorrectionCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: HDRTypeTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HDRTypeTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    HDRTypeCmdParam param{1};
    auto mod = std::make_shared<HDRTypeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::HDR_TYPE);
    EXPECT_EQ(mod->GetParam().hdrType_, 1u);

    HDRTypeCmdParam param2{2};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mod->GetParam().hdrType_, 2u);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("hdrType"), std::string::npos);
}

/**
 * @tc.name: HDRTypeTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, HDRTypeTest002, TestSize.Level1)
{
    HDRTypeCmdParam param{0};
    auto mod = std::make_shared<HDRTypeCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

/**
 * @tc.name: DarkColorModeTest001
 * @tc.desc: Test all methods
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, DarkColorModeTest001, TestSize.Level1)
{
    auto node = RSSurfaceNode::Create(RSSurfaceNodeConfig{});
    DarkColorModeCmdParam param{true};
    auto mod = std::make_shared<DarkColorModeCmdModifier>(node, param);
    EXPECT_EQ(mod->GetType(), RSCmdModifierType::SET_DARK_COLOR_MODE);
    EXPECT_TRUE(mod->GetParam().isDarkColorMode_);

    DarkColorModeCmdParam param2{false};
    bool ret = mod->SetParam(param2);
    EXPECT_TRUE(ret);
    EXPECT_FALSE(mod->GetParam().isDarkColorMode_);

    std::string out;
    mod->DumpParam(out);
    EXPECT_NE(out.find("isDarkColorMode"), std::string::npos);
}

/**
 * @tc.name: DarkColorModeTest002
 * @tc.desc: Test UpdateToRender with null node
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceNodeCommandModifierTest, DarkColorModeTest002, TestSize.Level1)
{
    DarkColorModeCmdParam param{true};
    auto mod = std::make_shared<DarkColorModeCmdModifier>(MakeExpiredNode(), param);
    ASSERT_TRUE(mod);
    mod->UpdateToRender();
}

} // namespace Rosen
} // namespace OHOS
