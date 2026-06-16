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

#ifndef RS_CORE_FEATURE_HWC_GPU_OFFLINE_BUFFER_H
#define RS_CORE_FEATURE_HWC_GPU_OFFLINE_BUFFER_H

#include <string>

#include "common/rs_common_def.h"
#include "feature/hwc/hpae_offline/rs_offline_util.h"
#include "engine/rs_base_render_engine.h"
#include "pipeline/rs_surface_handler.h"

namespace OHOS {
namespace Rosen {

struct NativeWindowBufferBaseInfo {
    OHOS::sptr<OHOS::SurfaceBuffer> sfbuffer = nullptr;
    int64_t uiTimestamp = 0;
    ~NativeWindowBufferBaseInfo()
    {
        sfbuffer = nullptr;
    }
};

struct NativeWindowBufferInfo : public Parcelable {
    NativeWindowBufferInfo(bool useSeq) : mUseAutoSeq(useSeq) {};
    NativeWindowBufferInfo(NativeWindowBufferBaseInfo &info) : baseInfo(info) {};
    NativeWindowBufferBaseInfo baseInfo;
    bool mUseAutoSeq = false;

    ~NativeWindowBufferInfo()
    {
        baseInfo.sfbuffer = nullptr;
        baseInfo.uiTimestamp = 0;
        mUseAutoSeq = false;
    }

    bool ReadFromParcel(MessageParcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static NativeWindowBufferInfo *Unmarshalling(Parcel &parcel, bool autoSeq = false);
};

class RSGPUOfflineBuffer {
public:
    RSGPUOfflineBuffer(const std::string& name, NodeId layerId);
    ~RSGPUOfflineBuffer();

    std::unique_ptr<RSRenderFrame> RequestFrame(std::shared_ptr<RSBaseRenderEngine> renderEngine,
        const BufferRequestConfig& config, bool isHebc);
    sptr<SurfaceBuffer> ConsumeAndGetBuffer();
    std::shared_ptr<RSSurfaceHandler> GetSurfaceHandler();
    void CleanCache(bool cleanAll);
    std::shared_ptr<RSSurfaceOhos> GetRSSurfaceOhos();
private:
    bool PreAllocBuffers(const BufferRequestConfig& config);
    bool ConvertColorGamutToSpaceType(const GraphicColorGamut& colorGamut,
        HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceType& colorSpaceType);
    bool CreateSurface(sptr<IBufferConsumerListener> listener);
    std::shared_ptr<Drawing::GPUContext> CreateShareGrContext();
    // for single buffer
    bool AttachSingleBuffer(std::unique_ptr<RSRenderFrame>& renderFrame);
    bool CopyAndAttachBufferToQueue(NativeWindowBufferInfo& info,
        OHOS::sptr<OHOS::Surface>& producerSurface, uint64_t bufferQueueId);

    sptr<Surface> pSurface_ = nullptr;
    std::shared_ptr<RSSurface> rsSurface_ = nullptr;
    std::shared_ptr<RSSurfaceHandler> surfaceHandler_;
    bool surfaceCreated_ = false;
    std::string layerName_;
    uint32_t bufferSize_ = 4;
    bool isFirstRequest_ = true;
    BufferRequestConfig currentConfig_{
        .width = 0,
        .height = 0,
        .strideAlignment = 0,
        .format = 0,
        .usage = 0,
        .timeout = 0,
    };
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_CORE_FEATURE_HWC_GPU_OFFLINE_BUFFER_H