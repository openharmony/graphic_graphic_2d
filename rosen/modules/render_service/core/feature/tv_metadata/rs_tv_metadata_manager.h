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

#ifndef RS_TV_METADATA_MANAGER_H
#define RS_TV_METADATA_MANAGER_H

#include <shared_mutex>
#include "metadata_helper.h"
#include "platform/ohos/rs_surface_ohos.h"
#include "hdi_layer.h"

namespace OHOS::Rosen {
class RSTvMetadataManager {
public:
    static RSTvMetadataManager& Instance();
    static void CopyFromLayersToSurface(const std::vector<LayerInfoPtr>& layers,
        std::shared_ptr<RSSurfaceOhos>& surface);

    void RecordAndCombineMetadata(const TvPQMetadata& metadata);
    void CopyTvMetadataToSurface(std::shared_ptr<RSSurfaceOhos>& surface);
    void Reset();
    void ResetDpPixelFormat();
    TvPQMetadata GetMetadata() const;

private:
    static void CombineMetadata(TvPQMetadata& dstMetadata, const TvPQMetadata& srcMetadata);
    RSTvMetadataManager() = default;
    ~RSTvMetadataManager() = default;
    RSTvMetadataManager(const RSTvMetadataManager&) = delete;
    RSTvMetadataManager& operator =(const RSTvMetadataManager&) = delete;

    TvPQMetadata metadata_;
    mutable std::mutex mutex_;
};
} // namespace OHOS::Rosen
#endif // RS_TV_METADATA_MANAGER_H
