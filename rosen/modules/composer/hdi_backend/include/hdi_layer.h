/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef HDI_BACKEND_HDI_LAYER_H
#define HDI_BACKEND_HDI_LAYER_H

#include <surface.h>
#include <surface_buffer.h>

#include "surface_type.h"
#include "display_type.h"

#include "hdi_layer_info.h"

namespace OHOS {
namespace Rosen {

using LayerInfoPtr = std::shared_ptr<HdiLayerInfo>;

class HdiLayer {
public:
    explicit HdiLayer(uint32_t screenId);
    virtual ~HdiLayer();

    static constexpr int FRAME_RECORDS_NUM = 128;

    /* output create and set layer info */
    static std::shared_ptr<HdiLayer> CreateHdiLayer(uint32_t screenId);

    bool Init(const LayerInfoPtr &layerInfo);
    void ReleaseBuffer();
    void MergeWithFramebufferFence(const sptr<SyncFence> &fbAcquireFence);
    void MergeWithLayerFence(const sptr<SyncFence> &layerReleaseFence);
    void UpdateCompositionType(CompositionType type);

    const LayerInfoPtr& GetLayerInfo();
    void SetLayerStatus(bool inUsing);
    bool GetLayerStatus() const;
    void UpdateLayerInfo(const LayerInfoPtr &layerInfo);
    void SetHdiLayerInfo();
    uint32_t GetLayerId() const;
    void RecordPresentTime(const sptr<SyncFence> &fbFence);
    void Dump(std::string &result);

    int32_t SetLayerColorTransform(const float *matrix) const;
    int32_t SetLayerColorDataSpace(ColorDataSpace colorSpace) const;
    int32_t GetLayerColorDataSpace(ColorDataSpace &colorSpace) const;
    int32_t SetLayerMetaData(const std::vector<HDRMetaData> &metaData) const;
    int32_t SetLayerMetaDataSet(HDRMetadataKey key, const std::vector<uint8_t> &metaData) const;
private:
    // layer buffer & fence
    class LayerBufferInfo : public RefBase {
    public:
        LayerBufferInfo() = default;
        virtual ~LayerBufferInfo() = default;

        sptr<SurfaceBuffer> sbuffer_ = nullptr;
        sptr<SyncFence> acquireFence_ = SyncFence::INVALID_FENCE;
        sptr<SyncFence> releaseFence_ = SyncFence::INVALID_FENCE;
    };

    struct PresentTimeRecord {
        int64_t presentTime = 0;
        sptr<SyncFence> presentFence = SyncFence::INVALID_FENCE;
    };

    PresentTimeRecord presentTimeRecords[FRAME_RECORDS_NUM] = {};
    uint32_t count = 0;
    uint32_t screenId_ = INT_MAX;
    uint32_t layerId_ = INT_MAX;
    bool isInUsing_ = false;
    sptr<LayerBufferInfo> currSbuffer_ = nullptr;
    sptr<LayerBufferInfo> prevSbuffer_ = nullptr;
    LayerInfoPtr layerInfo_ = nullptr;

    void CloseLayer();
    int32_t CreateLayer(const LayerInfoPtr &layerInfo);
    sptr<SyncFence> Merge(const sptr<SyncFence> &fence1, const sptr<SyncFence> &fence2);
    SurfaceError ReleasePrevBuffer();

    inline void CheckRet(int32_t ret, const char* func);
};
} // namespace Rosen
} // namespace OHOS

#endif // HDI_BACKEND_HDI_LAYER_H