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

#ifndef FRAMEWORKS_VSYNC_TEST_UNITTEST_MOCK_MOCK_STATIC_CALL_H
#define FRAMEWORKS_VSYNC_TEST_UNITTEST_MOCK_MOCK_STATIC_CALL_H

#include <gmock/gmock.h>

#include "static_call.h"

namespace OHOS {
namespace Vsync {
class MockStaticCall : public StaticCall {
public:
     MOCK_METHOD0(GetSystemAbilityManager, sptr<ISystemAbilityManager>());
     MOCK_METHOD2(GetSystemAbility, sptr<IRemoteObject>(sptr<ISystemAbilityManager>& sm, int32_t systemAbilityId));
     MOCK_METHOD1(GetCast, sptr<IVsyncManager>(sptr<IRemoteObject>& remoteObject));
     MOCK_METHOD2(GetVsyncFrequency, VsyncError(sptr<IVsyncManager>& server, uint32_t& freq));
     MOCK_METHOD2(ListenVsync, VsyncError(sptr<IVsyncManager>& server, sptr<IVsyncCallback>& cb));
     MOCK_METHOD0(Current, std::shared_ptr<AppExecFwk::EventHandler>());
};
} // namespace Vsync
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_TEST_UNITTEST_MOCK_MOCK_STATIC_CALL_H
