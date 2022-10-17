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
#include "transaction/rs_interfaces.h"

#include <securec.h>
#include <stddef.h>
#include <stdint.h>

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* data_ = nullptr;
size_t size_ = 0;
size_t pos;
} // namespace

/*
 * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (data_ == nullptr || objectSize > size_ - pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    pos += objectSize;
    return object;
}

bool RSScreenFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    // test
    RSInterfaces* rsInterfaces = &(RSInterfaces::GetInstance());
    uint64_t id = GetData<uint64_t>();
    rsInterfaces->RemoveVirtualScreen(static_cast<ScreenId>(id));
    uint32_t modeId = GetData<uint32_t>();
    rsInterfaces->SetScreenActiveMode(static_cast<ScreenId>(id), modeId);
    rsInterfaces->GetScreenActiveMode(static_cast<ScreenId>(id));
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    rsInterfaces->SetVirtualScreenResolution(static_cast<ScreenId>(id), width, height);
    rsInterfaces->GetVirtualScreenResolution(static_cast<ScreenId>(id));
    uint32_t status = GetData<uint32_t>();
    rsInterfaces->SetScreenPowerStatus(static_cast<ScreenId>(id), static_cast<ScreenPowerStatus>(status));
    rsInterfaces->GetScreenSupportedModes(static_cast<ScreenId>(id));
    rsInterfaces->GetScreenCapability(static_cast<ScreenId>(id));
    rsInterfaces->GetScreenPowerStatus(static_cast<ScreenId>(id));
    rsInterfaces->GetScreenData(static_cast<ScreenId>(id));
    rsInterfaces->GetScreenBacklight(static_cast<ScreenId>(id));
    uint32_t level = GetData<uint32_t>();
    rsInterfaces->SetScreenBacklight(static_cast<ScreenId>(id), level);
    std::vector<ScreenColorGamut> modes;
    rsInterfaces->GetScreenSupportedColorGamuts(static_cast<ScreenId>(id), modes);
    std::vector<ScreenHDRMetadataKey> keys;
    rsInterfaces->GetScreenSupportedMetaDataKeys(static_cast<ScreenId>(id), keys);
    ScreenColorGamut mode;
    rsInterfaces->GetScreenColorGamut(static_cast<ScreenId>(id), mode);
    int32_t modeIdx = GetData<uint32_t>();
    rsInterfaces->SetScreenColorGamut(static_cast<ScreenId>(id), modeIdx);
    ScreenGamutMap mapMode;
    rsInterfaces->GetScreenGamutMap(static_cast<ScreenId>(id), mapMode);
    rsInterfaces->SetScreenGamutMap(static_cast<ScreenId>(id), mapMode);
    RSScreenHDRCapability screenHdrCapability;
    rsInterfaces->GetScreenHDRCapability(static_cast<ScreenId>(id), screenHdrCapability);
    RSScreenType screenType;
    rsInterfaces->GetScreenType(static_cast<ScreenId>(id), screenType);
    uint32_t skipFrameInterval = GetData<uint32_t>();
    rsInterfaces->SetScreenSkipFrameInterval(static_cast<ScreenId>(id), skipFrameInterval);
    
    return true;
}

bool UpdateRenderModeFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    // get data
    bool isUniRender = GetData<bool>();

    // test
    RSInterfaces* rsInterfaces = &(RSInterfaces::GetInstance());
    rsInterfaces->UpdateRenderMode(isUniRender);

    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSScreenFuzzTest(data, size);
    OHOS::Rosen::UpdateRenderModeFuzzTest(data, size);
    return 0;
}