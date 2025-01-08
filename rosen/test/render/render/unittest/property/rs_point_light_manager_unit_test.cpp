/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
using namespace testing::Text;

namespace OHOS {
namespace Rosen {
class RSPointLightManagerUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPointLightManagerUnitTest::SetUpTestCase() {}
void RSPointLightManagerUnitTest::TearDownTestCase() {}
void RSPointLightManagerUnitTest::SetUp() {}
void RSPointLightManagerUnitTest::TearDown() {}

/**
 * @tc.name: TestInstance01
 * @tc.desc: verify function Instance
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestInstance01, TestSize.Level1)
{
    RSPointLightManager::Instance();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: TestRegisterLightSource01
 * @tc.desc: verify function RegisterLightSource
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestRegisterLightSource01, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto renderNode = std::make_shared<RSRenderNode>(0);
    instance->RegisterLightSource(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: TestRegisterIlluminated01
 * @tc.desc: verify function RegisterIlluminated
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestRegisterIlluminated01, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto renderNode = std::make_shared<RSRenderNode>(1);
    instance->RegisterIlluminated(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: TestUnRegisterLightSource01
 * @tc.desc: verify function UnRegisterLightSource
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestUnRegisterLightSource01, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto renderNode = std::make_shared<RSRenderNode>(1);
    instance->UnRegisterLightSource(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: TestUnRegisterIlluminated01
 * @tc.desc: verify function UnRegisterIlluminated
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestUnRegisterIlluminated01, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto renderNode = std::make_shared<RSRenderNode>(1);
    instance->UnRegisterIlluminated(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: TestAddDirtyLightSource01
 * @tc.desc: verify function AddDirtyLightSource
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestAddDirtyLightSource01, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::shared_ptr<RSRenderNode> sharedRenderNode = std::make_shared<RSRenderNode>(1);
    std::weak_ptr<RSRenderNode> renderNode = sharedRenderNode;
    instance->AddDirtyLightSource(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: TestAddDirtyIlluminated01
 * @tc.desc: verify function AddDirtyIlluminated
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestAddDirtyIlluminated01, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::shared_ptr<RSRenderNode> sharedRenderNode = std::make_shared<RSRenderNode>(1);
    std::weak_ptr<RSRenderNode> renderNode = sharedRenderNode;
    instance->AddDirtyIlluminated(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: TestClearDirtyList01
 * @tc.desc: verify function ClearDirtyList
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestClearDirtyList01, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    instance->ClearDirtyList();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: TestPrepareLight01
 * @tc.desc: verify function PrepareLight
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestPrepareLight01, TestSize.Level1)
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
 * @tc.name: TestPrepareLight02
 * @tc.desc: verify function PrepareLight
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestPrepareLight02, TestSize.Level1)
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
 * @tc.name: TestPrepareLight03
 * @tc.desc: verify function PrepareLight
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestPrepareLight03, TestSize.Level1)
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
    sharedRenderNode->GetMutableRenderProperties().illuminatedPtr_ = std::make_shared<RSIlluminated>();
    instance->PrepareLight();
    EXPECT_TRUE(instance->dirtyIlluminatedList_.empty());

    sharedRenderNode = nullptr;
    instance->dirtyIlluminatedList_.clear();
    instance->dirtyIlluminatedList_.push_back(sharedRenderNode);
    instance->PrepareLight();
    EXPECT_TRUE(instance->dirtyIlluminatedList_.empty());
}

/**
 * @tc.name: TestPrepareLight04
 * @tc.desc: verify function PrepareLight
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestPrepareLight04, TestSize.Level1)
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
 * @tc.name: TestCheckIlluminated01
 * @tc.desc: verify function CheckIlluminated
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestCheckIlluminated01, TestSize.Level1)
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
    lightSourceNode->GetMutableRenderProperties().lightSourcePtr_ = std::make_shared<RSLightSource>();
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
 * @tc.name: TestCheckIlluminated02
 * @tc.desc: verify function CheckIlluminated
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestCheckIlluminated02, TestSize.Level1)
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

    illuminatedNode->GetMutableRenderProperties().lightSourcePtr_ = std::make_shared<RSLightSource>();
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
 * @tc.name: TestCalculateLightPosForIlluminated01
 * @tc.desc: verify function CalculateLightPosForIlluminated
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerUnitTest, TestCalculateLightPosForIlluminated01, TestSize.Level1)
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