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

#ifndef CONICAL_GRADIENT_SHADER_OBJ_H
#define CONICAL_GRADIENT_SHADER_OBJ_H

#include "draw/ui_color.h"
#include "effect/gradient_shader_obj_base.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class ConicalGradientShaderObj final : public GradientShaderObjBase {
public:
    static std::shared_ptr<ConicalGradientShaderObj> CreateForUnmarshalling();
    static std::shared_ptr<ConicalGradientShaderObj> Create(const Point& startPt, scalar startRadius,
        const Point& endPt, scalar endRadius, const std::vector<UIColor>& colors,
        std::shared_ptr<ColorSpace> colorSpace, const std::vector<scalar>& pos,
        TileMode mode, const Matrix* matrix);
    ~ConicalGradientShaderObj() override = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) override;
    bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth = 0) override;
#endif
    std::shared_ptr<void> GenerateBaseObject() override;

private:
    ConicalGradientShaderObj();
    ConicalGradientShaderObj(const Point& startPt, scalar startRadius,
        const Point& endPt, scalar endRadius, const std::vector<UIColor>& colors,
        std::shared_ptr<ColorSpace> colorSpace, const std::vector<scalar>& pos,
        TileMode mode, const Matrix* matrix);

    Point startPt_;
    scalar startRadius_;
    Point endPt_;
    scalar endRadius_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // CONICAL_GRADIENT_SHADER_OBJ_H
