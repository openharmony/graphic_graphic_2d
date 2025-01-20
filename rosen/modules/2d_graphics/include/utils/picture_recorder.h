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

#ifndef PICTURERECORDER_H
#define PICTURERECORDER_H

#include "image/picture.h"
#include "impl_interface/picture_recorder_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class DRAWING_API PictureRecorder {
public:
    PictureRecorder() noexcept;
    virtual ~PictureRecorder() {};
    template <typename T>
    T* GetImpl() const
    {
        return pictureRecorderImplPtr_->DowncastingTo<T>();
    }

    /*
     *  @brief return the canvas that records the drawing commands.
     */
    std::shared_ptr<Canvas> BeginRecording(float width, float height);

    /*
     *  @brief Signal that the caller is done recording.
     */
    std::shared_ptr<Picture> FinishRecordingAsPicture();

private:
    std::shared_ptr<PictureRecorderImpl> pictureRecorderImplPtr_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif