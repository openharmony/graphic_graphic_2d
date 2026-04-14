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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_EXTERNER_DEPEND_TV_META_DATA_RS_TV_METADATA_UTIL_H
#define RENDER_SERVICE_COMPOSER_SERVICE_EXTERNER_DEPEND_TV_META_DATA_RS_TV_METADATA_UTIL_H

#include <shared_mutex>
#include "metadata_helper.h"
#include "platform/ohos/rs_surface_ohos.h"
#include "hdi_layer.h"

namespace OHOS::Rosen {
class RSTvMetadataUtil {
public:
    static void CopyFromLayersToSurface(const std::vector<RSLayerPtr>& layers,
        std::shared_ptr<RSSurfaceOhos>& surface);
    static void CombineMetadataForAllLayers(const std::vector<RSLayerPtr>& layers);
    static void CombineMetadata(TvPQMetadata& dstMetadata, const TvPQMetadata& srcMetadata);
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_COMPOSER_SERVICE_EXTERNER_DEPEND_TV_META_DATA_RS_TV_METADATA_UTIL_H
