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

#ifndef STABILITY_REGION_TEST_H
#define STABILITY_REGION_TEST_H
#include <map>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_region.h>

#include "test_base.h"
#include "test_common.h"

typedef enum {
    SETTING_FUNCTION_REGION_CREATE = 0,
    SETTING_FUNCTION_REGION_SET_RECT,
    SETTING_FUNCTION_REGION_SET_PATH,
    SETTING_FUNCTION_REGION_DESTROY,
} SettingFunctionRegion;

typedef enum {
    OPERATION_FUNCTION_REGION_CONTAINS = 0,
    OPERATION_FUNCTION_REGION_OP,
} OperationFunctionRegion;

// RegionCreate-RegionDestroy
class StabilityRegionCreate : public TestBase {
public:
    StabilityRegionCreate()
    {
        fileName_ = "StabilityRegionCreate";
    }
    ~StabilityRegionCreate() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// RegionCreate-相关配置接口全调用-RegionDestroy循环
class StabilityRegionAll : public TestBase {
public:
    StabilityRegionAll()
    {
        fileName_ = "StabilityRegionAll";
    }
    ~StabilityRegionAll() override {};

private:
    TestRend rand_;
    int index_ = 0;
    OH_Drawing_Region* region_ = nullptr;
    OH_Drawing_Region* other_ = nullptr;
    OH_Drawing_Rect* rect_ = nullptr;
    OH_Drawing_Path* path_ = nullptr;

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
    void RegionCreateTest(OH_Drawing_Region* region);
    void RegionSetRectTest(OH_Drawing_Region* region);
    void RegionSetPathTest(OH_Drawing_Region* region);
    void RegionDestroyTest(OH_Drawing_Region* region);
    using HandlerFunc = std::function<void(OH_Drawing_Region* region)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { SETTING_FUNCTION_REGION_CREATE, [this](OH_Drawing_Region* region) { return RegionCreateTest(region); } },
        { SETTING_FUNCTION_REGION_SET_RECT, [this](OH_Drawing_Region* region) { return RegionSetRectTest(region); } },
        { SETTING_FUNCTION_REGION_SET_PATH, [this](OH_Drawing_Region* region) { return RegionSetPathTest(region); } },
        { SETTING_FUNCTION_REGION_DESTROY, [this](OH_Drawing_Region* region) { return RegionDestroyTest(region); } },
    };
};

// Region相关操作接口随机循环调用
class StabilityRegionRandomFunc : public TestBase {
public:
    StabilityRegionRandomFunc()
    {
        fileName_ = "StabilityRegionRandomFunc";
    }
    ~StabilityRegionRandomFunc() override {};

private:
    TestRend rand_;
    int index_ = 0;
    OH_Drawing_Region* region_ = nullptr;
    OH_Drawing_Region* other_ = nullptr;

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
    void RegionContainsTest(OH_Drawing_Region* region);
    void RegionOpTest(OH_Drawing_Region* region);
    using HandlerFunc = std::function<void(OH_Drawing_Region* region)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { OPERATION_FUNCTION_REGION_CONTAINS,
            [this](OH_Drawing_Region* region) { return RegionContainsTest(region); } },
        { OPERATION_FUNCTION_REGION_OP, [this](OH_Drawing_Region* region) { return RegionOpTest(region); } },
    };
};

#endif // STABILITY_REGION_TEST_H