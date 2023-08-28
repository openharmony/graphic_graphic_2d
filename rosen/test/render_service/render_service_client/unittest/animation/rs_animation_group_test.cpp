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
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
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
 * @tc.name: AddAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationGroupTest, AddAnimationTest, Level1)
{
    bool res = true;
    auto animation = nullptr;
    RSAnimationGroup rsAnimationGroup;
    rsAnimationGroup.AddAnimation(animation);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: RemoveAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAnimationGroupTest, RemoveAnimationTest, Level1)
{
    bool res = true;
    auto animation = nullptr;
    RSAnimationGroup rsAnimationGroup;
    rsAnimationGroup.RemoveAnimation(animation);
    ASSERT_EQ(res, true);
}
}