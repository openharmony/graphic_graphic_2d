/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "input.h"

namespace OHOS {
namespace Rosen {
void Input::DoProcess(ProcessData& data)
{
    uint32_t errorCode = 0;
    OHOS::Media::SourceOptions sourceOpts;
    sourceOpts.formatHint = format_;
    std::unique_ptr<OHOS::Media::ImageSource> imageSource =
        OHOS::Media::ImageSource::CreateImageSource(srcImagePath_, sourceOpts, errorCode);
    OHOS::Media::DecodeOptions decodeOpts;
    pixelMap_ = imageSource->CreatePixelMap(decodeOpts, errorCode);
    data.textureWidth = pixelMap_->GetWidth();
    data.textureHeight = pixelMap_->GetHeight();

    glBindTexture(GL_TEXTURE_2D, data.srcTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.textureWidth, data.textureHeight,
        0, GL_RGBA, GL_UNSIGNED_BYTE, pixelMap_->GetPixels());
    glGenerateMipmap(GL_TEXTURE_2D);
}

FILTER_TYPE Input::GetFilterType()
{
    return FILTER_TYPE::INPUT;
}

void Input::SetValue(const std::string& key, void* value, int size)
{
    if (key == "format" && size > 0) {
        std::string format = (char*)value;
        if (format == "jpg" || format == "jpeg") {
            format_ = "image/jpg";
        } else if (format == "png") {
            format_ = "image/png";
        }
        LOGD("The input format is %{public}s.", format_.c_str());
    } else if (key == "path" && size > 0) {
        srcImagePath_ = (char*)value;
        LOGD("The input source image path is %{public}s.", srcImagePath_.c_str());
    }
}
} // namespcae Rosen
} // namespace OHOS