/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#ifndef INTERFACE_BRUSH_TEST_H
#define INTERFACE_BRUSH_TEST_H
#include "test_base.h"

class BrushReset : public TestBase {
public:
    explicit BrushReset(int type) : TestBase(type)
    {
        fileName_ = "BrushReset";
    };
    ~BrushReset() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class BrushSetColor4f : public TestBase {
public:
    explicit BrushSetColor4f(int type) : TestBase(type)
    {
        fileName_ = "BrushSetColor4f";
    };
    ~BrushSetColor4f() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class BrushGetAlphaFloat : public TestBase {
public:
    explicit BrushGetAlphaFloat(int type) : TestBase(type)
    {
        fileName_ = "BrushGetAlphaFloat";
    };
    ~BrushGetAlphaFloat() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class BrushGetRedFloat : public TestBase {
public:
    explicit BrushGetRedFloat(int type) : TestBase(type)
    {
        fileName_ = "BrushGetRedFloat";
    };
    ~BrushGetRedFloat() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class BrushGetBlueFloat : public TestBase {
public:
    explicit BrushGetBlueFloat(int type) : TestBase(type)
    {
        fileName_ = "BrushGetBlueFloat";
    };
    ~BrushGetBlueFloat() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class BrushGetGreenFloat : public TestBase {
public:
    explicit BrushGetGreenFloat(int type) : TestBase(type)
    {
        fileName_ = "BrushGetGreenFloat";
    };
    ~BrushGetGreenFloat() override {};

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
#endif // INTERFACE_BRUSH_TEST_H