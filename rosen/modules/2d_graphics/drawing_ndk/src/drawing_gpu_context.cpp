/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "drawing_gpu_context.h"
#ifdef RS_ENABLE_GPU
#include "drawing_gpu_context_manager.h"
#endif

#include "image/gpu_context.h"
#include "utils/log.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

OH_Drawing_GpuContext* OH_Drawing_GpuContextCreateFromGL(OH_Drawing_GpuContextOptions ops)
{
#ifdef RS_ENABLE_GPU
    GPUContextOptions contextOps;
    contextOps.SetAllowPathMaskCaching(ops.allowPathMaskCaching);
    std::shared_ptr<GPUContext> context = std::make_shared<GPUContext>();
    bool isSuccess = context->BuildFromGL(contextOps);
    if (isSuccess) {
        DrawingGpuContextManager::GetInstance().Insert(context.get(), context);
        return (OH_Drawing_GpuContext*)(context.get());
    }
#endif
    return nullptr;
}

OH_Drawing_GpuContext* OH_Drawing_GpuContextCreate()
{
#ifdef RS_ENABLE_GPU
    std::shared_ptr<GPUContext> context = DrawingGpuContextManager::GetInstance().CreateDrawingContext();
    if (context == nullptr) {
        LOGE("OH_Drawing_GpuContextCreate: create gpuContext failed.");
        return nullptr;
    }

    DrawingGpuContextManager::GetInstance().Insert(context.get(), context);
    return (OH_Drawing_GpuContext*)(context.get());
#endif
    return nullptr;
}

void OH_Drawing_GpuContextDestroy(OH_Drawing_GpuContext* cGpuContext)
{
    if (cGpuContext == nullptr) {
        return;
    }
#ifdef RS_ENABLE_GPU
    DrawingGpuContextManager::GetInstance().Remove(cGpuContext);
#endif
}
