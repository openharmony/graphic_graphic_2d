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

#ifndef FRAMEWORKS_VSYNC_INCLUDE_STATIC_CALL_H
#define FRAMEWORKS_VSYNC_INCLUDE_STATIC_CALL_H

#include <iservice_registry.h>

#include "vsync_helper_impl.h"

namespace OHOS {
namespace Vsync {
class StaticCall : public RefBase {
public:
    static sptr<StaticCall> GetInstance();
    static void SetInstance(sptr<StaticCall>& mockInstance);

    virtual sptr<ISystemAbilityManager> GetSystemAbilityManager();
    virtual sptr<IRemoteObject> GetSystemAbility(sptr<ISystemAbilityManager>& sm, int32_t systemAbilityId);
    virtual sptr<IVsyncManager> GetCast(sptr<IRemoteObject>& remoteObject);
    virtual VsyncError GetVsyncFrequency(sptr<IVsyncManager>& server, uint32_t& freq);
    virtual VsyncError ListenVsync(sptr<IVsyncManager>& server, sptr<IVsyncCallback>& cb);
    virtual std::shared_ptr<AppExecFwk::EventHandler> Current();
    virtual void Sync(int64_t, void *data);
private:
    StaticCall() = default;
    virtual ~StaticCall() = default;
    static inline sptr<StaticCall> instance = nullptr;
};
} // namespace Vsync
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_STATIC_CALL_H
