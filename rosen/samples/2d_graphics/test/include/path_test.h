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

#ifndef PATH_TEST_H
#define PATH_TEST_H

#include "draw/brush.h"
#include "draw/canvas.h"
#include "draw/color.h"
#include "draw/path.h"
#include "draw/pen.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PathTest {
public:
    using TestFunc = std::function<void(Canvas&, uint32_t, uint32_t)>;
    inline static PathTest& GetInstance()
    {
        static PathTest pathTestCase;
        return pathTestCase;
    }

    static void TestDrawStar(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestMoveTo(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestLineTo(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestArcTo(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestCubicTo(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestQuadTo(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestAddRect(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestAddOval(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestAddArc(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestAddPoly(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestAddCircle(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestAddRoundRect(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestAddPath(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestFillStyle(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestFillStyle2(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestFillStyle3(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestFillStyle4(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestOffset(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestTransform(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestOp(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestOp2(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestOp3(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestOp4(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestOp5(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestClose(Canvas& canvas, uint32_t width, uint32_t height);

    std::vector<TestFunc> PathTestCase();

private:
    PathTest() = default;
    virtual ~PathTest() = default;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // PATH_TEST_H