/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
    MemoryGraphic(int32_t pid, float glSize, float graphicSize);
    ~MemoryGraphic() noexcept = default;
    MemoryGraphic& operator+=(const MemoryGraphic& other);
    void IncreaseGLMemory(float glSize);
    void IncreaseGraphicMemory(float graphicSize);


    bool Marshalling(Parcel &parcel) const override;
    static MemoryGraphic *Unmarshalling(Parcel &parcel);

    int32_t GetPid() const;
    float GetGLMemorySize() const;
    float GetGraphicMemorySize() const;

    void SetPid(int32_t pid);
    void SetGLMemorySize(float glSize);
    void SetGraphicMemorySize(float graphicSize);

private:
    int32_t pid_;
    float glSize_;
    float graphicSize_;
};
} // namespace Rosen
} // namespace OHOS

#endif // MEMORY_GRAPHIC