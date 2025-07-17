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

#include "render/rs_pixel_map_shader_obj.h"

#include "effect/shader_effect_lazy.h"
#include "render/rs_pixel_map_util.h"
#include "utils/log.h"

#ifdef ROSEN_OHOS
#include "transaction/rs_marshalling_helper.h"
#include "utils/object_helper.h"
#endif

namespace OHOS {
namespace Rosen {

RSPixelMapShaderObj::RSPixelMapShaderObj()
    : Drawing::ShaderEffectObj(static_cast<int32_t>(Drawing::ShaderEffect::ShaderEffectType::IMAGE))
{
}

std::shared_ptr<RSPixelMapShaderObj> RSPixelMapShaderObj::CreateForUnmarshalling()
{
    return std::shared_ptr<RSPixelMapShaderObj>(new RSPixelMapShaderObj());
}

std::shared_ptr<Drawing::ShaderEffectLazy> RSPixelMapShaderObj::CreatePixelMapShader(
    std::shared_ptr<Media::PixelMap> pixelMap,
    Drawing::TileMode tileX,
    Drawing::TileMode tileY,
    const Drawing::SamplingOptions& sampling,
    const Drawing::Matrix& matrix)
{
    if (!pixelMap) {
        return nullptr;
    }

    // Create RSPixelMapShaderObj
    auto shaderObj = std::shared_ptr<RSPixelMapShaderObj>(new RSPixelMapShaderObj(pixelMap, tileX, tileY,
        sampling, matrix));

    // Create ShaderEffectLazy wrapping the ShaderObj
    return Drawing::ShaderEffectLazy::CreateFromShaderEffectObj(shaderObj);
}

RSPixelMapShaderObj::RSPixelMapShaderObj(
    std::shared_ptr<Media::PixelMap> pixelMap,
    Drawing::TileMode tileX,
    Drawing::TileMode tileY,
    const Drawing::SamplingOptions& sampling,
    const Drawing::Matrix& matrix)
    : Drawing::ShaderEffectObj(static_cast<int32_t>(Drawing::ShaderEffect::ShaderEffectType::IMAGE)),
      pixelMap_(pixelMap),
      tileX_(tileX),
      tileY_(tileY),
      sampling_(sampling),
      matrix_(matrix)
{
}

std::shared_ptr<void> RSPixelMapShaderObj::GenerateBaseObject()
{
    if (!pixelMap_) {
        return nullptr;
    }

    // Convert PixelMap to Drawing::Image
    auto image = RSPixelMapUtil::ExtractDrawingImage(pixelMap_);
    if (!image) {
        return nullptr;
    }

    // Create ImageShader using Drawing::Image
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, tileX_, tileY_, sampling_, matrix_);

    return std::static_pointer_cast<void>(imageShader);
}

#ifdef ROSEN_OHOS
bool RSPixelMapShaderObj::Marshalling(Parcel& parcel)
{
    // Return failure if pixelMap is null
    if (!pixelMap_) {
        return false;
    }

    // Serialize PixelMap parameters (type and subType handled externally)

    // Use RSMarshallingHelper for consistent parameter serialization
    return RSMarshallingHelper::Marshalling(parcel, pixelMap_) &&
           RSMarshallingHelper::Marshalling(parcel, tileX_) &&
           RSMarshallingHelper::Marshalling(parcel, tileY_) &&
           RSMarshallingHelper::Marshalling(parcel, sampling_) &&
           RSMarshallingHelper::Marshalling(parcel, matrix_);
}

bool RSPixelMapShaderObj::Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth)
{
    // Use RSMarshallingHelper for consistent parameter deserialization
    return RSMarshallingHelper::Unmarshalling(parcel, pixelMap_) &&
           RSMarshallingHelper::Unmarshalling(parcel, tileX_) &&
           RSMarshallingHelper::Unmarshalling(parcel, tileY_) &&
           RSMarshallingHelper::Unmarshalling(parcel, sampling_) &&
           RSMarshallingHelper::Unmarshalling(parcel, matrix_);
}

// Register RSPixelMapShaderObj to ObjectHelper using macro
OBJECT_UNMARSHALLING_REGISTER(RSPixelMapShaderObj,
    static_cast<int32_t>(Drawing::Object::ObjectType::SHADER_EFFECT),
    static_cast<int32_t>(Drawing::ShaderEffect::ShaderEffectType::IMAGE));

#endif
} // namespace Rosen
} // namespace OHOS