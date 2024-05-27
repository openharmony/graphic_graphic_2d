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
#include "common/rs_background_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBackgroundThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBackgroundThreadTest::SetUpTestCase() {}
void RSBackgroundThreadTest::TearDownTestCase() {}
void RSBackgroundThreadTest::SetUp() {}
void RSBackgroundThreadTest::TearDown() {}

/**
 * @tc.name: PostTaskTest
 * @tc.desc: test results of PostTaskTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBackgroundThreadTest, PostTaskTest, TestSize.Level1)
{
    std::function<void()> func = []() -> void {};
    RSBackgroundThread::Instance().PostTask(func);
}

/**
 * @tc.name: PostSyncTaskTest
 * @tc.desc: Verify function PostSyncTask
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSBackgroundThreadTest, PostSyncTaskTest, TestSize.Level1)
{
    std::function<void()> func = []() -> void {};
    RSBackgroundThread::Instance().PostSyncTask(func);
    EXPECT_TRUE(RSBackgroundThread::Instance().handler_!=nullptr);
}
} // namespace OHOS::Rosen