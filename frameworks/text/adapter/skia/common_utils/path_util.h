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
#ifndef SPTEXT_PATH_UTIL_H
#define SPTEXT_PATH_UTIL_H

#include <vector>

#include "draw/path.h"
#include "pixel_map.h"

namespace OHOS::Rosen::SPText {

enum class Orientation {
    COUNTER_CLOCKWISE,
    CLOCKWISE,
    COLLINEAR
};

bool IsPathClockwise(const Drawing::Path& path);
void ExtractOuterPath(const std::vector<Drawing::Path>& allPaths, Drawing::Path& outerPath);
} // namespace OHOS::Rosen::SPText
#endif // SPTEXT_PATH_UTIL_H