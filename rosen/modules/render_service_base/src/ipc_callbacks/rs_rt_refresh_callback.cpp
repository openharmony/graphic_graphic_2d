/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ipc_callbacks/rs_rt_refresh_callback.h"

namespace OHOS {
namespace Rosen {

RSRTRefreshCallback& RSRTRefreshCallback::Instance()
{
    static RSRTRefreshCallback instance;
    return instance;
}

void RSRTRefreshCallback::ExecuteRefresh()
{
    if (callback_) {
        callback_();
    }
}

void RSRTRefreshCallback::SetRefresh(std::function<void(void)> callback)
{
    callback_ = callback;
}

} // namespace Rosen
} // namespace OHOS
