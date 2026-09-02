/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_FRACTAL_GLASS_MASK_PARA_H
#define UIEFFECT_FRACTAL_GLASS_MASK_PARA_H

#include "pixel_map.h"
#include "ui_effect/mask/include/mask_para.h"
#include "common/rs_common_def.h"
#include "common/rs_vector4.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT FractalGlassMaskPara : public MaskPara {
public:
    FractalGlassMaskPara()
    {
        type_ = MaskPara::Type::FRACTAL_GLASS_MASK;
    }
    ~FractalGlassMaskPara() override = default;

    FractalGlassMaskPara(const FractalGlassMaskPara& other);

    void SetPixelMap(std::shared_ptr<Media::PixelMap>& pixelMap)
    {
        pixelMap_ = pixelMap;
    }

    const std::shared_ptr<Media::PixelMap>& GetPixelMap() const
    {
        return pixelMap_;
    }

    void SetSrc(const Vector4f& src)
    {
        src_ = src;
    }

    const Vector4f& GetSrc() const
    {
        return src_;
    }

    void SetDst(const Vector4f& dst)
    {
        dst_ = dst;
    }

    const Vector4f& GetDst() const
    {
        return dst_;
    }

    void SetGlassNum(float glassNum)
    {
        glassNum_ = glassNum;
    }

    const float& GetGlassNum() const
    {
        return glassNum_;
    }

    void SetGlassStrength(float glassStrength)
    {
        glassStrength_ = glassStrength;
    }

    const float& GetGlassStrength() const
    {
        return glassStrength_;
    }

    void SetGlassSoftness(float glassSoftness)
    {
        glassSoftness_ = glassSoftness;
    }

    const float& GetGlassSoftness() const
    {
        return glassSoftness_;
    }

    void SetIsSymmetric(bool isSymmetric)
    {
        isSymmetric_ = isSymmetric;
    }

    bool GetIsSymmetric() const
    {
        return isSymmetric_;
    }

    bool IsValid() const
    {
        return ROSEN_LE(src_.x_, src_.z_) && ROSEN_LE(src_.y_, src_.w_) &&
            ROSEN_LE(dst_.x_, dst_.z_) && ROSEN_LE(dst_.y_, dst_.w_);
    }

    bool Marshalling(Parcel& parcel) const override;

    static void RegisterUnmarshallingCallback();

    [[nodiscard]] static bool OnUnmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val);

    std::shared_ptr<MaskPara> Clone() const override;

private:
    std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
    Vector4f src_;
    Vector4f dst_;
    float glassNum_ = 25.0f;
    float glassStrength_ = 1.0f;
    float glassSoftness_ = 0.001f;
    bool isSymmetric_ = true;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FRACTAL_GLASS_MASK_PARA_H