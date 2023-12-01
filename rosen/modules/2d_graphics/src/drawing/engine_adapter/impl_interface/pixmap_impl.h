/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef PIXMAPIMPL_H
#define PIXMAPIMPL_H

#include "base_impl.h"
#include "image/image_info.h"
#include "utils/sampling_options.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Pixmap;

class PixmapImpl : public BaseImpl {
public:
    PixmapImpl() {}
    ~PixmapImpl() override {}
    virtual std::shared_ptr<ColorSpace> GetColorSpace() const = 0;
    virtual ColorType GetColorType() const = 0;
    virtual AlphaType GetAlphaType() const = 0;
    virtual ColorQuad GetColor(int x, int y) const = 0;
    virtual size_t GetRowBytes() const = 0;
    virtual const void* GetAddr() const = 0;
    virtual int32_t GetWidth() const = 0;
    virtual int32_t GetHeight() const = 0;
    virtual bool ScalePixels(const Pixmap& dst, const SamplingOptions& options) const = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
