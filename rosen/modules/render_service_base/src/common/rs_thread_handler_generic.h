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

#ifndef RENDER_SERVICE_BASE_ADAPTER_GENERIC_RS_THREAD_HANDLER_GENERIC_H
#define RENDER_SERVICE_BASE_ADAPTER_GENERIC_RS_THREAD_HANDLER_GENERIC_H

#include <functional>

#include "common/rs_thread_handler.h"
#include "rs_thread_looper_impl.h"

namespace OHOS {
namespace Rosen {
class RSThreadHandlerGeneric final : public RSThreadHandler {
public:
    RSThreadHandlerGeneric();
    ~RSThreadHandlerGeneric() override = default;
    RSTaskHandle CreateTask(const RSTaskMessage::RSTask task) const override;
    void PostTask(const RSTaskHandle taskHandle, int param) override;
    void PostTaskDelay(const RSTaskHandle taskHandle, int64_t nsecs, int param) override;
    void CancelTask(RSTaskHandle taskHandle) override;
    bool IsValid() const override
    {
        return looper_ != nullptr;
    }

private:
    friend class ThreadLooperGeneric;
    ThreadLooperImpl* looper_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_ADAPTER_GENERIC_RS_THREAD_HANDLER_GENERIC_H