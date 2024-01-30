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

#include <chrono>

#include "common/rs_common_tools.h"
#include "fstream"
#include "pixel_map.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace CommonTools {
void SavePixelmapToFile(const std::shared_ptr<Media::PixelMap>& pixelMap, const std::string& dst)
{
    int32_t w = pixelMap->GetWidth();
    int32_t h = pixelMap->GetHeight();
    int32_t totalSize = pixelMap->GetByteCount();
    uint64_t nowTime = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
    int32_t rowStirde = pixelMap->GetRowStride();
    std::string fileName = dst + std::to_string(nowTime) + "_w" + std::to_string(w) + "_h" + std::to_string(h) +
                           "_stride" + std::to_string(rowStirde) + ".dat";
    std::ofstream outfile(fileName, std::fstream::out);
    if (!outfile.is_open()) {
        RS_LOGE("SavePixelmapToFile write error, path=%{public}s", fileName.c_str());
        return;
    }
    outfile.write(reinterpret_cast<const char*>(pixelMap->GetPixels()), totalSize);
    outfile.close();
    RS_LOGI("SavePixelmapToFile write success, path=%{public}s", fileName.c_str());
}
}
} // namespace Rosen
} // namespace OHOS