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
#include "animation/rs_animation.h"
#include "animation/rs_implicit_animation_param.h"
#include "animation/rs_motion_path_option.h"
#include "modifier/rs_property.h"
#include "ui/rs_node.h"
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSImplicitAnimationParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSImplicitAnimationParamTest::SetUpTestCase() {}
void RSImplicitAnimationParamTest::TearDownTestCase() {}
void RSImplicitAnimationParamTest::SetUp() {}
void RSImplicitAnimationParamTest::TearDown() {}

/**
 * @tc.name: ApplyTimingProtocolTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimationParamTest, ApplyTimingProtocolTest, Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSImplicitAnimationParam rsImplicitAnimationParam(timingProtocol);
    auto animation = std::make_shared<RSAnimation>();
    rsImplicitAnimationParam.ApplyTimingProtocol(animation);
    ASSERT_NE(animation, nullptr);
}

/**
 * @tc.name: CreateAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSImplicitAnimationParamTest, CreateAnimationTest, Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSImplicitAnimationParam rsImplicitAnimationParam(timingProtocol);
    auto property = std::make_shared<RSPropertyBase>();
    auto startValue = std::make_shared<RSPropertyBase>();
    auto endValue = std::make_shared<RSPropertyBase>();
    rsImplicitAnimationParam.CreateAnimation(property, startValue, endValue);
    ASSERT_NE(startValue, nullptr);
}
}