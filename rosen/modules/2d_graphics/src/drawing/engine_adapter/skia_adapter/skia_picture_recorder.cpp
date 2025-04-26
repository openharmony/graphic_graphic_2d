/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "skia_picture_recorder.h"

#include "skia_adapter/skia_data.h"
#include "skia_adapter/skia_canvas.h"
#include "utils/data.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

SkiaPictureRecorder::SkiaPictureRecorder() noexcept : skPictureRecorder_(new SkPictureRecorder()) {}

SkPictureRecorder* SkiaPictureRecorder::GetPictureRecorder() const
{
    return skPictureRecorder_;
}

std::shared_ptr<Canvas> SkiaPictureRecorder::BeginRecording(float width, float height)
{
    if (skPictureRecorder_ == nullptr) {
        return nullptr;
    }
    auto skCanvas = skPictureRecorder_->beginRecording(width, height);
    auto canvas = std::make_shared<Canvas>();
    auto skiaCanvas = canvas->GetImpl<SkiaCanvas>();
    if (skiaCanvas == nullptr) {
        return nullptr;
    }
    skiaCanvas->ImportSkCanvas(skCanvas);
    return canvas;
}

std::shared_ptr<Picture> SkiaPictureRecorder::FinishRecordingAsPicture()
{
    if (skPictureRecorder_ == nullptr) {
        return nullptr;
    }
    auto skPicture = skPictureRecorder_->finishRecordingAsPicture();
    auto picture = std::make_shared<Picture>();
    auto skiaPicture = picture->GetImpl<SkiaPicture>();
    if (skiaPicture == nullptr) {
        return nullptr;
    }
    skiaPicture->SetSkPicture(skPicture);
    return picture;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS