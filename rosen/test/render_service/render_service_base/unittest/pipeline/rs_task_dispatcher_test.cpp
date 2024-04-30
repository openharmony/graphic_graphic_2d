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
#include "pipeline/rs_task_dispatcher.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSTaskDispatcherTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTaskDispatcherTest::SetUpTestCase() {}
void RSTaskDispatcherTest::TearDownTestCase() {}
void RSTaskDispatcherTest::SetUp() {}
void RSTaskDispatcherTest::TearDown() {}

/**
 * @tc.name: TestRegisterTaskDispatchFunc001
 * @tc.desc: RegisterTaskDispatchFunc test.
 * @tc.type: FUNC
 * @tc.require: issuesI9J766
 */
HWTEST_F(RSTaskDispatcherTest, TestRegisterTaskDispatchFunc001, TestSize.Level1)
{
    RSTaskDispatcher& dispatcher = RSTaskDispatcher::GetInstance();
    dispatcher.RegisterTaskDispatchFunc(1, [](RSTaskDispatcher::RSTask task, bool isSyncTask) {});
    EXPECT_TRUE(dispatcher.HasRegisteredTask(1));
}

/**
 * @tc.name: TestPostTask002
 * @tc.desc: PostTask test.
 * @tc.type: FUNC
 * @tc.require: issuesI9J766
 */
HWTEST_F(RSTaskDispatcherTest, TestPostTask002, TestSize.Level1)
{
    RSTaskDispatcher& dispatcher = RSTaskDispatcher::GetInstance();
    dispatcher.RegisterTaskDispatchFunc(1, [](RSTaskDispatcher::RSTask task, bool isSyncTask) {});
    dispatcher.PostTask(1, []() {});
    dispatcher.PostTask(0, []() {});
}

/**
 * @tc.name: TestHasRegisteredTask003
 * @tc.desc: HasRegisteredTask test.
 * @tc.type: FUNC
 * @tc.require: issuesI9J766
 */
HWTEST_F(RSTaskDispatcherTest, TestHasRegisteredTask003, TestSize.Level1)
{
    RSTaskDispatcher& dispatcher = RSTaskDispatcher::GetInstance();
    dispatcher.RegisterTaskDispatchFunc(1, [](RSTaskDispatcher::RSTask task, bool isSyncTask) {});

    EXPECT_TRUE(dispatcher.HasRegisteredTask(1));
    EXPECT_FALSE(dispatcher.HasRegisteredTask(0));
}
} // namespace OHOS::Rosen
