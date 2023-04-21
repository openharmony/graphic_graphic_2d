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

#ifndef ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_ANY_SPAN_H
#define ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_ANY_SPAN_H

#include <gmock/gmock.h>

#include "texgine/any_span.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class MockAnySpan : public AnySpan {
public:
    MOCK_CONST_METHOD0(GetWidth, double());
    MOCK_CONST_METHOD0(GetHeight, double());
    MOCK_CONST_METHOD0(GetAlignment, AnySpanAlignment());
    MOCK_CONST_METHOD0(GetBaseline, TextBaseline());
    MOCK_CONST_METHOD0(GetLineOffset, double());
    MOCK_METHOD3(Paint, void(TexgineCanvas &canvas, double offsetx, double offsety));
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEST_TEXGINE_UNITTEST_MOCK_MOCK_ANY_SPAN_H
