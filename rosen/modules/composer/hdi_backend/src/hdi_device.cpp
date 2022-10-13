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

#include <mutex>
#include <scoped_bytrace.h>
#include "hdi_device.h"
#include "hdi_device_impl.h"

namespace OHOS {
namespace Rosen {
HdiDevice* HdiDevice::GetInstance()
{
    static HdiDeviceImpl instance;
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    {
        if (instance.Init() != ROSEN_ERROR_OK) {
            HLOGE("hdi device get instances failed.");
            return nullptr;
        }
    }
    return &instance;
}
} // namespace Rosen
} // namespace OHOS