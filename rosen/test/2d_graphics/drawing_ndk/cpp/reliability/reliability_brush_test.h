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

#ifndef RELIABILITY_BRUSH_TEST_H
#define RELIABILITY_BRUSH_TEST_H
#include "test_base.h"
#include "test_common.h"
// detach brush后，再调用draw接口
class ReliabilityDetachBrush : public TestBase {
public:
    ReliabilityDetachBrush()
    {
        fileName_ = "ReliabilityDetachBrush";
    }
    ~ReliabilityDetachBrush() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// attach brush后调用destroy接口，再调用draw接口
class ReliabilityAttachBrush : public TestBase {
public:
    ReliabilityAttachBrush()
    {
        fileName_ = "ReliabilityAttachBrush";
    }
    ~ReliabilityAttachBrush() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// brush setShaderEffect后销毁ShaderEffect，再调用draw接口
class ReliabilityBrushSetShaderEffect : public TestBase {
public:
    ReliabilityBrushSetShaderEffect()
    {
        fileName_ = "ReliabilityBrushSetShaderEffect";
    }
    ~ReliabilityBrushSetShaderEffect() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// brush setShadowLayer后销毁ShadowLayer，再调用draw接口
class ReliabilityBrushSetShadowLayer : public TestBase {
public:
    ReliabilityBrushSetShadowLayer()
    {
        fileName_ = "ReliabilityBrushSetShadowLayer";
    }
    ~ReliabilityBrushSetShadowLayer() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// brush setFilter后销毁Filter，再调用draw接口
class ReliabilityBrushSetFilter : public TestBase {
public:
    ReliabilityBrushSetFilter()
    {
        fileName_ = "ReliabilityBrushSetFilter";
    }
    ~ReliabilityBrushSetFilter() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// brush setFilter后销毁ImageFilter，再调用draw接口
class ReliabilityBrushSetImageFilter : public TestBase {
public:
    ReliabilityBrushSetImageFilter()
    {
        fileName_ = "ReliabilityBrushSetImageFilter";
    }
    ~ReliabilityBrushSetImageFilter() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// brush setFilter后销毁MaskFilter，再调用draw接口
class ReliabilityBrushSetMaskFilter : public TestBase {
public:
    ReliabilityBrushSetMaskFilter()
    {
        fileName_ = "ReliabilityBrushSetMaskFilter";
    }
    ~ReliabilityBrushSetMaskFilter() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// brush setFilter后销毁ColorFilter，再调用draw接口
class ReliabilityBrushSetColorFilter : public TestBase {
public:
    ReliabilityBrushSetColorFilter()
    {
        fileName_ = "ReliabilityBrushSetColorFilter";
    }
    ~ReliabilityBrushSetColorFilter() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// draw过程中，detach brush
class ReliabilityDetachBrushJoinThread : public TestBase {
public:
    ReliabilityDetachBrushJoinThread()
    {
        fileName_ = "ReliabilityDetachBrushJoinThread";
    }
    ~ReliabilityDetachBrushJoinThread() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// draw过程中，brush调用destroy接口
class ReliabilityAttachBrushJoinThread : public TestBase {
public:
    ReliabilityAttachBrushJoinThread()
    {
        fileName_ = "ReliabilityAttachBrushJoinThread";
    }
    ~ReliabilityAttachBrushJoinThread() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// draw过程中，brush 销毁ShaderEffect
class ReliabilityBrushSetShaderEffectJoinThread : public TestBase {
public:
    ReliabilityBrushSetShaderEffectJoinThread()
    {
        fileName_ = "ReliabilityBrushSetShaderEffectJoinThread";
    }
    ~ReliabilityBrushSetShaderEffectJoinThread() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// draw过程中，brush 销毁ShadowLayer
class ReliabilityBrushSetShadowLayerJoinThread : public TestBase {
public:
    ReliabilityBrushSetShadowLayerJoinThread()
    {
        fileName_ = "ReliabilityBrushSetShadowLayerJoinThread";
    }
    ~ReliabilityBrushSetShadowLayerJoinThread() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// draw过程中，brush 销毁Filter
class ReliabilityBrushSetFilterJoinThread : public TestBase {
public:
    ReliabilityBrushSetFilterJoinThread()
    {
        fileName_ = "ReliabilityBrushSetFilterJoinThread";
    }
    ~ReliabilityBrushSetFilterJoinThread() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// draw过程中，brush 销毁ImageFilter
class ReliabilityBrushSetImageFilterJoinThread : public TestBase {
public:
    ReliabilityBrushSetImageFilterJoinThread()
    {
        fileName_ = "ReliabilityBrushSetImageFilterJoinThread";
    }
    ~ReliabilityBrushSetImageFilterJoinThread() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// draw过程中，brush 销毁MaskFilter
class ReliabilityBrushSetMaskFilterJoinThread : public TestBase {
public:
    ReliabilityBrushSetMaskFilterJoinThread()
    {
        fileName_ = "ReliabilityBrushSetMaskFilterJoinThread";
    }
    ~ReliabilityBrushSetMaskFilterJoinThread() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// draw过程中，brush 销毁ColorFilter
class ReliabilityBrushSetColorFilterJoinThread : public TestBase {
public:
    ReliabilityBrushSetColorFilterJoinThread()
    {
        fileName_ = "ReliabilityBrushSetColorFilterJoinThread";
    }
    ~ReliabilityBrushSetColorFilterJoinThread() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
#endif // RELIABILITY_BRUSH_TEST_H