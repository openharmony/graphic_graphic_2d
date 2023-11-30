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
bool SkiaRecording::GetCaptureEnabled() const
{
    return captureEnabled_;
}

void SkiaRecording::InitConfigsFromParam()
{
    captureEnabled_ = std::stoi(system::GetParameter("skpcapture.enabled", "0"));
    if (captureEnabled_) {
        captureFrameNum_ = std::stoi(system::GetParameter("skpcapture.frameNum", "0"));
        if (captureFrameNum_ < 1) {
            captureMode_ = SkiaCaptureMode::NONE;
        } else if (captureFrameNum_ == 1) {
            captureMode_ = SkiaCaptureMode::SINGLE_FRAME;
        } else {
            captureMode_ = SkiaCaptureMode::MULTI_FRAME;
        }
        captureFileName_ = system::GetParameter("skpcapture.path", "");
        std::string fileExtension = ".skp";
        if (captureFileName_.rfind(fileExtension) == (captureFileName_.size() - fileExtension.size())) {
            captureMode_ = SkiaCaptureMode::SINGLE_FRAME;
        }
    }
}

bool SkiaRecording::SetupMultiFrame()
{
    std::cout<< "Set up multi-frame capture, the frame number is " << captureFrameNum_ << std::endl;
    auto stream = std::make_unique<SkFILEWStream>(captureFileName_.c_str());
    if (!stream->isValid()) {
        std::cout<< "Could not open " << captureFileName_ << " for writing." << std::endl;
        captureFrameNum_ = 0;
        captureMode_ = SkiaCaptureMode::NONE;
        return false;
    }
    openMultiPicStream_ = std::move(stream);
    serialContext_ = std::make_unique<SkSharingSerialContext>();
    SkSerialProcs procs;
    procs.fImageProc = SkSharingSerialContext::serializeImage;
    procs.fImageCtx = serialContext_.get();
    procs.fTypefaceProc = [](SkTypeface* tf, void* ctx) {
        return tf->serialize(SkTypeface::SerializeBehavior::kDoIncludeData);
    };
    // SkDocuments don't take owership of the streams they write.
    // we need to keep it until after multiPic_.close()
    // procs is passed as a pointer, but just as a method of having an optional default.
    // procs doesn't need to outlive this Make call.
#ifdef NEW_SKIA
    multiPic_ = SkMakeMultiPictureDocument(openMultiPicStream_.get(), &procs,
        [sharingCtx = serialContext_.get()](const SkPicture* pic) {
        SkSharingSerialContext::collectNonTextureImagesFromPicture(pic, sharingCtx);
    });
#else
    multiPic_ = SkMakeMultiPictureDocument(openMultiPicStream_.get(), &procs);
#endif
    return true;
}

SkCanvas* SkiaRecording::BeginCapture(SkCanvas* canvas, int width, int height)
{
    if (canvas == nullptr) {
        return nullptr;
    }
    static bool isFirstFrame = true;
    if (isFirstFrame && captureMode_ == SkiaCaptureMode::MULTI_FRAME) {
        isFirstFrame = false;
        if (!SetupMultiFrame()) {
            return nullptr;
        }
    }
    // Create a canvas pointer, fill it depending on what kind of capture is requested (if any)
    switch (captureMode_) {
        case SkiaCaptureMode::SINGLE_FRAME:
            recorder_ = std::make_unique<SkPictureRecorder>();
            pictureCanvas_ = recorder_->beginRecording(width, height);
            break;
        case SkiaCaptureMode::MULTI_FRAME:
            // If a multi frame recording is active, initialize recording for a single frame of a
            // multi frame file.
            pictureCanvas_ = multiPic_->beginPage(width, height);
            break;
        case SkiaCaptureMode::NONE:
            // Returning here in the non-capture case means we can count on pictureCanvas being
            // non-null below.
            return nullptr;
    }

    // Setting up an nway canvas is common to any kind of capture.
    if (nwayCanvas_ == nullptr) {
        nwayCanvas_ = std::make_unique<SkNWayCanvas>(width, height);
    }
    nwayCanvas_->addCanvas(canvas);
    nwayCanvas_->addCanvas(pictureCanvas_);

    return nwayCanvas_.get();
}

void SkiaRecording::EndCapture()
{
    if (captureMode_ == SkiaCaptureMode::NONE) {
        return;
    }
    nwayCanvas_->removeAll();

    if (captureFrameNum_ > 0 && captureMode_ == SkiaCaptureMode::MULTI_FRAME) {
        if (!multiPic_) {
            std::cout << "multiPic_ is nullptr" << std::endl;
            return;
        }
        multiPic_->endPage();
        captureFrameNum_--;
        if (captureFrameNum_ == 0) {
            captureEnabled_ = false;
            captureMode_ = SkiaCaptureMode::NONE;
            // Pass multiPic_ and openMultiPicStream_ to a background thread, which will handle
            // the heavyweight serialization work and destroy them. openMultiPicStream_ is released
            // to a bare pointer because keeping it in a smart pointer makes the lambda
            // non-copyable. The lambda is only called once, so this is safe.
            SkFILEWStream* stream = openMultiPicStream_.release();
            multiPic_->close();
            delete stream;
            std::cout << "Multi frame SKP complete." << std::endl;
        }
    } else if (!recorder_) {
        std::cout << "recorder_ is nullptr" << std::endl;
    } else {
        sk_sp<SkPicture> picture = recorder_->finishRecordingAsPicture();
        if (picture->approximateOpCount() > 0) {
            // single frame skp to file
            SkSerialProcs procs;
            procs.fTypefaceProc = [](SkTypeface* tf, void* ctx) {
                return tf->serialize(SkTypeface::SerializeBehavior::kDoIncludeData);
            };
            auto data = picture->serialize(&procs);
            static int tmpId = 0;
            std::string fileName = captureFileName_;
            std::string fileExtension = ".skp";
            if (captureFileName_.rfind(fileExtension) == (captureFileName_.size() - fileExtension.size())) {
                fileName.insert(fileName.size() - fileExtension.size(), "_" + std::to_string(tmpId++));
            } else {
                fileName = fileName + "_" + std::to_string(tmpId++) + fileExtension;
            }
            SavePicture(data, fileName);
            if (--captureFrameNum_ == 0) {
                captureMode_ = SkiaCaptureMode::NONE;
            }
        }
        recorder_.reset();
    }
}

void SkiaRecording::SavePicture(const sk_sp<SkData>& data, const std::string& filename)
{
    SkFILEWStream stream(filename.c_str());
    if (stream.isValid()) {
        stream.write(data->data(), data->size());
        stream.flush();
        std::cout << "SKP Captured Drawing Output (" << stream.bytesWritten() << " bytes) for frame. " <<
            filename << std::endl;
    } else {
        std::cout << "Save picture failed. " << filename << " is not valid for frame." << filename << std::endl;
    }
}
}