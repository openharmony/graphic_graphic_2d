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

#ifndef STABILITY_PATH_EFFECT_TEST_H
#define STABILITY_PATH_EFFECT_TEST_H

#include "test_base.h"
#include "test_common.h"

// CreateDashPathEffect-PathEffectDestroy循环调用
class StabilityCreateDashPathEffect : public TestBase {
public:
    StabilityCreateDashPathEffect()
    {
        fileName_ = "StabilityCreateDashPathEffect";
    }
    ~StabilityCreateDashPathEffect() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

#endif // STABILITY_PATH_EFFECT_TEST_H