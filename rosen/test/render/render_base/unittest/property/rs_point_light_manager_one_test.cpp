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
class RSPointLightManagerOneTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPointLightManagerOneTest::SetUpTestCase() {}
void RSPointLightManagerOneTest::TearDownTestCase() {}
void RSPointLightManagerOneTest::SetUp() {}
void RSPointLightManagerOneTest::TearDown() {}

/**
 * @tc.name: RegisterLightSourceTest001
 * @tc.desc: test results of RegisterLightSource
 * @tc.type:FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSPointLightManagerOneTest, RegisterLightSourceTest001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto renderNode = std::make_shared<RSRenderNode>(0);
    instance->RegisterLightSource(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: InstanceTest001
 * @tc.desc: test results of Instance
 * @tc.type:FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSPointLightManagerOneTest, InstanceTest001, TestSize.Level1)
{
    RSPointLightManager::Instance();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: UnRegisterLightSourceTest001
 * @tc.desc: test results of UnRegisterLightSource
 * @tc.type:FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSPointLightManagerOneTest, UnRegisterLightSourceTest001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto renderNode = std::make_shared<RSRenderNode>(1);
    instance->UnRegisterLightSource(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: RegisterIlluminatedTest001
 * @tc.desc: test results of RegisterIlluminated
 * @tc.type:FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSPointLightManagerOneTest, RegisterIlluminatedTest001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto renderNode = std::make_shared<RSRenderNode>(1);
    instance->RegisterIlluminated(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: AddDirtyLightSourceTest001
 * @tc.desc: test results of AddDirtyLightSource
 * @tc.type:FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSPointLightManagerOneTest, AddDirtyLightSourceTest001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::shared_ptr<RSRenderNode> sharedRenderNode = std::make_shared<RSRenderNode>(1);
    std::weak_ptr<RSRenderNode> renderNode = sharedRenderNode;
    instance->AddDirtyLightSource(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: UnRegisterIlluminatedTest001
 * @tc.desc: test results of UnRegisterIlluminated
 * @tc.type:FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSPointLightManagerOneTest, UnRegisterIlluminatedTest001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    auto renderNode = std::make_shared<RSRenderNode>(1);
    instance->UnRegisterIlluminated(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ClearDirtyListTest001
 * @tc.desc: test results of ClearDirtyList
 * @tc.type:FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSPointLightManagerOneTest, ClearDirtyListTest001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    instance->ClearDirtyList();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: AddDirtyIlluminatedTest001
 * @tc.desc: test results of AddDirtyIlluminated
 * @tc.type:FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSPointLightManagerOneTest, AddDirtyIlluminatedTest001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::shared_ptr<RSRenderNode> sharedRenderNode = std::make_shared<RSRenderNode>(1);
    std::weak_ptr<RSRenderNode> renderNode = sharedRenderNode;
    instance->AddDirtyIlluminated(renderNode);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: PrepareLightTest001
 * @tc.desc: test results of PrepareLight
 * @tc.type:FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSPointLightManagerOneTest, PrepareLightTest001, TestSize.Level1)
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
 * @tc.name: PrepareLightTest002
 * @tc.desc: test results of PrepareLight
 * @tc.type:FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSPointLightManagerOneTest, PrepareLightTest002, TestSize.Level1)
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
 * @tc.name: PrepareLightTest003
 * @tc.desc: test results of PrepareLight
 * @tc.type: FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSPointLightManagerOneTest, PrepareLightTest003, TestSize.Level1)
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
 * @tc.name: PrepareLightTest004
 * @tc.desc: test results of PrepareLight
 * @tc.type: FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSPointLightManagerOneTest, PrepareLightTest004, TestSize.Level1)
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
 * @tc.name: CheckIlluminatedTest001
 * @tc.desc: test results of CheckIlluminated
 * @tc.type:FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSPointLightManagerOneTest, CheckIlluminatedTest001, TestSize.Level1)
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
 * @tc.name: CheckIlluminatedTest002
 * @tc.desc: test results of CheckIlluminated
 * @tc.type: FUNC
 * @tc.require: issuesIB7RWT
 */
HWTEST_F(RSPointLightManagerOneTest, CheckIlluminatedTest002, TestSize.Level1)
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

} // namespace Rosen
} // namespace OHOS