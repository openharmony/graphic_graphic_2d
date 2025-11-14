/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "common/rs_obj_abs_geometry.h"
#include "property/rs_point_light_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSPointLightManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPointLightManagerTest::SetUpTestCase() {}
void RSPointLightManagerTest::TearDownTestCase() {}
void RSPointLightManagerTest::SetUp() {}
void RSPointLightManagerTest::TearDown() {}

/**
 * @tc.name: Instance001
 * @tc.desc: test results of Instance
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, Instance001, TestSize.Level1)
{
    RSPointLightManager::Instance();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: RegisterLightSource001
 * @tc.desc: test results of RegisterLightSource
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, RegisterLightSource001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto renderNode = std::make_shared<RSRenderNode>(0);
    instance->RegisterLightSource(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: RegisterLightSource002
 * @tc.desc: test results of RegisterLightSource
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, RegisterLightSource002, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::shared_ptr<RSRenderNode> renderNode = nullptr;
    instance->lightSourceNodeMap_.clear();
    instance->RegisterLightSource(renderNode);
    EXPECT_TRUE(instance->lightSourceNodeMap_.empty());
}

/**
 * @tc.name: RegisterIlluminated001
 * @tc.desc: test results of RegisterIlluminated
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, RegisterIlluminated001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto renderNode = std::make_shared<RSRenderNode>(1);
    instance->RegisterIlluminated(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: RegisterIlluminated002
 * @tc.desc: test results of RegisterIlluminated
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, RegisterIlluminated002, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::shared_ptr<RSRenderNode> renderNode = nullptr;
    instance->illuminatedNodeMap_.clear();
    instance->RegisterIlluminated(renderNode);
    EXPECT_TRUE(instance->illuminatedNodeMap_.empty());
}

/**
 * @tc.name: UnRegisterLightSource001
 * @tc.desc: test results of UnRegisterLightSource
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, UnRegisterLightSource001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto renderNode = std::make_shared<RSRenderNode>(1);
    instance->UnRegisterLightSource(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: UnRegisterLightSource002
 * @tc.desc: test results of UnRegisterLightSource
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, UnRegisterLightSource002, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::shared_ptr<RSRenderNode> renderNode = nullptr;
    instance->lightSourceNodeMap_.clear();
    instance->UnRegisterLightSource(renderNode);
    EXPECT_TRUE(instance->lightSourceNodeMap_.empty());
}

/**
 * @tc.name: UnRegisterIlluminated001
 * @tc.desc: test results of UnRegisterIlluminated
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, UnRegisterIlluminated001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto renderNode = std::make_shared<RSRenderNode>(1);
    instance->UnRegisterIlluminated(renderNode);
    EXPECT_TRUE(true);
}


/**
 * @tc.name: UnRegisterIlluminated002
 * @tc.desc: test results of UnRegisterIlluminated
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, UnRegisterIlluminated002, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::shared_ptr<RSRenderNode> renderNode = nullptr;
    instance->UnRegisterIlluminated(renderNode);
    EXPECT_TRUE(instance->illuminatedNodeMap_.empty());
}

/**
 * @tc.name: AddDirtyLightSource001
 * @tc.desc: test results of AddDirtyLightSource
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, AddDirtyLightSource001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::shared_ptr<RSRenderNode> sharedRenderNode = std::make_shared<RSRenderNode>(1);
    std::weak_ptr<RSRenderNode> renderNode = sharedRenderNode;
    instance->AddDirtyLightSource(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: AddDirtyIlluminated001
 * @tc.desc: test results of AddDirtyIlluminated
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, AddDirtyIlluminated001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::shared_ptr<RSRenderNode> sharedRenderNode = std::make_shared<RSRenderNode>(1);
    std::weak_ptr<RSRenderNode> renderNode = sharedRenderNode;
    instance->AddDirtyIlluminated(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ClearDirtyList001
 * @tc.desc: test results of ClearDirtyList
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, ClearDirtyList001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    instance->ClearDirtyList();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: PrepareLight001
 * @tc.desc: test results of PrepareLight
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, PrepareLight001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    instance->PrepareLight();

    std::shared_ptr<RSRenderNode> rsRenderNode = std::make_shared<RSRenderNode>(0);

    instance->RegisterLightSource(rsRenderNode);
    instance->PrepareLight();

    instance->RegisterIlluminated(rsRenderNode);
    instance->PrepareLight();

    NodeId id = 1;
    RSRenderNode node(id);
    instance->AddDirtyIlluminated(node.weak_from_this());
    instance->PrepareLight();

    instance->AddDirtyLightSource(node.weak_from_this());
    instance->PrepareLight();
    EXPECT_TRUE(!instance->illuminatedNodeMap_.empty());
}

/**
 * @tc.name: PrepareLight002
 * @tc.desc: test results of PrepareLight
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, PrepareLight002, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> map;
    std::vector<std::weak_ptr<RSRenderNode>> dirtyList;
    instance->PrepareLight(map, dirtyList, true);
    EXPECT_TRUE(true);

    std::shared_ptr<RSRenderNode> sharedRenderNode = std::make_shared<RSRenderNode>(1);
    map[1] = sharedRenderNode;
    instance->PrepareLight(map, dirtyList, true);
    EXPECT_TRUE(true);

    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(1);
    dirtyList.push_back(renderNode);
    instance->PrepareLight(map, dirtyList, true);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: PrepareLight003
 * @tc.desc: test results of PrepareLight
 * @tc.type: FUNC
 * @tc.require: issueI9RBVH
 */
HWTEST_F(RSPointLightManagerTest, PrepareLight003, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    instance->illuminatedNodeMap_.clear();
    instance->PrepareLight();
    EXPECT_TRUE(instance->illuminatedNodeMap_.empty());

    std::shared_ptr<RSRenderNode> sharedRenderNode = std::make_shared<RSRenderNode>(0);
    instance->lightSourceNodeMap_[0] = sharedRenderNode;
    instance->PrepareLight();
    EXPECT_TRUE(!instance->lightSourceNodeMap_.empty());

    instance->illuminatedNodeMap_.clear();
    instance->lightSourceNodeMap_.clear();
    instance->lightSourceNodeMap_[0] = sharedRenderNode;
    instance->PrepareLight();
    EXPECT_TRUE(!instance->lightSourceNodeMap_.empty());

    instance->illuminatedNodeMap_.clear();
    instance->lightSourceNodeMap_.clear();
    instance->lightSourceNodeMap_[0] = sharedRenderNode;
    instance->illuminatedNodeMap_[0] = sharedRenderNode;
    instance->PrepareLight();
    EXPECT_TRUE(!instance->illuminatedNodeMap_.empty());

    instance->dirtyLightSourceList_.push_back(sharedRenderNode);
    instance->PrepareLight();
    EXPECT_TRUE(instance->dirtyLightSourceList_.empty());

    instance->dirtyIlluminatedList_.push_back(sharedRenderNode);
    sharedRenderNode->GetMutableRenderProperties().GetEffect().illuminatedPtr_ = std::make_shared<RSIlluminated>();
    instance->PrepareLight();
    EXPECT_TRUE(instance->dirtyIlluminatedList_.empty());

    sharedRenderNode = nullptr;
    instance->dirtyIlluminatedList_.clear();
    instance->dirtyIlluminatedList_.push_back(sharedRenderNode);
    instance->PrepareLight();
    EXPECT_TRUE(instance->dirtyIlluminatedList_.empty());
}

/**
 * @tc.name: PrepareLight004
 * @tc.desc: test results of PrepareLight
 * @tc.type: FUNC
 * @tc.require: issueI9RBVH
 */
HWTEST_F(RSPointLightManagerTest, PrepareLight004, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> map;
    std::vector<std::weak_ptr<RSRenderNode>> dirtyList;
    instance->PrepareLight(map, dirtyList, true);
    EXPECT_TRUE(map.empty());

    std::shared_ptr<RSRenderNode> sharedRenderNode = std::make_shared<RSRenderNode>(0);
    map[0] = sharedRenderNode;
    std::shared_ptr<RSRenderNode> renderNode = std::make_shared<RSRenderNode>(1);
    dirtyList.push_back(renderNode);
    instance->PrepareLight(map, dirtyList, true);
    EXPECT_TRUE(!map.empty());

    map.clear();
    dirtyList.clear();
    sharedRenderNode->isOnTheTree_ = true;
    map[0] = sharedRenderNode;
    renderNode->isOnTheTree_ = true;
    dirtyList.push_back(renderNode);
    instance->PrepareLight(map, dirtyList, true);
    EXPECT_TRUE(!map.empty());

    sharedRenderNode = nullptr;
    map.clear();
    dirtyList.clear();
    map[0] = sharedRenderNode;
    dirtyList.push_back(renderNode);
    instance->PrepareLight(map, dirtyList, true);
    EXPECT_TRUE(map.empty());

    sharedRenderNode = std::make_shared<RSRenderNode>(0);
    renderNode = nullptr;
    map.clear();
    dirtyList.clear();
    map[0] = sharedRenderNode;
    dirtyList.push_back(renderNode);
    instance->PrepareLight(map, dirtyList, true);
    EXPECT_TRUE(!map.empty());
}

/**
 * @tc.name: PrepareLight005
 * @tc.desc: test results of PrepareLight
 * @tc.type: FUNC
 * @tc.require: issueI9RBVH
 */
HWTEST_F(RSPointLightManagerTest, PrepareLight005, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>> illuminatedMap;
    std::vector<std::weak_ptr<RSRenderNode>> dirtyList;
    instance->PrepareLight();
    EXPECT_TRUE(illuminatedMap.empty());

    std::shared_ptr<RSRenderNode> illuminatedNode = std::make_shared<RSRenderNode>(0);
    illuminatedNode->GetMutableRenderProperties().GetEffect().illuminatedPtr_ = std::make_shared<RSIlluminated>();
    auto illuminatedPtr = illuminatedNode->GetMutableRenderProperties().GetIlluminated();
    EXPECT_TRUE(illuminatedPtr != nullptr);
    illuminatedPtr->lightSourcesAndPosMap_.emplace(
        std::make_shared<RSLightSource>(), Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    illuminatedMap[0] = illuminatedNode;
    std::shared_ptr<RSRenderNode> lightSourceNode = std::make_shared<RSRenderNode>(1);
    dirtyList.push_back(lightSourceNode);
    instance->PrepareLight();
    EXPECT_TRUE(instance->lastFrameIlluminatedNodeSet_.empty());
}

/**
 * @tc.name: CheckIlluminated001
 * @tc.desc: test results of CheckIlluminated
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, CheckIlluminated001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto lightSourceNode = std::make_shared<RSRenderNode>(1);
    auto illuminatedNode = std::make_shared<RSRenderNode>(1);
    instance->SetScreenRotation(ScreenRotation::ROTATION_0);
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(true);

    instance->SetScreenRotation(ScreenRotation::ROTATION_90);
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(true);

    instance->SetScreenRotation(ScreenRotation::ROTATION_0);
    illuminatedNode->GetMutableRenderProperties().SetBounds({0, 0, 10, 10});
    lightSourceNode->GetMutableRenderProperties().GetEffect().lightSourcePtr_ = std::make_shared<RSLightSource>();
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(true);
    instance->SetScreenRotation(ScreenRotation::ROTATION_90);
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(true);
    instance->SetScreenRotation(ScreenRotation::ROTATION_180);
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(true);
    instance->SetScreenRotation(ScreenRotation::ROTATION_270);
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: CheckIlluminated002
 * @tc.desc: test results of CheckIlluminated
 * @tc.type: FUNC
 * @tc.require: issueI9RBVH
 */
HWTEST_F(RSPointLightManagerTest, CheckIlluminated002, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto lightSourceNode = std::make_shared<RSRenderNode>(0);
    auto illuminatedNode = std::make_shared<RSRenderNode>(0);
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(lightSourceNode != nullptr);

    illuminatedNode->GetMutableRenderProperties().boundsGeo_->width_ = 1.f;
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(lightSourceNode != nullptr);

    illuminatedNode->GetMutableRenderProperties().boundsGeo_->height_ = 1.f;
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(lightSourceNode != nullptr);

    illuminatedNode->GetMutableRenderProperties().GetEffect().lightSourcePtr_ = std::make_shared<RSLightSource>();
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(lightSourceNode != nullptr);

    instance->screenRotation_ = ScreenRotation::ROTATION_0;
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(lightSourceNode != nullptr);

    instance->screenRotation_ = ScreenRotation::ROTATION_180;
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(lightSourceNode != nullptr);

    instance->screenRotation_ = ScreenRotation::ROTATION_90;
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(lightSourceNode != nullptr);

    instance->screenRotation_ = ScreenRotation::ROTATION_270;
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(lightSourceNode != nullptr);
}


/**
 * @tc.name: CheckIlluminated003
 * @tc.desc: test results of CheckIlluminated
 * @tc.type: FUNC
 * @tc.require: issueI9RBVH
 */
HWTEST_F(RSPointLightManagerTest, CheckIlluminated003, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto lightSourceNode = std::make_shared<RSRenderNode>(0);
    auto illuminatedNode = std::make_shared<RSRenderNode>(0);
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    
    illuminatedNode->instanceRootNodeId_ = 1;
    lightSourceNode->instanceRootNodeId_ = 0;
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_FALSE(illuminatedNode->IsDirty());

    illuminatedNode->instanceRootNodeId_ = 0;
    lightSourceNode->instanceRootNodeId_ = 0;
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_FALSE(illuminatedNode->IsDirty());

    lightSourceNode->GetMutableRenderProperties().GetEffect().lightSourcePtr_ = std::make_shared<RSLightSource>();
    illuminatedNode->GetMutableRenderProperties().GetEffect().illuminatedPtr_ = std::make_shared<RSIlluminated>();
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_FALSE(illuminatedNode->IsDirty());

    illuminatedNode->GetMutableRenderProperties().boundsGeo_->height_ = 1.f;
    illuminatedNode->GetMutableRenderProperties().boundsGeo_->width_ = 1.f;
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_FALSE(illuminatedNode->IsDirty());

    instance->lastFrameIlluminatedNodeSet_.insert(illuminatedNode->GetId());
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(illuminatedNode->IsDirty());

    illuminatedNode->GetMutableRenderProperties().boundsGeo_->absRect_ = RectI(0, 0, 100, 100);
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(illuminatedNode->IsDirty());
}

/**
 * @tc.name: ChildHasVisibleIlluminatedTest001
 * @tc.desc: test results of ChildHasVisibleIlluminated
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, ChildHasVisibleIlluminatedTest001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    constexpr NodeId nodeId = 1;
    auto node = std::make_shared<RSRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    EXPECT_FALSE(instance->GetChildHasVisibleIlluminated(nullptr));
    instance->SetChildHasVisibleIlluminated(nullptr, true);
    EXPECT_TRUE(instance->childHasVisbleIlluminatedNodeMap_.empty());
    node->InitRenderParams();
    instance->SetChildHasVisibleIlluminated(node, true);
    EXPECT_TRUE(instance->GetChildHasVisibleIlluminated(node));
    instance->SetChildHasVisibleIlluminated(node, false);
    EXPECT_FALSE(instance->GetChildHasVisibleIlluminated(node));
}
/**
 * @tc.name: CalculateLightPosForIlluminated001
 * @tc.desc: test results of CalculateLightPosForIlluminated
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, CalculateLightPosForIlluminated001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::shared_ptr<RSLightSource> lightSourcePtr = std::make_shared<RSLightSource>();
    lightSourcePtr->SetAbsLightPosition({ 20, 20, 20, 20 });

    std::shared_ptr<RSObjAbsGeometry> illuminatedGeoPtr = std::make_shared<RSObjAbsGeometry>();

    instance->SetScreenRotation(ScreenRotation::ROTATION_0);
    auto pos = instance->CalculateLightPosForIlluminated(*lightSourcePtr, illuminatedGeoPtr->GetAbsRect());
    EXPECT_EQ(pos.x_, 20);
    EXPECT_EQ(pos.y_, 20);

    instance->SetScreenRotation(ScreenRotation::ROTATION_90);
    pos = instance->CalculateLightPosForIlluminated(*lightSourcePtr, illuminatedGeoPtr->GetAbsRect());
    EXPECT_EQ(pos.x_, -20);
    EXPECT_EQ(pos.y_, 20);

    instance->SetScreenRotation(ScreenRotation::ROTATION_180);
    pos = instance->CalculateLightPosForIlluminated(*lightSourcePtr, illuminatedGeoPtr->GetAbsRect());
    EXPECT_EQ(pos.x_, -20);
    EXPECT_EQ(pos.y_, -20);

    instance->SetScreenRotation(ScreenRotation::ROTATION_270);
    pos = instance->CalculateLightPosForIlluminated(*lightSourcePtr, illuminatedGeoPtr->GetAbsRect());
    EXPECT_EQ(pos.x_, 20);
    EXPECT_EQ(pos.y_, -20);

    instance->SetScreenRotation(ScreenRotation::INVALID_SCREEN_ROTATION);
    pos = instance->CalculateLightPosForIlluminated(*lightSourcePtr, illuminatedGeoPtr->GetAbsRect());
    EXPECT_EQ(pos.x_, 0);
    EXPECT_EQ(pos.y_, 0);
}
} // namespace Rosen
} // namespace OHOS