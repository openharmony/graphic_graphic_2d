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

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PixmapImpl : public BaseImpl {
public:
    PixmapImpl() {}
    ~PixmapImpl() override {}
    virtual std::shared_ptr<ColorSpace> GetColorSpace() = 0;
    virtual ColorType GetColorType() = 0;
    virtual AlphaType GetAlphaType() = 0;
    virtual size_t GetRowBytes() = 0;
    virtual const void* GetAddr() = 0;
    virtual int32_t GetWidth() = 0;
    virtual int32_t GetHeight() = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
