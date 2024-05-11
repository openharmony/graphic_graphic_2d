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

#ifndef RS_LAYER_COMPOSE_COLLECTION_H
#define RS_LAYER_COMPOSE_COLLECTION_H

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {

struct LayerComposeInfo {
    int32_t uniformRenderFrameNumber = 0;
    int32_t offlineComposeFrameNumber = 0;
    int32_t redrawFrameNumber = 0;
    LayerComposeInfo()
        : uniformRenderFrameNumber(), offlineComposeFrameNumber(), redrawFrameNumber() {}
    LayerComposeInfo(int32_t uniformRenderFrameNumber_, int32_t offlineComposeFrameNumber_,
        int32_t redrawFrameNumber_)
        : uniformRenderFrameNumber(uniformRenderFrameNumber_), offlineComposeFrameNumber(offlineComposeFrameNumber_),
          redrawFrameNumber(redrawFrameNumber_) {}
};

class RSB_EXPORT LayerComposeCollection {
public:
    static LayerComposeCollection& GetInstance();

    void UpdateUniformOrOfflineComposeFrameNumberForDFX(size_t layerSize);
    void UpdateRedrawFrameNumberForDFX();
    LayerComposeInfo GetLayerComposeInfo() const;
    void ResetLayerComposeInfo();

private:
    LayerComposeCollection();
    ~LayerComposeCollection() noexcept;
    LayerComposeCollection(const LayerComposeCollection&) = delete;
    LayerComposeCollection(const LayerComposeCollection&&) = delete;
    LayerComposeCollection& operator=(const LayerComposeCollection&) = delete;
    LayerComposeCollection& operator=(const LayerComposeCollection&&) = delete;

    LayerComposeInfo layerComposeInfo_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_LAYER_COMPOSE_COLLECTION_H