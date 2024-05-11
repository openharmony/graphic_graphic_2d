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
#include "rs_profiler_capture_recorder.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>

#include "benchmarks/file_utils.h"
#include "include/core/SkDocument.h"
#include "include/core/SkPicture.h"
#include "include/core/SkPictureRecorder.h"
#include "include/core/SkSerialProcs.h"
#include "include/core/SkStream.h"
#include "include/utils/SkNWayCanvas.h"
#include "src/utils/SkMultiPictureDocument.h"
#include "tools/SkSharingProc.h"

#include "common/rs_common_def.h"
#include "draw/canvas.h"
#include "drawing/engine_adapter/skia_adapter/skia_canvas.h"
#include "pipeline/rs_recording_canvas.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "transaction/rs_marshalling_helper.h"

#include "rs_profiler.h"
#include "rs_profiler_network.h"
#include "rs_profiler_packet.h"
#include "rs_profiler_settings.h"
#include "rs_profiler_utils.h"

namespace OHOS::Rosen {

static const Int32Parameter MSKP_COUNTER("mskp.counter");
static const Int32Parameter MSKP_MAX("mskp.max");
static const StringParameter MSKP_PATH("mskp.path");

RSCaptureRecorder::RSCaptureRecorder() = default;
RSCaptureRecorder::~RSCaptureRecorder() = default;

Drawing::Canvas* RSCaptureRecorder::TryInstantCapture(float width, float height)
{
    if (!RSSystemProperties::GetInstantRecording()) {
        return nullptr;
    }
    if (RSSystemProperties::GetSaveRDC()) {
        // for saving .drawing file
        recordingTriggered_ = true;
        return TryInstantCaptureDrawing(width, height);
    }
    // for saving .mskp file
    mskpMaxLocal_ = *MSKP_MAX;
    mskpIdxNext_ = *MSKP_COUNTER;

    if (mskpMaxLocal_ > 0) {
        // record next frame, triggered by profiler step
        if (mskpIdxCurrent_ != mskpIdxNext_) {
            recordingTriggered_ = true;
            return TryCaptureMSKP(width, height);
        }
        return nullptr;
    }
    // for saving .skp file
    recordingTriggered_ = true;
    return TryInstantCaptureSKP(width, height);
}

void RSCaptureRecorder::EndInstantCapture()
{
    if (!RSSystemProperties::GetInstantRecording() || !recordingTriggered_) {
        return;
    }
    recordingTriggered_ = false;
    if (RSSystemProperties::GetSaveRDC()) {
        // for saving .drawing file
        EndInstantCaptureDrawing();
        return;
    }
    if (mskpMaxLocal_ > 0) {
        if (isPageActive_) {
            recordingTriggered_ = false;
            return EndCaptureMSKP();
        }
        return;
    }

    // for saving .skp file
    recordingTriggered_ = false;
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

void RSCaptureRecorder::InitMSKP()
{
    auto stream = std::make_unique<SkFILEWStream>((*MSKP_PATH).data());
    if (!stream->isValid()) {
        std::cout << "Could not open " << *MSKP_PATH << " for writing." << std::endl;
        return;
    }
    openMultiPicStream_ = std::move(stream);

    SkSerialProcs procs;
    serialContext_ = std::make_unique<SkSharingSerialContext>();
    procs.fImageProc = SkSharingSerialContext::serializeImage;
    procs.fImageCtx = serialContext_.get();
    procs.fTypefaceProc = [](SkTypeface* tf, void* ctx) {
        return tf->serialize(SkTypeface::SerializeBehavior::kDoIncludeData);
    };
    multiPic_ = SkMakeMultiPictureDocument(
        openMultiPicStream_.get(), &procs, [sharingCtx = serialContext_.get()](const SkPicture* pic) {
            SkSharingSerialContext::collectNonTextureImagesFromPicture(pic, sharingCtx);
        });
}

ExtendRecordingCanvas* RSCaptureRecorder::TryInstantCaptureDrawing(float width, float height)
{
    recordingCanvas_ = std::make_unique<ExtendRecordingCanvas>(width, height);
    return recordingCanvas_.get();
}

void RSCaptureRecorder::EndInstantCaptureDrawing()
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
    FILE* f = Utils::FileOpen(drawCmdListFilename, "wbe");
    if (f == nullptr) {
        RSSystemProperties::SetInstantRecording(false);
        return;
    }
    Utils::FileWrite(reinterpret_cast<uint8_t*>(parcelBuf), sizeof(uint8_t), parcelSize, f);
    Utils::FileClose(f);

    Network::SendDclPath(drawCmdListFilename);
    Network::SendMessage("Saved locally");
    RSSystemProperties::SetInstantRecording(false);
}

Drawing::Canvas* RSCaptureRecorder::TryInstantCaptureSKP(float width, float height)
{
    Network::SendMessage("Starting .skp capturing.");
    recordingSkpCanvas_ = std::make_shared<Drawing::Canvas>(width, height);
    skRecorder_ = std::make_unique<SkPictureRecorder>();
    SkCanvas* skiaCanvas = skRecorder_->beginRecording(width, height);
    if (skiaCanvas == nullptr) {
        return nullptr;
    }
    recordingSkpCanvas_->GetImpl<Drawing::SkiaCanvas>()->ImportSkCanvas(skiaCanvas);
    return recordingSkpCanvas_.get();
}

void RSCaptureRecorder::EndInstantCaptureSKP()
{
    Network::SendMessage("Finishing .skp capturing");
    if (skRecorder_ == nullptr) {
        RSSystemProperties::SetInstantRecording(false);
        return;
    }
    picture_ = skRecorder_->finishRecordingAsPicture();
    if (picture_ != nullptr) {
        if (picture_->approximateOpCount() > 0) {
            Network::SendMessage("OpCount: " + std::to_string(picture_->approximateOpCount()));
            SkSerialProcs procs;
            procs.fTypefaceProc = [](SkTypeface* tf, void* ctx) {
                return tf->serialize(SkTypeface::SerializeBehavior::kDoIncludeData);
            };
            auto data = picture_->serialize(&procs);
            if (data && (data->size() > 0)) {
                Network::SendSkp(data->data(), data->size());
            }
        }
    }
    RSSystemProperties::SetInstantRecording(false);
}

Drawing::Canvas* RSCaptureRecorder::TryCaptureMSKP(float width, float height)
{
    if (mskpIdxNext_ == 0) {
        Network::SendMessage("Starting .mskp capturing.");
        InitMSKP();
    }
    mskpIdxCurrent_ = mskpIdxNext_;
    recordingSkpCanvas_ = std::make_shared<Drawing::Canvas>(width, height);
    SkCanvas* skiaCanvas = multiPic_->beginPage(width, height);
    Network::SendMessage("Begin .mskp page: " + std::to_string(mskpIdxCurrent_));
    if (skiaCanvas == nullptr) {
        return nullptr;
    }
    recordingSkpCanvas_->GetImpl<Drawing::SkiaCanvas>()->ImportSkCanvas(skiaCanvas);
    isPageActive_ = true;
    return recordingSkpCanvas_.get();
}

void RSCaptureRecorder::EndCaptureMSKP()
{
    Network::SendMessage("Close .mskp page: " + std::to_string(mskpIdxCurrent_));
    multiPic_->endPage();
    isPageActive_ = false;

    if (mskpIdxCurrent_ == mskpMaxLocal_) {
        Network::SendMessage("Finishing / Serializing .mskp capturing");
        RSSystemProperties::SetInstantRecording(false);
        // setting to default
        mskpMaxLocal_ = 0;
        mskpIdxCurrent_ = -1;
        mskpIdxNext_ = -1;

        std::thread thread([this]() mutable {
            SkFILEWStream* stream = this->openMultiPicStream_.release();
            Network::SendMessage("MSKP serialization started.");
            this->multiPic_->close();
            Network::SendMessage("MSKP Serialization done.");
            delete stream;
            Network::SendMskpPath(*MSKP_PATH);
        });
        thread.detach();
    }
}

} // namespace OHOS::Rosen