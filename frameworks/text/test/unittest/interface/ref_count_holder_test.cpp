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

#include <atomic>
#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <vector>

#include "rosen_text/ref_count_holder.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

namespace {
struct TrackedObj {
    static std::atomic<int> destroyedCount;
    int value = 42;
    ~TrackedObj()
    {
        destroyedCount.fetch_add(1, std::memory_order_relaxed);
    }
};
std::atomic<int> TrackedObj::destroyedCount(0);

RefCountedHolder<TrackedObj>* CreateTrackedHolder()
{
    TrackedObj::destroyedCount = 0;
    return new RefCountedHolder<TrackedObj>(std::make_unique<TrackedObj>());
}
} // namespace

class RefCountHolderTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override { TrackedObj::destroyedCount = 0; }
    void TearDown() override {}
};

/**
 * @tc.name: InitialReferenceKeepsObjectAlive001
 * @tc.desc: verify the creator-owned initial reference keeps the object alive and Get returns it
 */
HWTEST_F(RefCountHolderTest, InitialReferenceKeepsObjectAlive001, TestSize.Level0)
{
    auto* holder = CreateTrackedHolder();
    ASSERT_NE(holder, nullptr);
    ASSERT_NE(holder->Get(), nullptr);
    EXPECT_EQ(holder->Get()->value, 42);
    EXPECT_EQ(TrackedObj::destroyedCount.load(), 0);
    holder->Release();
    EXPECT_EQ(TrackedObj::destroyedCount.load(), 1);
}

/**
 * @tc.name: AddReleaseKeepObjectAlive002
 * @tc.desc: verify object survives when references are added and dropped in pairs
 */
HWTEST_F(RefCountHolderTest, AddReleaseKeepObjectAlive002, TestSize.Level0)
{
    auto* holder = CreateTrackedHolder();
    holder->AddRef();
    holder->AddRef();
    holder->Release();
    EXPECT_EQ(TrackedObj::destroyedCount.load(), 0);
    EXPECT_EQ(holder->Get()->value, 42);
    holder->Release();
    EXPECT_EQ(TrackedObj::destroyedCount.load(), 0);
    holder->Release();
    EXPECT_EQ(TrackedObj::destroyedCount.load(), 1);
}

/**
 * @tc.name: RaiiRefCountedPtrLifecycle003
 * @tc.desc: verify RefCountedPtr acquires and releases references correctly
 */
HWTEST_F(RefCountHolderTest, RaiiRefCountedPtrLifecycle003, TestSize.Level0)
{
    auto* holder = CreateTrackedHolder();

    // default-constructed pointer owns nothing
    RefCountedPtr<TrackedObj> empty;
    EXPECT_EQ(empty.Get(), nullptr);
    EXPECT_FALSE(static_cast<bool>(empty));

    // acquire via constructor, drop the creator reference: object must stay alive
    {
        RefCountedPtr<TrackedObj> ref(holder);
        EXPECT_NE(ref.Get(), nullptr);
        EXPECT_TRUE(static_cast<bool>(ref));
        holder->Release(); // creator reference
        EXPECT_EQ(TrackedObj::destroyedCount.load(), 0);
    } // last reference dropped here
    EXPECT_EQ(TrackedObj::destroyedCount.load(), 1);
}

/**
 * @tc.name: RaiiRefCountedPtrCopyMove004
 * @tc.desc: verify copy/move semantics of RefCountedPtr balance references
 */
HWTEST_F(RefCountHolderTest, RaiiRefCountedPtrCopyMove004, TestSize.Level0)
{
    auto* holder = CreateTrackedHolder();
    RefCountedPtr<TrackedObj> first(holder);
    holder->Release(); // creator reference
    {
        RefCountedPtr<TrackedObj> copied = first; // copy acquires
        EXPECT_EQ(copied.Get(), first.Get());
        EXPECT_EQ(TrackedObj::destroyedCount.load(), 0);
        RefCountedPtr<TrackedObj> moved = std::move(copied); // move takes over
        EXPECT_EQ(moved.Get(), first.Get());
        EXPECT_EQ(copied.Get(), nullptr);
        EXPECT_EQ(TrackedObj::destroyedCount.load(), 0);
    } // moved reference dropped, first still holds one
    EXPECT_EQ(TrackedObj::destroyedCount.load(), 0);
    EXPECT_EQ(first.Get()->value, 42);
}

/**
 * @tc.name: ConcurrentAddReleaseDestroyOnce005
 * @tc.desc: verify concurrent AddRef/Release destroys the wrapped object exactly once
 */
HWTEST_F(RefCountHolderTest, ConcurrentAddReleaseDestroyOnce005, TestSize.Level0)
{
    constexpr int threadNum = 8;
    constexpr int opsPerThread = 500;
    auto* holder = CreateTrackedHolder();
    RefCountedPtr<TrackedObj> keeper(holder); // main-thread reference
    std::vector<std::thread> threads;
    for (int i = 0; i < threadNum; ++i) {
        threads.emplace_back([&holder]() {
            for (int j = 0; j < opsPerThread; ++j) {
                holder->AddRef();
                holder->Release();
            }
        });
    }
    for (auto& t : threads) {
        t.join();
    }
    EXPECT_EQ(TrackedObj::destroyedCount.load(), 0);
    keeper.Reset(); // last reference
    EXPECT_EQ(TrackedObj::destroyedCount.load(), 1);
}
