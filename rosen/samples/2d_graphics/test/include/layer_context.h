/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef LAYER_CONTEXT_H
#define LAYER_CONTEXT_H

#include <display_type.h>
#include <surface.h>

#include "hdi_layer_info.h"
#include "sync_fence.h"

#include "draw/canvas.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum LayerType : uint32_t { LAYER_STATUS, LAYER_LAUNCHER, LAYER_NAVIGATION };

class LayerContext : public IBufferConsumerListenerClazz {
public:
    using TestFunc = std::function<void(Canvas&, uint32_t, uint32_t)>;
    LayerContext(IRect dst, IRect src, uint32_t zorder, LayerType layerType);
    virtual ~LayerContext();

    virtual void OnBufferAvailable() override;
    SurfaceError DrawBuffer(TestFunc testFunc);
    SurfaceError FillHDILayer();
    const std::shared_ptr<HdiLayerInfo> GetHdiLayer();

private:
    IRect dst_;
    IRect src_;
    uint32_t zorder_ = 0;
    sptr<SyncFence> prevFence_;
    OHOS::sptr<Surface> pSurface_;
    OHOS::sptr<Surface> cSurface_;
    OHOS::sptr<SurfaceBuffer> prevBuffer_;
    std::shared_ptr<HdiLayerInfo> hdiLayer_;
    LayerType layerType_ = LayerType::LAYER_STATUS;

    void DrawBaseLayer(void* image, int width, int height, TestFunc testFuc);
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // LAYER_CONTEXT_H