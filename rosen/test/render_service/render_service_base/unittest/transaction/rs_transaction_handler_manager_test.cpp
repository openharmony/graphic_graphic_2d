/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "transaction/rs_transaction_handler_manager.h"
#include "transaction/rs_transaction_handler.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSTransactionHandlerManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t TEST_TOKEN = 12345;
    static constexpr uint64_t TEST_TOKEN2 = 67890;
};

void RSTransactionHandlerManagerTest::SetUpTestCase() {}
void RSTransactionHandlerManagerTest::TearDownTestCase() {}
void RSTransactionHandlerManagerTest::SetUp() {}
void RSTransactionHandlerManagerTest::TearDown()
{
    auto& manager = RSTransactionHandlerManager::Instance();
    manager.Unregister(TEST_TOKEN);
    manager.Unregister(TEST_TOKEN2);
}

/**
 * @tc.name: RegisterWithNullHandler001
 * @tc.desc: Register with nullptr handler should not crash and not register
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionHandlerManagerTest, RegisterWithNullHandler001, TestSize.Level1)
{
    auto& manager = RSTransactionHandlerManager::Instance();
    manager.Register(TEST_TOKEN, nullptr);
    auto result = manager.Get(TEST_TOKEN);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: RegisterAndGet001
 * @tc.desc: Register a valid handler and retrieve it by token
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionHandlerManagerTest, RegisterAndGet001, TestSize.Level1)
{
    auto& manager = RSTransactionHandlerManager::Instance();
    auto handler = std::make_shared<RSTransactionHandler>();
    ASSERT_NE(handler, nullptr);

    manager.Register(TEST_TOKEN, handler);
    auto result = manager.Get(TEST_TOKEN);
    EXPECT_EQ(result, handler);
}

/**
 * @tc.name: GetWithUnregisteredToken001
 * @tc.desc: Get with unregistered token should return nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionHandlerManagerTest, GetWithUnregisteredToken001, TestSize.Level1)
{
    auto& manager = RSTransactionHandlerManager::Instance();
    auto result = manager.Get(TEST_TOKEN);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetAnyWithEmptyManager001
 * @tc.desc: GetAny with empty manager should return nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionHandlerManagerTest, GetAnyWithEmptyManager001, TestSize.Level1)
{
    auto& manager = RSTransactionHandlerManager::Instance();
    auto result = manager.GetAny();
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetAnyWithNonEmptyManager001
 * @tc.desc: GetAny with non-empty manager should return first handler
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionHandlerManagerTest, GetAnyWithNonEmptyManager001, TestSize.Level1)
{
    auto& manager = RSTransactionHandlerManager::Instance();
    auto handler = std::make_shared<RSTransactionHandler>();
    ASSERT_NE(handler, nullptr);

    manager.Register(TEST_TOKEN, handler);
    auto result = manager.GetAny();
    EXPECT_EQ(result, handler);
}

/**
 * @tc.name: Unregister001
 * @tc.desc: Unregister should remove handler from manager
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionHandlerManagerTest, Unregister001, TestSize.Level1)
{
    auto& manager = RSTransactionHandlerManager::Instance();
    auto handler = std::make_shared<RSTransactionHandler>();
    ASSERT_NE(handler, nullptr);

    manager.Register(TEST_TOKEN, handler);
    EXPECT_EQ(manager.Get(TEST_TOKEN), handler);

    manager.Unregister(TEST_TOKEN);
    EXPECT_EQ(manager.Get(TEST_TOKEN), nullptr);
}

/**
 * @tc.name: UnregisterNonExistingToken001
 * @tc.desc: Unregister non-existing token should not crash
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionHandlerManagerTest, UnregisterNonExistingToken001, TestSize.Level1)
{
    auto& manager = RSTransactionHandlerManager::Instance();
    manager.Unregister(TEST_TOKEN);
    EXPECT_EQ(manager.Get(TEST_TOKEN), nullptr);
}

/**
 * @tc.name: MultipleHandlers001
 * @tc.desc: Register multiple handlers and get correct one by token
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionHandlerManagerTest, MultipleHandlers001, TestSize.Level1)
{
    auto& manager = RSTransactionHandlerManager::Instance();
    auto handler1 = std::make_shared<RSTransactionHandler>();
    auto handler2 = std::make_shared<RSTransactionHandler>();
    ASSERT_NE(handler1, nullptr);
    ASSERT_NE(handler2, nullptr);

    manager.Register(TEST_TOKEN, handler1);
    manager.Register(TEST_TOKEN2, handler2);

    EXPECT_EQ(manager.Get(TEST_TOKEN), handler1);
    EXPECT_EQ(manager.Get(TEST_TOKEN2), handler2);

    auto anyHandler = manager.GetAny();
    EXPECT_NE(anyHandler, nullptr);
}

/**
 * @tc.name: RegisterOverwrite001
 * @tc.desc: Register with same token should overwrite existing handler
 * @tc.type: FUNC
 */
HWTEST_F(RSTransactionHandlerManagerTest, RegisterOverwrite001, TestSize.Level1)
{
    auto& manager = RSTransactionHandlerManager::Instance();
    auto handler1 = std::make_shared<RSTransactionHandler>();
    auto handler2 = std::make_shared<RSTransactionHandler>();
    ASSERT_NE(handler1, nullptr);
    ASSERT_NE(handler2, nullptr);

    manager.Register(TEST_TOKEN, handler1);
    EXPECT_EQ(manager.Get(TEST_TOKEN), handler1);

    manager.Register(TEST_TOKEN, handler2);
    EXPECT_EQ(manager.Get(TEST_TOKEN), handler2);
}
} // namespace Rosen
} // namespace OHOS
