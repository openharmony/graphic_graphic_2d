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

#ifndef STABILITY_COLOR_TEST_H
#define STABILITY_COLOR_TEST_H
#include "test_base.h"

// ColorFilterCreateBlendMode-ColorFilterDestroy循环
class StabilityColorFilterCreateBlendMode : public TestBase {
public:
    StabilityColorFilterCreateBlendMode()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityColorFilterCreateBlendMode";
    }
    ~StabilityColorFilterCreateBlendMode() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ColorFilterCreateCompose-ColorFilterDestroy循环
class StabilityColorFilterCreateCompose : public TestBase {
public:
    StabilityColorFilterCreateCompose()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityColorFilterCreateCompose";
    }
    ~StabilityColorFilterCreateCompose() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ColorFilterCreateMatrix-ColorFilterDestroy循环
class StabilityColorFilterCreateMatrix : public TestBase {
public:
    StabilityColorFilterCreateMatrix()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityColorFilterCreateMatrix";
    }
    ~StabilityColorFilterCreateMatrix() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ColorFilterCreateLinearToSrgbGamma-ColorFilterDestroy循环
class StabilityColorFilterCreateLinearToSrgbGamma : public TestBase {
public:
    StabilityColorFilterCreateLinearToSrgbGamma()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityColorFilterCreateLinearToSrgbGamma";
    }
    ~StabilityColorFilterCreateLinearToSrgbGamma() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ColorFilterCreateSrgbGammaToLinear-ColorFilterDestroy循环
class StabilityColorFilterCreateSrgbGammaToLinear : public TestBase {
public:
    StabilityColorFilterCreateSrgbGammaToLinear()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityColorFilterCreateSrgbGammaToLinear";
    }
    ~StabilityColorFilterCreateSrgbGammaToLinear() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// todo:用例中暂时不包含，后续使用
class StabilityColorFilterCreateLuma : public TestBase {
public:
    StabilityColorFilterCreateLuma()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityColorFilterCreateLuma";
    }
    ~StabilityColorFilterCreateLuma() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ColorSpaceCreateSrgb-ColorSpaceDestroy循环
class StabilityColorSpaceCreateSrgb : public TestBase {
public:
    StabilityColorSpaceCreateSrgb()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityColorSpaceCreateSrgb";
    }
    ~StabilityColorSpaceCreateSrgb() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// ColorSpaceCreateSrgbLinear-ColorSpaceDestroy循环
class StabilityColorSpaceCreateSrgbLinear : public TestBase {
public:
    StabilityColorSpaceCreateSrgbLinear()
    {
        bitmapWidth_ = 640;  // 640宽度
        bitmapHeight_ = 480; // 480高度
        fileName_ = "StabilityColorSpaceCreateSrgbLinear";
    }
    ~StabilityColorSpaceCreateSrgbLinear() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};
#endif // STABILITY_COLOR_TEST_H