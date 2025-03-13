/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef DRAWING_GPU_CONTEXT_MANAGER_H
#define DRAWING_GPU_CONTEXT_MANAGER_H

#include <mutex>
#include <unordered_map>

#include "render_context/render_context.h"

namespace OHOS {
namespace Rosen {

class DrawingGpuContextManager {
public:
    DrawingGpuContextManager(const DrawingGpuContextManager&) = delete;
    DrawingGpuContextManager(const DrawingGpuContextManager&&) = delete;
    DrawingGpuContextManager& operator=(const DrawingGpuContextManager&) = delete;
    DrawingGpuContextManager& operator=(const DrawingGpuContextManager&&) = delete;

    static DrawingGpuContextManager& GetInstance();

    std::shared_ptr<RenderContext> GetRenderContext();

    std::shared_ptr<Drawing::GPUContext> CreateDrawingContext();

    void Insert(void* key, std::shared_ptr<Drawing::GPUContext> gpuContext);
    std::shared_ptr<Drawing::GPUContext> Find(void* key);
    bool Remove(void* key);

private:
    DrawingGpuContextManager();

    std::shared_ptr<RenderContext> renderContext_ = nullptr;
    std::unordered_map<void*, std::shared_ptr<Drawing::GPUContext>> gpuContext_;
    std::mutex mutex_;
};
} // namespace Rosen
} // namespace OHOS
#endif // DRAWING_GPU_CONTEXT_MANAGER_H