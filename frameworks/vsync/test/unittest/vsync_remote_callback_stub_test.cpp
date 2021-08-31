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

#include "vsync_remote_callback_stub_test.h"
#include "return_value_tester.h"

#include <unistd.h>

#include <iservice_registry.h>

namespace OHOS {
namespace Vsync {
void VsyncCallbackStubTest::SetUp()
{
}

void VsyncCallbackStubTest::TearDown()
{
}

void VsyncCallbackStubTest::SetUpTestCase()
{
    vc_ = new  VsyncCallback();
    ASSERT_NE(vc_, nullptr);
}

void VsyncCallbackStubTest::TearDownTestCase()
{
    vc_ = nullptr;
}

namespace {
HWTEST_F(VsyncCallbackStubTest, OnRemoteRequest1, testing::ext::TestSize.Level0)
{
    MessageOption opt;
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(u"DaDaDa");
    auto ret = vc_->OnRemoteRequest(IVsyncCallback::IVSYNC_CALLBACK_ON_VSYNC, data, reply, opt);
    ASSERT_EQ(ret, ERR_INVALID_STATE);
}

HWTEST_F(VsyncCallbackStubTest, OnRemoteRequest2, testing::ext::TestSize.Level0)
{
    MessageOption opt;
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(IVsyncCallback::GetDescriptor());
    auto ret = vc_->OnRemoteRequest(4, data, reply, opt);
    ASSERT_EQ(ret, 1);
}

HWTEST_F(VsyncCallbackStubTest, OnRemoteRequest3, testing::ext::TestSize.Level0)
{
    MessageOption opt;
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(IVsyncCallback::GetDescriptor());
    auto ret = vc_->OnRemoteRequest(IVsyncCallback::IVSYNC_CALLBACK_ON_VSYNC, data, reply, opt);
    ASSERT_EQ(ret, 1);
}

HWTEST_F(VsyncCallbackStubTest, OnRemoteRequest4, testing::ext::TestSize.Level0)
{
    MessageOption opt;
    MessageParcel data;
    MessageParcel reply;
    data.WriteInterfaceToken(IVsyncCallback::GetDescriptor());
    data.WriteInt64(12);
    auto ret = vc_->OnRemoteRequest(IVsyncCallback::IVSYNC_CALLBACK_ON_VSYNC, data, reply, opt);
    ASSERT_EQ(ret, 0);
}
} // namespace

VsyncError VsyncCallback::OnVsync(int64_t timestamp)
{
    return VSYNC_ERROR_OK;
}
} // namespace Vsync
} // namespace OHOS
