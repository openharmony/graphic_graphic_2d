/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ROSENRENDER_ROSEN_WEBGL_SHADER_PRECISION_FORMAT
#define ROSENRENDER_ROSEN_WEBGL_SHADER_PRECISION_FORMAT

#include "../../../common/napi/n_exporter.h"

namespace OHOS {
namespace Rosen {
class WebGLShaderPrecisionFormat final : public NExporter {
public:
    inline static const std::string className = "WebGLShaderPrecisionFormat";

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);

    void SetShaderPrecisionFormatRangeMin(int rangeMin)
    {
        m_rangeMin = rangeMin;
    }

    void SetShaderPrecisionFormatRangeMax(int rangeMax)
    {
        m_rangeMax = rangeMax;
    }

    void SetShaderPrecisionFormatPrecision(int precision)
    {
        m_precision = precision;
    }

    int GetsetShaderPrecisionFormatRangeMin() const
    {
        return m_rangeMin;
    }

    int GetShaderPrecisionFormatRangeMax() const
    {
        return m_rangeMax;
    }

    int GetShaderPrecisionFormatPrecision() const
    {
        return m_precision;
    }
    explicit WebGLShaderPrecisionFormat() : m_rangeMin(0), m_rangeMax(0), m_precision(0) {};

    WebGLShaderPrecisionFormat(napi_env env, napi_value exports) : NExporter(env, exports),
        m_rangeMin(0), m_rangeMax(0), m_precision(0) {};

    ~WebGLShaderPrecisionFormat() {};
private:
    int m_rangeMin;
    int m_rangeMax;
    int m_precision;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_SHADER_PRECISION_FORMAT
