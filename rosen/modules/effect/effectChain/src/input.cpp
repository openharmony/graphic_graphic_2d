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
    if (format_ == "image/jpg" || format_ == "image/png") {
        DecodeFromFile(data);
    } else if (format_ == "pixelMap") {
        DecodeFromPixelMap(data);
    } else if (format_ == "buffer") {
        DecodeFromBuffer(data);
    }
}

void Input::DecodeFromFile(ProcessData& data)
{
    uint32_t errorCode = 0;
    OHOS::Media::SourceOptions sourceOpts;
    sourceOpts.formatHint = format_;
    std::unique_ptr<OHOS::Media::ImageSource> imageSource =
        OHOS::Media::ImageSource::CreateImageSource(srcImagePath_, sourceOpts, errorCode);
    OHOS::Media::DecodeOptions decodeOpts;
    if (imageSource == nullptr) {
        LOGD("Failed to create imageSource!");
        return;
    }
    pixelMap_ = imageSource->CreatePixelMap(decodeOpts, errorCode);
    DecodeFromPixelMap(data);
}

void Input::DecodeFromPixelMap(ProcessData& data)
{
    if (pixelMap_ == nullptr) {
        LOGD("Failed to create PixelMap!");
        return;
    }
    data.textureWidth = pixelMap_->GetWidth();
    data.textureHeight = pixelMap_->GetHeight();

    glBindTexture(GL_TEXTURE_2D, data.srcTextureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.textureWidth, data.textureHeight,
        0, GL_RGBA, GL_UNSIGNED_BYTE, pixelMap_->GetPixels());
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Input::DecodeFromBuffer(ProcessData& data)
{
    if (buffer_ == nullptr) {
        return;
    }
    data.textureWidth = bufferWidth_;
    data.textureHeight = bufferHeight_;
    glBindTexture(GL_TEXTURE_2D, data.srcTextureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.textureWidth, data.textureHeight,
        0, GL_RGBA, GL_UNSIGNED_BYTE, buffer_.get());
    glGenerateMipmap(GL_TEXTURE_2D);
}
FILTER_TYPE Input::GetFilterType()
{
    return FILTER_TYPE::INPUT;
}

void Input::SetValue(const std::string& key, std::shared_ptr<void> value, int size)
{
    if (key == "format" && size > 0) {
        std::shared_ptr<std::string> format = std::static_pointer_cast<std::string>(value);
        format_ = *(format.get());
        if (format_ == "jpg" || format_ == "jpeg") {
            format_ = "image/jpg";
        } else if (format_ == "png") {
            format_ = "image/png";
        } else if (format_ == "pixelMap") {
            format_ = "pixelMap";
        } else if (format_ == "buffer") {
            format_ = "buffer";
        }
        LOGD("The input format is %{public}s.", format_.c_str());
    } else if (key == "src" && size > 0) {
        if (format_ == "image/jpg" || format_ == "image/png") {
            std::shared_ptr<std::string> srcImagePath = std::static_pointer_cast<std::string>(value);
            srcImagePath_ = *(srcImagePath.get());
            LOGD("The input source image path is %{public}s.", srcImagePath_.c_str());
        } else if (format_ == "pixelMap") {
            pixelMap_ = std::static_pointer_cast<OHOS::Media::PixelMap>(value);
        } else if (format_ == "buffer") {
            buffer_ = std::static_pointer_cast<uint8_t>(value);
        }
    } else if (key == "bufferWidth" && size > 0) {
        std::shared_ptr<int> bufferWidth = std::static_pointer_cast<int>(value);
        bufferWidth_ = *(bufferWidth.get());
    } else if (key == "bufferHeight" && size > 0) {
        std::shared_ptr<int> bufferHeight = std::static_pointer_cast<int>(value);
        bufferHeight_ = *(bufferHeight.get());
    }
}
} // namespcae Rosen
} // namespace OHOS