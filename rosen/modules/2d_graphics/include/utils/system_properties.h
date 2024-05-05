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

#ifndef SYSTEM_PROPERTIES
#define SYSTEM_PROPERTIES

namespace OHOS {
namespace Rosen {
enum class GpuApiType {
    OPENGL = 0,
    VULKAN,
    DDGR,
};
namespace Drawing {
class SystemProperties {
public:
    static inline GpuApiType GetGpuApiType()
    {
        return SystemProperties::systemGpuApiType_;
    }

    static inline bool IsUseVulkan()
    {
        return SystemProperties::GetGpuApiType() != GpuApiType::OPENGL;
    }

    static bool GetHMSymbolEnable();
    static GpuApiType GetSystemGraphicGpuType();

private:
    static const GpuApiType systemGpuApiType_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SYSTEM_PROPERTIES