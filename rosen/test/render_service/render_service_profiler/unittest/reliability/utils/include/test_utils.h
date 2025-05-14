/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RS_PROFILER_UNITTEST_RELIABILITY_PLATFORM_OHOS_UTILS
#define RS_PROFILER_UNITTEST_RELIABILITY_PLATFORM_OHOS_UTILS

#include "pixel_map.h"
#include "utils/data.h"

namespace OHOS::Rosen {
bool CheckConsistencyWithPixelMap(std::shared_ptr<Media::PixelMap> pixelMap, size_t position, size_t skipFromParcel);

bool CheckConsistencyWithData(std::shared_ptr<Drawing::Data> data, size_t position);
} // namespace OHOS::Rosen

#endif // RS_PROFILER_UNITTEST_RELIABILITY_PLATFORM_OHOS_UTILS