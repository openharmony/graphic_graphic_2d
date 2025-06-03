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
#include "animation/rs_animation_group.h"
#include "ui/rs_node.h"
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAnimationGroupTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAnimationGroupTest::SetUpTestCase() {}
void RSAnimationGroupTest::TearDownTestCase() {}
void RSAnimationGroupTest::SetUp() {}
void RSAnimationGroupTest::TearDown() {}

/**
 * @tc.name: AddAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationGroupTest, AddAnimationTest001, Level1)
{
    int res = 1;
    auto animation = std::make_shared<RSAnimation>();
    RSAnimationGroup rsAnimationGroup;
    NodeId id_;
    auto per = std::make_shared<RSNode>(true, id_);
    rsAnimationGroup.StartInner(per);
    rsAnimationGroup.AddAnimation(animation);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: AddAnimationTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationGroupTest, AddAnimationTest002, Level2)
{
    bool res = true;
    auto animation = nullptr;
    RSAnimationGroup rsAnimationGroup;
    rsAnimationGroup.AddAnimation(animation);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: AddAnimationTest003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationGroupTest, AddAnimationTest003, Level3)
{
    bool res = true;
    RSAnimationGroup rsAnimationGroup;
    NodeId id_;
    auto animation = std::make_shared<RSAnimation>();
    auto per = std::make_shared<RSNode>(true, id_);
    rsAnimationGroup.StartInner(per);
    animation->RSAnimation::IsStarted();
    rsAnimationGroup.AddAnimation(animation);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: RemoveAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationGroupTest, RemoveAnimationTest001, Level1)
{
    int res = 1;
    auto animation = nullptr;
    RSAnimationGroup rsAnimationGroup;
    NodeId id_;
    auto per = std::make_shared<RSNode>(true, id_);
    rsAnimationGroup.StartInner(per);
    rsAnimationGroup.RemoveAnimation(animation);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: RemoveAnimationTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationGroupTest, RemoveAnimationTest002, Level2)
{
    bool res = true;
    auto animation = nullptr;
    RSAnimationGroup rsAnimationGroup;
    rsAnimationGroup.RemoveAnimation(animation);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: RemoveAnimationTest003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationGroupTest, RemoveAnimationTest003, Level3)
{
    bool res = true;
    RSAnimationGroup rsAnimationGroup;
    NodeId id_;
    auto animation = std::make_shared<RSAnimation>();
    auto per = std::make_shared<RSNode>(true, id_);
    rsAnimationGroup.StartInner(per);
    animation->RSAnimation::IsStarted();
    rsAnimationGroup.RemoveAnimation(animation);
    ASSERT_EQ(res, true);
}
}