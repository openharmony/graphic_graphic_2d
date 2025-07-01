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

#include "drawable/dfx/rs_skp_capture_dfx.h"

#include "rs_trace.h"

#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

#ifdef RS_PROFILER_ENABLED
#include "rs_profiler_capture_recorder.h"
#endif

namespace OHOS::Rosen {

void RSSkpCaptureDfx::TryCapture() const
{
    if (!curCanvas_) {
        RS_LOGE("TryCapture curCanvas_ is nullptr");
        return;
    }
    if (!RSSystemProperties::GetRecordingEnabled()) {
#ifdef RS_PROFILER_ENABLED
        const auto width = static_cast<float>(curCanvas_->GetWidth());
        const auto height = static_cast<float>(curCanvas_->GetHeight());
        if (auto canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(width, height, SkpCaptureType::DEFAULT)) {
            curCanvas_->AddCanvas(canvas);
        }
#endif
        return;
    } else {
        RS_LOGE("TryCapture failed, the recording parameters have not been set.");
    }
}

void RSSkpCaptureDfx::EndCapture() const
{
#ifdef RS_ENABLE_GPU
    auto renderContext = RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext();
    if (!renderContext) {
        RS_LOGE("EndCapture renderContext is nullptr");
        return;
    }
#endif
#ifdef RS_PROFILER_ENABLED
    RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::DEFAULT);
#endif
    return;
}

} // namespace OHOS::Rosen