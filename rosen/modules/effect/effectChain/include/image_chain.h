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

#ifndef IMAGE_CHAIN_H
#define IMAGE_CHAIN_H

#include <memory>
#include "algo_filter.h"
#include "input.h"

namespace OHOS {
namespace Rosen {
class ImageChain {
public:
    ImageChain(std::vector<std::shared_ptr<Input> > inputs);
    void Render();

private:
    void CreatTexture(unsigned int& TextureID);
    void SeriesRendering(ProcessData& data);
    void ParallelRendering() {};
    bool flagSeries_ = false;
    unsigned int frameBufferID_;
    unsigned int srcTextureID_;
    unsigned int dstTextureID_;
    std::vector<std::shared_ptr<Input> > inputs_;
};
} // namespace Rosen
} // namespace OHOS
#endif // IMAGE_CHAIN_H
