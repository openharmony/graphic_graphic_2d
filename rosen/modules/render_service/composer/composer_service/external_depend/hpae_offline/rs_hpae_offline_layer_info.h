/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#ifndef RS_HPAE_OFFLINE_LAYER_INFO_H
#define RS_HPAE_OFFLINE_LAYER_INFO_H

#include "pipeline/rs_surface_handler.h"
#include "surface_buffer.h"
#include "sync_fence.h"
 
namespace OHOS {
namespace Rosen {
 
struct HpaeOriginalInfo {
    sptr<SurfaceBuffer> originalBuffer = nullptr;
    sptr<SurfaceBuffer> originalPreBuffer = nullptr;
    sptr<SyncFence> originalAcquireFence = nullptr;
    GraphicTransformType originalTransformType = GraphicTransformType::GRAPHIC_ROTATE_BUTT;
    GraphicIRect originalCropRect = {0, 0, 0, 0};
 
    bool operator==(const HpaeOriginalInfo& other) const
    {
        return originalBuffer == other.originalBuffer &&
            originalPreBuffer == other.originalPreBuffer &&
            originalAcquireFence == other.originalAcquireFence &&
            originalTransformType == other.originalTransformType &&
            originalCropRect == other.originalCropRect;
    }
};

} // namespace Rosen
} // namespace OHOS
 
#endif // RS_HPAE_OFFLINE_LAYER_INFO_H