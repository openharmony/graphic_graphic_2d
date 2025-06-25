/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_RENDER_RENDER_RS_SHADER_H
#define RENDER_SERVICE_BASE_RENDER_RENDER_RS_SHADER_H

#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "effect/shader_effect.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSShader {
public:
    enum class ShaderType{
        DRAWING = 0,
        DOT_MATRIX,
        FLOW_LIGHT_SWEEP,
        COMPLEX,
        BORDER_LIGHT,
    };

    RSShader() = default;
    virtual ~RSShader() = default;
    static std::shared_ptr<RSShader> CreateRSShader();
    static std::shared_ptr<RSShader> CreateRSShader(const ShaderType& type);
    static std::shared_ptr<RSShader> CreateRSShader(const std::shared_ptr<Drawing::ShaderEffect>& drShader);

    void SetDrawingShader(const std::shared_ptr<Drawing::ShaderEffect>& drShader);
    virtual void MakeDrawingShader(const RectF& rect, float progress) {};
    virtual void MakeDrawingShader(const RectF& rect, std::vector<float> params) {};
    virtual const std::shared_ptr<Drawing::ShaderEffect>& GetDrawingShader() const;

    virtual bool Marshalling(Parcel& parcel);
    virtual bool Unmarshalling(Parcel& parcel, bool& needReset);

    inline const ShaderType& GetShaderType() const { return type_; } ;

private:
    RSShader(const RSShader&) = delete;
    RSShader(const RSShader&&) = delete;
    RSShader& operator=(const RSShader&) = delete;
    RSShader& operator=(const RSShader&&) = delete;

protected:
    ShaderType type_ = ShaderType::DRAWING;
    std::shared_ptr<Drawing::ShaderEffect> drShader_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_RENDER_RS_SHADER_H
