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

#ifndef RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_LAYER_COMMON_DEF_H
#define RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_LAYER_COMMON_DEF_H

#include "hdi_display_type.h"
#include "surface_type.h"

namespace OHOS {
namespace Rosen {

using RSLayerId = uint64_t;
using RSLayerPropertyId = uint64_t;

enum class LayerMask : uint16_t {
    LAYER_MASK_NORMAL = 0,
    LAYER_MASK_HBM_SYNC = 1,
};
}
}
#endif // RENDER_SERVICE_COMPOSER_BASE_RENDER_LAYER_RS_LAYER_COMMON_DEF_H
