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


#include "pipeline/rs_software_processor.h"

#include <cinttypes>
#include <securec.h>
#include "rs_trace.h"
#include "sync_fence.h"

#include "include/core/SkMatrix.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_util.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"

namespace OHOS {
namespace Rosen {

RSSoftwareProcessor::RSSoftwareProcessor() {}

RSSoftwareProcessor::~RSSoftwareProcessor() {}

void RSSoftwareProcessor::Init(ScreenId id, int32_t offsetX, int32_t offsetY)
{
    offsetX_ = offsetX;
    offsetY_ = offsetY;
    sptr<RSScreenManager> screenManager = CreateOrGetScreenManager();
    if (screenManager == nullptr) {
        RS_LOGE("RSSoftwareProcessor::Init: failed to get screen manager!");
        return;
    }

    producerSurface_ = screenManager->GetProducerSurface(id);
    if (producerSurface_ == nullptr) {
        RS_LOGE("RSSoftwareProcessor::Init for Screen(id %{public}" PRIu64 "): ProducerSurface is null!", id);
        return;
    }

    rotation_ = screenManager->GetRotation(id);

    currScreenInfo_ = screenManager->QueryScreenInfo(id);
    BufferRequestConfig requestConfig = {
        .width = currScreenInfo_.width,
        .height = currScreenInfo_.height,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    rsSurface_ = std::make_shared<RSSurfaceOhosRaster>(producerSurface_);
    auto skCanvas = CreateCanvas(rsSurface_, requestConfig);
    if (skCanvas == nullptr) {
        RS_LOGE("RSSoftwareProcessor canvas is null.");
        return;
    }
    canvas_ = std::make_unique<RSPaintFilterCanvas>(skCanvas);
}

void RSSoftwareProcessor::PostProcess()
{
    SetBufferTimeStamp();
    if (!rsSurface_ || !currFrame_) {
        RS_LOGE("RSSoftwareProcessor::PostProcess rsSurface_ is null.");
        return;
    }
    rsSurface_->FlushFrame(currFrame_);
}

void RSSoftwareProcessor::ProcessSurface(RSSurfaceRenderNode& node)
{
    if (!canvas_) {
        RS_LOGE("RSSoftwareProcessor::ProcessSurface: Canvas is null!");
        return;
    }
    if (node.GetBuffer() == nullptr) {
        return;
    }

    std::string inf;
    char strBuffer[UINT8_MAX] = { 0 };
    if (sprintf_s(strBuffer, UINT8_MAX, "ProcessSurfaceNode:%s ", node.GetName().c_str()) != -1) {
        inf.append(strBuffer);
    }
    RS_TRACE_NAME(inf.c_str());

    if (!GenerateParamAndDrawBuffer(node)) {
        return;
    }
}

bool RSSoftwareProcessor::GenerateParamAndDrawBuffer(RSSurfaceRenderNode& node)
{
    auto geoPtr = std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry());
    if (geoPtr == nullptr) {
        RS_LOGE("RsDebug RSSoftwareProcessor::ProcessSurface geoPtr == nullptr");
        return false;
    }
    node.SetDstRect({geoPtr->GetAbsRect().left_ - offsetX_, geoPtr->GetAbsRect().top_ - offsetY_,
        geoPtr->GetAbsRect().width_, geoPtr->GetAbsRect().height_});
    BufferDrawParam params;
    if (GetMirror()) {
        RS_LOGI("RSSoftwareProcessor::ProcessSurface mirrorScreen is not support rotation");
        params = RsRenderServiceUtil::CreateBufferDrawParam(node);
    } else {
        params = RsRenderServiceUtil::CreateBufferDrawParam(node, currScreenInfo_.rotationMatrix, rotation_);
    }
    RsRenderServiceUtil::DrawBuffer(*canvas_, params);
    return true;
}
} // namespace Rosen
} // namespace OHOS
