/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef COMMON_H
#define COMMON_H
#include <string>
#include "pixel_map.h"
#include "draw/color.h"
#include "effect/color_space.h"

#define RET_OK 0
#define RET_FAILED 1
#define RET_PARAM_ERR 2

namespace OHOS {
namespace Rosen {
class TestCommon {
public:
    TestCommon() = default;
    ~TestCommon() = default;
    static int PackingPixmap(std::shared_ptr<OHOS::Media::PixelMap> pixmap, std::string fileName);
    static std::shared_ptr<Drawing::ColorSpace> ColorSpaceToDrawingColorSpace(OHOS::Media::ColorSpace colorSpace);
    static Drawing::ColorType PixelFormatToDrawingColorType(OHOS::Media::PixelFormat pixelFormat);
    static Drawing::AlphaType AlphaTypeToDrawingAlphaType(OHOS::Media::AlphaType alphaType);
};
} // namespace Rosen
} // namespace OHOS
#endif // COMMON_H