/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "vsync_manager_test.h"

#include <gtest/gtest.h>

#include "vsync_callback_stub.h"
#include "vsync_manager.h"

using namespace OHOS;

class VsyncManagerTest : public testing::Test {
public:
    void SetUp()
    {
        manager_ = new VsyncManager();
    }
    void TearDown()
    {
        manager_ = nullptr;
    }

private:
    sptr<VsyncManager> manager_;
    MessageParcel data_;
    MessageParcel reply_;
    MessageOption option_;
};

class VsyncCallback : public VsyncCallbackStub {
public:
    void OnVsync(int64_t timestamp)
    {
        isCall_ = true;
    }

    bool IsCall() const
    {
        return isCall_;
    }

private:
    bool isCall_ = false;
};

namespace {
HWTEST_F(VsyncManagerTest, OnRemoteRequest, testing::ext::TestSize.Level0)
{
    ASSERT_NE(manager_->OnRemoteRequest(-1, data_, reply_, option_), 0);

    data_.WriteInterfaceToken(IVsyncManager::GetDescriptor());
    data_.WriteInt32(0);
    ASSERT_EQ(manager_->OnRemoteRequest(IVsyncManager::IVSYNC_MANAGER_LISTEN_NEXT_VSYNC,
                                        data_, reply_, option_), 0);
    ASSERT_NE(reply_.ReadInt32(), VSYNC_ERROR_OK);
}

HWTEST_F(VsyncManagerTest, ListenNextVsync, testing::ext::TestSize.Level0)
{
    sptr<IVsyncCallback> cb = nullptr;
    ASSERT_NE(manager_->ListenNextVsync(cb), VSYNC_ERROR_OK);

    cb = new VsyncCallback();
    ASSERT_EQ(manager_->ListenNextVsync(cb), VSYNC_ERROR_OK);
}

HWTEST_F(VsyncManagerTest, OnVsync, testing::ext::TestSize.Level0)
{
    sptr<VsyncCallback> cb = new VsyncCallback();
    sptr<IVsyncCallback> icb = cb;

    ASSERT_EQ(manager_->ListenNextVsync(icb), VSYNC_ERROR_OK);
    ASSERT_FALSE(cb->IsCall());

    constexpr int64_t testTimestamp = 0;
    manager_->Callback(testTimestamp);

    ASSERT_TRUE(cb->IsCall());
}
} // namespace
