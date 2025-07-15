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
#include <sstream>

#include "common/rs_common_def.h"
#include "draw/canvas.h"
#include "pipeline/rs_recording_canvas.h"
#include "platform/common/rs_system_properties.h"
#include "transaction/rs_marshalling_helper.h"

#include "rs_profiler_network.h"
#include "rs_profiler_packet.h"
#include "rs_profiler_settings.h"
#include "rs_profiler_utils.h"

namespace OHOS::Rosen {

static const Int32Parameter MSKP_COUNTER("mskp.counter");
static const Int32Parameter MSKP_MAX("mskp.max");
static const StringParameter MSKP_PATH("mskp.path");
static const BoolParameter IS_MSKP("mskp.isMskp");

// 0 - default capturing from canvas in DisplayNode::OnDraw (which is used for imgCache via snapshot())
// 1 - capturing from mirroredDrawable::OnCapture() call in DisplayNode::DrawMirror + security/black screen
// 2 - capturing offscreen rendering in DisplayNode::OnDraw for extended screen
// 3 - capturing the actual drawing of imgCache as texture in DisplayNode::DrawMirrorCopy
static Uint32Parameter CAPTURE_TYPE("capture.type");

bool RSCaptureRecorder::PrintMessage(std::string message, bool forcedSend)
{
    if (RSSystemProperties::GetInstantRecording() || forcedSend) {
        Network::SendMessage(message);
        return true;
    }
    return false;
}

RSCaptureRecorder::RSCaptureRecorder()
{
    profilerEnabled_ = RSSystemProperties::GetProfilerEnabled();
    InvalidateDrawingCanvasNodeId();
}

RSCaptureRecorder::~RSCaptureRecorder() = default;

void RSCaptureRecorder::SetProfilerEnabled(bool val)
{
    profilerEnabled_ = val;
}

bool RSCaptureRecorder::IsRecordingEnabled()
{
    return profilerEnabled_ && RSSystemProperties::GetInstantRecording();
}

void RSCaptureRecorder::InvalidateDrawingCanvasNodeId()
{
    SetDrawingCanvasNodeId(INVALID_NODEID);
}

void RSCaptureRecorder::SetDrawingCanvasNodeId(uint64_t nodeId)
{
    drawingCanvasNodeId_ = nodeId;
}

void RSCaptureRecorder::SetComponentScreenshotFlag(bool flag)
{
    isComponentScreenshot_ = flag;
}

void RSCaptureRecorder::SetCaptureType(SkpCaptureType type)
{
    CAPTURE_TYPE = static_cast<uint32_t>(type);
}

void RSCaptureRecorder::SetCaptureTypeClear(bool flag)
{
    captureTypeToClear_ = flag;
}

Drawing::Canvas* RSCaptureRecorder::TryInstantCapture(float width, float height, SkpCaptureType type)
{
    if (!IsRecordingEnabled()) {
        return nullptr;
    }
    if (RSSystemProperties::GetSaveRDC()) {
        // for saving .drawing file
        recordingTriggeredFullFrame_ = true;
        return TryInstantCaptureDrawing(width, height);
    }
    // for saving .mskp file
    mskpMaxLocal_ = *MSKP_MAX;
    mskpIdxNext_ = *MSKP_COUNTER;

    if (*IS_MSKP) {
        // record next frame, triggered by profiler step
        if ((mskpIdxCurrent_ != mskpIdxNext_) && (mskpMaxLocal_ > 0)) {
            recordingTriggeredFullFrame_ = true;
            return TryCaptureMSKP(width, height);
        }
        return nullptr;
    }
    if (isMskpActive_) {
        return nullptr;
    }

    if (static_cast<uint32_t>(type) != CAPTURE_TYPE.Get()) {
        return nullptr;
    }

    // for saving .skp file
    recordingTriggeredFullFrame_ = true;
    return TryInstantCaptureSKP(width, height);
}

void RSCaptureRecorder::EndInstantCapture(SkpCaptureType type)
{
    if (!(IsRecordingEnabled() && recordingTriggeredFullFrame_) && !isMskpActive_) {
        return;
    }
    recordingTriggeredFullFrame_ = false;
    if (RSSystemProperties::GetSaveRDC()) {
        // for saving .drawing file
        EndInstantCaptureDrawing();
        return;
    }
    if (((mskpMaxLocal_ > 0) && isPageActive_) || isMskpActive_) {
        recordingTriggeredFullFrame_ = false;
        return EndCaptureMSKP();
    }

    if (static_cast<uint32_t>(type) != CAPTURE_TYPE.Get()) {
        return;
    }

    // for saving .skp file
    recordingTriggeredFullFrame_ = false;
    Network::SendMessage("Finishing .skp capturing");
    EndInstantCaptureSKP();
}

Drawing::Canvas* RSCaptureRecorder::TryDrawingCanvasCapture(float width, float height, uint64_t nodeId)
{
    if (drawingCanvasNodeId_ != nodeId) {
        return nullptr;
    }

    recordingTriggeredDrawingCanvas_ = true;
    return TryInstantCaptureSKP(width, height);
}

void RSCaptureRecorder::EndDrawingCanvasCapture()
{
    if (!recordingTriggeredDrawingCanvas_) {
        return;
    }
    recordingTriggeredDrawingCanvas_ = false;
    EndInstantCaptureSKP();
}

Drawing::Canvas* RSCaptureRecorder::TryOffscreenCanvasCapture(float width, float height)
{
    recordingTriggeredOffscreenCanvas_ = true;
    return TryInstantCaptureSKP(width, height);
}

void RSCaptureRecorder::EndOffscreenCanvasCapture()
{
    if (!recordingTriggeredOffscreenCanvas_) {
        return;
    }
    recordingTriggeredOffscreenCanvas_ = false;
    EndInstantCaptureSKP();
}

Drawing::Canvas* RSCaptureRecorder::TryComponentScreenshotCapture(float width, float height)
{
    if (!isComponentScreenshot_) {
        return nullptr;
    }
    recordingTriggeredComponentScreenshot_ = true;
    return TryInstantCaptureSKP(width, height);
}

void RSCaptureRecorder::EndComponentScreenshotCapture()
{
    if (!recordingTriggeredComponentScreenshot_) {
        return;
    }
    recordingTriggeredComponentScreenshot_ = false;
    EndInstantCaptureSKP();
    SetComponentScreenshotFlag(false);
}

std::pair<uint32_t, uint32_t> RSCaptureRecorder::GetDirtyRect(uint32_t displayWidth, uint32_t displayHeight)
{
    if (IsRecordingEnabled()) {
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
    auto stream = std::make_unique<Drawing::FileWStream>((*MSKP_PATH).data());
    if (!stream->IsValid()) {
        std::cout << "Could not open " << *MSKP_PATH << " for writing." << std::endl;
        return;
    }
    openMultiPicStream_ = std::move(stream);

    serialContext_ = std::make_unique<Drawing::SharingSerialContext>();
    Drawing::SerialProcs procs;
    multiPic_ = Drawing::Document::MakeMultiPictureDocument(openMultiPicStream_.get(), &procs, serialContext_);
    isMskpActive_ = true;
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
    recorder_ = std::make_unique<Drawing::PictureRecorder>();
    auto canvas = recorder_->BeginRecording(width, height);
    if (canvas == nullptr) {
        return nullptr;
    }
    recordingSkpCanvas_ = canvas;
    return recordingSkpCanvas_.get();
}

void RSCaptureRecorder::EndInstantCaptureSKP()
{
    if (recorder_ == nullptr) {
        RSSystemProperties::SetInstantRecording(false);
        return;
    }
    InvalidateDrawingCanvasNodeId();
    picture_ = recorder_->FinishRecordingAsPicture();
    Network::SendMessage("Finishing .skp capturing.");
    if (picture_ != nullptr) {
        if (picture_->ApproximateOpCount() > 0) {
            Network::SendMessage("OpCount: " + std::to_string(picture_->ApproximateOpCount()));
            Drawing::SerialProcs procs;
            procs.SetHasTypefaceProc(true);
            auto data = picture_->Serialize(&procs);
            if (data && (data->GetSize() > 0)) {
                    Network::SendMessage("Sending SKP");
                    Network::SendSkp(data->GetData(), data->GetSize());
                }
        }
    } else {
        Network::SendMessage("Empty, nothing to record to .skp");
    }
    RSSystemProperties::SetInstantRecording(false);
    if (captureTypeToClear_) {
        SetCaptureType(SkpCaptureType::DEFAULT);
        captureTypeToClear_ = false;
    }
}

Drawing::Canvas* RSCaptureRecorder::TryCaptureMSKP(float width, float height)
{
    if (!isMskpActive_) {
        Network::SendMessage("Starting .mskp capturing.");
        SetCaptureType(SkpCaptureType::DEFAULT);
        InitMSKP();
    }
    mskpIdxCurrent_ = mskpIdxNext_;
    recordingSkpCanvas_ = std::make_shared<Drawing::Canvas>(width, height);
    std::shared_ptr<Drawing::Canvas> canvas = multiPic_->BeginPage(width, height);
    Network::SendMessage("Begin .mskp page: " + std::to_string(mskpIdxCurrent_));
    if (canvas == nullptr) {
        return nullptr;
    }
    recordingSkpCanvas_ = canvas;
    isPageActive_ = true;
    return recordingSkpCanvas_.get();
}

void RSCaptureRecorder::EndCaptureMSKP()
{
    if (isPageActive_) {
        Network::SendMessage("Close .mskp page: " + std::to_string(mskpIdxCurrent_));
        multiPic_->EndPage();
    }
    isPageActive_ = false;

    if (!*IS_MSKP || (mskpIdxNext_ == -1)) {
        Network::SendMessage("Finishing / Serializing .mskp capturing");
        RSSystemProperties::SetInstantRecording(false);
        // setting to default
        mskpMaxLocal_ = 0;
        mskpIdxCurrent_ = -1;
        mskpIdxNext_ = -1;
        isMskpActive_ = false;

        std::thread thread([this]() mutable {
            Drawing::FileWStream* stream = this->openMultiPicStream_.release();
            Network::SendMessage("MSKP serialization started.");
            this->multiPic_->Close();
            Network::SendMessage("MSKP Serialization done.");
            delete stream;
            Network::SendMskpPath(*MSKP_PATH);
        });
        thread.detach();
    }
}

} // namespace OHOS::Rosen