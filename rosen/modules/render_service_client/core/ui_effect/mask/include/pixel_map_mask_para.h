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
#ifndef UIEFFECT_PIXEL_MAP_MASK_PARA_H
#define UIEFFECT_PIXEL_MAP_MASK_PARA_H

#include "pixel_map.h"
#include "ui_effect/mask/include/mask_para.h"
#include "common/rs_common_def.h"
#include "common/rs_vector4.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT PixelMapMaskPara : public MaskPara {
public:
    PixelMapMaskPara()
    {
        type_ = MaskPara::Type::PIXEL_MAP_MASK;
    }
    ~PixelMapMaskPara() override = default;

    PixelMapMaskPara(const PixelMapMaskPara& other);

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

    void SetFillColor(const Vector4f& fillColor)
    {
        fillColor_ = fillColor;
    }

    const Vector4f& GetFillColor() const
    {
        return fillColor_;
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
    Vector4f fillColor_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_PIXEL_MAP_MASK_PARA_H
