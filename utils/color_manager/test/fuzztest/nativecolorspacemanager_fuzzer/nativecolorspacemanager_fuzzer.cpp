/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nativecolorspacemanager_fuzzer.h"

#include <securec.h>

#include "color.h"
#include "color_space.h"
#include "color_space_convertor.h"
#include "native_color_space_manager.h"

namespace OHOS {
namespace ColorSpaceManager {
namespace {
    const uint8_t* g_data = nullptr;
    size_t g_size = 0;
    size_t g_pos;
} // namespace

/*
* describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
* tips: only support basic type
*/
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
 
    g_data = data;
    g_size = size;
    g_pos = 0;
    return true;
}

bool ColorFuzzTest(const uint8_t* data, size_t size)
{
    Color randomColor = Color(GetData<float>(), GetData<float>(), GetData<float>(), GetData<float>());
    ColorSpace csObject = ColorSpace(GetData<ColorSpaceName>());
    ColorSpaceConvertor convertor = ColorSpaceConvertor(GetData<ColorSpaceName>(), GetData<ColorSpaceName>(),
                                        GetData<GamutMappingMode>());
    // color test
    Color convertColor = Color();
    (void)randomColor.Convert(GetData<ColorSpaceName>());
    (void)randomColor.Convert(csObject);
    convertColor = randomColor.Convert(convertor);
    (void)randomColor.ColorEqual(convertColor);
 
    return true;
}

bool DoOH_NativeColorSpaceManager_CreateFromPrimariesAndGammaFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
 
    OH_NativeColorSpaceManager* nativeColorSpaceManager = nullptr;
    ColorSpaceName colorSpaceName = ColorSpaceName::NONE;
    nativeColorSpaceManager = OH_NativeColorSpaceManager_CreateFromName(colorSpaceName);
 
    ColorSpacePrimaries primaries = {0.640f, 0.330f, 0.210f, 0.710f, 0.150f, 0.060f, 0.3127f, 0.3290f};
    float gamma = GetData<float>();
    nativeColorSpaceManager->OH_NativeColorSpaceManager_CreateFromPrimariesAndGamma(primaries, gamma)
    return true;
}

bool DoOH_NativeColorSpaceManager_GetColorSpaceNameFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
 
    OH_NativeColorSpaceManager* nativeColorSpaceManager = nullptr;
    ColorSpaceName colorSpaceName = ColorSpaceName::NONE;
    nativeColorSpaceManager = OH_NativeColorSpaceManager_CreateFromName(colorSpaceName);
    ColorSpaceName colorSpaceName = static_cast<ColorSpaceName>(
        OH_NativeColorSpaceManager_GetColorSpaceName(nativeColorSpaceManager));
    return true;
}

bool DoOH_NativeColorSpaceManager_GetGammaFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
 
    OH_NativeColorSpaceManager* nativeColorSpaceManager = nullptr;
    ColorSpaceName colorSpaceName = ColorSpaceName::NONE;
    nativeColorSpaceManager = OH_NativeColorSpaceManager_CreateFromName(colorSpaceName);
    ColorSpaceName colorSpaceName = static_cast<ColorSpaceName>(
        OH_NativeColorSpaceManager_GetGamma(nativeColorSpaceManager));
    return true;
}
} // namespace ColorSpaceManager
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::ColorManager::Init(data, size)) {
        return -1;
    }
    
    /* Run your code on data */
    OHOS::ColorManager::ColorFuzzTest(data, size);
    OHOS::ColorManager::ColorSpaceFuzzTest(data, size);
    OHOS::ColorManager::ColorSpaceConvertorFuzzTest(data, size);
    return 0;
}
