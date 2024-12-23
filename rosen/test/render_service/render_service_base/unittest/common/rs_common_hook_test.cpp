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

#include "common/rs_common_hook.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsCommonHookTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsCommonHookTest::SetUpTestCase() {}
void RsCommonHookTest::TearDownTestCase() {}
void RsCommonHookTest::SetUp() {}
void RsCommonHookTest::TearDown() {}

/**
 * @tc.name: RegisterStartNewAnimationListenerTest
 * @tc.desc: test results of RegisterStartNewAnimationListenerTest
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(RsCommonHookTest, RegisterStartNewAnimationListenerTest, TestSize.Level1)
{
    auto callback = []() {};
    RsCommonHook::Instance().RegisterStartNewAnimationListener(callback);
    ASSERT_NE(RsCommonHook::Instance().startNewAniamtionFunc_, nullptr);
}

/**
 * @tc.name: OnStartNewAnimationTest1
 * @tc.desc: test results of OnStartNewAnimationTest1
 * @tc.type:FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(RsCommonHookTest, OnStartNewAnimationTest1, TestSize.Level1)
{
    auto callback = []() {};
    RsCommonHook::Instance().RegisterStartNewAnimationListener(callback);
    RsCommonHook::Instance().OnStartNewAnimation();
    ASSERT_EQ(RsCommonHook::Instance().startNewAniamtionFunc_, nullptr);
}

/**
 * @tc.name: OnStartNewAnimationTest2
 * @tc.desc: test results of OnStartNewAnimationTest2
 * @tc.type:FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(RsCommonHookTest, OnStartNewAnimationTest2, TestSize.Level1)
{
    RsCommonHook::Instance().RegisterStartNewAnimationListener(nullptr);
    RsCommonHook::Instance().OnStartNewAnimation();
    ASSERT_EQ(RsCommonHook::Instance().startNewAniamtionFunc_, nullptr);
}
} // namespace OHOS::Rosen