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

#include "common/rs_switching_thread.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSwitchingThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSwitchingThreadTest::SetUpTestCase() {}
void RSSwitchingThreadTest::TearDownTestCase() {}
void RSSwitchingThreadTest::SetUp() {}
void RSSwitchingThreadTest::TearDown() {}
 
/**
 * @tc.name: PostTaskTest
 * @tc.desc: test results of PostTaskTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSwitchingThreadTest, PostTaskTest, TestSize.Level1)
{
    std::function<void()> func = []() -> void {};
    RSSwitchingThread::Instance().PostTask(func);
    EXPECT_NE(RSSwitchingThread::Instance().handler_, nullptr);
}

/**
 * @tc.name: PostSyncTaskTest
 * @tc.desc: Verify function PostSyncTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSwitchingThreadTest, PostSyncTaskTest, TestSize.Level1)
{
    std::function<void()> func = []() -> void {};
    RSSwitchingThread::Instance().PostSyncTask(func);
    EXPECT_TRUE(RSSwitchingThread::Instance().handler_!=nullptr);
}
} // namespace OHOS::Rosen