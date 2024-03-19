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
#include <filesystem>

#include "rs_profiler_capture_recorder.h"

#ifndef REPLAY_TOOL_CLIENT
#include "rs_profiler.h"
#include "platform/common/rs_log.h"
#else
#include "rs_log.h"
#endif

namespace OHOS::Rosen {

Drawing::Canvas* RSCaptureRecorder::TryInstantCapture(float width, float height)
{
    if (!RSSystemProperties::GetInstantRecording()) {
        return nullptr;
    }
    if (RSSystemProperties::GetSaveRDC()) {
        // for saving .drawing file
        return TryInstantCaptureDrawing(width, height);
    }
    // for saving .skp file
    return TryInstantCaptureSKP(width, height);
}

void RSCaptureRecorder::EndInstantCapture() const
{
    if (!RSSystemProperties::GetInstantRecording()) {
        return;
    }
    if (RSSystemProperties::GetSaveRDC()) {
        // for saving .drawing file
        EndInstantCaptureDrawing();
        return;
    }
    // for saving .skp file
    EndInstantCaptureSKP();
}

std::pair<uint32_t, uint32_t> RSCaptureRecorder::GetDirtyRect(uint32_t displayWidth, uint32_t displayHeight)
{
    if (RSSystemProperties::GetInstantRecording()) {
        return std::pair<uint32_t, uint32_t>(displayWidth, displayHeight);
    }
    return std::pair<uint32_t, uint32_t>(0, 0);
}

bool RSCaptureRecorder::PullAndSendRdc()
{
    const std::string path("/data/autocaps");
    if (!std::filesystem::exists(path)) {
        return false;
    }
    std::vector<std::string> files;
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path)) {
        const std::filesystem::path& path = entry.path();
        if (path.extension() == ".rdc") {
            files.emplace_back(path.generic_string());
        }
    }
    const size_t filesRequired = 1;
    if (files.size() == filesRequired) {
        Network::SendRdcPath(files[0]);
        return true;
    }
    return false;
}

ExtendRecordingCanvas* RSCaptureRecorder::TryInstantCaptureDrawing(float width, float height)
{
    recordingCanvas_ = std::make_unique<ExtendRecordingCanvas>(width, height);
    return recordingCanvas_.get();
}

void RSCaptureRecorder::EndInstantCaptureDrawing() const
{
    auto drawCmdList = recordingCanvas_->GetDrawCmdList();

    const size_t recordingParcelCapacity = 234 * 1000 * 1024;
    std::shared_ptr<MessageParcel> messageParcel = std::make_shared<MessageParcel>();
    messageParcel->SetMaxCapacity(recordingParcelCapacity);
    RSMarshallingHelper::BeginNoSharedMem(std::this_thread::get_id());
    bool marshallingComplete = RSMarshallingHelper::Marshalling(*messageParcel, drawCmdList);
    RSMarshallingHelper::EndNoSharedMem();

    if (!marshallingComplete) {
        RSSystemProperties::SetInstantRecording(false);
        return;
    }

    size_t parcelSize = messageParcel->GetDataSize();
    uintptr_t parcelBuf = messageParcel->GetData();

    // Create file and write the parcel
    const std::string drawCmdListFilename = "/data/default.drawing";
    FILE *f = Utils::FileOpen(drawCmdListFilename, "wbe");
    if (f == nullptr) {
        RSSystemProperties::SetInstantRecording(false);
        return;
    }
    Utils::FileWrite(reinterpret_cast<uint8_t *>(parcelBuf), sizeof(uint8_t), parcelSize, f);
    Utils::FileClose(f);

    Network::SendDclPath(drawCmdListFilename);
    Network::SendMessage("Saved locally");
    RSSystemProperties::SetInstantRecording(false);
}

Drawing::Canvas* RSCaptureRecorder::TryInstantCaptureSKP(float width, float height)
{
    Network::SendMessage("Starting .skp capturing.");

    recordingSkpCanvas_ = std::make_shared<Drawing::Canvas>(width, height);
    skRecorder_.reset();
    skRecorder_ = std::make_unique<SkPictureRecorder>();
    SkCanvas* skiaCanvas = skRecorder_->beginRecording(width, height);
    if (skiaCanvas == nullptr) {
        return nullptr;
    }
    recordingSkpCanvas_->GetImpl<Drawing::SkiaCanvas>()->ImportSkCanvas(skiaCanvas);
    return recordingSkpCanvas_.get();
}

void RSCaptureRecorder::EndInstantCaptureSKP() const
{
    Network::SendMessage("Finishing .skp capturing");

    sk_sp<SkPicture> picture = skRecorder_->finishRecordingAsPicture();
    if (picture != nullptr) {
        if (picture->approximateOpCount() > 0) {
            SkSerialProcs procs;
            procs.fTypefaceProc = [](SkTypeface* tf, void* ctx) {
                return tf->serialize(SkTypeface::SerializeBehavior::kDoIncludeData);
            };
            auto data = picture->serialize(&procs);
            if (data && (data->size() > 0)) {
                Network::SendSkp(data->data(), data->size());
            }
        }
    }
    RSSystemProperties::SetInstantRecording(false);
}
} // namespace OHOS::Rosen