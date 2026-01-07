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
#include <memory>

#include "gtest/gtest.h"
#include "pipeline/main_thread/rs_main_thread.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSMainThreadProfilerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMainThreadProfilerTest::SetUpTestCase() {}
void RSMainThreadProfilerTest::TearDownTestCase() {}
void RSMainThreadProfilerTest::SetUp() {}
void RSMainThreadProfilerTest::TearDown() {}

/**
 * @tc.name: TransitionDataMutexLockIfNoCommandsTest
 * @tc.desc: Test TransitionDataMutexLockIfNoCommands
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMainThreadProfilerTest, TransitionDataMutexLockIfNoCommandsTest, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    mainThread->cachedTransactionDataMap_.clear();
    mainThread->effectiveTransactionDataIndexMap_.clear();
    ASSERT_TRUE(mainThread->TransitionDataMutexLockIfNoCommands());
    mainThread->TransitionDataMutexUnlock();

    mainThread->cachedTransactionDataMap_.emplace(0, std::vector<std::unique_ptr<RSTransactionData>>());
    auto pair = std::make_pair(1, std::vector<std::unique_ptr<RSTransactionData>>());
    mainThread->effectiveTransactionDataIndexMap_.emplace(0, std::move(pair));
    ASSERT_TRUE(mainThread->TransitionDataMutexLockIfNoCommands());
    mainThread->TransitionDataMutexUnlock();

    mainThread->cachedTransactionDataMap_.clear();
    mainThread->effectiveTransactionDataIndexMap_.clear();
    std::vector<std::unique_ptr<RSTransactionData>> cachedVec;
    cachedVec.emplace_back(std::make_unique<RSTransactionData>());
    mainThread->cachedTransactionDataMap_.emplace(0, std::move(cachedVec));
    ASSERT_FALSE(mainThread->TransitionDataMutexLockIfNoCommands());

    mainThread->cachedTransactionDataMap_.clear();
    mainThread->effectiveTransactionDataIndexMap_.clear();
    std::vector<std::unique_ptr<RSTransactionData>> effectivedVec;
    effectivedVec.emplace_back(std::make_unique<RSTransactionData>());
    auto effectivedPair = std::make_pair(1, std::move(effectivedVec));
    mainThread->effectiveTransactionDataIndexMap_.emplace(0, std::move(effectivedPair));
    ASSERT_FALSE(mainThread->TransitionDataMutexLockIfNoCommands());
}
} // namespace OHOS::Rosen
