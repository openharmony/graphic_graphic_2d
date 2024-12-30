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

#ifndef STABILITY_PATH_TEST_H
#define STABILITY_PATH_TEST_H
#include <map>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_path.h>

#include "test_base.h"
#include "test_common.h"

typedef enum {
    // setting function
    SETTING_FUNCTION_PATH_CREATE = 0,
    SETTING_FUNCTION_PATH_COPY,
    SETTING_FUNCTION_PATH_BUILD_FROM_SVG_STRING,
    SETTING_FUNCTION_PATH_SET_FILL_TYPE,
    SETTING_FUNCTION_PATH_RESET,
    SETTING_FUNCTION_PATH_DESTROY,
} SettingFunctionPath;

typedef enum {
    // operate function
    OPERATION_FUNCTION_PATH_OP = 0,
    OPERATION_FUNCTION_PATH_MOVE_TO,
    OPERATION_FUNCTION_PATH_LINE_TO,
    OPERATION_FUNCTION_PATH_ARC_TO,
    OPERATION_FUNCTION_PATH_QUAD_TO,
    OPERATION_FUNCTION_PATH_CONIC_TO,
    OPERATION_FUNCTION_PATH_CUBIC_TO,
    OPERATION_FUNCTION_PATH_R_MOVE_TO,
    OPERATION_FUNCTION_PATH_R_LINE_TO,
    OPERATION_FUNCTION_PATH_R_QUAD_TO,
    OPERATION_FUNCTION_PATH_R_CONIC_TO,
    OPERATION_FUNCTION_PATH_R_CUBIC_TO,
    OPERATION_FUNCTION_PATH_ADD_RECT,
    OPERATION_FUNCTION_PATH_ADD_RECT_WITH_INITIAL_CORNER,
    OPERATION_FUNCTION_PATH_ADD_ROUND_RECT,
    OPERATION_FUNCTION_PATH_ADD_OVAL_WITH_INITIAL_POINT,
    OPERATION_FUNCTION_PATH_ADD_OVAL,
    OPERATION_FUNCTION_PATH_ADD_ARC,
    OPERATION_FUNCTION_PATH_ADD_PATH,
    OPERATION_FUNCTION_PATH_ADD_PATH_WITH_MATRIX_AND_MODE,
    OPERATION_FUNCTION_PATH_ADD_PATH_WITH_MODE,
    OPERATION_FUNCTION_PATH_ADD_PATH_WITH_OFFSET_AND_MODE,
    OPERATION_FUNCTION_PATH_ADD_POLYGON,
    OPERATION_FUNCTION_PATH_ADD_CIRCLE,
    OPERATION_FUNCTION_PATH_CONTAINS,
    OPERATION_FUNCTION_PATH_TRANSFORM,
    OPERATION_FUNCTION_PATH_TRANSFORM_WITH_PERSPECTIVE_CLIP,
    OPERATION_FUNCTION_PATH_GET_LENGTH,
    OPERATION_FUNCTION_PATH_GET_BOUNDS,
    OPERATION_FUNCTION_PATH_OFFSET,
    OPERATION_FUNCTION_PATH_GET_POSITION_TANGENT,
    OPERATION_FUNCTION_PATH_GET_MATRIX,
    OPERATION_FUNCTION_PATH_CLOSE,
    OPERATION_FUNCTION_PATH_IS_CLOSED,
} OperationFunctionPath;

// PathCreate-PathDestroy循环
class StabilityPathCreate : public TestBase {
public:
    StabilityPathCreate()
    {
        fileName_ = "StabilityPathCreate";
    }
    ~StabilityPathCreate() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// PathCopy-PathDestroy循环
class StabilityPathCopy : public TestBase {
public:
    StabilityPathCopy()
    {
        fileName_ = "StabilityPathCopy";
    }
    ~StabilityPathCopy() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
};

// PathCreate-相关配置接口全调用-PathDestroy循环
class StabilityPathAll : public TestBase {
private:
    OH_Drawing_Path* path_ = nullptr;
    const char* str_ = "M150 0 L75 200 L225 200 Z";
    int index_ = 0;

public:
    StabilityPathAll()
    {
        fileName_ = "StabilityPathAll";
    }
    ~StabilityPathAll() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
    void PathCreateTest(OH_Drawing_Path* path);
    void PathCopyTest(OH_Drawing_Path* path);
    void PathBuildFromSvgStringTest(OH_Drawing_Path* path);
    void PathSetFillTypeTest(OH_Drawing_Path* path);
    void PathResetTest(OH_Drawing_Path* path);
    void PathDestroyTest(OH_Drawing_Path* path);
    using HandlerFunc = std::function<void(OH_Drawing_Path*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { SETTING_FUNCTION_PATH_CREATE, [this](OH_Drawing_Path* path) { return PathCreateTest(path); } },
        { SETTING_FUNCTION_PATH_COPY, [this](OH_Drawing_Path* path) { return PathCopyTest(path); } },
        { SETTING_FUNCTION_PATH_BUILD_FROM_SVG_STRING,
            [this](OH_Drawing_Path* path) { return PathBuildFromSvgStringTest(path); } },
        { SETTING_FUNCTION_PATH_SET_FILL_TYPE, [this](OH_Drawing_Path* path) { return PathSetFillTypeTest(path); } },
        { SETTING_FUNCTION_PATH_RESET, [this](OH_Drawing_Path* path) { return PathResetTest(path); } },
        { SETTING_FUNCTION_PATH_DESTROY, [this](OH_Drawing_Path* path) { return PathDestroyTest(path); } },
    };
};

// Path相关操作接口随机循环调用
class StabilityPathRandom : public TestBase {
private:
    float sMin_ = 0;
    float sMax_ = 100;
    float endMin_ = 200;
    float endMax_ = 300;
    float maxWeight_ = 1;
    float radMin_ = 1;
    float radMax_ = 20;
    float angleMin_ = 0;
    float angleMax_ = 360;
    float left_ = 0;
    float top_ = 0;
    float right_ = 0;
    float bottom_ = 0;
    float startDeg_ = 0;
    float sweepDeg_ = 0;
    float weight_ = 0;
    uint32_t start_ = 0;
    float xRad_ = 0;
    float yRad_ = 0;
    float startAngle_ = 0;
    float sweepAngle_ = 0;
    bool forceClosed_ = false;
    float distance_ = 0;
    bool applyPerspectiveClip_ = true;
    uint32_t count_ = 2;
    bool isClosed_ = false;
    const char* str_ = "M150 0 L75 200 L225 200 Z";
    OH_Drawing_Rect* rect_ = nullptr;
    OH_Drawing_RoundRect* RoundRect_ = nullptr;
    OH_Drawing_Path* path_ = nullptr;
    OH_Drawing_Path* srcPath_ = nullptr;
    OH_Drawing_Path* dstPath_ = nullptr;
    OH_Drawing_Matrix* matrix_ = nullptr;
    int index_ = 0;

public:
    StabilityPathRandom()
    {
        fileName_ = "StabilityPathRandom";
    }
    ~StabilityPathRandom() override {};

protected:
    void OnTestStability(OH_Drawing_Canvas* canvas) override;
    void PathOpTest(OH_Drawing_Path* path);
    void PathMoveToTest(OH_Drawing_Path* path);
    void PathLineToTest(OH_Drawing_Path* path);
    void PathArcToTest(OH_Drawing_Path* path);
    void PathQuadToTest(OH_Drawing_Path* path);
    void PathConicToTest(OH_Drawing_Path* path);
    void PathCubicToTest(OH_Drawing_Path* path);
    void PathRMoveToTest(OH_Drawing_Path* path);
    void PathRLineToTest(OH_Drawing_Path* path);
    void PathRQuadToTest(OH_Drawing_Path* path);
    void PathRConicToTest(OH_Drawing_Path* path);
    void PathRCubicToTest(OH_Drawing_Path* path);
    void PathAddRectTest(OH_Drawing_Path* path);
    void PathAddRectWithInitialCornerTest(OH_Drawing_Path* path);
    void PathAddRoundRectTest(OH_Drawing_Path* path);
    void PathAddOvalWithInitialPointTest(OH_Drawing_Path* path);
    void PathAddOvalTest(OH_Drawing_Path* path);
    void PathAddArcTest(OH_Drawing_Path* path);
    void PathAddPathTest(OH_Drawing_Path* path);
    void PathAddPathWithMatrixAndModeTest(OH_Drawing_Path* path);
    void PathAddPathWithModeTest(OH_Drawing_Path* path);
    void PathAddPathWithOffsetAndModeTest(OH_Drawing_Path* path);
    void PathAddPolygonTest(OH_Drawing_Path* path);
    void PathAddCircleTest(OH_Drawing_Path* path);
    void PathContainsTest(OH_Drawing_Path* path);
    void PathTransformTest(OH_Drawing_Path* path);
    void PathTransformWithPerspectiveClipTest(OH_Drawing_Path* path);
    void PathGetLengthTest(OH_Drawing_Path* path);
    void PathGetBoundsTest(OH_Drawing_Path* path);
    void PathOffsetTest(OH_Drawing_Path* path);
    void PathGetPositionTangentTest(OH_Drawing_Path* path);
    void PathGetMatrixTest(OH_Drawing_Path* path);
    void PathCloseTest(OH_Drawing_Path* path);
    void PathIsClosedTest(OH_Drawing_Path* path);
    using HandlerFunc = std::function<void(OH_Drawing_Path*)>;
    std::map<uint32_t, HandlerFunc> handlers_ = {
        { OPERATION_FUNCTION_PATH_OP, [this](OH_Drawing_Path* path) { return PathOpTest(path); } },
        { OPERATION_FUNCTION_PATH_MOVE_TO, [this](OH_Drawing_Path* path) { return PathMoveToTest(path); } },
        { OPERATION_FUNCTION_PATH_LINE_TO, [this](OH_Drawing_Path* path) { return PathLineToTest(path); } },
        { OPERATION_FUNCTION_PATH_ARC_TO, [this](OH_Drawing_Path* path) { return PathArcToTest(path); } },
        { OPERATION_FUNCTION_PATH_QUAD_TO, [this](OH_Drawing_Path* path) { return PathQuadToTest(path); } },
        { OPERATION_FUNCTION_PATH_CONIC_TO, [this](OH_Drawing_Path* path) { return PathConicToTest(path); } },
        { OPERATION_FUNCTION_PATH_CUBIC_TO, [this](OH_Drawing_Path* path) { return PathCubicToTest(path); } },
        { OPERATION_FUNCTION_PATH_R_MOVE_TO, [this](OH_Drawing_Path* path) { return PathRMoveToTest(path); } },
        { OPERATION_FUNCTION_PATH_R_LINE_TO, [this](OH_Drawing_Path* path) { return PathRLineToTest(path); } },
        { OPERATION_FUNCTION_PATH_R_QUAD_TO, [this](OH_Drawing_Path* path) { return PathRQuadToTest(path); } },
        { OPERATION_FUNCTION_PATH_R_CONIC_TO, [this](OH_Drawing_Path* path) { return PathRConicToTest(path); } },
        { OPERATION_FUNCTION_PATH_R_CUBIC_TO, [this](OH_Drawing_Path* path) { return PathRCubicToTest(path); } },
        { OPERATION_FUNCTION_PATH_ADD_RECT, [this](OH_Drawing_Path* path) { return PathAddRectTest(path); } },
        { OPERATION_FUNCTION_PATH_ADD_RECT_WITH_INITIAL_CORNER,
            [this](OH_Drawing_Path* path) { return PathAddRectWithInitialCornerTest(path); } },
        { OPERATION_FUNCTION_PATH_ADD_ROUND_RECT,
            [this](OH_Drawing_Path* path) { return PathAddRoundRectTest(path); } },
        { OPERATION_FUNCTION_PATH_ADD_OVAL_WITH_INITIAL_POINT,
            [this](OH_Drawing_Path* path) { return PathAddOvalWithInitialPointTest(path); } },
        { OPERATION_FUNCTION_PATH_ADD_OVAL, [this](OH_Drawing_Path* path) { return PathAddOvalTest(path); } },
        { OPERATION_FUNCTION_PATH_ADD_ARC, [this](OH_Drawing_Path* path) { return PathAddArcTest(path); } },
        { OPERATION_FUNCTION_PATH_ADD_PATH, [this](OH_Drawing_Path* path) { return PathAddPathTest(path); } },
        { OPERATION_FUNCTION_PATH_ADD_PATH_WITH_MATRIX_AND_MODE,
            [this](OH_Drawing_Path* path) { return PathAddPathWithMatrixAndModeTest(path); } },
        { OPERATION_FUNCTION_PATH_ADD_PATH_WITH_MODE,
            [this](OH_Drawing_Path* path) { return PathAddPathWithModeTest(path); } },
        { OPERATION_FUNCTION_PATH_ADD_PATH_WITH_OFFSET_AND_MODE,
            [this](OH_Drawing_Path* path) { return PathAddPathWithOffsetAndModeTest(path); } },
        { OPERATION_FUNCTION_PATH_ADD_POLYGON, [this](OH_Drawing_Path* path) { return PathAddPolygonTest(path); } },
        { OPERATION_FUNCTION_PATH_ADD_CIRCLE, [this](OH_Drawing_Path* path) { return PathAddCircleTest(path); } },
        { OPERATION_FUNCTION_PATH_CONTAINS, [this](OH_Drawing_Path* path) { return PathContainsTest(path); } },
        { OPERATION_FUNCTION_PATH_TRANSFORM, [this](OH_Drawing_Path* path) { return PathTransformTest(path); } },
        { OPERATION_FUNCTION_PATH_TRANSFORM_WITH_PERSPECTIVE_CLIP,
            [this](OH_Drawing_Path* path) { return PathTransformWithPerspectiveClipTest(path); } },
        { OPERATION_FUNCTION_PATH_GET_LENGTH, [this](OH_Drawing_Path* path) { return PathGetLengthTest(path); } },
        { OPERATION_FUNCTION_PATH_GET_BOUNDS, [this](OH_Drawing_Path* path) { return PathGetBoundsTest(path); } },
        { OPERATION_FUNCTION_PATH_OFFSET, [this](OH_Drawing_Path* path) { return PathOffsetTest(path); } },
        { OPERATION_FUNCTION_PATH_GET_POSITION_TANGENT,
            [this](OH_Drawing_Path* path) { return PathGetPositionTangentTest(path); } },
        { OPERATION_FUNCTION_PATH_GET_MATRIX, [this](OH_Drawing_Path* path) { return PathGetMatrixTest(path); } },
        { OPERATION_FUNCTION_PATH_CLOSE, [this](OH_Drawing_Path* path) { return PathCloseTest(path); } },
        { OPERATION_FUNCTION_PATH_IS_CLOSED, [this](OH_Drawing_Path* path) { return PathIsClosedTest(path); } },
    };
};

#endif // STABILITY_PATH_TEST_H