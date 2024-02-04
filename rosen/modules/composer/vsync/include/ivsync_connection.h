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

#ifndef VSYNC_IVSYNC_CONNECTION_H
#define VSYNC_IVSYNC_CONNECTION_H

#include <iremote_broker.h>
#include "graphic_common.h"

namespace OHOS {
namespace Rosen {
class IVSyncConnection : public IRemoteBroker {
public:
    virtual VsyncError RequestNextVSync() = 0;
    virtual VsyncError RequestNextVSync(const std::string& fromWhom, int64_t lastVSyncTS) = 0;

    virtual VsyncError GetReceiveFd(int32_t &fd) = 0;

    // We assume that the vsync frequency is FREQ:
    // if rate <= 0, we just return error
    // if rate > 0, we will continue to send Vsync signals at a frequency of FREQ / rate
    virtual VsyncError SetVSyncRate(int32_t rate) = 0;

    virtual VsyncError Destroy() = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"IVSyncConnection");

protected:
    enum {
        IVSYNC_CONNECTION_REQUEST_NEXT_VSYNC,
        IVSYNC_CONNECTION_GET_RECEIVE_FD,
        IVSYNC_CONNECTION_SET_RATE,
        IVSYNC_CONNECTION_DESTROY,
    };
};
} // namespace Vsync
} // namespace OHOS

#endif // VSYNC_IVSYNC_CONNECTION_H
