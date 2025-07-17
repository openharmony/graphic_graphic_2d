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

#ifndef SHADER_IMAGE_FILTER_OBJ_H
#define SHADER_IMAGE_FILTER_OBJ_H

#include "effect/image_filter.h"
#include "effect/image_filter_obj.h"
#include "effect/shader_effect.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class ShaderImageFilterObj final : public ImageFilterObj {
public:
    static std::shared_ptr<ShaderImageFilterObj> CreateForUnmarshalling();
    static std::shared_ptr<ShaderImageFilterObj> Create(const std::shared_ptr<ShaderEffect>& shader,
        const Rect& cropRect);
    ~ShaderImageFilterObj() override = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) override;
    bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth = 0) override;
#endif
    std::shared_ptr<void> GenerateBaseObject() override;

private:
    ShaderImageFilterObj();
    ShaderImageFilterObj(const std::shared_ptr<ShaderEffect>& shader, const Rect& cropRect);
    std::shared_ptr<ShaderEffect> shader_ = nullptr;
    Rect cropRect_ = {};
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif  // SHADER_IMAGE_FILTER_OBJ_H