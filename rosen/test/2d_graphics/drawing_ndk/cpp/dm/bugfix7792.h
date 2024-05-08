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
#ifndef BUGFIX7792_H
#define BUGFIX7792_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

// 源代码用例名bug7792位置gm\pathfill.cpp.这里用BugFix7792类
class BugFix7792 : public TestBase {
public:
    BugFix7792();
    ~BugFix7792() override {};
    void CanvasDrawPath1(OH_Drawing_Canvas *canvas, OH_Drawing_Path *path);
    void CanvasDrawPath2(OH_Drawing_Canvas *canvas, OH_Drawing_Path *path);
    void CanvasDrawPath3(OH_Drawing_Canvas *canvas, OH_Drawing_Path *path);
    void CanvasDrawPath4(OH_Drawing_Canvas *canvas, OH_Drawing_Path *path);
    void CanvasDrawPath5(OH_Drawing_Canvas *canvas, OH_Drawing_Path *path);

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // BUGFIX7792_H