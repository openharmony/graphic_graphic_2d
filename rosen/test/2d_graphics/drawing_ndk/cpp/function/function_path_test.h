/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DYSFUNCTION_PATH_TEST_H
#define DYSFUNCTION_PATH_TEST_H

#include <native_drawing/drawing_path.h>

#include "test_base.h"
#include "test_common.h"

// OH_Drawing_PathAddRectWithInitialCorner接口正常入參验证第三个参数为PATH_DIRECTION_CCW，第四个参数设置为1
// OH_Drawing_PathAddRectWithInitialCorner接口正常入參验证第三个参数为PATH_DIRECTION_CCW，第四个参数设置为2
// OH_Drawing_PathAddRectWithInitialCorner接口正常入參验证第三个参数为PATH_DIRECTION_CCW，第四个参数设置为3
// OH_Drawing_PathAddRectWithInitialCorner接口正常入參验证第三个参数为PATH_DIRECTION_CW
class FunctionPathAddRectWithInitialCornerTest : public TestBase {
public:
    FunctionPathAddRectWithInitialCornerTest()
    {
        fileName_ = "FunctionPathAddRectWithInitialCornerTest";
    }
    ~FunctionPathAddRectWithInitialCornerTest() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_PathRCubicTo接口绘制图形，一半画布内，一半画布外
// OH_Drawing_PathRCubicTo接口绘制图形，全部画布外
class FunctionPathRCubicToTest : public TestBase {
private:
    struct Point {
        float x;
        float y;
    };

public:
    FunctionPathRCubicToTest()
    {
        fileName_ = "FunctionPathRCubicToTest";
    }
    ~FunctionPathRCubicToTest() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_PathRConicTo接口正常入参验证第六个参数是小于等于0的数
// OH_Drawing_PathRConicTo接口正常入参验证第六个参数是1
// OH_Drawing_PathRConicTo接口绘制图形，一半画布内，一半画布外
// OH_Drawing_PathRConicTo接口绘制图形，全部画布外
class FunctionPathRConicToTest : public TestBase {
public:
    FunctionPathRConicToTest()
    {
        fileName_ = "FunctionPathRConicToTest";
    }
    ~FunctionPathRConicToTest() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_PathRQuadTo接口绘制图形，一半画布内，一半画布外
// OH_Drawing_PathRQuadTo接口绘制图形，全部画布外
class FunctionPathRQuadToTest : public TestBase {
public:
    FunctionPathRQuadToTest()
    {
        fileName_ = "FunctionPathRQuadToTest";
    }
    ~FunctionPathRQuadToTest() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_PathRLineTo接口绘制图形，一半画布内，一半画布外
// OH_Drawing_PathRLineTo接口绘制图形，全部画布外
class FunctionPathRLineToTest : public TestBase {
public:
    FunctionPathRLineToTest()
    {
        fileName_ = "FunctionPathRLineToTest";
    }
    ~FunctionPathRLineToTest() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_PathRMoveTo接口绘制图形，一半画布内，一半画布外
// OH_Drawing_PathRMoveTo接口绘制图形，全部画布外
class FunctionPathRMoveToTest : public TestBase {
public:
    FunctionPathRMoveToTest()
    {
        fileName_ = "FunctionPathRMoveToTest";
    }
    ~FunctionPathRMoveToTest() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_PathConicTo接口正常入参验证第六个参数是小于等于0的数
// OH_Drawing_PathConicTo接口正常入参验证第六个参数是1
// OH_Drawing_PathConicTo接口绘制图形，一半画布内，一半画布外
// OH_Drawing_PathConicTo接口绘制图形，全部画布外
class FunctionPathConicToTest : public TestBase {
public:
    FunctionPathConicToTest()
    {
        fileName_ = "FunctionPathConicToTest";
    }
    ~FunctionPathConicToTest() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_PathArcTo接口正常入参验证第七个参数为负数
// OH_Drawing_PathArcTo接口绘制图形，一半画布内，一半画布外
// OH_Drawing_PathArcTo接口绘制图形，全部画布外
class FunctionPathArcToTest : public TestBase {
public:
    FunctionPathArcToTest()
    {
        fileName_ = "FunctionPathArcToTest";
    }
    ~FunctionPathArcToTest() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
#endif