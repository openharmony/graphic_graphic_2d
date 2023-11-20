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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RCD_RENDER_RS_SUB_THREAD_H
#define RENDER_SERVICE_CORE_PIPELINE_RCD_RENDER_RS_SUB_THREAD_H

#pragma once
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "render_context/render_context.h"
#include "event_handler.h"

namespace OHOS {
namespace Rosen {
class RSSubThreadRCD {
public:
    RSSubThreadRCD(){};
    virtual ~RSSubThreadRCD();
    void Start(RenderContext* context);
    void PostTask(const std::function<void()>& task);
private:
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    RenderContext *renderContext_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_RCD_RENDER_RS_SUB_THREAD_H