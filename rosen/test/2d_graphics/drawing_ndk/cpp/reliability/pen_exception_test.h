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

#ifndef RELIABILITY_PEN_EXCEPTION_TEST_H
#define RELIABILITY_PEN_EXCEPTION_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

// detach pen后，再调用draw接口
class ReliabilityDetachPen : public TestBase {
public:
    ReliabilityDetachPen()
    {
        fileName_ = "ReliabilityDetachPen";
    }
    ~ReliabilityDetachPen() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// attach pen后调用destroy接口，再调用draw接口
class ReliabilityDestroyPen : public TestBase {
public:
    ReliabilityDestroyPen()
    {
        fileName_ = "ReliabilityDestroyPen";
    }
    ~ReliabilityDestroyPen() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// pen setShaderEffect后销毁ShaderEffect，再调用draw接口
class ReliabilitySetShaderEffect : public TestBase {
public:
    ReliabilitySetShaderEffect()
    {
        fileName_ = "ReliabilitySetShaderEffect";
    }
    ~ReliabilitySetShaderEffect() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// brush setShadowLayer后销毁ShadowLayer，再调用draw接口
class ReliabilitySetShadowLayer : public TestBase {
public:
    ReliabilitySetShadowLayer()
    {
        fileName_ = "ReliabilitySetShadowLayer";
    }
    ~ReliabilitySetShadowLayer() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// pen SetPathEffect后销毁PathEffect，再调用draw接口
class ReliabilitySetPathEffect : public TestBase {
public:
    ReliabilitySetPathEffect()
    {
        fileName_ = "ReliabilitySetPathEffect";
    }
    ~ReliabilitySetPathEffect() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// pen setFilter后销毁Filter，再调用draw接口
class ReliabilitySetFilter : public TestBase {
public:
    ReliabilitySetFilter()
    {
        fileName_ = "ReliabilitySetFilter";
    }
    ~ReliabilitySetFilter() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// pen setFilter后销毁ImageFilter，再调用draw接口
class ReliabilityImageFilterDestroy : public TestBase {
public:
    ReliabilityImageFilterDestroy()
    {
        fileName_ = "ReliabilityImageFilterDestroy";
    }
    ~ReliabilityImageFilterDestroy() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// pen setFilter后销毁MaskFilter，再调用draw接口
class ReliabilityMaskFilterDestroy : public TestBase {
public:
    ReliabilityMaskFilterDestroy()
    {
        fileName_ = "ReliabilityMaskFilterDestroy";
    }
    ~ReliabilityMaskFilterDestroy() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// pen setFilter后销毁ColorFilter，再调用draw接口
class ReliabilityColorFilterDestroy : public TestBase {
public:
    ReliabilityColorFilterDestroy()
    {
        fileName_ = "ReliabilityColorFilterDestroy";
    }
    ~ReliabilityColorFilterDestroy() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// draw过程中，detach pen
class ReliabilityThreadDetachPen : public TestBase {
public:
    ReliabilityThreadDetachPen()
    {
        fileName_ = "ReliabilityThreadDetachPen";
    }
    ~ReliabilityThreadDetachPen() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// draw过程中，pen调用destroy接口
class ReliabilityThreadDestroyPen : public TestBase {
public:
    ReliabilityThreadDestroyPen()
    {
        fileName_ = "ReliabilityThreadDestroyPen";
    }
    ~ReliabilityThreadDestroyPen() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// draw过程中，pen 销毁ShaderEffect
class ReliabilityThreadDestroyShaderEffect : public TestBase {
public:
    ReliabilityThreadDestroyShaderEffect()
    {
        fileName_ = "ReliabilityThreadDestroyShaderEffect";
    }
    ~ReliabilityThreadDestroyShaderEffect() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// draw过程中，pen 销毁ShadowLayer
class ReliabilityThreadDestroyShadowLayer : public TestBase {
public:
    ReliabilityThreadDestroyShadowLayer()
    {
        fileName_ = "ReliabilityThreadDestroyShadowLayer";
    }
    ~ReliabilityThreadDestroyShadowLayer() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// draw过程中，pen 销毁PathEffect
class ReliabilityThreadDestroyPathEffect : public TestBase {
public:
    ReliabilityThreadDestroyPathEffect()
    {
        fileName_ = "ReliabilityThreadDestroyPathEffect";
    }
    ~ReliabilityThreadDestroyPathEffect() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// draw过程中，pen 销毁Filter
class ReliabilityThreadDestroyFilter : public TestBase {
public:
    ReliabilityThreadDestroyFilter()
    {
        fileName_ = "ReliabilityThreadDestroyFilter";
    }
    ~ReliabilityThreadDestroyFilter() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// draw过程中，pen 销毁ImageFilter
class ReliabilityThreadDestroyImageFilter : public TestBase {
public:
    ReliabilityThreadDestroyImageFilter()
    {
        fileName_ = "ReliabilityThreadDestroyImageFilter";
    }
    ~ReliabilityThreadDestroyImageFilter() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// draw过程中，pen 销毁MaskFilter
class ReliabilityThreadDestroyMaskFilter : public TestBase {
public:
    ReliabilityThreadDestroyMaskFilter()
    {
        fileName_ = "ReliabilityThreadDestroyMaskFilter";
    }
    ~ReliabilityThreadDestroyMaskFilter() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// draw过程中，pen 销毁ColorFilter
class ReliabilityThreadDestroyColorFilter : public TestBase {
public:
    ReliabilityThreadDestroyColorFilter()
    {
        fileName_ = "ReliabilityThreadDestroyColorFilter";
    }
    ~ReliabilityThreadDestroyColorFilter() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
#endif // RELIABILITY_PEN_EXCEPTION_TEST_H
