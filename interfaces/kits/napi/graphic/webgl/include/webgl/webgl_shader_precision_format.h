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

#ifndef WEBGL_SHADER_PRECISION_FORMAT_H
#define WEBGL_SHADER_PRECISION_FORMAT_H

#include "napi/n_exporter.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
class WebGLShaderPrecisionFormat final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLShaderPrecisionFormat";
    inline static const int objectType = WEBGL_OBJECT_SHADER;

    bool Export(napi_env env, napi_value exports) override;

    std::string GetClassName() override;

    static napi_value Constructor(napi_env env, napi_callback_info info);
    static WebGLShaderPrecisionFormat *GetObjectFromArg(napi_env env, napi_callback_info info);
    static napi_value GetShaderPrecisionFormatRangeMin(napi_env env, napi_callback_info info);
    static napi_value GetShaderPrecisionFormatRangeMax(napi_env env, napi_callback_info info);
    static napi_value GetShaderPrecisionFormatPrecision(napi_env env, napi_callback_info info);
    static NVal CreateObjectInstance(napi_env env, WebGLShaderPrecisionFormat **instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLShaderPrecisionFormat>(env, instance);
    }

    void SetShaderPrecisionFormatRangeMin(int rangeMin)
    {
        rangeMin_ = rangeMin;
    }

    void SetShaderPrecisionFormatRangeMax(int rangeMax)
    {
        rangeMax_ = rangeMax;
    }

    void SetShaderPrecisionFormatPrecision(int precision)
    {
        precision_ = precision;
    }

    int GetShaderPrecisionFormatRangeMin() const
    {
        return rangeMin_;
    }

    int GetShaderPrecisionFormatRangeMax() const
    {
        return rangeMax_;
    }

    int GetShaderPrecisionFormatPrecision() const
    {
        return precision_;
    }
    explicit WebGLShaderPrecisionFormat() : rangeMin_(0), rangeMax_(0), precision_(0) {};

    WebGLShaderPrecisionFormat(napi_env env, napi_value exports) : NExporter(env, exports),
        rangeMin_(0), rangeMax_(0), precision_(0) {};

    ~WebGLShaderPrecisionFormat() {};
private:
    int rangeMin_;
    int rangeMax_;
    int precision_;
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_SHADER_PRECISION_FORMAT_H
