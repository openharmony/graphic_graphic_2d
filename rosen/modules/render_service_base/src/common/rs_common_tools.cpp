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
#include <ctime>
#include <iomanip>
#include <sstream>

#include "common/rs_common_tools.h"
#include "fstream"
#include "pixel_map.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace CommonTools {
std::string GetLocalTime()
{
    // time string : "year-month-day hour_minute_second.millisecond"
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000;
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    std::stringstream ss;
    int millSecondWidth = 3; // millsecond width
    ss << std::put_time(&tm, "%Y-%m-%d %H_%M_%S.") << std::setfill('0') << std::setw(millSecondWidth) << ms.count();
    return ss.str();
}

void SavePixelmapToFile(const std::shared_ptr<Media::PixelMap>& pixelMap, const std::string& dst)
{
    int32_t w = pixelMap->GetWidth();
    int32_t h = pixelMap->GetHeight();
    int32_t totalSize = pixelMap->GetByteCount();
    std::string localTime = GetLocalTime();
    int32_t rowStirde = pixelMap->GetRowStride();
    std::string fileName = dst + localTime + "_w" + std::to_string(w) + "_h" + std::to_string(h) +
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