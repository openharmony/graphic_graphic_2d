/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef UIEFFECT_EFFECT_MASK_UNMARSHALLING_SINGLETON_H
#define UIEFFECT_EFFECT_MASK_UNMARSHALLING_SINGLETON_H

#include <unordered_map>
#include <functional>
#include <mutex>
#include <memory>

#include <parcel.h>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

class MaskPara;

class RSC_EXPORT MaskUnmarshallingSingleton final {
public:
    using UnmarshallingFunc = std::function<bool (Parcel&, std::shared_ptr<MaskPara>&)>;

    static MaskUnmarshallingSingleton& GetInstance();

    MaskUnmarshallingSingleton(const MaskUnmarshallingSingleton&) = delete;
    MaskUnmarshallingSingleton& operator=(const MaskUnmarshallingSingleton&) = delete;
    MaskUnmarshallingSingleton(MaskUnmarshallingSingleton&&) = delete;
    MaskUnmarshallingSingleton& operator=(MaskUnmarshallingSingleton&&) = delete;

    void RegisterCallback(uint16_t type, UnmarshallingFunc func);
    void UnregisterCallback(uint16_t type);
    [[nodiscard]] UnmarshallingFunc GetCallback(uint16_t type);

private:
    MaskUnmarshallingSingleton() = default;
    ~MaskUnmarshallingSingleton() = default;

private:
    std::unordered_map<uint16_t, UnmarshallingFunc> unmarshallingFuncs_;
    std::mutex mutex_;
};

} // namespace Rosen
} // namespace OHOS

#endif // UIEFFECT_EFFECT_MASK_UNMARSHALLING_SINGLETON_H