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

#ifndef INTERFACE_SHADER_EFFECT_TEST_H
#define INTERFACE_SHADER_EFFECT_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

class ShaderEffectCreateLinearGradient : public TestBase {
public:
    explicit ShaderEffectCreateLinearGradient(int type) : TestBase(type)
    {
        fileName_ = "ShaderEffectCreateLinearGradient";
    }
    ~ShaderEffectCreateLinearGradient() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class ShaderEffectCreateRadialGradient : public TestBase {
public:
    explicit ShaderEffectCreateRadialGradient(int type) : TestBase(type)
    {
        fileName_ = "ShaderEffectCreateRadialGradient";
    }
    ~ShaderEffectCreateRadialGradient() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class ShaderEffectCreateImageShader : public TestBase {
public:
    explicit ShaderEffectCreateImageShader(int type) : TestBase(type)
    {
        fileName_ = "ShaderEffectCreateImageShader";
    }
    ~ShaderEffectCreateImageShader() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
class ShaderEffectCreateSweepGradient : public TestBase {
public:
    explicit ShaderEffectCreateSweepGradient(int type) : TestBase(type)
    {
        fileName_ = "ShaderEffectCreateSweepGradient";
    }
    ~ShaderEffectCreateSweepGradient() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif // INTERFACE_SHADER_EFFECT_TEST_H