/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef SDF_SHAPER_BASE_H
#define SDF_SHAPER_BASE_H

#include "common/rs_macros.h"
#include "utils/drawing_macros.h"
#include <memory>
#include <vector>

namespace OHOS {
namespace Rosen {
namespace Drawing {

class DRAWING_API SDFShapeBase
{
public:
    SDFShapeBase() {};
    ~SDFShapeBase() {};

    virtual std::string Getshader() const = 0;
    virtual float GetSize() const = 0;
    virtual void SetSize(float size) = 0;

    virtual int GetParaNum() const = 0;
    virtual std::vector<float> GetPara() const = 0;
    virtual std::vector<float> GetTransPara() const = 0;

    virtual std::vector<float> GetColorPara() const = 0;

    virtual void UpdateTime(float time) = 0;
    virtual void BuildShader() = 0;
    virtual void SetColor(std::string fillColor, std::string strokeColor, float alpha) = 0;

private:

};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_SDF_SHAPER_BASE_H