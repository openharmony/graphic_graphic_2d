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

#include "image_chain.h"

namespace OHOS {
namespace Rosen {
ImageChain::ImageChain(std::vector<std::shared_ptr<Input> > inputs) : inputs_(inputs)
{
    CreatTexture(srcTextureID_);
    CreatTexture(dstTextureID_);

    glGenFramebuffers(1, &frameBufferID_);
    if (inputs_.size() == 1) {
        flagSeries_ = true;
    }
}

void ImageChain::Render()
{
    if (flagSeries_) {
        ProcessData data {srcTextureID_, dstTextureID_, frameBufferID_, 0, 0};
        SeriesRendering(data);
    }

    if (!flagSeries_) {
        ParallelRendering();
    }
}

void ImageChain::SeriesRendering(ProcessData& data)
{
    inputs_.at(0)->Process(data);
}

void ImageChain::CreatTexture(unsigned int& textureID)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}
} // namespcae Rosen
} // namespace OHOS