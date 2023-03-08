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
    MemoryGraphic(int32_t pid, uint64_t glSize, uint64_t graphicSize);
    ~MemoryGraphic() noexcept = default;

    bool Marshalling(Parcel &parcel) const override;
    static MemoryGraphic *Unmarshalling(Parcel &parcel);

    int32_t GetPid() const;
    uint64_t GetGLMemorySize() const;
    uint64_t GetGraphicMemorySize() const;

    void SetPid(int32_t pid);
    void SetGLMemorySize(uint64_t glSize);
    void SetGraphicMemorySize(uint64_t graphicSize);

private:
    int32_t pid_;
    uint64_t glSize_;
    uint64_t graphicSize_;
};
} // namespace Rosen
} // namespace OHOS

#endif // MEMORY_GRAPHIC