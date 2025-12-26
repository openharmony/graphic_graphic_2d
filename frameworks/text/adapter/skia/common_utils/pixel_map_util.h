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
#ifndef SPTEXT_PIXEL_MAP_UTIL_H
#define SPTEXT_PIXEL_MAP_UTIL_H
#ifdef ENABLE_OHOS_ENHANCE
#include <memory>

#include "pixel_map.h"
#include "typography.h"
#include "modules/skparagraph/include/Paragraph.h"

namespace OHOS {
namespace Rosen {
namespace SPText {

std::shared_ptr<OHOS::Media::PixelMap> CreatePixelMap(
    const ImageOptions& options, const std::vector<skia::textlayout::PathInfo>& pathInfos);

} // namespace SPText
} // namespace Rosen
} // namespace OHOS
#endif // ENABLE_OHOS_ENHANCE
#endif // SPTEXT_PIXEL_MAP_UTIL_H