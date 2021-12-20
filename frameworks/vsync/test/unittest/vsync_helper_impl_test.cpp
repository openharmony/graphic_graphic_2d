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

#include "vsync_helper_impl_test.h"

#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "mock/mock_static_call.h"
#include "vsync_helper_impl.h"

namespace OHOS {
namespace Vsync {
void VsyncHelperImplTest::SetUp()
{
}

void VsyncHelperImplTest::TearDown()
{
}

void VsyncHelperImplTest::SetUpTestCase()
{
}

void VsyncHelperImplTest::TearDownTestCase()
{
}

namespace {
HWTEST_F(VsyncHelperImplTest, Init1, testing::ext::TestSize.Level0)
{
    sptr<MockStaticCall> mockInstance = new MockStaticCall();
    sptr<VsyncClient> client = VsyncClient::GetInstance();
    sptr<StaticCall> staticCall = mockInstance;
    auto origin = StaticCall::GetInstance();
    StaticCall::SetInstance(staticCall);
    EXPECT_CALL(*mockInstance, GetSystemAbilityManager())
                .Times(1).WillRepeatedly(testing::Return(nullptr));
    auto ret = client->Init();
    ASSERT_EQ(ret, VSYNC_ERROR_SAMGR);
    StaticCall::SetInstance(origin);
}

HWTEST_F(VsyncHelperImplTest, Init2, testing::ext::TestSize.Level0)
{
    sptr<MockStaticCall> mockInstance = new MockStaticCall();
    sptr<VsyncClient> client = VsyncClient::GetInstance();
    sptr<StaticCall> staticCall = mockInstance;
    auto origin = StaticCall::GetInstance();
    StaticCall::SetInstance(staticCall);
    auto sm =SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_CALL(*mockInstance, GetSystemAbilityManager())
                .Times(1).WillRepeatedly(testing::Return(sm));
    EXPECT_CALL(*mockInstance, GetSystemAbility(testing::_, testing::_))
                .Times(1).WillRepeatedly(testing::Return(nullptr));
    auto ret = client->Init();
    ASSERT_EQ(ret, VSYNC_ERROR_SERVICE_NOT_FOUND);
    StaticCall::SetInstance(origin);
}

HWTEST_F(VsyncHelperImplTest, Init3, testing::ext::TestSize.Level0)
{
    sptr<MockStaticCall> mockInstance = new MockStaticCall();
    sptr<VsyncClient> client = VsyncClient::GetInstance();
    sptr<StaticCall> staticCall = mockInstance;
    auto origin = StaticCall::GetInstance();
    StaticCall::SetInstance(staticCall);
    auto sm =SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = sm->GetSystemAbility(VSYNC_MANAGER_ID);
    EXPECT_CALL(*mockInstance, GetSystemAbilityManager())
                .Times(1).WillRepeatedly(testing::Return(sm));
    EXPECT_CALL(*mockInstance, GetSystemAbility(testing::_, testing::_))
                .Times(1).WillRepeatedly(testing::Return(remoteObject));
    EXPECT_CALL(*mockInstance, GetCast(testing::_))
                .Times(1).WillRepeatedly(testing::Return(nullptr));
    auto ret = client->Init();
    ASSERT_EQ(ret, VSYNC_ERROR_PROXY_NOT_INCLUDE);
    StaticCall::SetInstance(origin);
}

HWTEST_F(VsyncHelperImplTest, Init4, testing::ext::TestSize.Level0)
{
    sptr<MockStaticCall> mockInstance = new MockStaticCall();
    sptr<VsyncClient> client = VsyncClient::GetInstance();
    sptr<StaticCall> staticCall = mockInstance;
    auto origin = StaticCall::GetInstance();
    StaticCall::SetInstance(staticCall);
    auto sm =SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = sm->GetSystemAbility(VSYNC_MANAGER_ID);
    auto servive = iface_cast<IVsyncManager>(remoteObject);
    EXPECT_CALL(*mockInstance, GetSystemAbilityManager())
                .Times(1).WillRepeatedly(testing::Return(sm));
    EXPECT_CALL(*mockInstance, GetSystemAbility(testing::_, testing::_))
                .Times(1).WillRepeatedly(testing::Return(remoteObject));
    EXPECT_CALL(*mockInstance, GetCast(testing::_))
                .Times(1).WillRepeatedly(testing::Return(servive));
    EXPECT_CALL(*mockInstance, GetVsyncFrequency(testing::_, testing::_))
                .Times(1).WillRepeatedly(testing::Return(VSYNC_ERROR_INVALID_OPERATING));
    auto ret = client->Init();
    ASSERT_EQ(ret, VSYNC_ERROR_INVALID_OPERATING);
    StaticCall::SetInstance(origin);
}

HWTEST_F(VsyncHelperImplTest, Init5, testing::ext::TestSize.Level0)
{
    sptr<MockStaticCall> mockInstance = new MockStaticCall();
    sptr<VsyncClient> client = VsyncClient::GetInstance();
    sptr<StaticCall> staticCall = mockInstance;
    auto origin = StaticCall::GetInstance();
    StaticCall::SetInstance(staticCall);
    auto sm =SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = sm->GetSystemAbility(VSYNC_MANAGER_ID);
    auto servive = iface_cast<IVsyncManager>(remoteObject);
    EXPECT_CALL(*mockInstance, GetVsyncFrequency(testing::_, testing::_))
                .Times(1).WillRepeatedly(testing::Return(VSYNC_ERROR_OK));
    auto ret = client->Init();
    ASSERT_EQ(ret, VSYNC_ERROR_INNER);
    StaticCall::SetInstance(origin);
}

HWTEST_F(VsyncHelperImplTest, Init7, testing::ext::TestSize.Level0)
{
    sptr<VsyncClient> client = VsyncClient::GetInstance();
    auto ret = client->Init();
    ASSERT_EQ(ret, VSYNC_ERROR_OK);
}

HWTEST_F(VsyncHelperImplTest, RequestFrameCallback1, testing::ext::TestSize.Level0)
{
    sptr<VsyncClient> client = VsyncClient::GetInstance();
    FrameCallback cb;
    auto ret = client->RequestFrameCallback(cb);
    ASSERT_EQ(ret, VSYNC_ERROR_NULLPTR);
}

HWTEST_F(VsyncHelperImplTest, RequestFrameCallback2, testing::ext::TestSize.Level0)
{
    sptr<VsyncClient> client = VsyncClient::GetInstance();
    FrameCallback cb;
    cb.callback_ = std::bind(&StaticCall::Sync, StaticCall::GetInstance(), SYNC_FUNC_ARG);
    cb.frequency_ = 31;
    auto ret = client->RequestFrameCallback(cb);
    ASSERT_EQ(ret, VSYNC_ERROR_INVALID_ARGUMENTS);
}

HWTEST_F(VsyncHelperImplTest, Current, testing::ext::TestSize.Level0)
{
    sptr<VsyncHelperImpl> helper =new VsyncHelperImpl(handler);
    sptr<MockStaticCall> mockInstance = new MockStaticCall();
    sptr<VsyncClient> client = VsyncClient::GetInstance();
    sptr<StaticCall> staticCall = mockInstance;
    auto origin = StaticCall::GetInstance();
    StaticCall::SetInstance(staticCall);
    EXPECT_CALL(*mockInstance, Current())
                .Times(1).WillRepeatedly(testing::Return(nullptr));
    auto ret = helper->Current();
    ASSERT_EQ(ret, nullptr);
    StaticCall::SetInstance(origin);
}
} // namespace
} // namespace Vsync
} // namespace OHOS
