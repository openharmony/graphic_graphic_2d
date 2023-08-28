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
#include "animation/rs_path_animation.h"
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPathAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPathAnimationTest::SetUpTestCase() {}
void RSPathAnimationTest::TearDownTestCase() {}
void RSPathAnimationTest::SetUp() {}
void RSPathAnimationTest::TearDown() {}

/**
 * @tc.name: SetEndFractionTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, SetEndFractionTest, Level1)
{
    bool res = true;
    auto prop = nullptr;
    auto animationPath = nullptr;
    float frac = 1.0f;
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.SetEndFraction(frac);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: SetPathNeedAddOriginTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, SetPathNeedAddOriginTest, Level1)
{
    bool res = true;
    bool needAddOrigin = true;
    auto prop = nullptr;
    auto animationPath = nullptr;
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.SetPathNeedAddOrigin(needAddOrigin);
    ASSERT_EQ(res, true);
}
}