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

#include "rsinterfaces_fuzzer.h"

#include <securec.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
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

bool RSPhysicalScreenFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    uint64_t id = GetData<uint64_t>();
    uint32_t modeId = GetData<uint32_t>();
    uint32_t status = GetData<uint32_t>();
    uint32_t level = GetData<uint32_t>();
    int32_t modeIdx = GetData<uint32_t>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    bool canvasRotation = GetData<bool>();
    uint32_t scaleMode = GetData<uint32_t>();

    // test
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenActiveMode(static_cast<ScreenId>(id), modeId);
    rsInterfaces.SetScreenPowerStatus(static_cast<ScreenId>(id), static_cast<ScreenPowerStatus>(status));
    rsInterfaces.SetScreenBacklight(static_cast<ScreenId>(id), level);
    rsInterfaces.SetScreenColorGamut(static_cast<ScreenId>(id), modeIdx);
    ScreenGamutMap mapMode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    rsInterfaces.SetScreenGamutMap(static_cast<ScreenId>(id), mapMode);
    rsInterfaces.SetScreenSkipFrameInterval(static_cast<ScreenId>(id), skipFrameInterval);
    
    rsInterfaces.GetScreenActiveMode(static_cast<ScreenId>(id));
    rsInterfaces.GetScreenSupportedModes(static_cast<ScreenId>(id));
    rsInterfaces.GetScreenCapability(static_cast<ScreenId>(id));
    rsInterfaces.GetScreenPowerStatus(static_cast<ScreenId>(id));
    rsInterfaces.GetScreenData(static_cast<ScreenId>(id));
    rsInterfaces.GetScreenBacklight(static_cast<ScreenId>(id));
    std::vector<ScreenColorGamut> modes = { ScreenColorGamut::COLOR_GAMUT_INVALID };
    rsInterfaces.GetScreenSupportedColorGamuts(static_cast<ScreenId>(id), modes);
    std::vector<ScreenHDRMetadataKey> keys = {ScreenHDRMetadataKey::MATAKEY_RED_PRIMARY_X};
    rsInterfaces.GetScreenSupportedMetaDataKeys(static_cast<ScreenId>(id), keys);
    ScreenColorGamut mode = ScreenColorGamut::COLOR_GAMUT_INVALID;
    rsInterfaces.GetScreenColorGamut(static_cast<ScreenId>(id), mode);
    rsInterfaces.GetScreenGamutMap(static_cast<ScreenId>(id), mapMode);
    RSScreenHDRCapability screenHdrCapability;
    rsInterfaces.GetScreenHDRCapability(static_cast<ScreenId>(id), screenHdrCapability);
    RSScreenType screenType = RSScreenType::BUILT_IN_TYPE_SCREEN;
    rsInterfaces.GetScreenType(static_cast<ScreenId>(id), screenType);
    SurfaceOcclusionChangeCallback surfaceOcclusionCb = [](float) {};
    std::vector<float> partitionPoints;
    rsInterfaces.RegisterSurfaceOcclusionChangeCallback(static_cast<NodeId>(id), surfaceOcclusionCb, partitionPoints);
    rsInterfaces.UnRegisterSurfaceOcclusionChangeCallback(static_cast<NodeId>(id));
    rsInterfaces.ResizeVirtualScreen(static_cast<NodeId>(id), width, height);
    rsInterfaces.SetVirtualMirrorScreenCanvasRotation(static_cast<ScreenId>(id), canvasRotation);
    rsInterfaces.SetVirtualMirrorScreenScaleMode(static_cast<ScreenId>(id), static_cast<ScreenScaleMode>(scaleMode));

    sleep(1);

    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSPhysicalScreenFuzzTest(data, size);
    return 0;
}