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
#ifndef CONVEX_PATHS_H
#define CONVEX_PATHS_H
#include <bits/alltypes.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_rect.h>

#include "../test_base.h"
#include "test_common.h"

#include "common/log_common.h"

class ConvexPaths : public TestBase {
public:
    ConvexPaths()
    {
        bitmapWidth_ = 1200;  // 1200宽度
        bitmapHeight_ = 1100; // 1100高度
        fileName_ = "convexpaths";
    }
    ~ConvexPaths() = default;
    void MakePath();

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void MakePath1();
    void MakePath2();
    void MakePath3();
    void MakePath4();
    void MakePath5();
    void MakePath6();
};

#endif // CONVEX_PATHS_H
