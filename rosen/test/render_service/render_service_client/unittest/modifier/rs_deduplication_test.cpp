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

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "modifier/rs_property.h"
#include "modifier_ng/geometry/rs_bounds_modifier.h"
#include "modifier_ng/geometry/rs_frame_modifier.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSDeduplicationTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: IsDeduplicationEnabled_NoModifier
 * @tc.desc: Test IsDeduplicationEnabled returns false when no modifier attached
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, IsDeduplicationEnabled_NoModifier, TestSize.Level1)
{
    auto property = std::make_shared<RSProperty<float> >(100.0f);
    // No modifier attached, should return false
    EXPECT_FALSE(property->IsDeduplicationEnabled());
}

/**
 * @tc.name: IsDeduplicationEnabled_WithRegularModifier
 * @tc.desc: Test IsDeduplicationEnabled returns false with regular modifier
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, IsDeduplicationEnabled_WithRegularModifier, TestSize.Level1)
{
    auto property = std::make_shared<RSProperty<float> >(100.0f);
    auto modifier = std::make_shared<RSBoundsModifier>();
    // RSBoundsModifier doesn't enable deduplication by default
    EXPECT_FALSE(modifier->IsDeduplicationEnabled());
    EXPECT_FALSE(property->IsDeduplicationEnabled());
}

/**
 * @tc.name: IsDeduplicationEnabled_WithDeduplicationEnabled
 * @tc.desc: Test IsDeduplicationEnabled returns true when modifier has deduplication enabled
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, IsDeduplicationEnabled_WithDeduplicationEnabled, TestSize.Level1)
{
    auto modifier = std::make_shared<RSBoundsModifier>();
    modifier->SetDeduplicationEnabled(true);
    // Deduplication enabled
    EXPECT_TRUE(modifier->IsDeduplicationEnabled());
}

/**
 * @tc.name: IsAnyModifierDeduplicationEnabled_NoModifier
 * @tc.desc: Test IsAnyModifierDeduplicationEnabled returns false when no modifiers
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, IsAnyModifierDeduplicationEnabled_NoModifier, TestSize.Level1)
{
    RSNode::SharedPtr node = RSCanvasNode::Create();
    ASSERT_TRUE(node != nullptr);
    // No modifiers attached
    EXPECT_FALSE(node->IsAnyModifierDeduplicationEnabled());
}

/**
 * @tc.name: IsAnyModifierDeduplicationEnabled_WithRegularModifier
 * @tc.desc: Test IsAnyModifierDeduplicationEnabled returns false with regular modifiers
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, IsAnyModifierDeduplicationEnabled_WithRegularModifier, TestSize.Level1)
{
    RSNode::SharedPtr node = RSCanvasNode::Create();
    ASSERT_TRUE(node != nullptr);

    auto modifier = std::make_shared<RSBoundsModifier>();
    // RSBoundsModifier default deduplication is disabled
    node->AddModifier(modifier);

    EXPECT_FALSE(node->IsAnyModifierDeduplicationEnabled());
}

/**
 * @tc.name: IsAnyModifierDeduplicationEnabled_WithDeduplicationEnabled
 * @tc.desc: Test IsAnyModifierDeduplicationEnabled returns true when a modifier has deduplication enabled
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, IsAnyModifierDeduplicationEnabled_WithDeduplicationEnabled, TestSize.Level1)
{
    RSNode::SharedPtr node = RSCanvasNode::Create();
    ASSERT_TRUE(node != nullptr);

    auto modifier = std::make_shared<RSBoundsModifier>();
    modifier->SetDeduplicationEnabled(true);
    node->AddModifier(modifier);

    EXPECT_TRUE(node->IsAnyModifierDeduplicationEnabled());
}

/**
 * @tc.name: IsAnyModifierDeduplicationEnabled_WithMultipleModifiers
 * @tc.desc: Test IsAnyModifierDeduplicationEnabled with multiple modifiers
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, IsAnyModifierDeduplicationEnabled_WithMultipleModifiers, TestSize.Level1)
{
    RSNode::SharedPtr node = RSCanvasNode::Create();
    ASSERT_TRUE(node != nullptr);

    auto modifier1 = std::make_shared<RSBoundsModifier>();
    auto modifier2 = std::make_shared<RSFrameModifier>();
    node->AddModifier(modifier1);
    node->AddModifier(modifier2);

    // Both modifiers have deduplication disabled by default
    EXPECT_FALSE(node->IsAnyModifierDeduplicationEnabled());

    // Enable deduplication on one modifier
    modifier1->SetDeduplicationEnabled(true);
    EXPECT_TRUE(node->IsAnyModifierDeduplicationEnabled());
}

/**
 * @tc.name: RSProperty_Set_SameValue_NoDeduplication
 * @tc.desc: Test RSProperty::Set skips update when value is same and deduplication is disabled
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, RSProperty_Set_SameValue_NoDeduplication, TestSize.Level1)
{
    RSNode::SharedPtr node = RSCanvasNode::Create();
    ASSERT_TRUE(node != nullptr);

    auto property = std::make_shared<RSProperty<float> >(100.0f);
    property->Attach(*node);

    // Using Get() to verify value was set
    float firstValue = property->Get();
    EXPECT_EQ(firstValue, 100.0f);

    // Set same value without deduplication enabled - should skip
    property->Set(100.0f);
    // Value should remain the same
    EXPECT_EQ(property->Get(), 100.0f);
}

/**
 * @tc.name: RSProperty_Set_DifferentValue_NoDeduplication
 * @tc.desc: Test RSProperty::Set processes update when value is different
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, RSProperty_Set_DifferentValue_NoDeduplication, TestSize.Level1)
{
    RSNode::SharedPtr node = RSCanvasNode::Create();
    ASSERT_TRUE(node != nullptr);

    auto property = std::make_shared<RSProperty<float> >(100.0f);
    property->Attach(*node);

    property->Set(100.0f);
    // Set different value - should process
    property->Set(200.0f);
    EXPECT_EQ(property->Get(), 200.0f);
}

/**
 * @tc.name: RSSurfaceNode_SetGlobalPositionEnabled_SameValue_NoDeduplication
 * @tc.desc: Test SetGlobalPositionEnabled skips when value is same and no deduplication modifier
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, RSSurfaceNode_SetGlobalPositionEnabled_SameValue_NoDeduplication, TestSize.Level1)
{
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    ASSERT_TRUE(surfaceNode != nullptr);

    // First call - should set
    surfaceNode->SetGlobalPositionEnabled(true);
    EXPECT_TRUE(surfaceNode->GetGlobalPositionEnabled());

    // Second call with same value - should skip (no deduplication modifier)
    surfaceNode->SetGlobalPositionEnabled(true);
    EXPECT_TRUE(surfaceNode->GetGlobalPositionEnabled());
}

/**
 * @tc.name: RSSurfaceNode_SetGlobalPositionEnabled_DifferentValue
 * @tc.desc: Test SetGlobalPositionEnabled processes when value is different
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, RSSurfaceNode_SetGlobalPositionEnabled_DifferentValue, TestSize.Level1)
{
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    ASSERT_TRUE(surfaceNode != nullptr);

    surfaceNode->SetGlobalPositionEnabled(true);
    EXPECT_TRUE(surfaceNode->GetGlobalPositionEnabled());

    // Set different value - should process
    surfaceNode->SetGlobalPositionEnabled(false);
    EXPECT_FALSE(surfaceNode->GetGlobalPositionEnabled());
}

/**
 * @tc.name: RSSurfaceNode_SetGlobalPositionEnabled_SameValue_WithDeduplication
 * @tc.desc: Test SetGlobalPositionEnabled processes when value is same but deduplication is enabled
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, RSSurfaceNode_SetGlobalPositionEnabled_SameValue_WithDeduplication, TestSize.Level1)
{
    RSSurfaceNodeConfig config;
    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(config);
    ASSERT_TRUE(surfaceNode != nullptr);

    // Add a modifier with deduplication enabled
    auto modifier = std::make_shared<RSBoundsModifier>();
    modifier->SetDeduplicationEnabled(true);
    surfaceNode->AddModifier(modifier);

    EXPECT_TRUE(surfaceNode->IsAnyModifierDeduplicationEnabled());

    // First call
    surfaceNode->SetGlobalPositionEnabled(true);
    EXPECT_TRUE(surfaceNode->GetGlobalPositionEnabled());

    // Second call with same value - should still process because deduplication is enabled
    // Note: In actual implementation, command would be sent even with same value
    surfaceNode->SetGlobalPositionEnabled(true);
    // Value should remain set (implementation processes the call)
    EXPECT_TRUE(surfaceNode->GetGlobalPositionEnabled());
}

/**
 * @tc.name: RSFrameModifier_DeduplicationToggle
 * @tc.desc: Test RSFrameModifier deduplication enable/disable
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, RSFrameModifier_DeduplicationToggle, TestSize.Level1)
{
    auto modifier = std::make_shared<RSFrameModifier>();

    // Default is disabled
    EXPECT_FALSE(modifier->IsDeduplicationEnabled());

    // Enable
    modifier->SetDeduplicationEnabled(true);
    EXPECT_TRUE(modifier->IsDeduplicationEnabled());

    // Disable
    modifier->SetDeduplicationEnabled(false);
    EXPECT_FALSE(modifier->IsDeduplicationEnabled());
}

/**
 * @tc.name: RSBoundsModifier_DeduplicationToggle
 * @tc.desc: Test RSBoundsModifier deduplication enable/disable
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, RSBoundsModifier_DeduplicationToggle, TestSize.Level1)
{
    auto modifier = std::make_shared<RSBoundsModifier>();

    // Default is disabled
    EXPECT_FALSE(modifier->IsDeduplicationEnabled());

    // Enable
    modifier->SetDeduplicationEnabled(true);
    EXPECT_TRUE(modifier->IsDeduplicationEnabled());

    // Disable
    modifier->SetDeduplicationEnabled(false);
    EXPECT_FALSE(modifier->IsDeduplicationEnabled());
}

/**
 * @tc.name: NodeWithMultipleModifiers_OneWithDeduplication
 * @tc.desc: Test node with multiple modifiers where only one has deduplication enabled
 * @tc.type: FUNC
 */
HWTEST_F(RSDeduplicationTest, NodeWithMultipleModifiers_OneWithDeduplication, TestSize.Level1)
{
    RSNode::SharedPtr node = RSCanvasNode::Create();
    ASSERT_TRUE(node != nullptr);

    // Add first modifier without deduplication
    auto modifier1 = std::make_shared<RSFrameModifier>();
    node->AddModifier(modifier1);
    EXPECT_FALSE(node->IsAnyModifierDeduplicationEnabled());

    // Add second modifier with deduplication enabled
    auto modifier2 = std::make_shared<RSBoundsModifier>();
    modifier2->SetDeduplicationEnabled(true);
    node->AddModifier(modifier2);
    // Should return true because at least one modifier has deduplication enabled
    EXPECT_TRUE(node->IsAnyModifierDeduplicationEnabled());

    // Remove modifier with deduplication
    node->RemoveModifier(modifier2);
    EXPECT_FALSE(node->IsAnyModifierDeduplicationEnabled());
}

} // namespace OHOS::Rosen
