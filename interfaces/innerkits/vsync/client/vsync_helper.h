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

#include <refbase.h>
#include <event_handler.h>

#include "vsync_type.h"

namespace OHOS {
struct FrameCallback {
    int64_t timestamp_;
    void* userdata_;
    void (*callback_)(int64_t, void*);

    bool operator < (const struct FrameCallback& cb) const
    {
        return timestamp_ < cb.timestamp_;
    }
};

class VsyncHelper : public RefBase {
public:
    static sptr<VsyncHelper> Current();
    static sptr<VsyncHelper> FromHandler(std::shared_ptr<AppExecFwk::EventHandler>& handler);

    virtual VsyncError RequestFrameCallback(struct FrameCallback& cb) = 0;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_VSYNC_CLIENT_VSYNC_HELPER_H
