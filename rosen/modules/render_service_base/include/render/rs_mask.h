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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_MASK_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_MASK_H

#include <memory>
#include "common/rs_macros.h"
#include "draw/pen.h"
#include "draw/brush.h"
#include "draw/path.h"
#include "image/picture.h"
#if defined(NEW_SKIA)
#include "modules/svg/include/SkSVGDOM.h"
#else
#include "experimental/svg/model/SkSVGDOM.h"
#endif
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
enum class MaskType {
    NONE = 0,
    SVG,
    GRADIENT,
    PATH,
};

class RSB_EXPORT RSMask : public std::enable_shared_from_this<RSMask> {
public:
    RSMask();
    virtual ~RSMask();
    static std::shared_ptr<RSMask> CreateGradientMask(const Drawing::Brush& maskBrush);
    static std::shared_ptr<RSMask> CreatePathMask(const Drawing::Path& maskPath, const Drawing::Brush& maskBrush);
    static std::shared_ptr<RSMask> CreatePathMask(
        const Drawing::Path& maskPath, const Drawing::Pen&  maskPen, const Drawing::Brush& maskBrush);
    static std::shared_ptr<RSMask> CreateSVGMask(double x, double y, double scaleX, double scaleY,
        const sk_sp<SkSVGDOM>& svgDom);

    void SetSvgX(double x);
    double GetSvgX() const;
    void SetSvgY(double y);
    double GetSvgY() const;
    void SetScaleX(double scaleX);
    double GetScaleX() const;
    void SetScaleY(double scaleY);
    double GetScaleY() const;
    void SetMaskPath(const Drawing::Path& path);
    std::shared_ptr<Drawing::Path> GetMaskPath() const;
    void SetMaskPen(const Drawing::Pen& pen);
    Drawing::Pen GetMaskPen() const;
    void SetMaskBrush(const Drawing::Brush& brush);
    Drawing::Brush GetMaskBrush() const;
    bool MarshallingPathAndBrush(Parcel& parcel) const;
    void SetSvgDom(const sk_sp<SkSVGDOM>& svgDom);
    sk_sp<SkSVGDOM> GetSvgDom() const;
    std::shared_ptr<Drawing::Picture> GetSvgPicture() const;
    void SetMaskType(MaskType type);
    bool IsSvgMask() const;
    bool IsGradientMask() const;
    bool IsPathMask() const;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const;
    [[nodiscard]] static RSMask* Unmarshalling(Parcel& parcel);
#endif

protected:
    RSMask(const RSMask&) = delete;
    RSMask(const RSMask&&) = delete;
    RSMask& operator=(const RSMask&) = delete;
    RSMask& operator=(const RSMask&&) = delete;

private:
    MaskType type_ = MaskType::NONE;
    double svgX_ = 0.0f;
    double svgY_ = 0.0f;
    double scaleX_ = 1.0f;
    double scaleY_ = 1.0f;
    sk_sp<SkSVGDOM> svgDom_;
    std::shared_ptr<Drawing::Picture> svgPicture_;
    Drawing::Pen maskPen_;
    Drawing::Brush maskBrush_;
    std::shared_ptr<Drawing::Path> maskPath_;

};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_MASK_H