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

#include "common/rs_thread_handler.h"

#include "common/rs_thread_handler_generic.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSThreadHandlerGenericTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSThreadHandlerGenericTest::SetUpTestCase() {}
void RSThreadHandlerGenericTest::TearDownTestCase() {}
void RSThreadHandlerGenericTest::SetUp() {}
void RSThreadHandlerGenericTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc: test results of Create
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSThreadHandlerGenericTest, Create001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create
     */
    auto rsThreadHandlerGeneric = RSThreadHandler::Create();
    ASSERT_NE(rsThreadHandlerGeneric, nullptr);
}

/**
 * @tc.name: CreateTaskTest
 * @tc.desc: Verify function CreateTask
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSThreadHandlerGenericTest, CreateTaskTest, TestSize.Level1)
{
    RSTaskMessage::RSTask task = []() -> void {};
    auto rsThreadHandlerGeneric = std::make_shared<RSThreadHandlerGeneric>();
    EXPECT_TRUE(rsThreadHandlerGeneric->CreateTask(task) != nullptr);
}

/**
 * @tc.name: PostTaskTest
 * @tc.desc: Verify function PostTask
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSThreadHandlerGenericTest, PostTaskTest, TestSize.Level1)
{
    RSTaskMessage::RSTask task = []() -> void {};
    auto generic = std::make_shared<RSThreadHandlerGeneric>();
    RSTaskHandle taskHandle = generic->CreateTask(task);
    generic->PostTask(taskHandle, 1);
    ThreadLooperImpl::CreateThreadInstance();
    auto rsThreadHandlerGeneric = std::make_shared<RSThreadHandlerGeneric>();
    RSTaskHandle taskHandle2 = rsThreadHandlerGeneric->CreateTask(task);
    rsThreadHandlerGeneric->PostTask(taskHandle2, 1);
    EXPECT_TRUE(rsThreadHandlerGeneric->looper_ != nullptr);
}

/**
 * @tc.name: PostTaskDelayTest
 * @tc.desc: Verify function PostTaskDelay
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSThreadHandlerGenericTest, PostTaskDelayTest, TestSize.Level1)
{
    RSTaskMessage::RSTask task = []() -> void {};
    auto generic = std::make_shared<RSThreadHandlerGeneric>();
    RSTaskHandle taskHandle = generic->CreateTask(task);
    generic->PostTaskDelay(taskHandle, 1, 1);
    ThreadLooperImpl::CreateThreadInstance();
    auto rsThreadHandlerGeneric = std::make_shared<RSThreadHandlerGeneric>();
    RSTaskHandle taskHandle2 = rsThreadHandlerGeneric->CreateTask(task);
    rsThreadHandlerGeneric->PostTaskDelay(taskHandle2, 1, 1);
    EXPECT_TRUE(rsThreadHandlerGeneric->looper_ != nullptr);
}

/**
 * @tc.name: CancelTaskTest
 * @tc.desc: Verify function CancelTask
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSThreadHandlerGenericTest, CancelTaskTest, TestSize.Level1)
{
    RSTaskMessage::RSTask task = []() -> void {};
    auto generic = std::make_shared<RSThreadHandlerGeneric>();
    RSTaskHandle taskHandle = generic->CreateTask(task);
    generic->CancelTask(taskHandle);
    ThreadLooperImpl::CreateThreadInstance();
    auto rsThreadHandlerGeneric = std::make_shared<RSThreadHandlerGeneric>();
    RSTaskHandle taskHandle2 = rsThreadHandlerGeneric->CreateTask(task);
    rsThreadHandlerGeneric->CancelTask(taskHandle2);
    EXPECT_TRUE(rsThreadHandlerGeneric->looper_ != nullptr);
}

/**
 * @tc.name: StaticCreateTaskTest
 * @tc.desc: Verify function StaticCreateTask
 * @tc.type: FUNC
 * @tc.require: issuesI9OX7J
 */
HWTEST_F(RSThreadHandlerGenericTest, StaticCreateTaskTest, TestSize.Level1)
{
    RSTaskMessage::RSTask task = []() -> void {};
    auto rsThreadHandlerGeneric = std::make_shared<RSThreadHandlerGeneric>();
    EXPECT_TRUE(rsThreadHandlerGeneric->StaticCreateTask(task) != nullptr);
}
} // namespace OHOS::Rosen