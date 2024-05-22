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
	
    RSObjGeometry rsObjGeometry;
    rsObjGeometry.SetWidth(-0.1f);
    rsObjGeometry.SetHeight(-0.1f);
    instance->CheckIlluminated(lightSourceNode, illuminatedNode);
    EXPECT_TRUE(true);
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

    instance->SetScreenRotation(ScreenRotation::ROTATION_90);
    pos = instance->CalculateLightPosForIlluminated(*lightSourcePtr, illuminatedGeoPtr->GetAbsRect());

    instance->SetScreenRotation(ScreenRotation::ROTATION_180);
    pos = instance->CalculateLightPosForIlluminated(*lightSourcePtr, illuminatedGeoPtr->GetAbsRect());

    instance->SetScreenRotation(ScreenRotation::ROTATION_270);
    pos = instance->CalculateLightPosForIlluminated(*lightSourcePtr, illuminatedGeoPtr->GetAbsRect());
}
} // namespace Rosen
} // namespace OHOS