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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_LAYER_COMMON_DEF_H
#define RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_LAYER_COMMON_DEF_H

#include "hdi_display_type.h"
#include "surface_type.h"

namespace OHOS {
namespace Rosen {

using RSLayerId = uint64_t;
using RSLayerPropertyId = uint64_t;

enum RSLayerParcelType : uint16_t {
    RS_LAYER_DESTROY = 0x0001,
    RS_LAYER_UPDATE,
    RS_RCD_LAYER_UPDATE,
};

enum class LayerMask : uint16_t {
    LAYER_MASK_NORMAL = 0,
    LAYER_MASK_HBM_SYNC = 1,
};

using GraphicSolidColorLayerProperty = struct GraphicSolidColorLayerProperty {
    int32_t zOrder = 0;
    GraphicTransformType transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    GraphicIRect layerRect = { 0, 0, 0, 0 };
    GraphicCompositionType compositionType = GraphicCompositionType::GRAPHIC_COMPOSITION_BUTT;
    GraphicLayerColor layerColor = { 0, 0, 0, 0 };

    bool operator==(const GraphicSolidColorLayerProperty& solidColorLayerProperty) const
    {
        return (zOrder == solidColorLayerProperty.zOrder) &&
            (transformType == solidColorLayerProperty.transformType) &&
            (layerRect == solidColorLayerProperty.layerRect) &&
            (compositionType == solidColorLayerProperty.compositionType) &&
            (layerColor == solidColorLayerProperty.layerColor);
    }
};
}
}
#endif // RENDER_SERVICE_COMPOSER_SERVICE_TRANSACTION_COMMAND_RS_LAYER_COMMON_DEF_H
