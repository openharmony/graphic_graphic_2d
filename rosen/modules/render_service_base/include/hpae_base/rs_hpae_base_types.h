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

#ifndef RENDER_SERVICE_BASE_INCLUDE_HPAE_BASE_TYPES_H
#define RENDER_SERVICE_BASE_INCLUDE_HPAE_BASE_TYPES_H

#include "draw/surface.h"
#include "image/gpu_context.h"

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;

using HpaeBufferInfo = struct HpaeBufferInfo_ {
    std::shared_ptr<Drawing::Surface> surface = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> canvas = nullptr;
    std::shared_ptr<Drawing::GPUContext> grContext = nullptr;
    void* bufferHandle = nullptr;
    std::shared_ptr<Drawing::Image> snapshot = nullptr;

    void operator=(const HpaeBufferInfo_ &hpaeBufferInfo) {
        this->surface = hpaeBufferInfo.surface;
        this->canvas = hpaeBufferInfo.canvas;
        this->grContext = hpaeBufferInfo.grContext;
        this->bufferHandle = hpaeBufferInfo.bufferHandle;
        this->snapshot = hpaeBufferInfo.snapshot;
    }
};

using HpaeTask = struct {
    uint32_t taskId;
    void *taskPtr; // task function
};

struct HpaeBackgroundCacheItem {
    std::shared_ptr<Drawing::Surface> surface_;
    std::shared_ptr<Drawing::Image> blurImage_;
    HpaeTask hpaeTask_; // id for this cache item
    uint64_t gpFrameId_ = 0;
    float radius_ = 0;
    bool useCache_ = false;
};

} // Rosen
} // OHOS

#endif // RENDER_SERVICE_BASE_INCLUDE_HPAE_BASE_TYPES_H