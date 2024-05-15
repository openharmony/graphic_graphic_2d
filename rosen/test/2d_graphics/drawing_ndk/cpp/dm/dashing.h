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
#ifndef DASHING_H
#define DASHING_H
#include "../test_base.h"

struct Dashings {
    float finalX = (600);
    float finalY = (0);
    float phase = (0);
    float startX = (0);
    float startY = (0);
};

class Dashing : public TestBase {
public:
    Dashing();
    ~Dashing() override;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};

class Dashing2 : public TestBase {
public:
    Dashing2();
    ~Dashing2() override;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};

class Dashing4 : public TestBase {
public:
    Dashing4();
    ~Dashing4() override;

protected:
    struct Intervals {
        int fOnInterval;
        int fOffInterval;
    };
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void DashingNum(OH_Drawing_Canvas *canvas, OH_Drawing_Pen *pen);
    void DashingTow(OH_Drawing_Canvas *canvas, OH_Drawing_Pen *pen);
    void DashingThree(OH_Drawing_Canvas *canvas, OH_Drawing_Pen *pen);
};
#endif // DASHING_H