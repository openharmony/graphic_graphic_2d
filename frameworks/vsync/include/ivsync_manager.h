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

#ifndef FRAMEWORKS_VSYNC_INCLUDE_IVSYNC_MANAGER_H
#define FRAMEWORKS_VSYNC_INCLUDE_IVSYNC_MANAGER_H

#include <graphic_common.h>
#include <iremote_broker.h>

#include "ivsync_callback.h"

namespace OHOS {
namespace Vsync {
class IVsyncManager : public IRemoteBroker {
public:
    virtual GSError ListenVsync(sptr<IVsyncCallback>& cb, int32_t &cbid) = 0;
    virtual GSError RemoveVsync(int32_t cbid) = 0;
    virtual GSError GetVsyncFrequency(uint32_t &freq) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"IVsyncManager");

protected:
    enum {
        IVSYNC_MANAGER_LISTEN_VSYNC = 0,
        IVSYNC_MANAGER_REMOVE_VSYNC = 1,
        IVSYNC_MANAGER_GET_VSYNC_FREQUENCY = 2,
    };
};
} // namespace Vsync
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_IVSYNC_MANAGER_H
