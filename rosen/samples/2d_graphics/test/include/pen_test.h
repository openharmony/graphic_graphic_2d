/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef PEN_TEST_H
#define PEN_TEST_H

#include "draw/canvas.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PenTest {
public:
    using TestFunc = std::function<void(Canvas&, uint32_t, uint32_t)>;
    inline static PenTest& GetInstance()
    {
        static PenTest penTestCase;
        return penTestCase;
    }

    static void TestPenColor(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestPenAlpha(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestPenWidth(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestPenMiterLimit(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestPenCapStyle(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestPenJoinStyle(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestPenBlendMode(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestPenAntiAlias(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestPenPathEffect(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestPenFilter(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestPenReset(Canvas& canvas, uint32_t width, uint32_t height);

    std::vector<TestFunc> PenTestCase();

private:
    PenTest() = default;
    virtual ~PenTest() = default;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // PEN_TEST_H