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

#include "benchmarks/rs_recording_thread.h"
#include "rs_trace.h"

#include "pipeline/rs_uni_render_thread.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS::Rosen {

void RSSkpCaptureDfx::TryCapture() const
{
    if (!RSSystemProperties::GetRecordingEnabled() || !curCanvas_) {
        return;
    }
    recordingCanvas_ = std::make_shared<ExtendRecordingCanvas>(curCanvas_->GetWidth(), curCanvas_->GetHeight());
    RS_TRACE_NAME("RSDisplayRenderNodeDrawable:Recording begin");
    std::shared_ptr<RenderContext> renderContext;
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    renderContext = RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext();
    if (!recordingCanvas_) {
        RS_LOGE("TryCapture recordingCanvas_ is nullptr");
        return;
    }
    if (!renderContext) {
        RS_LOGE("TryCapture renderContext is nullptr");
        return;
    }
    recordingCanvas_->SetGrRecordingContext(renderContext->GetSharedDrGPUContext());
#endif
    if (!curCanvas_) {
        RS_LOGE("TryCapture curCanvas_ is nullptr");
        return;
    }
    curCanvas_->AddCanvas(recordingCanvas_.get());
    RSRecordingThread::Instance(renderContext.get()).CheckAndRecording();
}

void RSSkpCaptureDfx::EndCapture() const
{
    auto renderContext = RSUniRenderThread::Instance().GetRenderEngine()->GetRenderContext();
    if (!renderContext) {
        RS_LOGE("EndCapture renderContext is nullptr");
        return;
    }
    if (!RSRecordingThread::Instance(renderContext.get()).GetRecordingEnabled()) {
        return;
    }
    auto drawCmdList = recordingCanvas_->GetDrawCmdList();
    RS_TRACE_NAME("RSUniRender:RecordingToFile curFrameNum = " +
                  std::to_string(RSRecordingThread::Instance(renderContext.get()).GetCurDumpFrame()));
    RSRecordingThread::Instance(renderContext.get()).RecordingToFile(drawCmdList);
}

} // namespace OHOS::Rosen