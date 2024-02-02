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
 * @tc.name: PrepareLight001
 * @tc.desc: test
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
 * @tc.name: CalculateLightPosForIlluminated001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPointLightManagerTest, CalculateLightPosForIlluminated001, TestSize.Level1)
{
    auto instance = RSPointLightManager::Instance();
    std::shared_ptr<RSLightSource> lightSourcePtr = std::make_shared<RSLightSource>();
    lightSourcePtr->SetAbsLightPosition({ 20, 20, 20, 20 });

    std::shared_ptr<RSObjAbsGeometry> illuminatedGeoPtr = std::make_shared<RSObjAbsGeometry>();

    instance->CalculateLightPosForIlluminated(nullptr, illuminatedGeoPtr);

    instance->CalculateLightPosForIlluminated(lightSourcePtr, nullptr);

    instance->SetScreenRotation(ScreenRotation::ROTATION_0);
    auto pos = instance->CalculateLightPosForIlluminated(lightSourcePtr, illuminatedGeoPtr);

    instance->SetScreenRotation(ScreenRotation::ROTATION_90);
    pos = instance->CalculateLightPosForIlluminated(lightSourcePtr, illuminatedGeoPtr);

    instance->SetScreenRotation(ScreenRotation::ROTATION_180);
    pos = instance->CalculateLightPosForIlluminated(lightSourcePtr, illuminatedGeoPtr);

    instance->SetScreenRotation(ScreenRotation::ROTATION_270);
    pos = instance->CalculateLightPosForIlluminated(lightSourcePtr, illuminatedGeoPtr);
}
} // namespace Rosen
} // namespace OHOS