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

    static void TestDrawPath(Canvas& canvas, uint32_t width, uint32_t height);
    static void TestDrawPathPro(Canvas& canvas, uint32_t width, uint32_t height);
    std::vector<TestFunc> PathTestCase();

private:
    PathTest() = default;
    virtual ~PathTest() = default;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // PATH_TEST_H