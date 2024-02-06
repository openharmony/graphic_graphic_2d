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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMON_RS_COMMON_TOOL_H
#define ROSEN_RENDER_SERVICE_BASE_COMMON_RS_COMMON_TOOL_H

#include <memory>
#include <string>

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
namespace CommonTools {
std::string GetLocalTime();
void SavePixelmapToFile(const std::shared_ptr<Media::PixelMap>& pixelMap, const std::string& dst);
} // namespace CommonTools
} // namespace Rosen
} // namespace OHOS

#endif // RS_FILE_UTILs