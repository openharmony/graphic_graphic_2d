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

#include "vsync_remote_manager_stub_test.h"

#include <iservice_registry.h>
#include <unistd.h>

#include "return_value_tester.h"

namespace OHOS {
namespace Vsync {
void VsyncManagerStubTest::SetUp()
{
}

void VsyncManagerStubTest::TearDown()
{
}

void VsyncManagerStubTest::SetUpTestCase()
{
    vc_ = new VsyncManager();
    ASSERT_NE(vc_, nullptr);
}

void VsyncManagerStubTest::TearDownTestCase()
{
    vc_ = nullptr;
}

namespace {
HWTEST_F(VsyncManagerStubTest, OnRemoteRequest1, testing::ext::TestSize.Level0)
{
    MessageOption opt;
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(u"DaDaDa");
    auto ret = vc_->OnRemoteRequest(IVsyncManager::IVSYNC_MANAGER_LISTEN_VSYNC, data, reply, opt);
    ASSERT_EQ(ret, ERR_INVALID_STATE);
}

HWTEST_F(VsyncManagerStubTest, OnRemoteRequest2, testing::ext::TestSize.Level0)
{
    MessageOption opt;
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(IVsyncManager::GetDescriptor());
    auto ret = vc_->OnRemoteRequest(3, data, reply, opt);
    ASSERT_EQ(ret, 1);
}

HWTEST_F(VsyncManagerStubTest, OnRemoteRequest3, testing::ext::TestSize.Level0)
{
    MessageOption opt;
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(IVsyncManager::GetDescriptor());
    data.WriteRemoteObject(nullptr);
    vc_->OnRemoteRequest(IVsyncManager::IVSYNC_MANAGER_LISTEN_VSYNC, data, reply, opt);
    GSError err = (GSError)reply.ReadInt32();
    ASSERT_EQ(err, GSERROR_INVALID_ARGUMENTS);
}

HWTEST_F(VsyncManagerStubTest, OnRemoteRequest4, testing::ext::TestSize.Level0)
{
    MessageOption opt;
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(IVsyncManager::GetDescriptor());
    int count = 0;
    sptr<IVsyncCallback> cb = new  VsyncCallback(count);
    data.WriteRemoteObject(cb->AsObject());
    auto ret = vc_->OnRemoteRequest(IVsyncManager::IVSYNC_MANAGER_LISTEN_VSYNC, data, reply, opt);
    ASSERT_EQ(ret, 0);
}

HWTEST_F(VsyncManagerStubTest, ListenVsync1, testing::ext::TestSize.Level0)
{
    sptr<IVsyncCallback> cb = nullptr;
    auto ret = vc_->ListenVsync(cb);
    ASSERT_EQ(ret, GSERROR_INVALID_ARGUMENTS);
}

HWTEST_F(VsyncManagerStubTest, ListenVsync2, testing::ext::TestSize.Level0)
{
    int count = 0;
    sptr<IVsyncCallback> cb = new VsyncCallback(count);
    auto ret = vc_->ListenVsync(cb);
    ASSERT_EQ(ret, GSERROR_OK);
}

HWTEST_F(VsyncManagerStubTest, GetVsyncFrequency, testing::ext::TestSize.Level0)
{
    uint32_t freq = 30;
    vc_->GetVsyncFrequency(freq);
    int result = static_cast<int>(freq);
    ASSERT_EQ(result, 60);
}

HWTEST_F(VsyncManagerStubTest, Callback1, testing::ext::TestSize.Level0)
{
    int count = 0;
    sptr<IVsyncCallback> cb = new VsyncCallback(count);
    vc_->ListenVsync(cb);
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    int64_t timestamp = (int64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
    vc_->Callback(timestamp);
    ASSERT_EQ(count, 1);
}

HWTEST_F(VsyncManagerStubTest, Callback2, testing::ext::TestSize.Level0)
{
    int count = 0;
    sptr<IVsyncCallback> cb = new VsyncCallback(count);
    vc_->ListenVsync(cb);
    vc_->Callback(0);
    ASSERT_EQ(count, 0);
}
} // namespace

VsyncCallback::VsyncCallback(int &count) : count_(&count)
{
}

GSError VsyncCallback::OnVsync(int64_t timestamp)
{
    if (timestamp <= 0) {
        return GSERROR_BINDER;
    }
    *count_ = *count_ + 1;
    return GSERROR_OK;
}
} // namespace Vsync
} // namespace OHOS
