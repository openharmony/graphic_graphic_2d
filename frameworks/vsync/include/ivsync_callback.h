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

#ifndef FRAMEWORKS_VSYNC_INCLUDE_IVSYNC_CALLBACK_H
#define FRAMEWORKS_VSYNC_INCLUDE_IVSYNC_CALLBACK_H

#include <iremote_broker.h>

namespace OHOS {
class IVsyncCallback : public IRemoteBroker {
public:
    enum {
        IVSYNC_CALLBACK_ON_VSYNC,
    };
    virtual void OnVsync(int64_t timestamp) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"IVsyncCallback");
};
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_INCLUDE_IVSYNC_CALLBACK_H
