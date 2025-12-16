/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "platform/ohos/rs_jank_report_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSJankReportThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSJankReportThreadTest::SetUpTestCase() {}
void RSJankReportThreadTest::TearDownTestCase() {}
void RSJankReportThreadTest::SetUp() {}
void RSJankReportThreadTest::TearDown() {}

/**
 * @tc.name: PostTaskTest
 * @tc.desc: test results of PostTaskTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSJankReportThreadTest, PostTaskTest, TestSize.Level1)
{
    std::function<void()> func = []() -> void {};
    RSJankReportThread::Instance().PostTask(func);
    EXPECT_TRUE(RSJankReportThread::Instance().handler_ != nullptr);
}

/**
 * @tc.name: PostSyncTaskTest001
 * @tc.desc: Verify function PostSyncTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSJankReportThreadTest, PostSyncTaskTest001, TestSize.Level1)
{
    std::function<void()> func = []() -> void {};
    RSJankReportThread::Instance().PostSyncTask(func);
    EXPECT_TRUE(RSJankReportThread::Instance().handler_ != nullptr);
}

/**
 * @tc.name: PostSyncTaskTest002
 * @tc.desc: Verify function PostSyncTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSJankReportThreadTest, PostSyncTaskTest002, TestSize.Level1)
{
    std::function<void()> func = []() -> void {};
    RSJankReportThread& thread = RSJankReportThread::Instance();
    thread.handler_ = nullptr;
    thread.PostTask(func);
    thread.PostSyncTask(func);
    EXPECT_TRUE(RSJankReportThread::Instance().handler_ == nullptr);
}
} // namespace OHOS::Rosen