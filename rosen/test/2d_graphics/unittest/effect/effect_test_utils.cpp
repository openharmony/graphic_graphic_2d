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

#include "effect_test_utils.h"

#include "effect/image_filter.h"
#include "effect/shader_effect.h"

#ifdef ROSEN_OHOS
#include "transaction/rs_marshalling_helper.h"
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

#ifdef ROSEN_OHOS
std::function<bool(Parcel&, std::shared_ptr<Data>)> EffectTestUtils::originalMarshallingCallback_;
std::function<std::shared_ptr<Data>(Parcel&)> EffectTestUtils::originalUnmarshallingCallback_;
#endif

bool EffectTestUtils::CompareImageFilter(const std::shared_ptr<ImageFilter>& filter1,
                                         const std::shared_ptr<ImageFilter>& filter2)
{
    // Handle null filters - both null is equal
    if (filter1 == nullptr && filter2 == nullptr) {
        return true;
    }
    if (filter1 == nullptr || filter2 == nullptr) {
        return false;
    }

    // Serialize both filters
    auto data1 = filter1->Serialize();
    auto data2 = filter2->Serialize();
    // Handle null serialization data - both null is equal
    if (data1 == nullptr && data2 == nullptr) {
        return true;
    }
    if (data1 == nullptr || data2 == nullptr) {
        return false;
    }

    // Handle empty data - both empty is equal
    if (data1->GetSize() == 0 && data2->GetSize() == 0) {
        return true;
    }
    if (data1->GetSize() == 0 || data2->GetSize() == 0 ||
        data1->GetSize() != data2->GetSize()) {
        return false;
    }

    // Compare serialized content byte by byte
    const uint8_t* bytes1 = static_cast<const uint8_t*>(data1->GetData());
    const uint8_t* bytes2 = static_cast<const uint8_t*>(data2->GetData());
    if (bytes1 == nullptr || bytes2 == nullptr) {
        return false;
    }

    int memResult = memcmp(bytes1, bytes2, data1->GetSize());
    return memResult == 0;
}

bool EffectTestUtils::CompareShaderEffect(const std::shared_ptr<ShaderEffect>& shader1,
                                          const std::shared_ptr<ShaderEffect>& shader2)
{
    // Handle null shaders - both null is equal
    if (shader1 == nullptr && shader2 == nullptr) {
        return true;
    }
    if (shader1 == nullptr || shader2 == nullptr) {
        return false;
    }

    // Serialize both shaders
    auto data1 = shader1->Serialize();
    auto data2 = shader2->Serialize();
    // Handle null serialization data - both null is equal
    if (data1 == nullptr && data2 == nullptr) {
        return true;
    }
    if (data1 == nullptr || data2 == nullptr) {
        return false;
    }

    // Handle empty data - both empty is equal
    if (data1->GetSize() == 0 && data2->GetSize() == 0) {
        return true;
    }
    if (data1->GetSize() == 0 || data2->GetSize() == 0 ||
        data1->GetSize() != data2->GetSize()) {
        return false;
    }

    // Compare serialized content byte by byte
    const uint8_t* bytes1 = static_cast<const uint8_t*>(data1->GetData());
    const uint8_t* bytes2 = static_cast<const uint8_t*>(data2->GetData());
    if (bytes1 == nullptr || bytes2 == nullptr) {
        return false;
    }

    int memResult = memcmp(bytes1, bytes2, data1->GetSize());
    return memResult == 0;
}

#ifdef ROSEN_OHOS
void EffectTestUtils::SetupMarshallingCallbacks()
{
    // Save original callbacks
    originalMarshallingCallback_ = ObjectHelper::Instance().GetDataMarshallingCallback();
    originalUnmarshallingCallback_ = ObjectHelper::Instance().GetDataUnmarshallingCallback();

    // Register Data marshalling/unmarshalling callbacks
    ObjectHelper::Instance().SetDataMarshallingCallback(
        [](Parcel& parcel, std::shared_ptr<Data> data) -> bool {
            return RSMarshallingHelper::Marshalling(parcel, data);
        });
    ObjectHelper::Instance().SetDataUnmarshallingCallback(
        [](Parcel& parcel) -> std::shared_ptr<Data> {
            std::shared_ptr<Data> data;
            return RSMarshallingHelper::Unmarshalling(parcel, data) ? data : nullptr;
        });
}

void EffectTestUtils::RestoreMarshallingCallbacks()
{
    // Restore original callbacks
    ObjectHelper::Instance().SetDataMarshallingCallback(originalMarshallingCallback_);
    ObjectHelper::Instance().SetDataUnmarshallingCallback(originalUnmarshallingCallback_);
}

std::function<bool(Parcel&, std::shared_ptr<Data>)> EffectTestUtils::ClearMarshallingCallback()
{
    auto savedCallback = ObjectHelper::Instance().GetDataMarshallingCallback();
    ObjectHelper::Instance().SetDataMarshallingCallback(nullptr);
    return savedCallback;
}

void EffectTestUtils::RestoreMarshallingCallback(
    const std::function<bool(Parcel&, std::shared_ptr<Data>)>& callback)
{
    ObjectHelper::Instance().SetDataMarshallingCallback(callback);
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS