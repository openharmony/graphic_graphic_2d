/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "info_collection/rs_layer_compose_collection.h"

namespace OHOS {
namespace Rosen {
LayerComposeCollection& LayerComposeCollection::GetInstance()
{
    static LayerComposeCollection instance;
    return instance;
}

LayerComposeCollection::LayerComposeCollection()
{
}

LayerComposeCollection::~LayerComposeCollection() noexcept
{
}

void LayerComposeCollection::UpdateUniformOrOfflineComposeFrameNumberForDFX(size_t layerSize)
{
    std::lock_guard<std::mutex> lock(layerMtx_);
    if (layerSize == 1) {
        ++layerComposeInfo_.uniformRenderFrameNumber;
        return;
    }
    ++layerComposeInfo_.offlineComposeFrameNumber;
}

void LayerComposeCollection::UpdateRedrawFrameNumberForDFX()
{
    std::lock_guard<std::mutex> lock(layerMtx_);
    --layerComposeInfo_.offlineComposeFrameNumber;
    ++layerComposeInfo_.redrawFrameNumber;
}

LayerComposeInfo LayerComposeCollection::GetLayerComposeInfo() const
{
    std::lock_guard<std::mutex> lock(layerMtx_);
    return layerComposeInfo_;
}

void LayerComposeCollection::ResetLayerComposeInfo()
{
    std::lock_guard<std::mutex> lock(layerMtx_);
    layerComposeInfo_ = LayerComposeInfo {};
}
} // namespace Rosen
} // namespace OHOS