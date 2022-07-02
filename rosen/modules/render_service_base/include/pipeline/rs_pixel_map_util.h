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

#ifndef RS_PIXEL_MAP_UTIL_H
#define RS_PIXEL_MAP_UTIL_H

#include "include/core/SkImage.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
class RSPixelMapUtil {
public:
    static sk_sp<SkImage> PixelMapToSkImage(const std::shared_ptr<Media::PixelMap>& pixmap);

    static SkImageInfo MakeSkImageInfoFromPixelMap(const std::shared_ptr<Media::PixelMap>& pixmap);
    static sk_sp<SkColorSpace> ColorSpaceToSkColorSpace(const std::shared_ptr<Media::PixelMap>& pixmap);
    static SkColorType PixelFormatToSkColorType(const std::shared_ptr<Media::PixelMap>& pixmap);
    static SkAlphaType AlphaTypeToSkAlphaType(const std::shared_ptr<Media::PixelMap>& pixmap);
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_PIXEL_MAP_UTIL_H
