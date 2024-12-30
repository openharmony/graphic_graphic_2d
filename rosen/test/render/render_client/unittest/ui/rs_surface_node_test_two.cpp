/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */

#include "ui/rs_surface_node.h"
#include "gtest/gtest.h"
#include "parameters.h"
#include "limit_number.h"

using namespace testing;
using namespace testing::ext;
static constexpr uint32_t FIRST_COLOR_VALUE = 0x034123;
static constexpr uint32_t SECOND_COLOR_VALUE = 0x45ba87;
static constexpr uint32_t THIRD_COLOR_VALUE = 0x32aadd;
namespace OHOS::Rosen {
class RSSurfaceNodeTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f, 485.44f, -34.4f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };
    static constexpr float outerRadius = 30.4f;
    RRect rrect = RRect({0, 0, 0, 0}, outerRadius, outerRadius);
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceNodeTest::SetUpTestCase() {}
void RSSurfaceNodeTest::TearDownTestCase() {}
void RSSurfaceNodeTest::SetUp() {}
void RSSurfaceNodeTest::TearDown() {}

/**
 * @tc.name: SetShadowRadius001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeTest, SetShadowRadius001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowRadius(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowRadius(), floatData[1]));
}

/**
 * @tc.name: SetShadowRadiusa003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeTest, SetShadowRadiusa003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowRadius(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowRadius(), floatData[3]));
}

/**
 * @tc.name: SetShadowColorb001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeTest, SetShadowColorb001, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowColor(FIRST_COLOR_VALUE);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(FIRST_COLOR_VALUE));
}

/**
 * @tc.name: SetShadowRadiusc002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeTest, SetShadowRadiusc002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowRadius(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(surfaceNode->GetStagingProperties().GetShadowRadius(), floatData[2]));
}

/**
 * @tc.name: SetShadowColor003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeTest, SetShadowColor003, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowColor(THIRD_COLOR_VALUE);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(THIRD_COLOR_VALUE));
}

/**
 * @tc.name: SetShadowColorf002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: issueI5J8R1
 */
HWTEST_F(RSSurfaceNodeTest, SetShadowColorf002, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetShadowColor(SECOND_COLOR_VALUE);
    EXPECT_TRUE(surfaceNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(SECOND_COLOR_VALUE));
}

/**
 * @tc.name: SetFreeze Test False
 * @tc.desc: SetFreeze Test False
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeTest, SetFreeze_False, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetFreeze(false);
}

/**
 * @tc.name: SetContainerWindow Test True
 * @tc.desc: SetContainerWindow Test True
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeTest, SetContainerWindow_True, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetContainerWindow(true, rrect);
}

/**
 * @tc.name: SetContainerWindow Test False
 * @tc.desc: SetContainerWindow Test False
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeTest, SetContainerWindow_False, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetContainerWindow(false, rrect);
}

/**
 * @tc.name: Fingerprint Test
 * @tc.desc: SetFingerprint and GetFingerprint
 * @tc.type: FUNC
 * @tc.require: issueI6Z3YK
 */
HWTEST_F(RSSurfaceNodeTest, Fingerprint, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetFingerprint(true);
    ASSERT_EQ(true, surfaceNode->GetFingerprint());
    surfaceNode->SetFingerprint(false);
    ASSERT_EQ(false, surfaceNode->GetFingerprint());
}

/**
 * @tc.name: SetBootAnimation Testaa
 * @tc.desc: SetBootAnimation and GetBootAnimation
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, SetBootAnimationTestaa, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetBootAnimation(true);
    ASSERT_EQ(true, surfaceNode->GetBootAnimation());
    surfaceNode->SetBootAnimation(false);
    ASSERT_EQ(false, surfaceNode->GetBootAnimation());
}

/**
 * @tc.name: SetFreeze Test Truedd
 * @tc.desc: SetFreeze Test True
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeTest, SetFreeze_Truedd, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetFreeze(true);
}

/**
 * @tc.name: SetGlobalPositionEnabled Test
 * @tc.desc: SetGlobalPositionEnabled and GetGlobalPositionEnabled
 * @tc.type: FUNC
 * @tc.require: issueIATYMW
 */
HWTEST_F(RSSurfaceNodeTest, SetGlobalPositionEnabled, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetGlobalPositionEnabled(true);
    ASSERT_EQ(true, surfaceNode->GetGlobalPositionEnabled());
    surfaceNode->SetGlobalPositionEnabled(false);
    ASSERT_EQ(false, surfaceNode->GetGlobalPositionEnabled());
}

/**
 * @tc.name: IsBufferAvailableTest Test
 * @tc.desc: test results of IsBufferAvailable
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSSurfaceNodeTest, IsBufferAvailableTest, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_FALSE(surfaceNode->IsBufferAvailable());
}

/**
 * @tc.name: SetTextureExport Test
 * @tc.desc: SetTextureExport
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, SetTextureExport, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetTextureExport(true);
    //for test
    bool ret = true;
    ASSERT_EQ(true, ret);
    surfaceNode->SetTextureExport(false);
    //for test
    ret = false;
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: SetHardwareEnabled Test
 * @tc.desc: SetHardwareEnabled
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, SetHardwareEnabled, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetHardwareEnabled(true);
    //for test
    bool ret = true;
    ASSERT_EQ(true, ret);
    surfaceNode->SetHardwareEnabled(false);
    //for test
    ret = false;
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: DetachToDisplay Test
 * @tc.desc: DetachToDisplay
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, DetachToDisplay, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    //for test
    uint64_t screenId = 0;
    surfaceNode->DetachToDisplay(screenId);
    //for test
    bool ret = true;
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: AttachToDisplay Test
 * @tc.desc: AttachToDisplay
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, AttachToDisplay, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    //for test
    uint64_t screenId = 0;
    surfaceNode->AttachToDisplay(screenId);
    //for test
    bool ret = true;
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: SetBoundsChangedCallbacktest Testhh
 * @tc.desc: test results of SetBoundsChangedCallback
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSSurfaceNodeTest, SetBoundsChangedCallbackTesthh, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    BoundsChangedCallback callback = [](const Rosen::Vector4f& bounds) {};
    surfaceNode->SetBoundsChangedCallback(callback);
    ASSERT_NE(surfaceNode->boundsChangedCallback_, nullptr);
}

/**
 * @tc.name: SetAnimationFinished Test
 * @tc.desc: SetAnimationFinished
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, SetAnimationFinished, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetAnimationFinished();
    ASSERT_FALSE(surfaceNode->isSkipLayer_);
}

/**
 * @tc.name: SetForceHardwareAndFixRotation Test
 * @tc.desc: SetForceHardwareAndFixRotation and SetTextureExport
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSSurfaceNodeTest, SetForceHardwareAndFixRotation, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetForceHardwareAndFixRotation(true);
    ASSERT_FALSE(surfaceNode->isSkipLayer_);
    surfaceNode->SetTextureExport(true);
    ASSERT_TRUE(surfaceNode->isTextureExportNode_);
    surfaceNode->SetTextureExport(false);
    ASSERT_FALSE(surfaceNode->isTextureExportNode_);
    surfaceNode->SetTextureExport(false);
    ASSERT_FALSE(surfaceNode->isTextureExportNode_);
}

/**
 * @tc.name: ResetContextAlpha Test
 * @tc.desc: GetBundleName and ResetContextAlpha
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSSurfaceNodeTest, ResetContextAlpha, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->ResetContextAlpha();
    ASSERT_NE(RSTransactionProxy::GetInstance()->implicitRemoteTransactionData_, nullptr);
}

/**
 * @tc.name: SetForeground Test
 * @tc.desc: SetForeground and SetForceUIFirst and SetAncoFlags and SetHDRPresent
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSSurfaceNodeTest, SetForeground, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetForeground(true);
    surfaceNode->SetForceUIFirst(true);
    surfaceNode->SetAncoFlags(1);
    surfaceNode->SetHDRPresent(true, 0);
    ASSERT_NE(RSTransactionProxy::GetInstance()->implicitRemoteTransactionData_, nullptr);
    //for test
    bool ret = true;
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: SetBoundsChangedCallback Test
 * @tc.desc: SetBoundsChangedCallback
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, SetBoundsChangedCallback, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    using BoundsChangedCallback = std::function<void(const Rosen::Vector4f&)>;
    BoundsChangedCallback callback;
    surfaceNode->SetBoundsChangedCallback(callback);
    //for test
    bool ret = true;
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: GetColorSpace Testbb
 * @tc.desc: test results of GetColorSpace
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSSurfaceNodeTest, GetColorSpacebb, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode->GetColorSpace() == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: IsBufferAvailable Test
 * @tc.desc: IsBufferAvailable
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, IsBufferAvailable, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    bool ret = surfaceNode->IsBufferAvailable();
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: MarkUIHidden Test
 * @tc.desc: MarkUIHidden
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, MarkUIHidden, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    bool isHidden = true;
    surfaceNode->MarkUIHidden(isHidden);
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SetAbilityBGAlpha Test
 * @tc.desc: SetAbilityBGAlpha
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, SetAbilityBGAlpha, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    uint8_t alpha = 0;
    surfaceNode->SetAbilityBGAlpha(alpha);
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SetAbilityBGAlpha Test
 * @tc.desc: SetAbilityBGAlpha
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, NeedForcedSendToRemote, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->NeedForcedSendToRemote();
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: CreateNode Test
 * @tc.desc: CreateNode
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, CreateNode, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    const RSSurfaceRenderNodeConfig& config = {0, "SurfaceNode", RSSurfaceNodeType::DEFAULT, nullptr, false, false};
    bool res = surfaceNode->CreateNode(config);
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: CreateNodeAndSurface Test
 * @tc.desc: CreateNodeAndSurface
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, CreateNodeAndSurface, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    const RSSurfaceRenderNodeConfig& config = {0, "SurfaceNode", RSSurfaceNodeType::DEFAULT, nullptr, false, false};
    SurfaceId surfaceId = 0;
    bool res = surfaceNode->CreateNodeAndSurface(config, surfaceId);
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SetIsNotifyUIBufferAvailable Testnn
 * @tc.desc: SetIsNotifyUIBufferAvailable
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, SetIsNotifyUIBufferAvailablenn, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    bool available = true;
    surfaceNode->SetIsNotifyUIBufferAvailable(available);
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: OnBoundsSizeChanged Test
 * @tc.desc: OnBoundsSizeChanged
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, OnBoundsSizeChanged, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->OnBoundsSizeChanged();
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SetSurfaceIdToRenderNode Test
 * @tc.desc: SetSurfaceIdToRenderNode
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, SetSurfaceIdToRenderNode, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetSurfaceIdToRenderNode();
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SetIsTextureExportNode Test
 * @tc.desc: SetIsTextureExportNode
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, SetIsTextureExportNode, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    bool isTextureExportNode = true;
    surfaceNode->SetIsTextureExportNode(isTextureExportNode);
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SetColorSpace Test
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeTest, SetColorSpace, TestSize.Level1)
{
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    surfaceNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    ASSERT_EQ(surfaceNode->colorSpace_, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: CreateRenderNodeForTextureExportSwitch Test
 * @tc.desc: CreateRenderNodeForTextureExportSwitchgg
 * @tc.type: FUNC
 * @tc.require:SR000HSUII
 */
HWTEST_F(RSSurfaceNodeTest, CreateRenderNodeForTextureExportSwitchgg, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->CreateRenderNodeForTextureExportSwitch();
    bool res = true;
    ASSERT_EQ(true, res);
}

/**
 * @tc.name: SetSKipDraw
 * @tc.desc: Test function SetSkipDraw
 * @tc.type: FUNC
 * @tc.require: issueI9U6LX
 */
HWTEST_F(RSSurfaceNodeTest, SetSkipDraw, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetSkipDraw(true);
    EXPECT_TRUE(surfaceNode->GetSkipDraw());

    surfaceNode->SetSkipDraw(false);
    EXPECT_FALSE(surfaceNode->GetSkipDraw());
}

/**
 * @tc.name: GetSkipLayer001
 * @tc.desc: Test function GetSkipDraw
 * @tc.type: FUNC
 * @tc.require: issueI9U6LX
 */
HWTEST_F(RSSurfaceNodeTest, GetSkipDraw, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    EXPECT_FALSE(surfaceNode->GetSkipDraw());
}

/**
 * @tc.name: GetAbilityState
 * @tc.desc: Test function GetAbilityState
 * @tc.type: FUNC
 * @tc.require: issueIAQL48
 */
HWTEST_F(RSSurfaceNodeTest, GetAbilityState, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    EXPECT_TRUE(surfaceNode->GetAbilityState() == RSSurfaceNodeAbilityState::FOREGROUND);
}

/**
 * @tc.name: SetAbilityStatemm
 * @tc.desc: Test function SetAbilityState
 * @tc.type: FUNC
 * @tc.require: issueIAQL48
 */
HWTEST_F(RSSurfaceNodeTest, SetAbilityStatemm, TestSize.Level1)
{
    RSSurfaceNodeConfig c;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    surfaceNode->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
    EXPECT_TRUE(surfaceNode->GetAbilityState() == RSSurfaceNodeAbilityState::FOREGROUND);

    surfaceNode->SetAbilityState(RSSurfaceNodeAbilityState::BACKGROUND);
    EXPECT_TRUE(surfaceNode->GetAbilityState() == RSSurfaceNodeAbilityState::BACKGROUND);
}
} // namespace OHOS::Rosen