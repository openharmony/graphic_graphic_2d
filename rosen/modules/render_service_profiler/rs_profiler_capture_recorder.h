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

#include "include/core/SkRefCnt.h"

class SkDocument;
class SkPicture;
class SkPictureRecorder;
class SkFILEWStream;
struct SkSharingSerialContext;

namespace OHOS::Rosen::Drawing {
class Canvas;
} // namespace OHOS::Rosen::Drawing

namespace OHOS::Rosen {

class ExtendRecordingCanvas;

class RSCaptureRecorder final {
public:
    static RSCaptureRecorder& GetInstance()
    {
        static RSCaptureRecorder instance;
        return instance;
    }

    Drawing::Canvas* TryInstantCapture(float width, float height);
    void EndInstantCapture();

    // to check if .rdc is recorded and send the filename to client
    static bool PullAndSendRdc();
    static std::pair<uint32_t, uint32_t> GetDirtyRect(uint32_t displayWidth, uint32_t displayHeight);

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
    std::unique_ptr<SkPictureRecorder> skRecorder_;
    std::shared_ptr<Drawing::Canvas> recordingSkpCanvas_;
    // to make sure the capture start/finish happen for the same frame / thread
    bool recordingTriggered_ = false;

    sk_sp<SkPicture> picture_;

    std::unique_ptr<SkSharingSerialContext> serialContext_;
    std::unique_ptr<SkFILEWStream> openMultiPicStream_;

    sk_sp<SkDocument> multiPic_;
    int32_t mskpMaxLocal_ = 0;
    int32_t mskpIdxCurrent_ = -1;
    int32_t mskpIdxNext_ = -1;
    bool isPageActive_ = false;
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_ENABLED

#endif // RS_PROFILER_CAPTURE_RECORDER_H