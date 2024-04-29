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

#ifndef SHADOW_UTILS_H
#define SHADOW_UTILS_H
#include <bits/alltypes.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_rect.h>

#include "../test_base.h"

class ShadowUtils : public TestBase {
public:
    enum ShadowMode { K_NO_OCCLUDERS, K_OCCLUDERS, K_GRAY_SCALE };
    explicit ShadowUtils(ShadowMode m);
    ~ShadowUtils() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    void draw_paths(OH_Drawing_Canvas* canvas, ShadowMode mode);
    ShadowMode sMode;
};
#endif // AA_RECT_MODES_H