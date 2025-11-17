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

#include "png.h"
#include "rs_graphic_log.h"
#include "rs_graphic_test_utils.h"
#include "../include/rs_graphic_test_utils.h"

#include <cstdint>
#include <iostream>
#include <ostream>
#include <set>
#include <string>

#include "image_packer.h"

namespace OHOS {
namespace Rosen {

bool WriteToPngWithPixelMap(const std::string& fileName, OHOS::Media::PixelMap& pixelMap)
{
    std::string fileType = ".png";
    OHOS::Media::ImagePacker imagePacker;
    OHOS::Media::PackOption option;
    option.format = "image/png";
    option.quality = PNG_PACHER_QUALITY;
    option.numberHint = 1;
    std::set<std::string> formats;
    auto ret = imagePacker.GetSupportedFormats(formats);
    if (ret) {
        std::cout << "error: get supported formats error" << std::endl;
        return false;
    }

    imagePacker.StartPacking(fileName, option);
    imagePacker.AddImage(pixelMap);
    int64_t packedSize = 0;
    uint32_t res = imagePacker.FinalizePacking(packedSize);
    if (res != PACKER_SUCCESS) {
        std::cout << "error: finalize packing error" << std::endl;
        return false;
    }
    return true;
}

void WaitTimeout(int ms)
{
    auto time = std::chrono::milliseconds(ms);
    std::this_thread::sleep_for(time);
}

}
}
