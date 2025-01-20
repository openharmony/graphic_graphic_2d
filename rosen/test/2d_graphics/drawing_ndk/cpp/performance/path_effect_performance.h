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

#ifndef SIMPLE_PERFORMANCE_PATH_EFFECT_TEST_H
#define SIMPLE_PERFORMANCE_PATH_EFFECT_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"


class PerformancePathEffectCreateDiscretePathEffect : public TestBase {
public:
    explicit PerformancePathEffectCreateDiscretePathEffect(int type) : TestBase(type)
    {
        fileName_ = "PerformancePathEffectCreateDiscretePathEffect";
    }
    ~PerformancePathEffectCreateDiscretePathEffect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformancePathEffectCreateCornerPathEffect : public TestBase {
public:
    explicit PerformancePathEffectCreateCornerPathEffect(int type) : TestBase(type)
    {
        fileName_ = "PerformancePathEffectCreateCornerPathEffect";
    }
    ~PerformancePathEffectCreateCornerPathEffect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformancePathEffectCreateComposePathEffect : public TestBase {
public:
    explicit PerformancePathEffectCreateComposePathEffect(int type) : TestBase(type)
    {
        fileName_ = "PerformancePathEffectCreateComposePathEffect";
    }
    ~PerformancePathEffectCreateComposePathEffect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformancePathEffectCreatePathDashEffect : public TestBase {
public:
    explicit PerformancePathEffectCreatePathDashEffect(int type) : TestBase(type)
    {
        fileName_ = "PerformancePathEffectCreatePathDashEffect";
    }
    ~PerformancePathEffectCreatePathDashEffect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class PerformancePathEffectCreateSumPathEffect : public TestBase {
public:
    explicit PerformancePathEffectCreateSumPathEffect(int type) : TestBase(type)
    {
        fileName_ = "PerformancePathEffectCreateSumPathEffect";
    }
    ~PerformancePathEffectCreateSumPathEffect() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

#endif // SIMPLE_PERFORMANCE_PATH_EFFECT_TEST_H