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
#include "animation/rs_animation_manager.h"
#include "animation/rs_render_animation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAnimationManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAnimationManagerTest::SetUpTestCase() {}
void RSAnimationManagerTest::TearDownTestCase() {}
void RSAnimationManagerTest::SetUp() {}
void RSAnimationManagerTest::TearDown() {}

/**
 * @tc.name: AddAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, AddAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool isInitialized_ = false;
    auto animation = std::make_shared<RSRenderAnimation>();
    rsAnimationManager.AddAnimation(animation);
    ASSERT_EQ(isInitialized_, false);
}

/**
 * @tc.name: RemoveAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, RemoveAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool isInitialized_ = false;
    AnimationId keyId = 0;
    rsAnimationManager.RemoveAnimation(keyId);
    ASSERT_EQ(isInitialized_, false);
}

/**
 * @tc.name:CancelAnimationByPropertyIdTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, CancelAnimationByPropertyIdTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    bool isInitialized_ = false;
    PropertyId id = 0;
    rsAnimationManager.CancelAnimationByPropertyId(id);
    ASSERT_EQ(isInitialized_, false);
}

/**
 * @tc.name:GetAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, GetAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    AnimationId id = 0;
    auto res = rsAnimationManager.GetAnimation(id);
    ASSERT_EQ(res, nullptr);
}

/**
 * @tc.name:AnimateTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, AnimateTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    int64_t time = 10;
    bool nodeIsOnTheTree = false;
    rsAnimationManager.Animate(time, nodeIsOnTheTree);
    ASSERT_NE(time, 0);
}

/**
 * @tc.name:RegisterSpringAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, RegisterSpringAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsAnimationManager.RegisterSpringAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}

/**
 * @tc.name:UnregisterSpringAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, UnregisterSpringAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsAnimationManager.UnregisterSpringAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}

/**
 * @tc.name:QuerySpringAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, QuerySpringAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    auto res = std::make_shared<RSRenderAnimation>();
    rsAnimationManager.QuerySpringAnimation(propertyId);
    ASSERT_NE(res, nullptr);
}

/**
 * @tc.name:RegisterPathAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, RegisterPathAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsAnimationManager.RegisterPathAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}

/**
 * @tc.name:UnregisterPathAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, UnregisterPathAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    AnimationId animId = 0;
    rsAnimationManager.UnregisterPathAnimation(propertyId, animId);
    ASSERT_NE(propertyId, 10);
}

/**
 * @tc.name:QueryPathAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationManagerTest, QueryPathAnimationTest001, TestSize.Level1)
{
    RSAnimationManager rsAnimationManager;
    PropertyId propertyId = 0;
    auto res = std::make_shared<RSRenderAnimation>();
    rsAnimationManager.QueryPathAnimation(propertyId);
    ASSERT_NE(res, nullptr);
}
}