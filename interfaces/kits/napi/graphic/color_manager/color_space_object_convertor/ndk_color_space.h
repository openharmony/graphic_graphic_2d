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

#ifndef OHOS_NDK_COLOR_SPACE_H
#define OHOS_NDK_COLOR_SPACE_H

#include "color_space.h"

#ifdef __cplusplus
extern "C" {
#endif

struct NativeColorSpaceManager {
public:
    explicit NativeColorSpaceManager(
        OHOS::ColorManager::ColorSpaceName colorSpaceName) : colorSpaceNDKToken_(colorSpaceName)
    {}

    NativeColorSpaceManager(
        OHOS::ColorManager::ColorSpacePrimaries primaries, float gamma) : colorSpaceNDKToken_(primaries, gamma)
    {}

    NativeColorSpaceManager(
        OHOS::ColorManager::ColorSpace colorSpace) : colorSpaceNDKToken_(colorSpace)
    {}

    const OHOS::ColorManager::ColorSpace& GetInnerColorSpace()
    {
        return colorSpaceNDKToken_;
    }

private:
    OHOS::ColorManager::ColorSpace colorSpaceNDKToken_;
};

#ifdef __cplusplus
};
#endif

#endif //OHOS_NDK_COLOR_SPACE_H