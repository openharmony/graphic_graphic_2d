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

#ifndef RS_PROFILER_CAPTURE_RECORDER_H
#define RS_PROFILER_CAPTURE_RECORDER_H

#include <cstdio>
#include <cstdlib>

#include "platform/common/rs_system_properties.h"
#include "transaction/rs_marshalling_helper.h"
#include "benchmarks/file_utils.h"
#include "common/rs_common_def.h"
#include "draw/canvas.h"
#include "drawing/engine_adapter/skia_adapter/skia_canvas.h"
#include "pipeline/rs_recording_canvas.h"

#include "include/core/SkPictureRecorder.h"
#include "include/core/SkPicture.h"
#include "include/core/SkSerialProcs.h"
#include "include/utils/SkNWayCanvas.h"

#include "rs_profiler_network.h"
#include "rs_profiler_packet.h"
#include "rs_profiler_utils.h"

namespace OHOS::Rosen {

class RSCaptureRecorder final {
public:
    RSCaptureRecorder() = default;
    ~RSCaptureRecorder() = default;
    
    Drawing::Canvas* TryInstantCapture(float width, float height);
    void EndInstantCapture();
private:
    ExtendRecordingCanvas* TryInstantCaptureDrawing(float width, float height);
    void EndInstantCaptureDrawing();
    Drawing::Canvas* TryInstantCaptureSKP(float width, float height);
    void EndInstantCaptureSKP();

    const static size_t RECORDING_PARCEL_CAPCITY = 234 * 1000 * 1024;
    // used for .rdc capturing
    const std::string drawCmdListFilename_ = "/data/default.drawing";
    std::unique_ptr<ExtendRecordingCanvas> recordingCanvas_;
    // used for .skp capturing
    std::unique_ptr<SkPictureRecorder> skRecorder_;
    std::unique_ptr<SkNWayCanvas> nwayCanvas_;
    SkCanvas* skiaCanvas_ = nullptr; // used for holding a pointer returned from SkPictureRecorder::beginRecording(...)
    std::shared_ptr<Drawing::Canvas> recordingSkpCanvas_;
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_CAPTURE_RECORDER_H