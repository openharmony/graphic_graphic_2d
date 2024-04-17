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

#ifndef RENDER_SERVICE_DRAWABLE_DFX_RS_SKP_CAPTURE_DFX_H
#define RENDER_SERVICE_DRAWABLE_DFX_RS_SKP_CAPTURE_DFX_H

#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_recording_canvas.h"
#include "render_context/render_context.h"

namespace OHOS::Rosen {

class RSSkpCaptureDfx {
public:
    RSSkpCaptureDfx(std::shared_ptr<RSPaintFilterCanvas>& curCanvas) : curCanvas_(curCanvas)
    {
        TryCapture();
    }
    ~RSSkpCaptureDfx()
    {
        EndCapture();
    }

private:
    void TryCapture() const;
    void EndCapture() const;

    mutable std::shared_ptr<RSPaintFilterCanvas> curCanvas_;
    mutable std::shared_ptr<ExtendRecordingCanvas> recordingCanvas_;
};

} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_DFX_RS_SKP_CAPTURE_DFX_H