/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <iostream>

#include "pixel_map.h"
#include "transaction/rs_interfaces.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

constexpr uint32_t SIZE_WIDTH = 1;
constexpr uint32_t SIZE_HEIGHT = 1;

// Switch of watermark.
int main()
{
    const uint32_t color[1] = { 0x6f0000ff };
    uint32_t colorLength = sizeof(color) / sizeof(color[0]);
    const int32_t offset = 0;
    Media::InitializationOptions opts;
    opts.size.width = SIZE_WIDTH;
    opts.size.height = SIZE_HEIGHT;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    int32_t stride = opts.size.width;
    std::unique_ptr<Media::PixelMap> pixelMap1 = Media::PixelMap::Create(color, colorLength, offset, stride, opts);

    cout << "ShowWatermark interface. Open: 1 Close: 0" << endl;
    cout << "Enter: ";
    int open;
    cin >> open;

    if (open) {
        RSInterfaces::GetInstance().ShowWatermark(std::move(pixelMap1), true);
    } else {
        RSInterfaces::GetInstance().ShowWatermark(nullptr, false);
    }
    return 0;
}
