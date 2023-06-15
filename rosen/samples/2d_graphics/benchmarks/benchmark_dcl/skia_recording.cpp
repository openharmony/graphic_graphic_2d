/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "skia_recording.h"

#include <iostream>

#include "parameters.h"

namespace OHOS::Rosen {
bool SkiaRecording::GetCaptureEnabled()
{
    return captureEnabled_;
}

void SkiaRecording::InitConfigsFromParam()
{
    captureEnabled_ = std::stoi(system::GetParameter("debug.graphic.skpcapture.enabled", "0"));
    if (captureEnabled_) {
        captureFrameNum_ = std::stoi(system::GetParameter("debug.graphic.skpcapture.frameNum", "0"));
        if (captureFrameNum_ < 1) {
            captureMode_ = SkiaCaptureMode::None;
        } else if (captureFrameNum_ == 1) {
            captureMode_ = SkiaCaptureMode::SingleFrame;
        } else {
            captureMode_ = SkiaCaptureMode::MultiFrame;
        }
        captureFileName_ = system::GetParameter("debug.graphic.skpcapture.path", "");
    }
}

void SkiaRecording::SetupMultiFrame()
{
    std::cout<< "Set up multi-frame capture, the frame number is " << captureFrameNum_ << std::endl;
    auto stream = std::make_unique<SkFILEWStream>(captureFileName_.c_str());
    if (!stream->isValid()) {
        std::cout<< "Could not open " << capturedFile_ << " for writing." << std::endl;
        captureSequence_ = 0;
        captureMode_ = CaptureMode::None;
        return false;
    }
    openMultiPicStream_ = std::move(stream);
    serialContext_.reset(new SkSharingSerialContext());
    SkSerialProcs procs;
    procs.fImageProc = SkSharingSerialContext::serializeImage;
    procs.fImageCtx = serialContext_.get();
    procs.fTypefaceProc = [](SkTypeface* tf, void* ctx){
        return tf->serialize(SkTypeface::SerializeBehavior::kDoIncludeData);
    };
    // SkDocuments don't take owership of the streams they write.
    // we need to keep it until after multiPic_.close()
    // procs is passed as a pointer, but just as a method of having an optional default.
    // procs doesn't need to outlive this Make call.
    multiPic_ = SkMakeMultiPictureDocument(openMultiPicStream_.get(), &procs,
        [sharingCtx = serialContext_.get()](const SkPicture* pic) {
                SkSharingSerialContext::collectNonTextureImagesFromPicture(pic, sharingCtx);
        });
    return true;
}

SkCanvas* SkiaRecording::BeginCapture(SkCanvas* canvas, int width, int height)
{
    SkCanvas* recordingCanvas = nullptr;
    bool isFirstFrame = true;
    if (isFirstFrame && captureMode_ == SkiaCaptureMode::MultiFrame) {
        isFirstFrame = false;
        if (!SetupMultiFrame()) {
            return nullptr;
        }
    }
    // Create a canvas pointer, fill it depending on what kind of capture is requested (if any)
    SkCanvas* pictureCanvas = nullptr;
    switch (captureMode_) {
        case CaptureMode::CallbackAPI:
        case CaptureMode::SingleFrameSKP:
            recorder_.reset(new SkPictureRecorder());
            pictureCanvas = recorder_->beginRecording(width, height);
            break;
        case CaptureMode::MultiFrameSKP:
            // If a multi frame recording is active, initialize recording for a single frame of a
            // multi frame file.
            pictureCanvas = multiPic_->beginPage(width, height);
            break;
        case CaptureMode::None:
            // Returning here in the non-capture case means we can count on pictureCanvas being
            // non-null below.
            return nullptr;
    }

    // Setting up an nway canvas is common to any kind of capture.
    nwayCanvas_ = std::make_unique<SkNWayCanvas>(width, height);
    nwayCanvas_->addCanvas(canvas);
    nwayCanvas_->addCanvas(pictureCanvas);

    return nwayCanvas_.get();
}

void SkiaRecording::endCapture() {
    if (CC_LIKELY(captureMode_ == CaptureMode::None)) { return; }
    nwayCanvas_.reset();

    if (captureFrameNum_ > 0 && captureMode_ == CaptureMode::MultiFrameSKP) {
        multiPic_->endPage();
        captureFrameNum_--;
        if (captureFrameNum_ == 0) {
            captureEnabled_ = false;
            captureMode_ = CaptureMode::None;
            // Pass multiPic_ and openMultiPicStream_ to a background thread, which will handle
            // the heavyweight serialization work and destroy them. openMultiPicStream_ is released
            // to a bare pointer because keeping it in a smart pointer makes the lambda
            // non-copyable. The lambda is only called once, so this is safe.
            SkFILEWStream* stream = openMultiPicStream_.release();
            CommonPool::post([doc = std::move(multiPic_), stream]{
                std::cout << "Finalizing multi frame SKP" << std::endl;
                doc->close();
                delete stream;
                std::cout << "Multi frame SKP complete." << std::endl;
            });
        }
    } else {
        sk_sp<SkPicture> picture = recorder_->finishRecordingAsPicture();
        if (picture->approximateOpCount() > 0) {
            if (mPictureCapturedCallback) {
                std::invoke(mPictureCapturedCallback, std::move(picture));
            } else {
                // single frame skp to file
                SkSerialProcs procs;
                procs.fTypefaceProc = [](SkTypeface* tf, void* ctx){
                    return tf->serialize(SkTypeface::SerializeBehavior::kDoIncludeData);
                };
                auto data = picture->serialize(&procs);
                savePictureAsync(data, captureFileName_);
                captureFrameNum_ = 0;
                captureMode_ = CaptureMode::None;
            }
        }
        recorder_.reset();
    }
}
}