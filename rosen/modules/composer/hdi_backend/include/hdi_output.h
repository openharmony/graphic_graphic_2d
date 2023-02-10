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

#ifndef HDI_BACKEND_HDI_OUTPUT_H
#define HDI_BACKEND_HDI_OUTPUT_H

#include <array>
#include <stdint.h>
#include <vector>
#include <unordered_map>

#include "surface_type.h"
#include "hdi_layer.h"
#include "hdi_framebuffer_surface.h"
#include "hdi_screen.h"

namespace OHOS {
namespace Rosen {

using LayerPtr = std::shared_ptr<HdiLayer>;
static constexpr uint32_t LAYER_COMPOSITION_CAPACITY_INVALID = 0;

// dump layer
struct LayerDumpInfo {
    uint64_t surfaceId;
    LayerPtr layer;
};

class HdiOutput {
public:
    HdiOutput(uint32_t screenId);
    virtual ~HdiOutput();

    static constexpr uint32_t COMPOSITION_RECORDS_NUM = 128;

    /* for RS begin */
    void SetLayerInfo(const std::vector<LayerInfoPtr> &layerInfos);
    void SetOutputDamage(uint32_t num, const IRect &outputDamage);
    uint32_t GetScreenId() const;
    void SetLayerCompCapacity(uint32_t layerCompositionCapacity);
    uint32_t GetLayerCompCapacity() const;
    /* for RS end */

    static std::shared_ptr<HdiOutput> CreateHdiOutput(uint32_t screenId);
    RosenError Init();
    const std::unordered_map<uint32_t, LayerPtr>& GetLayers();
    IRect& GetOutputDamage();
    uint32_t GetOutputDamageNum() const;
    sptr<Surface> GetFrameBufferSurface();
    std::unique_ptr<FrameBufferEntry> GetFramebuffer();
    int32_t ReleaseFramebuffer(
        sptr<SurfaceBuffer> &buffer, const sptr<SyncFence>& releaseFence);

    void Dump(std::string &result) const;
    void DumpFps(std::string &result, const std::string &arg) const;
    void ClearFpsDump(std::string &result, const std::string &arg);
    void RecordCompositionTime(int64_t timeStamp);
    void SetDirectClientCompEnableStatus(bool enableStatus);
    bool GetDirectClientCompEnableStatus() const;
    void UpdatePrevLayerInfo();

private:
    std::array<int64_t, COMPOSITION_RECORDS_NUM> compositionTimeRecords_ = {};
    uint32_t compTimeRcdIndex_ = 0;
    sptr<HdiFramebufferSurface> fbSurface_ = nullptr;
    // layerId -- layer ptr
    std::unordered_map<uint32_t, LayerPtr> layerIdMap_;
    // surface unique id -- layer ptr
    std::unordered_map<uint64_t, LayerPtr> surfaceIdMap_;
    uint32_t screenId_;
    uint32_t layerCompCapacity_ = LAYER_COMPOSITION_CAPACITY_INVALID;
    IRect outputDamage_;
    uint32_t outputDamageNum_;
    bool directClientCompositionEnabled_ = true;

    int32_t CreateLayer(uint64_t surfaceId, const LayerInfoPtr &layerInfo);
    void DeletePrevLayers();
    void ResetLayerStatus();
    void ReorderLayerInfo(std::vector<LayerDumpInfo> &dumpLayerInfos) const;

    inline bool CheckFbSurface();
};
} // namespace Rosen
} // namespace OHOS

#endif // HDI_BACKEND_HDI_OUTPUT_H