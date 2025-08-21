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

#ifdef RS_PROFILER_ENABLED

#include "image/picture.h"
#include "utils/document.h"
#include "utils/file_w_stream.h"
#include "utils/picture_recorder.h"
#include "utils/serial_procs.h"
#include "utils/sharing_serial_context.h"

namespace OHOS::Rosen::Drawing {
class Canvas;
} // namespace OHOS::Rosen::Drawing

namespace OHOS::Rosen {

class ExtendRecordingCanvas;

enum class SkpCaptureType { DEFAULT = 0, ON_CAPTURE = 1, EXTENDED = 2, IMG_CACHED = 3 };

class RSCaptureRecorder final {
public:
    static RSCaptureRecorder& GetInstance()
    {
        static RSCaptureRecorder instance;
        return instance;
    }

    bool PrintMessage(std::string message, bool forcedSend = false);

    void SetProfilerEnabled(bool val);

    Drawing::Canvas* TryInstantCapture(float width, float height, SkpCaptureType type = SkpCaptureType::DEFAULT);
    void EndInstantCapture(SkpCaptureType type = SkpCaptureType::DEFAULT);
    // Used for .skp capturing of DrawingCanvasNode's local canvas
    Drawing::Canvas* TryDrawingCanvasCapture(float width, float height, uint64_t nodeId);
    void EndDrawingCanvasCapture();
    // Used for .skp capturing from offscreen Canvas nodes
    Drawing::Canvas* TryOffscreenCanvasCapture(float width, float height);
    void EndOffscreenCanvasCapture();
    // Used for .skp capturing for component screenshot scenario with RSUiCaptureTaskParallel
    Drawing::Canvas* TryComponentScreenshotCapture(float width, float height);
    void EndComponentScreenshotCapture();

    void InvalidateDrawingCanvasNodeId();
    void SetDrawingCanvasNodeId(uint64_t nodeId);
    void SetComponentScreenshotFlag(bool flag);

    void SetCaptureType(SkpCaptureType type);
    void SetCaptureTypeClear(bool flag);

    // to check if .rdc is recorded and send the filename to client
    static bool PullAndSendRdc();
    std::pair<uint32_t, uint32_t> GetDirtyRect(uint32_t displayWidth, uint32_t displayHeight);
    bool IsRecordingEnabled();

private:
    RSCaptureRecorder();
    ~RSCaptureRecorder();

    void InitMSKP();
    ExtendRecordingCanvas* TryInstantCaptureDrawing(float width, float height);
    void EndInstantCaptureDrawing();
    Drawing::Canvas* TryInstantCaptureSKP(float width, float height);
    void EndInstantCaptureSKP();
    Drawing::Canvas* TryCaptureMSKP(float width, float height);
    void EndCaptureMSKP();

    // used for .rdc capturing
    std::unique_ptr<ExtendRecordingCanvas> recordingCanvas_;
    // used for .skp capturing
    std::unique_ptr<Drawing::PictureRecorder> recorder_;
    std::shared_ptr<Drawing::Canvas> recordingSkpCanvas_;
    // to make sure the capture start/finish happen for the same frame / thread
    bool recordingTriggeredFullFrame_ = false;
    bool recordingTriggeredDrawingCanvas_ = false;
    bool recordingTriggeredOffscreenCanvas_ = false;
    bool recordingTriggeredComponentScreenshot_ = false;

    std::shared_ptr<Drawing::Picture> picture_;
    std::unique_ptr<Drawing::SharingSerialContext> serialContext_;
    std::unique_ptr<Drawing::FileWStream> openMultiPicStream_;
    std::shared_ptr<Drawing::Document> multiPic_;
    int32_t mskpMaxLocal_ = 0;
    int32_t mskpIdxCurrent_ = -1;
    int32_t mskpIdxNext_ = -1;
    bool isPageActive_ = false;
    bool isMskpActive_ = false;
    uint64_t drawingCanvasNodeId_ = 0;
    bool isComponentScreenshot_ = false;
    bool captureTypeToClear_ = false;
    bool profilerEnabled_ = false;

    static bool testingTriggering_;
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_ENABLED

#endif // RS_PROFILER_CAPTURE_RECORDER_H