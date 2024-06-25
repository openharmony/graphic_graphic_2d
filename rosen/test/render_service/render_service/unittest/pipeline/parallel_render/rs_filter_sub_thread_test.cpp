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

#include "pipeline/parallel_render/rs_filter_sub_thread.h"
#include "render/rs_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsFilterSubThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsFilterSubThreadTest::SetUpTestCase() {}
void RsFilterSubThreadTest::TearDownTestCase() {}
void RsFilterSubThreadTest::SetUp() {}
void RsFilterSubThreadTest::TearDown() {}

/**
 * @tc.name: PostTaskTest
 * @tc.desc: Test RsFilterSubThreadTest.PostTaskTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsFilterSubThreadTest, PostTaskTest, TestSize.Level1)
{
    auto curThread = std::make_shared<RSFilterSubThread>(nullptr);
    ASSERT_TRUE(curThread != nullptr);
    curThread->PostTask([] {});
    usleep(1000 * 1000); // 1000 * 1000us
}

/**
 * @tc.name: PostSyncTaskTest
 * @tc.desc: Test RsFilterSubThreadTest.PostSyncTaskTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsFilterSubThreadTest, PostSyncTaskTest, TestSize.Level1)
{
    auto curThread = std::make_shared<RSFilterSubThread>(nullptr);
    ASSERT_TRUE(curThread != nullptr);
    curThread->PostSyncTask([] {});
    usleep(1000 * 1000); // 1000 * 1000us
}

/**
 * @tc.name: ResetGrContextTest
 * @tc.desc: Test RsFilterSubThreadTest.ResetGrContextTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsFilterSubThreadTest, ResetGrContextTest, TestSize.Level1)
{
    auto curThread = std::make_shared<RSFilterSubThread>(nullptr);
    ASSERT_TRUE(curThread != nullptr);
    curThread->ResetGrContext();
    usleep(1000 * 1000); // 1000 * 1000us
}

/**
 * @tc.name: GetAppGpuMemoryInMBTest
 * @tc.desc: Test RsFilterSubThreadTest.GetAppGpuMemoryInMBTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsFilterSubThreadTest, GetAppGpuMemoryInMBTest, TestSize.Level1)
{
    auto curThread = std::make_shared<RSFilterSubThread>(nullptr);
    ASSERT_TRUE(curThread != nullptr);
    curThread->GetAppGpuMemoryInMB();
    usleep(1000 * 1000); // 1000 * 1000us
}
} // namespace OHOS::Rosen
