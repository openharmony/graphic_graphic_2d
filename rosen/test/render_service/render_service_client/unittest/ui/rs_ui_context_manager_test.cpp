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

#include <mutex>
#include <unordered_map>

#include "ui/rs_ui_context_manager.h"
#include "ui/rs_ui_director.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint64_t TEST_NULL_CONTEXT_TOKEN = 1;

// Saves the singleton's context map on construction and restores it on destruction so each
// case starts from and leaves behind the exact process-wide state it inherited (failure
// paths included).
class ScopedContextMap {
public:
    explicit ScopedContextMap(RSUIContextManager& manager) : manager_(manager)
    {
        std::unique_lock<std::mutex> lock(manager_.mutex_);
        savedMap_ = manager_.rsUIContextMap_;
    }

    ~ScopedContextMap()
    {
        std::unique_lock<std::mutex> lock(manager_.mutex_);
        manager_.rsUIContextMap_ = savedMap_;
    }

private:
    RSUIContextManager& manager_;
    std::unordered_map<uint64_t, std::shared_ptr<RSUIContext>> savedMap_;
};

// Temporarily forces a bool member of the singleton and restores the inherited value on
// destruction. Member access relies on the "-Dprivate = public" cflag shared by every
// target in this directory.
class ScopedBoolFlag {
public:
    ScopedBoolFlag(bool& flag, bool forcedValue) : flag_(flag), savedValue_(flag)
    {
        flag_ = forcedValue;
    }

    ~ScopedBoolFlag()
    {
        flag_ = savedValue_;
    }

private:
    bool& flag_;
    bool savedValue_;
};
} // namespace

class RSUIContextManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: HasSceneBoardInstanceGuardClosed001
 * @tc.desc: Guard branch of HasSceneBoardInstance (rs_ui_context_manager.cpp:100): the
 *           multi-instance switch closed makes it return false before touching the map.
 * @tc.type: FUNC
 */
HWTEST_F(RSUIContextManagerTest, HasSceneBoardInstanceGuardClosed001, TestSize.Level1)
{
    auto& manager = RSUIContextManager::MutableInstance();
    ScopedBoolFlag multiInstanceFlag(manager.isMultiInstanceOpen_, false);
    EXPECT_FALSE(manager.HasSceneBoardInstance());
}

/**
 * @tc.name: HasSceneBoardInstanceEmptyMap001
 * @tc.desc: Loop-completes branch of HasSceneBoardInstance (rs_ui_context_manager.cpp:109):
 *           an empty context map makes the loop fall through to return false.
 * @tc.type: FUNC
 */
HWTEST_F(RSUIContextManagerTest, HasSceneBoardInstanceEmptyMap001, TestSize.Level1)
{
    auto& manager = RSUIContextManager::MutableInstance();
    ScopedBoolFlag multiInstanceFlag(manager.isMultiInstanceOpen_, true);
    ScopedContextMap mapGuard(manager);
    {
        std::unique_lock<std::mutex> lock(manager.mutex_);
        manager.rsUIContextMap_.clear();
    }
    EXPECT_FALSE(manager.HasSceneBoardInstance());
}

/**
 * @tc.name: HasSceneBoardInstanceNullContext001
 * @tc.desc: Null-context branch of HasSceneBoardInstance (rs_ui_context_manager.cpp:105):
 *           an entry holding a null shared_ptr short-circuits the ctx->IsSceneBoard() call
 *           and the loop keeps scanning until it falls through to return false.
 * @tc.type: FUNC
 */
HWTEST_F(RSUIContextManagerTest, HasSceneBoardInstanceNullContext001, TestSize.Level1)
{
    auto& manager = RSUIContextManager::MutableInstance();
    ScopedBoolFlag multiInstanceFlag(manager.isMultiInstanceOpen_, true);
    ScopedContextMap mapGuard(manager);
    {
        std::unique_lock<std::mutex> lock(manager.mutex_);
        manager.rsUIContextMap_.clear();
        manager.rsUIContextMap_[TEST_NULL_CONTEXT_TOKEN] = nullptr;
    }
    EXPECT_FALSE(manager.HasSceneBoardInstance());
}

/**
 * @tc.name: HasSceneBoardInstanceMarkedInstance001
 * @tc.desc: Loop true branch of HasSceneBoardInstance (rs_ui_context_manager.cpp:106): a
 *           null entry is skipped and a marked real context found afterwards makes it
 *           return true on the same scan.
 * @tc.type: FUNC
 */
HWTEST_F(RSUIContextManagerTest, HasSceneBoardInstanceMarkedInstance001, TestSize.Level1)
{
    OHOS::sptr<OHOS::IRemoteObject> remote;
    auto& manager = RSUIContextManager::MutableInstance();
    // Creation is only allowed while the multi-instance switch is open; force it so the
    // case does not depend on the device-side system property.
    ScopedBoolFlag multiInstanceFlag(manager.isMultiInstanceOpen_, true);
    auto context = manager.CreateRSUIContext(remote);
    ASSERT_NE(context, nullptr);
    auto director = RSUIDirector::Create(remote, context);
    ASSERT_NE(director, nullptr);
    director->MarkAsSceneBoard();

    // The null entry must not terminate the scan: the marked context after it is still found.
    {
        std::unique_lock<std::mutex> lock(manager.mutex_);
        manager.rsUIContextMap_[TEST_NULL_CONTEXT_TOKEN] = nullptr;
    }
    EXPECT_TRUE(manager.HasSceneBoardInstance());
}
} // namespace OHOS::Rosen
