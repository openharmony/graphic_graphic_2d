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

#ifndef FUNCTION_RECT_TEST_H
#define FUNCTION_RECT_TEST_H
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>

#include "test_base.h"
#include "test_common.h"
// OH_Drawing_RectCopy一半画布内，一半画布外)
class FunctionRectCopy : public TestBase {
public:
    FunctionRectCopy()
    {
        fileName_ = "FunctionRectCopy";
    }
    ~FunctionRectCopy() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
/*
 * OH_Drawing_RectGetWidth接口绘制图形，一半画布内，一半画布外
 * OH_Drawing_RectGetWidth接口绘制图形全部画布外
 * OH_Drawing_RectGetHeight接口绘制图形，一半画布内，一半画布外
 * OH_Drawing_RectGetHeight接口绘制图形全部画布外
 * OH_Drawing_RectGetBottom接口绘制图形，一半画布内，一半画布外
 * OH_Drawing_RectGetBottom接口绘制图形全部画布外
 * OH_Drawing_RectGetRight接口绘制图形，一半画布内，一半画布外
 * OH_Drawing_RectGetRight接口绘制图形全部画布外
 * OH_Drawing_RectGetTop接口绘制图形，一半画布内，一半画布外
 * OH_Drawing_RectGetTop接口绘制图形全部画布外
 * OH_Drawing_RectGetLeft接口绘制图形，一半画布内，一半画布外
 * OH_Drawing_RectGetLeft接口绘制图形全部画布外
 * OH_Drawing_RectSetBottom接口绘制图形，一半画布内，一半画布外
 * OH_Drawing_RectSetBottom接口绘制图形全部画布外
 * OH_Drawing_RectSetRight接口绘制图形，一半画布内，一半画布外
 * OH_Drawing_RectSetRight接口绘制图形全部画布外
 * OH_Drawing_RectSetTop接口绘制图形，一半画布内，一半画布外
 * OH_Drawing_RectSetTop接口绘制图形全部画布外
 * OH_Drawing_RectSetLeft接口绘制图形，一半画布内，一半画布外
 * OH_Drawing_RectSetLeft接口绘制图形全部画布外
 */
class FunctionRectSetAndGet : public TestBase {
public:
    FunctionRectSetAndGet()
    {
        fileName_ = "FunctionRectSetAndGet";
    }
    ~FunctionRectSetAndGet() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
/*
 * OH_Drawing_RectIntersect接口正常入参验证（大矩形包含小矩形，参数1，2互换验证)
 * OH_Drawing_RectIntersect接口正常入参验证（左右相交判断，上下相交判断）
 * OH_Drawing_RectIntersect接口正常入参验证（rect的left >right 的情况判断，top >bottom的情况判断）
 * OH_Drawing_RectIntersect接口正常入参验证（rect1.left = rect2.right情况判断,rect1.top = rect2.bottom情况判断）
 */
class FunctionRectIntersect : public TestBase {
public:
    FunctionRectIntersect()
    {
        fileName_ = "FunctionRectIntersect";
    }
    ~FunctionRectIntersect() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
// OH_Drawing_RegionSetRect接口rect设置一半画布内一半画布外
class FunctionRegionSetRect : public TestBase {
public:
    FunctionRegionSetRect()
    {
        fileName_ = "FunctionRegionSetRect";
    }
    ~FunctionRegionSetRect() override {};

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
};
#endif // FUNCTION_RECT_TEST_H
