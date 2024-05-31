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
#ifndef RENDER_SERVICE_BASE_CORE_PIPELINE_RS_VSYNC_CLIENT_H
#define RENDER_SERVICE_BASE_CORE_PIPELINE_RS_VSYNC_CLIENT_H

#include <functional>
#include <memory>

namespace OHOS {
namespace Rosen {
class RSVsyncClient {
public:
    using VsyncCallback = std::function<void(int64_t, int64_t)>;

    static std::unique_ptr<RSVsyncClient> Create();
    virtual ~RSVsyncClient() {};

    virtual void RequestNextVsync() = 0;
    virtual void SetVsyncCallback(VsyncCallback callback) = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_CORE_PIPELINE_RS_VSYNC_CLIENT_H