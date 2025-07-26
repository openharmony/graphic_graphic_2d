/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>
#include "vsync_system_ability_listener.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class VSyncSystemAbilityListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline sptr<VSyncSystemAbilityListener> vsyncAbilityListener_ = nullptr;
};

void VSyncSystemAbilityListenerTest::SetUpTestCase()
{
    vsyncAbilityListener_ = new VSyncSystemAbilityListener("testThread", "testUid", "testPid", "testTid");
}

void VSyncSystemAbilityListenerTest::TearDownTestCase()
{
    vsyncAbilityListener_ = nullptr;
}

/**
 * @tc.name: OnRemoveSystemAbility001
 * @tc.desc: OnRemoveSystemAbility Test , make sure that the deviceId has not changed.
 * @tc.type: FUNC
 * @tc.require: issueICO7O7
 */
HWTEST_F(VSyncSystemAbilityListenerTest, OnRemoveSystemAbility001, Function | MediumTest | Level2)
{
    std::string deviceId("testDevice");
    vsyncAbilityListener_->OnRemoveSystemAbility(0, deviceId);
    ASSERT_EQ(deviceId.compare("testDevice"), true);
}

/**
 * @tc.name: OnAddSystemAbility001
 * @tc.desc: OnAddSystemAbility Test , make sure that the deviceId has not changed.
 * @tc.type: FUNC
 * @tc.require: issueICO7O7
 */
HWTEST_F(VSyncSystemAbilityListenerTest, OnAddSystemAbility001, Function | MediumTest | Level2)
{
    std::string deviceId("testDevice");
    vsyncAbilityListener_->OnAddSystemAbility(0, deviceId);
    ASSERT_EQ(deviceId.compare("testDevice"), true);
}
}
