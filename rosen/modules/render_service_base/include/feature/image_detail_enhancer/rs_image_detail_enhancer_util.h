/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RS_IMAGE_DETAIL_ENHANCER_UTIL_H
#define RS_IMAGE_DETAIL_ENHANCER_UTIL_H

#include <vector>

namespace OHOS {
namespace Rosen {
struct RSImageDetailEnhanceParams {
    bool isValid = false;           // Indicates if the params is valid.
    int minSize = 0;                // The min value of width/height RSImageDetailEnhance support.
    int maxSize = 0;                // The max value of width/height RSImageDetailEnhance support.
    float minScaleRatio = 0.0f;     // The min scale ratio RSImageDetailEnhance support.
    float maxScaleRatio = 0.0f;     // The max scale ratio RSImageDetailEnhance support.
};

struct RSImageDetailEnhanceRangeParams {
    float rangeMin = 0.0f;          // The min value of the range
    float rangeMax = 0.0f;          // The max value of the range
    float effectParam = 0.0f;       // The effect param value for the range
};

struct RSImageDetailEnhanceAlgoParams {
    bool isValid = false;           // Indicates if the params is valid.
    std::vector<RSImageDetailEnhanceRangeParams> rangeParams {};  // The params of ranges
    int minSize = 0;                // The min image size algorithm support.
    int maxSize = 0;                // The max image size algorithm support.
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_IMAGE_DETAIL_ENHANCER_UTIL_H