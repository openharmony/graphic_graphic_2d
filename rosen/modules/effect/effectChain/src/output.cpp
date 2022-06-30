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

#include "output.h"
#include <GLES3/gl32.h>
#include <cmath>
#include <memory>

namespace OHOS {
namespace Rosen {
Output::Output()
{
    CreateProgram(GetVertexShader(), GetFragmentShader());
}

FILTER_TYPE Output::GetFilterType()
{
    return FILTER_TYPE::OUTPUT;
}

std::unique_ptr<OHOS::Media::PixelMap> Output::GetPixelMap()
{
    return std::move(pixelMap_);
}

std::shared_ptr<uint8_t> Output::GetColorBuffer()
{
    return std::move(colorBuffer_);
}

void Output::DoProcess(ProcessData& data)
{
    if (format_ == "image/jpeg" || format_ == "image/png") {
        EncodeToFile(data);
    } else if (format_ == "pixelMap") {
        EncodeToPixelMap(data);
    } else if (format_ == "buffer") {
        WriteToBuffer(data);
    } else {
        LOGE("The format of Output is incorrect!!!");
    }
}

void Output::EncodeToFile(ProcessData& data)
{
    EncodeToPixelMap(data);
    OHOS::Media::ImagePacker imagePacker;
    OHOS::Media::PackOption option;
    option.format = format_;
    std::set<std::string> formats;
    uint32_t ret = imagePacker.GetSupportedFormats(formats);
    if (ret != 0) {
        return;
    }
    imagePacker.StartPacking(dstImagePath_, option);
    imagePacker.AddImage(*pixelMap_);
    int64_t packedSize = 0;
    imagePacker.FinalizePacking(packedSize);
}

void Output::EncodeToPixelMap(ProcessData& data)
{
    WriteToBuffer(data);
    WriteToBuffer(data);
    Media::InitializationOptions opts;
    opts.size.width = std::ceil(data.textureWidth);
    opts.size.height = std::ceil(data.textureHeight);
    opts.editable = true;
    pixelMap_ = Media::PixelMap::Create(opts);
    pixelMap_->WritePixels(colorBuffer_.get(), pixelMap_->GetByteCount());
}

void Output::WriteToBuffer(ProcessData& data)
{
    uint32_t bufferSize = data.textureWidth * data.textureHeight * COLOR_CHANNEL;
    auto colorBuf = new uint8_t[bufferSize];
    std::shared_ptr<uint8_t> colorBuffer(colorBuf, [] (uint8_t *ptr) {
        delete[] ptr;
    });
    colorBuffer_ = std::move(colorBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, data.frameBufferID);
    glBindTexture(GL_TEXTURE_2D, data.dstTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.textureWidth, data.textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data.dstTextureID, 0);
    Use();
    glViewport(0, 0, data.textureWidth, data.textureHeight);
    glBindVertexArray(mesh_->VAO_);
    glBindTexture(GL_TEXTURE_2D, data.srcTextureID);
    glDrawElements(GL_TRIANGLES, AlgoFilter::DRAW_ELEMENTS_NUMBER, GL_UNSIGNED_INT, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, data.textureWidth, data.textureHeight, GL_RGBA, GL_UNSIGNED_BYTE, colorBuffer_.get());
}

void Output::SetValue(const std::string& key, std::shared_ptr<void> value, int size)
{
    if (key == "format" && size > 0) {
        std::shared_ptr<std::string> format = std::static_pointer_cast<std::string>(value);
        format_ = *(format.get());
        if (format_ == "jpg" || format_ == "jpeg") {
            format_ = "image/jpeg";
        } else if (format_ == "png") {
            format_ = "image/png";
        }
        LOGD("The output format is %{public}s.", format_.c_str());
    } else if (key == "dst" && size > 0) {
        if (format_ == "image/jpeg" || format_ == "image/png") {
            std::shared_ptr<std::string> dstImagePath = std::static_pointer_cast<std::string>(value);
            dstImagePath_ = *(dstImagePath.get());
            LOGD("The ouput source image path is %{public}s.", dstImagePath_.c_str());
        }
    }
}

std::string Output::GetVertexShader()
{
    return R"SHADER(#version 320 es
        precision mediump float;
        layout (location = 0) in vec3 vertexCoord;
        layout (location = 1) in vec2 inputTexCoord;
        out vec2 texCoord;

        void main()
        {
            gl_Position = vec4(vertexCoord, 1.0);
            texCoord = inputTexCoord;
        }
    )SHADER";
}

std::string Output::GetFragmentShader()
{
    return R"SHADER(#version 320 es
        precision mediump float;
        in vec2 texCoord;
        out vec4 fragColor;
        uniform sampler2D uTexture;
        void main()
        {
            fragColor = texture(uTexture, texCoord);
        }
    )SHADER";
}
} // namespcae Rosen
} // namespace OHOS