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

#ifndef STABILITY_SHADER_EFFECT_H
#define STABILITY_SHADER_EFFECT_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

// ShaderEffectCreateColorShader-ShaderEffectDestroy循环
class StabilityShaderEffectCreateColorShader : public TestBase {
public:
    StabilityShaderEffectCreateColorShader()
    {
        fileName_ = "StabilityShaderEffectCreateColorShader";
    }
    ~StabilityShaderEffectCreateColorShader() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ShaderEffectCreateLinearGradient-ShaderEffectDestroy循环
class StabilityShaderEffectCreateLinearGradient : public TestBase {
public:
    StabilityShaderEffectCreateLinearGradient()
    {
        fileName_ = "StabilityShaderEffectCreateLinearGradient";
    }
    ~StabilityShaderEffectCreateLinearGradient() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ShaderEffectCreateLinearGradientWithLocalMatrix-ShaderEffectDestroy循环
class StabilityShaderEffectCreateLinearGradientWithLocalMatrix : public TestBase {
public:
    StabilityShaderEffectCreateLinearGradientWithLocalMatrix()
    {
        fileName_ = "StabilityShaderEffectCreateLinearGradientWithLocalMatrix";
    }
    ~StabilityShaderEffectCreateLinearGradientWithLocalMatrix() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ShaderEffectCreateRadialGradient-ShaderEffectDestroy循环
class StabilityShaderEffectCreateRadialGradient : public TestBase {
public:
    StabilityShaderEffectCreateRadialGradient()
    {
        fileName_ = "StabilityShaderEffectCreateRadialGradient";
    }
    ~StabilityShaderEffectCreateRadialGradient() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ShaderEffectCreateRadialGradientWithLocalMatrix-ShaderEffectDestroy循环
class StabilityShaderEffectCreateRadialGradientWithLocalMatrix : public TestBase {
public:
    StabilityShaderEffectCreateRadialGradientWithLocalMatrix()
    {
        fileName_ = "StabilityShaderEffectCreateRadialGradientWithLocalMatrix";
    }
    ~StabilityShaderEffectCreateRadialGradientWithLocalMatrix() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ShaderEffectCreateSweepGradient-ShaderEffectDestroy循环
class StabilityShaderEffectCreateSweepGradient : public TestBase {
public:
    StabilityShaderEffectCreateSweepGradient()
    {
        fileName_ = "StabilityShaderEffectCreateSweepGradient";
    }
    ~StabilityShaderEffectCreateSweepGradient() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ShaderEffectCreateImageShader-ShaderEffectDestroy循环
class StabilityShaderEffectCreateImageShader : public TestBase {
public:
    StabilityShaderEffectCreateImageShader()
    {
        fileName_ = "StabilityShaderEffectCreateImageShader";
    }
    ~StabilityShaderEffectCreateImageShader() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ShaderEffectCreateTwoPointConicalGradient-ShaderEffectDestroy循环
class StabilityShaderEffectCreateTwoPointConicalGradient : public TestBase {
public:
    StabilityShaderEffectCreateTwoPointConicalGradient()
    {
        fileName_ = "StabilityShaderEffectCreateTwoPointConicalGradient";
    }
    ~StabilityShaderEffectCreateTwoPointConicalGradient() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

#endif // STABILITY_SHADER_EFFECT_H
