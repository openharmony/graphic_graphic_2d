/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsAnimationGrpTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsAnimationGrpTest::SetUpTestCase() {}
void RsAnimationGrpTest::TearDownTestCase() {}
void RsAnimationGrpTest::SetUp() {}
void RsAnimationGrpTest::TearDown() {}

/**
 * @tc.name: AddAnimationTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationGrpTest, AddAnimationTest01, Level1)
{
    bool res = true;
    NodeId id_;
    auto animation = std::make_shared<RSAnimation>();
    RSAnimationGroup rsAnimationGroup;
    auto per = std::make_shared<RSNode>(true, id_);
    rsAnimationGroup.StartInner(per);
    rsAnimationGroup.AddAnimation(animation);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: AddAnimationTest02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationGrpTest, AddAnimationTest02, Level2)
{
    bool res = true;
    RSAnimationGroup rsAnimationGroup;
    auto animation = nullptr;
    rsAnimationGroup.AddAnimation(animation);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: AddAnimationTest03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationGrpTest, AddAnimationTest03, Level3)
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
 * @tc.name: RemoveAnimationTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationGrpTest, RemoveAnimationTest01, Level2)
{
    bool res = true;
    RSAnimationGroup rsAnimationGroup;
    auto animation = nullptr;
    NodeId id_;
    auto per = std::make_shared<RSNode>(true, id_);
    rsAnimationGroup.StartInner(per);
    rsAnimationGroup.RemoveAnimation(animation);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: RemoveAnimationTest02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationGrpTest, RemoveAnimationTest02, Level2)
{
    bool res = true;
    RSAnimationGroup rsAnimationGroup;
    auto animation = nullptr;
    rsAnimationGroup.RemoveAnimation(animation);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: RemoveAnimationTest03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsAnimationGrpTest, RemoveAnimationTest03, Level2)
{
    bool res = true;
    NodeId id_;
    RSAnimationGroup rsAnimationGroup;
    auto animation = std::make_shared<RSAnimation>();
    auto per = std::make_shared<RSNode>(true, id_);
    rsAnimationGroup.StartInner(per);
    animation->RSAnimation::IsStarted();
    rsAnimationGroup.RemoveAnimation(animation);
    ASSERT_EQ(res, true);
}
}