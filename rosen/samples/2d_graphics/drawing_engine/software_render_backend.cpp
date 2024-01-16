/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "software_render_backend.h"

#include "drawing_utils.h"
#include "surface_ohos_raster.h"

namespace OHOS {
namespace Rosen {
SkCanvas* SoftwareRenderBackend::AcquireSkCanvas(std::unique_ptr<SurfaceFrame>& frame)
{
    SurfaceFrameOhosRaster* framePtr = static_cast<SurfaceFrameOhosRaster*>(frame.get());
    sptr<SurfaceBuffer> buffer = framePtr->GetBuffer();
    if ((buffer == nullptr) || (buffer->GetWidth() <= 0) || (buffer->GetHeight() <= 0)) {
        LOGE("Buffer is invalide");
        return nullptr;
    }

    auto addr = static_cast<uint32_t*>(buffer->GetVirAddr());
    if (addr == nullptr) {
        LOGE("buffer addr is invalid");
        return nullptr;
    }
    SkImageInfo info =
        SkImageInfo::Make(buffer->GetWidth(), buffer->GetHeight(), kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    auto uniqueCanvasPtr = SkCanvas::MakeRasterDirect(info, addr, buffer->GetSize() / buffer->GetHeight());
    skCanvas_ = std::move(uniqueCanvasPtr);

    LOGD("SoftwareRenderBackend::AcquireCanvas canvas");
    return skCanvas_.get();
}
}
}