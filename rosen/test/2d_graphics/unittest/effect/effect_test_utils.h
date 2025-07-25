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

#ifndef EFFECT_TEST_UTILS_H
#define EFFECT_TEST_UTILS_H

#include <functional>
#include <memory>
#include "gtest/gtest.h"

#ifdef ROSEN_OHOS
#include <parcel.h>
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {

class ImageFilter;
class ShaderEffect;
class Data;

class EffectTestUtils {
public:
    // Compare serialization data between two ImageFilters
    static bool CompareImageFilter(const std::shared_ptr<ImageFilter>& filter1,
                                   const std::shared_ptr<ImageFilter>& filter2);

    // Compare serialization data between two ShaderEffects
    static bool CompareShaderEffect(const std::shared_ptr<ShaderEffect>& shader1,
                                    const std::shared_ptr<ShaderEffect>& shader2);

#ifdef ROSEN_OHOS
    // Setup marshalling/unmarshalling callbacks for effect tests
    static void SetupMarshallingCallbacks();

    // Restore original marshalling/unmarshalling callbacks
    static void RestoreMarshallingCallbacks();

    // Temporarily clear marshalling callback and return saved callback for restoration
    static std::function<bool(Parcel&, std::shared_ptr<Data>)> ClearMarshallingCallback();

    // Restore specific marshalling callback
    static void RestoreMarshallingCallback(const std::function<bool(Parcel&, std::shared_ptr<Data>)>& callback);

private:
    static std::function<bool(Parcel&, std::shared_ptr<Data>)> originalMarshallingCallback_;
    static std::function<std::shared_ptr<Data>(Parcel&)> originalUnmarshallingCallback_;
#endif
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // EFFECT_TEST_UTILS_H