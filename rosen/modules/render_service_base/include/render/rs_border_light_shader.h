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
#ifndef RENDER_SERVICE_BASE_CORE_RENDER_RS_BORDER_LIGHT_SHADER_H
#define RENDER_SERVICE_BASE_CORE_RENDER_RS_BORDER_LIGHT_SHADER_H

#include "common/rs_vector3.h"
#include "common/rs_vector4.h"
#include "render/rs_shader.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
class GEBorderLightShader;
struct RSBorderLightParams {
    Vector3f lightPosition_;
    Vector4f lightColor_;
    float lightIntensity_;
    float lightWidth_;
    Drawing::Matrix matrix_;
};

class RSB_EXPORT RSBorderLightShader : public RSShader {
public:
    RSBorderLightShader();
    RSBorderLightShader(const RSBorderLightParams& borderLightParam);
    RSBorderLightShader(const RSBorderLightShader&) = delete;
    RSBorderLightShader operator=(const RSBorderLightShader&) = delete;
    ~RSBorderLightShader() override = default;

    void MakeDrawingShader(const RectF& rect, float progress) override;
    const std::shared_ptr<Drawing::ShaderEffect>& GetDrawingShader() const override;
    void SetRSBorderLightParams(const RSBorderLightParams& borderLightParam);
    void SetMatrix(const Drawing::Matrix& matrix);
    bool Marshalling(Parcel& parcel) override;
    bool Unmarshalling(Parcel& parcel, bool& needReset) override;

private:
    RSBorderLightParams borderLightParam_;
    std::shared_ptr<GEBorderLightShader> geShader_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif