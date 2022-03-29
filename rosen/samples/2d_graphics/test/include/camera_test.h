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

#ifndef CAMERA_TEST_H
#define CAMERA_TEST_H

#include "draw/brush.h"
#include "draw/canvas.h"
#include "draw/color.h"
#include "draw/pen.h"
#include "utils/camera3d.h"
#include "utils/log.h"
#include "utils/matrix.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class CameraTest {
public:
    using TestFunc = std::function<void(Canvas&, uint32_t, uint32_t)>;
    inline static CameraTest& GetInstance()
    {
        static CameraTest cameraTestCase;
        return cameraTestCase;
    }

    static void TestCamera(Canvas& canvas, uint32_t width, uint32_t height);
    std::vector<TestFunc> CameraTestCase();

private:
    CameraTest() = default;
    virtual ~CameraTest() = default;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // CAMERA_TEST_H