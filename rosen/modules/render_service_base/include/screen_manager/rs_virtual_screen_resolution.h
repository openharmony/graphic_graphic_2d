/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RS_VIRTUAL_SCREEN_RESOLUTION
#define RS_VIRTUAL_SCREEN_RESOLUTION

#include <cstdint>
#include <parcel.h>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSVirtualScreenResolution : public Parcelable {
public:
    RSVirtualScreenResolution() = default;
    RSVirtualScreenResolution(uint32_t width, uint32_t height);
    ~RSVirtualScreenResolution() noexcept = default;

    RSVirtualScreenResolution(const RSVirtualScreenResolution& other);
    RSVirtualScreenResolution& operator=(const RSVirtualScreenResolution& other);

    bool Marshalling(Parcel &parcel) const override;
    static RSVirtualScreenResolution *Unmarshalling(Parcel &parcel);

    uint32_t GetVirtualScreenWidth() const;
    uint32_t GetVirtualScreenHeight() const;
    void SetVirtualScreenWidth(uint32_t width);
    void SetVirtualScreenHeight(uint32_t height);

private:
    uint32_t width_ = 0;
    uint32_t height_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_VIRTUAL_SCREEN_RESOLUTION