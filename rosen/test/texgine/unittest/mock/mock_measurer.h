/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_MEASURER_H
#define ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_MEASURER_H

#include <gmock/gmock.h>

#include "measurer.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class MockMeasurer : public Measurer {
public:
    MockMeasurer() : Measurer({}, {{}}) {}
    MOCK_METHOD(int, Measure, (CharGroups &cgs));
    MOCK_METHOD(const std::vector<Boundary> &, GetWordBoundary, (), (const));
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_MEASURER_H
