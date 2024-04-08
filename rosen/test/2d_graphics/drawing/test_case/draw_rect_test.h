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
#ifndef DRAWING_DEMO_DRAW_RECT_TEST_H
#define DRAWING_DEMO_DRAW_RECT_TEST_H
#include "test_base.h"

namespace OHOS {
namespace Rosen {
class DrawRectTest : public TestBase {
public:
    DrawRectTest() : TestBase() {}
    ~DrawRectTest() override = default;
protected:
    void OnTestFunction(Drawing::Canvas* canvas) override;
    void OnTestPerformance(Drawing::Canvas* canvas) override;
};
} // namespace Rosen
} // namespace OHOS
#endif // DRAWING_DEMO_DRAW_RECT_TEST_H