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

#ifndef RENDER_SERVICE_BASE_CORE_COMMON_RS_THREAD_HANDLER_H
#define RENDER_SERVICE_BASE_CORE_COMMON_RS_THREAD_HANDLER_H

#include <functional>
#include <memory>

namespace OHOS {
namespace Rosen {
class RSTaskMessage {
public:
    using RSTask = std::function<void()>;
};

using RSTaskHandle = std::shared_ptr<RSTaskMessage>;

class RSThreadHandler {
public:
    RSThreadHandler() = default;
    virtual ~RSThreadHandler() = default;

    static std::unique_ptr<RSThreadHandler> Create();
    virtual RSTaskHandle CreateTask(const RSTaskMessage::RSTask task) const = 0;
    virtual void PostTask(const RSTaskHandle task, int param = 0) = 0;
    virtual void PostTaskDelay(const RSTaskHandle task, int64_t nsecs, int param = 0) = 0;
    virtual void CancelTask(RSTaskHandle task) = 0;
    virtual bool IsValid() const = 0;
    static RSTaskHandle StaticCreateTask(const RSTaskMessage::RSTask task);

private:
    RSThreadHandler(const RSThreadHandler&) = delete;
    const RSThreadHandler& operator=(const RSThreadHandler&) = delete;
    RSThreadHandler(const RSThreadHandler&&) = delete;
    const RSThreadHandler& operator=(const RSThreadHandler&&) = delete;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_CORE_COMMON_RS_THREAD_HANDLER_H
