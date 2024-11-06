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

#ifndef FUNCTION_PATH_TEST_H
#define FUNCTION_PATH_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"

// OH_Drawing_PathReset接口绘制图形，一半画布内，一半画布外
// OH_Drawing_PathReset接口绘制图形，全部画布外
class FunctionPathReset : public TestBase {
public:
    FunctionPathReset()
    {
        fileName_ = "FunctionPathReset";
    }
    ~FunctionPathReset() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_PathAddOval接口正常入参验证(枚举值逆时针方向)
// OH_Drawing_PathAddOval接口绘制图形，一半画布内，一半画布外
// OH_Drawing_PathAddOval接口绘制图形，全部画布外
class FunctionPathAddOval : public TestBase {
public:
    FunctionPathAddOval()
    {
        fileName_ = "FunctionPathAddOval";
    }
    ~FunctionPathAddOval() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_PathTransformWithPerspectiveClip接口正常入参验证srcpath不为空，第四个参数写true
// OH_Drawing_PathTransformWithPerspectiveClip接口正常入参验证srcpath不为空，第四个参数写false
// OH_Drawing_PathTransformWithPerspectiveClip接口正常入参验证srcpath不为空，第四个参数写false接口绘制图形，一半画布内，一半画布外
// OH_Drawing_PathTransformWithPerspectiveClip接口正常入参验证srcpath不为空，第四个参数写false接口绘制图形，全部画布外
class FunctionPathTransformWithPerspectiveClip : public TestBase {
public:
    FunctionPathTransformWithPerspectiveClip()
    {
        fileName_ = "FunctionPathTransformWithPerspectiveClip";
    }
    ~FunctionPathTransformWithPerspectiveClip() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_PathAddPathWithOffsetAndMode接口绘制图形，一半画布内，一半画布外
// OH_Drawing_PathAddPathWithOffsetAndMode接口绘制图形，全部画布外
class FunctionPathAddPathWithOffsetAndMode : public TestBase {
public:
    FunctionPathAddPathWithOffsetAndMode()
    {
        fileName_ = "FunctionPathAddPathWithOffsetAndMode";
    }
    ~FunctionPathAddPathWithOffsetAndMode() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_PathAddPathWithMode接口正常入参验证PATH_ADD_MODE_APPEND
// OH_Drawing_PathAddPathWithMode接口绘制图形，一半画布内，一半画布外
// OH_Drawing_PathAddPathWithMode接口绘制图形，全部画布外
class FunctionPathAddPathWithMode : public TestBase {
public:
    FunctionPathAddPathWithMode()
    {
        fileName_ = "FunctionPathAddPathWithMode";
    }
    ~FunctionPathAddPathWithMode() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_PathAddPathWithMatrixAndMode接口正常入参验证第三个参数为空，PATH_ADD_MODE_APPEND
// OH_Drawing_PathAddPathWithMatrixAndMode接口绘制图形，一半画布内，一半画布外
// OH_Drawing_PathAddPathWithMatrixAndMode接口绘制图形，全部画布外
class FunctionPathAddPathWithMatrixAndMode : public TestBase {
public:
    FunctionPathAddPathWithMatrixAndMode()
    {
        fileName_ = "FunctionPathAddPathWithMatrixAndMode";
    }
    ~FunctionPathAddPathWithMatrixAndMode() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};

// OH_Drawing_PathAddOvalWithInitialPoint接口正常入参验证第三个参数为负数，PATH_DIRECTION_CW
// OH_Drawing_PathAddOvalWithInitialPoint接口正常入参验证第三个参数为小数，PATH_DIRECTION_CW
// OH_Drawing_PathAddOvalWithInitialPoint接口正常入参验证，PATH_DIRECTION_CCW
// OH_Drawing_PathAddOvalWithInitialPoint接口绘制图形，一半画布内，一半画布外
// OH_Drawing_PathAddOvalWithInitialPoint接口绘制图形，全部画布外
class FunctionPathAddOvalWithInitialPoint : public TestBase {
public:
    FunctionPathAddOvalWithInitialPoint()
    {
        fileName_ = "FunctionPathAddOvalWithInitialPoint";
    }
    ~FunctionPathAddOvalWithInitialPoint() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
#endif // FUNCTION_PATH_TEST_H
