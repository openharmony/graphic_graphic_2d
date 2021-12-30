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

#ifndef RENDER_SERVICE_BASE_CORE_COMMON_RS_THREAD_LOOPER_H
#define RENDER_SERVICE_BASE_CORE_COMMON_RS_THREAD_LOOPER_H

#include <memory>

#include "common/rs_thread_handler.h"

namespace OHOS {
namespace Rosen {
class RSThreadLooper {
public:
    RSThreadLooper() = default;
    virtual ~RSThreadLooper() = default;

    static std::unique_ptr<RSThreadLooper> Create();

    virtual void ProcessOneMessage(int timeoutMillis) = 0;
    virtual void ProcessAllMessages(int timeoutMillis) = 0;
    virtual void WakeUp() = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_CORE_COMMON_RS_THREAD_LOOPER_H
