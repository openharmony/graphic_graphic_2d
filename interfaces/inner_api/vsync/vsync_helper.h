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

#ifndef INTERFACES_INNERKITS_VSYNC_CLIENT_VSYNC_HELPER_H
#define INTERFACES_INNERKITS_VSYNC_CLIENT_VSYNC_HELPER_H

#include <functional>
#include <vector>

#include <event_handler.h>
#include <functional>
#include <refbase.h>

#include <graphic_common.h>

namespace OHOS {
using SyncFunc = std::function<void(int64_t, void *)>;
#define SYNC_FUNC_ARG ::std::placeholders::_1, ::std::placeholders::_2

struct FrameCallback {
    uint32_t frequency_;
    int64_t timestamp_;
    void *userdata_;
    SyncFunc callback_;
};

class VsyncHelper : public RefBase {
public:
    static sptr<VsyncHelper> Current();
    static sptr<VsyncHelper> FromHandler(std::shared_ptr<AppExecFwk::EventHandler>& handler);

    virtual GSError RequestFrameCallback(const struct FrameCallback &cb) = 0;
    virtual GSError GetSupportedVsyncFrequencys(std::vector<uint32_t>& freqs) = 0;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_VSYNC_CLIENT_VSYNC_HELPER_H
