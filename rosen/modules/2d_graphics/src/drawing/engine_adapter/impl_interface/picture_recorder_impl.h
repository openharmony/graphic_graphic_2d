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

#ifndef PICTURE_RECORDER_IMPL_H
#define PICTURE_RECORDER_IMPL_H

#include "base_impl.h"
#include "draw/canvas.h"
#include "utils/data.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class PictureRecorderImpl : public BaseImpl {
public:
    PictureRecorderImpl() noexcept {}
    ~PictureRecorderImpl() override {}

    virtual std::shared_ptr<Drawing::Canvas> BeginRecording(float width, float height) = 0;
    virtual std::shared_ptr<Drawing::Picture> FinishRecordingAsPicture() = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif