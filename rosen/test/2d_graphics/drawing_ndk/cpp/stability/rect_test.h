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

#ifndef STABILITY_RECT_TEST_H
#define STABILITY_RECT_TEST_H

#include <map>

#include "stability/path_test.h"
#include "test_base.h"
#include "test_common.h"

typedef enum {
    SETTING_FUNCTION_RECT_CREATE = 0,
    SETTING_FUNCTION_RECT_COPY,
    SETTING_FUNCTION_RECT_SET_LEFT,
    SETTING_FUNCTION_RECT_SET_TOP,
    SETTING_FUNCTION_RECT_SET_RIGHT,
    SETTING_FUNCTION_RECT_SET_BOTTOM,
    SETTING_FUNCTION_RECT_SET_DESTROY,
} SettingFunctionRect;

typedef enum {
    OPERATION_FUNCTION_RECT_INTERSECT = 0,
    OPERATION_FUNCTION_RECT_JOIN,
    OPERATION_FUNCTION_RECT_GET_LEFT,
    OPERATION_FUNCTION_RECT_GET_TOP,
    OPERATION_FUNCTION_RECT_GET_RIGHT,
    OPERATION_FUNCTION_RECT_GET_BOTTOM,
    OPERATION_FUNCTION_RECT_GET_HEIGHT,
    OPERATION_FUNCTION_RECT_GET_WIDTH,
} OperationFunctionRect;

// RectCreate-RectDestroy循环
class StabilityRectCreate : public TestBase {
public:
    StabilityRectCreate()
    {
        fileName_ = "StabilityRectCreate";
    }
    ~StabilityRectCreate() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// RectCopy-RectDestroy循环
class StabilityRectCopy : public TestBase {
public:
    StabilityRectCopy()
    {
        fileName_ = "StabilityRectCopy";
    }
    ~StabilityRectCopy() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// RectCreate-相关配置接口全调用-RectDestroy循环
class StabilityRectAll : public TestBase {
public:
    StabilityRectAll()
    {
        fileName_ = "StabilityRectAll";
    }
    ~StabilityRectAll() override {};

private:
    TestRend rand_;
    int index_ = 0;
    OH_Drawing_Rect* rect_ = nullptr;
    OH_Drawing_Rect* other_ = nullptr;

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
    void RectCreateTest(OH_Drawing_Rect* rect);
    void RectSetLeftTest(OH_Drawing_Rect* rect);
    void RectSetTopTest(OH_Drawing_Rect* rect);
    void RectSetRightTest(OH_Drawing_Rect* rect);
    void RectSetBottomTest(OH_Drawing_Rect* rect);
    void RectCopyTest(OH_Drawing_Rect* rect);
    void RectDestroyTest(OH_Drawing_Rect* rect);
    using HandlerFunc = std::function<void(OH_Drawing_Rect* rect)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { SETTING_FUNCTION_RECT_CREATE, [this](OH_Drawing_Rect* rect) { return RectCreateTest(rect); } },
        { SETTING_FUNCTION_RECT_SET_LEFT, [this](OH_Drawing_Rect* rect) { return RectSetLeftTest(rect); } },
        { SETTING_FUNCTION_RECT_SET_TOP, [this](OH_Drawing_Rect* rect) { return RectSetTopTest(rect); } },
        { SETTING_FUNCTION_RECT_SET_RIGHT, [this](OH_Drawing_Rect* rect) { return RectSetRightTest(rect); } },
        { SETTING_FUNCTION_RECT_SET_BOTTOM, [this](OH_Drawing_Rect* rect) { return RectSetBottomTest(rect); } },
        { SETTING_FUNCTION_RECT_COPY, [this](OH_Drawing_Rect* rect) { return RectCopyTest(rect); } },
        { SETTING_FUNCTION_RECT_SET_DESTROY, [this](OH_Drawing_Rect* rect) { return RectDestroyTest(rect); } },
    };
};

// Rec相关操作接口随机循环调用
class StabilityRectRandom : public TestBase {
public:
    StabilityRectRandom()
    {
        fileName_ = "StabilityRectRandom";
    }
    ~StabilityRectRandom() override {};

private:
    OH_Drawing_Rect* rect_ = nullptr;
    OH_Drawing_Rect* other_ = nullptr;

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
    void RectIntersectTest(OH_Drawing_Rect* rect);
    void RectJoinTest(OH_Drawing_Rect* rect);
    void RectGetLeftTest(OH_Drawing_Rect* rect);
    void RectGetTopTest(OH_Drawing_Rect* rect);
    void RectGetRightTest(OH_Drawing_Rect* rect);
    void RectGetBottomTest(OH_Drawing_Rect* rect);
    void RectGetHeightTest(OH_Drawing_Rect* rect);
    void RectGetWidthTest(OH_Drawing_Rect* rect);
    using HandlerFunc = std::function<void(OH_Drawing_Rect* rect)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { OPERATION_FUNCTION_RECT_INTERSECT, [this](OH_Drawing_Rect* rect) { return RectIntersectTest(rect); } },
        { OPERATION_FUNCTION_RECT_JOIN, [this](OH_Drawing_Rect* rect) { return RectJoinTest(rect); } },
        { OPERATION_FUNCTION_RECT_GET_LEFT, [this](OH_Drawing_Rect* rect) { return RectGetLeftTest(rect); } },
        { OPERATION_FUNCTION_RECT_GET_TOP, [this](OH_Drawing_Rect* rect) { return RectGetTopTest(rect); } },
        { OPERATION_FUNCTION_RECT_GET_RIGHT, [this](OH_Drawing_Rect* rect) { return RectGetRightTest(rect); } },
        { OPERATION_FUNCTION_RECT_GET_BOTTOM, [this](OH_Drawing_Rect* rect) { return RectGetBottomTest(rect); } },
        { OPERATION_FUNCTION_RECT_GET_HEIGHT, [this](OH_Drawing_Rect* rect) { return RectGetHeightTest(rect); } },
        { OPERATION_FUNCTION_RECT_GET_WIDTH, [this](OH_Drawing_Rect* rect) { return RectGetWidthTest(rect); } },
    };
};

#endif // STABILITY_RECT_TEST_H