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
#include <memory>

namespace OHOS {
namespace Rosen {
Output::Output()
{
    CreateProgram(GetVertexShader(), GetFragmentShader());
}

Output::~Output()
{
    delete[] colorBuffer;
}

FILTER_TYPE Output::GetFilterType()
{
    return FILTER_TYPE::OUTPUT;
}

void Output::DoProcess(ProcessData& data)
{
    uint32_t bufferSize = data.textureWidth * data.textureHeight;
    colorBuffer = new RGBAColor[bufferSize];
    glBindFramebuffer(GL_FRAMEBUFFER, data.frameBufferID);
    glBindTexture(GL_TEXTURE_2D, data.dstTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, data.textureWidth, data.textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data.dstTextureID, 0);
    Use();
    glBindVertexArray(mesh_->VAO_);
    glBindTexture(GL_TEXTURE_2D, data.srcTextureID);
    glDrawElements(GL_TRIANGLES, AlgoFilter::DRAW_ELEMENTS_NUMBER, GL_UNSIGNED_INT, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, data.textureWidth, data.textureHeight, GL_RGB, GL_UNSIGNED_BYTE, colorBuffer);

    std::unique_ptr<OHOS::Media::PixelMap> pixelMap = std::make_unique<OHOS::Media::PixelMap>();
    OHOS::Media::ImageInfo info;
    info.size.width = data.textureWidth;
    info.size.height = data.textureHeight;
    info.pixelFormat = OHOS::Media::PixelFormat::RGB_888;
    info.colorSpace = OHOS::Media::ColorSpace::SRGB;
    pixelMap->SetImageInfo(info);
    pixelMap->SetPixelsAddr(colorBuffer, nullptr, bufferSize, OHOS::Media::AllocatorType::HEAP_ALLOC, nullptr);

    OHOS::Media::ImagePacker imagePacker;
    OHOS::Media::PackOption option;
    option.format = format_;
    std::set<std::string> formats;
    uint32_t ret = imagePacker.GetSupportedFormats(formats);
    if (ret != 0) {
        return;
    }
    imagePacker.StartPacking(dstImagePath_, option);
    imagePacker.AddImage(*pixelMap);
    int64_t packedSize = 0;
    imagePacker.FinalizePacking(packedSize);
    AddNextFilter(nullptr);
}

void Output::SetValue(const std::string& key, void* value, int size)
{
    if (key == "format" && size > 0) {
        std::string format = (char*)value;
        if (format == "jpg" || format == "jpeg") {
            format_ = "image/jpeg";
        } else if (format == "png") {
            format_ = "image/png";
        }
        LOGD("The output format is %{public}s.", format_.c_str());
    } else if (key == "path" && size > 0) {
        dstImagePath_ = (char*)value;
        LOGD("The ouput source image path is %{public}s.", dstImagePath_.c_str());
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