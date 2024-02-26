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
#include <cstring>

#include "rs_profiler_capture_recorder.h"

#ifndef REPLAY_TOOL_CLIENT
#include "rs_profiler_base.h"
#include "platform/common/rs_log.h"
#else
#include "../rs_log.h"
#endif

namespace OHOS::Rosen {

Drawing::Canvas* RSCaptureRecorder::TryInstantCapture(float width, float height)
{
    if (RSSystemProperties::GetSaveRDC()) {
        // for saving .drawing file
        return TryInstantCaptureDrawing(width, height);
    }
    // for saving .skp file
    return TryInstantCaptureSKP(width, height);
}

void RSCaptureRecorder::EndInstantCapture()
{
    if (RSSystemProperties::GetSaveRDC()) {
        // for saving .drawing file
        EndInstantCaptureDrawing();
        return;
    }
    // for saving .skp file
    EndInstantCaptureSKP();
}

ExtendRecordingCanvas* RSCaptureRecorder::TryInstantCaptureDrawing(float width, float height)
{
    if (!RSSystemProperties::GetInstantRecording()) {
        return nullptr;
    }
    recordingCanvas_ = std::make_unique<ExtendRecordingCanvas>(width, height);
    return recordingCanvas_.get();
}

void RSCaptureRecorder::EndInstantCaptureDrawing()
{
    if (!RSSystemProperties::GetInstantRecording()) {
        return;
    }
    auto drawCmdList = recordingCanvas_->GetDrawCmdList();

    std::shared_ptr<MessageParcel> messageParcel = std::make_shared<MessageParcel>();
    messageParcel->SetMaxCapacity(recordingParcelCapacity_);
    RSMarshallingHelper::BeginNoSharedMem(std::this_thread::get_id());
    RSMarshallingHelper::Marshalling(*messageParcel, drawCmdList);
    RSMarshallingHelper::EndNoSharedMem();

    size_t parcelSize = messageParcel->GetDataSize();
    uintptr_t parcelBuf = messageParcel->GetData();

    // Create file and write the parcel
    FILE *f = fopen(drawCmdListFilename_.c_str(), "wbe");
    if (f == NULL) {
        RS_LOGE("Error while writing to file");
    } else {
        Utils::FileWrite(reinterpret_cast<uint8_t *>(parcelBuf), sizeof(uint8_t), parcelSize, f);
        fclose(f);
    }
    
    const size_t minPacketSize = 2;
    char* buf = new char[minPacketSize];
    buf[0] = static_cast<char>(OHOS::Rosen::PackageID::RS_PROFILER_DCL_BINARY);
    buf[1] = 't';
    OHOS::Rosen::Network::SendBinary(buf, minPacketSize);
    
    OHOS::Rosen::Network::SendMessage("Saved locally");
    RSSystemProperties::SetInstantRecording(false);
}

Drawing::Canvas* RSCaptureRecorder::TryInstantCaptureSKP(float width, float height)
{
    if (!RSSystemProperties::GetInstantRecording()) {
        return nullptr;
    }
    OHOS::Rosen::Network::SendMessage("Starting .skp capturing.");

    recordingSkpCanvas_ = std::make_shared<Drawing::Canvas>(width, height);
    skRecorder_.reset();
    skRecorder_ = std::make_unique<SkPictureRecorder>();
    skiaCanvas_ = skRecorder_->beginRecording(width, height);

    recordingSkpCanvas_->GetImpl<Drawing::SkiaCanvas>()->ImportSkCanvas(skiaCanvas_);
    return recordingSkpCanvas_.get();
}

void RSCaptureRecorder::EndInstantCaptureSKP()
{
    if (!RSSystemProperties::GetInstantRecording()) {
        return;
    }
    OHOS::Rosen::Network::SendMessage("Finishing .skp capturing");

    sk_sp<SkPicture> picture = skRecorder_->finishRecordingAsPicture();
    if (picture->approximateOpCount() > 0) {
        SkSerialProcs procs;
        procs.fTypefaceProc = [](SkTypeface* tf, void* ctx) {
            return tf->serialize(SkTypeface::SerializeBehavior::kDoIncludeData);
        };
        auto data = picture->serialize(&procs);

        // to send to client as binary
        const int size = 1 + data->size();
        char* buf = new char[size];
        buf[0] = static_cast<char>(OHOS::Rosen::PackageID::RS_PROFILER_SKP_BINARY);
        memcpy_s(&buf[1], data->size(), data->data(), data->size());
        
        OHOS::Rosen::Network::SendMessage("SKP file sending");
        OHOS::Rosen::Network::SendBinary(buf, size);
        OHOS::Rosen::Network::SendMessage("SKP file sent");
        delete [] buf;
    }
    RSSystemProperties::SetInstantRecording(false);
}
} // namespace OHOS::Rosen