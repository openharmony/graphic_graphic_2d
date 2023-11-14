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

#ifndef PIXMAP_H
#define PIXMAP_H

#include "drawing/engine_adapter/impl_interface/pixmap_impl.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API Pixmap {
public:
    Pixmap();
    Pixmap(const ImageInfo& imageInfo, const void* addr, size_t rowBytes);
    virtual ~Pixmap();
    std::shared_ptr<ColorSpace> GetColorSpace();
    ColorType GetColorType();
    AlphaType GetAlphaType();
    size_t GetRowBytes();
    const void* GetAddr();
    int GetWidth();
    int GetHeight();
    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return pixmapImplPtr->DowncastingTo<T>();
    }

private:
    std::shared_ptr<PixmapImpl> pixmapImplPtr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
