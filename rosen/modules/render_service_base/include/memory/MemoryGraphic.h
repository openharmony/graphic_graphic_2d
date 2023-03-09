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

#ifndef MEMORY_GRAPHIC
#define MEMORY_GRAPHIC

#include <cstdint>
#include <parcel.h>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT MemoryGraphic : public Parcelable {
public:
    MemoryGraphic() = default;
    MemoryGraphic(int32_t pid, float cpuMemSize, float gpuMemSize);
    ~MemoryGraphic() noexcept override = default;
    MemoryGraphic& operator+=(const MemoryGraphic& other);
    void IncreaseCpuMemory(float glSize);
    void IncreaseGpuMemory(float graphicSize);


    bool Marshalling(Parcel &parcel) const override;
    static MemoryGraphic *Unmarshalling(Parcel &parcel);

    int32_t GetPid() const;
    float GetGpuMemorySize() const; // Memory of gpu = SkiaGpu
    float GetCpuMemorySize() const; // Memory of cpu = RSCpu + SkiaCpu

    void SetPid(int32_t pid);
    void SetGpuMemorySize(float glSize);
    void SetCpuMemorySize(float graphicSize);

private:
    int32_t pid_ = 0;
    float cpuMemSize_ = 0.0f;
    float gpuMemSize_ = 0.0f;
};
} // namespace Rosen
} // namespace OHOS

#endif // MEMORY_GRAPHIC