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

#ifndef LARGE_CLIPPED_PATH_H
#define LARGE_CLIPPED_PATH_H
#include <native_drawing/drawing_path.h>

#include "../test_base.h"

class LargeClippedPath : public TestBase {
    uint32_t defaultBitmapWidth = 1000;  // 1000是位图宽度
    uint32_t defaultBitmapHeight = 1000; // 1000是位图高度

public:
    explicit LargeClippedPath(OH_Drawing_PathFillType type) : fType(type)
    {
        bitmapWidth_ = defaultBitmapWidth;
        bitmapHeight_ = defaultBitmapHeight;
        fileName_ = "largeclippedpath";
    }

    ~LargeClippedPath() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas* canvas) override;
    OH_Drawing_PathFillType fType;
};
#endif // LARGE_CLIPPED_PATH_H