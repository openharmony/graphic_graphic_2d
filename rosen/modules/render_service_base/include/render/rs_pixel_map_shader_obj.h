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

#ifndef RENDER_SERVICE_BASE_RS_PIXEL_MAP_SHADER_OBJ_H
#define RENDER_SERVICE_BASE_RS_PIXEL_MAP_SHADER_OBJ_H

#include "common/rs_macros.h"
#include "effect/shader_effect_obj.h"
#include "effect/shader_effect_lazy.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
// Only supports the Drawing NDK interface.
class RSB_EXPORT RSPixelMapShaderObj final : public Drawing::ShaderEffectObj {
public:
    static std::shared_ptr<Drawing::ShaderEffectLazy> CreatePixelMapShader(
        std::shared_ptr<Media::PixelMap> pixelMap,
        Drawing::TileMode tileX,
        Drawing::TileMode tileY,
        const Drawing::SamplingOptions& sampling,
        const Drawing::Matrix& matrix);
    static std::shared_ptr<RSPixelMapShaderObj> CreateForUnmarshalling();
    ~RSPixelMapShaderObj() override = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) override;
    bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth = 0) override;
#endif
    std::shared_ptr<void> GenerateBaseObject() override;

private:
    RSPixelMapShaderObj();
    RSPixelMapShaderObj(
        std::shared_ptr<Media::PixelMap> pixelMap,
        Drawing::TileMode tileX,
        Drawing::TileMode tileY,
        const Drawing::SamplingOptions& sampling,
        const Drawing::Matrix& matrix);
    std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
    Drawing::TileMode tileX_ = Drawing::TileMode::CLAMP;
    Drawing::TileMode tileY_ = Drawing::TileMode::CLAMP;
    Drawing::SamplingOptions sampling_;
    Drawing::Matrix matrix_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RS_PIXEL_MAP_SHADER_OBJ_H