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

#ifndef INTERFACE_COLOR_TEST_H
#define INTERFACE_COLOR_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

class ColorFilterCreateBlendMode : public TestBase {
public:
    explicit ColorFilterCreateBlendMode(int type) : TestBase(type)
    {
        fileName_ = "ColorFilterCreateBlendMode";
    };
    ~ColorFilterCreateBlendMode() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class ColorFilterCreateCompose : public TestBase {
public:
    explicit ColorFilterCreateCompose(int type) : TestBase(type)
    {
        fileName_ = "ColorFilterCreateCompose";
    };
    ~ColorFilterCreateCompose() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class ColorFilterCreateMatrix : public TestBase {
public:
    explicit ColorFilterCreateMatrix(int type) : TestBase(type)
    {
        fileName_ = "ColorFilterCreateMatrix";
    };
    ~ColorFilterCreateMatrix() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class ColorFilterCreateLinearToSrgbGamma : public TestBase {
public:
    explicit ColorFilterCreateLinearToSrgbGamma(int type) : TestBase(type)
    {
        fileName_ = "ColorFilterCreateLinearToSrgbGamma";
    };
    ~ColorFilterCreateLinearToSrgbGamma() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class ColorFilterCreateSrgbGammaToLinear : public TestBase {
public:
    explicit ColorFilterCreateSrgbGammaToLinear(int type) : TestBase(type)
    {
        fileName_ = "ColorFilterCreateSrgbGammaToLinear";
    };
    ~ColorFilterCreateSrgbGammaToLinear() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class ColorFilterCreateLuma : public TestBase {
public:
    explicit ColorFilterCreateLuma(int type) : TestBase(type)
    {
        fileName_ = "ColorFilterCreateLuma";
    };
    ~ColorFilterCreateLuma() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class ColorSpaceCreateSrgb : public TestBase {
public:
    explicit ColorSpaceCreateSrgb(int type) : TestBase(type)
    {
        fileName_ = "ColorSpaceCreateSrgb";
    };
    ~ColorSpaceCreateSrgb() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class ColorSpaceCreateSrgbLinear : public TestBase {
public:
    explicit ColorSpaceCreateSrgbLinear(int type) : TestBase(type)
    {
        fileName_ = "ColorSpaceCreateSrgbLinear";
    };
    ~ColorSpaceCreateSrgbLinear() override {};

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif // INTERFACE_COLOR_TEST_H