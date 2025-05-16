/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "path_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"

#include "drawing_matrix.h"
#include "drawing_path.h"
#include "drawing_rect.h"
#include "drawing_round_rect.h"
#include "drawing_types.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
constexpr size_t PATH_DIRECTION_ENUM_SIZE = 2;
constexpr size_t PATH_ADD_MODE_ENUM_SIZE = 2;
constexpr size_t PATH_FILL_TYPE_ENUM_SIZE = 4;
constexpr size_t PATH_OP_MODE_ENUM_SIZE = 5;
constexpr size_t PATH_MATRIX_FLAGS_ENUM_SIZE = 3;
constexpr uint32_t MAX_ARRAY_SIZE = 5000;
} // namespace

namespace Drawing {
void NativeDrawingPathTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    float x = GetObject<float>();
    float y = GetObject<float>();
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(nullptr, x, y);
    OH_Drawing_PathMoveTo(path, GetObject<float>(), GetObject<float>());
    OH_Drawing_PathRMoveTo(nullptr, GetObject<float>(), GetObject<float>());
    OH_Drawing_PathRMoveTo(path, GetObject<float>(), GetObject<float>());
    OH_Drawing_PathLineTo(nullptr, GetObject<float>(), GetObject<float>());
    OH_Drawing_PathLineTo(path, GetObject<float>(), GetObject<float>());
    OH_Drawing_PathRLineTo(nullptr, GetObject<float>(), GetObject<float>());
    OH_Drawing_PathRLineTo(path, GetObject<float>(), GetObject<float>());
    OH_Drawing_PathClose(nullptr);
    OH_Drawing_PathClose(path);

    OH_Drawing_Path* pathCopy = OH_Drawing_PathCreate();
    pathCopy = OH_Drawing_PathCopy(nullptr);
    pathCopy = OH_Drawing_PathCopy(path);

    OH_Drawing_PathDestroy(pathCopy);
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingPathTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    float x1 = GetObject<float>();
    float y1 = GetObject<float>();
    float x2 = GetObject<float>();
    float y2 = GetObject<float>();
    float startDeg = GetObject<float>();
    float sweepDeg = GetObject<float>();

    OH_Drawing_Path* pathArcTo = OH_Drawing_PathCreate();
    OH_Drawing_PathArcTo(nullptr, x1, y1, x2, y2, startDeg, sweepDeg);
    OH_Drawing_PathArcTo(pathArcTo, GetObject<float>(), GetObject<float>(), GetObject<float>(),GetObject<float>(),
        GetObject<float>(), GetObject<float>());

    OH_Drawing_Path* pathQuadTo = OH_Drawing_PathCreate();
    OH_Drawing_PathQuadTo(nullptr, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());
    OH_Drawing_PathQuadTo(pathQuadTo, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());
    OH_Drawing_PathRQuadTo(nullptr, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());
    OH_Drawing_PathRQuadTo(pathQuadTo, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>());

    OH_Drawing_Path* pathConicTo = OH_Drawing_PathCreate();
    OH_Drawing_PathConicTo(nullptr, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>());
    OH_Drawing_PathConicTo(pathConicTo, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>());
    OH_Drawing_PathRConicTo(nullptr, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>());
    OH_Drawing_PathRConicTo(pathConicTo, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>());
    OH_Drawing_Path* pathCubicTo = OH_Drawing_PathCreate();
    OH_Drawing_PathCubicTo(nullptr, GetObject<float>(), GetObject<float>(), GetObject<float>(),GetObject<float>(),
        GetObject<float>(), GetObject<float>());
    OH_Drawing_PathCubicTo(pathCubicTo, GetObject<float>(), GetObject<float>(), GetObject<float>(),GetObject<float>(),
        GetObject<float>(), GetObject<float>());
    OH_Drawing_PathRCubicTo(nullptr, GetObject<float>(), GetObject<float>(), GetObject<float>(),GetObject<float>(),
        GetObject<float>(), GetObject<float>());
    OH_Drawing_PathRCubicTo(pathCubicTo, GetObject<float>(), GetObject<float>(), GetObject<float>(),GetObject<float>(),
        GetObject<float>(), GetObject<float>());

    OH_Drawing_PathDestroy(pathArcTo);
    OH_Drawing_PathDestroy(pathQuadTo);
    OH_Drawing_PathDestroy(pathConicTo);
    OH_Drawing_PathDestroy(pathCubicTo);
}

void NativeDrawingPathTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    float xRad = GetObject<float>();
    float yRad = GetObject<float>();
    uint32_t start = GetObject<uint32_t>();
    uint32_t pathDirection = GetObject<uint32_t>();

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathAddRect(path, left, top, right, bottom, static_cast<OH_Drawing_PathDirection>(pathDirection));
    OH_Drawing_PathAddRect(nullptr, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>(),
                           static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE));
    OH_Drawing_PathAddRect(path, GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>(),
                           static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE));

    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(GetObject<float>(), GetObject<float>(), GetObject<float>(),
        GetObject<float>());
    OH_Drawing_PathAddRectWithInitialCorner(path, rect, static_cast<OH_Drawing_PathDirection>(pathDirection), start);
    OH_Drawing_PathAddRectWithInitialCorner(nullptr, rect,
        static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE),
        GetObject<uint32_t>());
    OH_Drawing_PathAddRectWithInitialCorner(path, nullptr,
        static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE),
        GetObject<uint32_t>());
    OH_Drawing_PathAddRectWithInitialCorner(path, rect,
        static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE),
        GetObject<uint32_t>());

    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, xRad, yRad);
    OH_Drawing_PathAddRoundRect(path, roundRect, static_cast<OH_Drawing_PathDirection>(pathDirection));
    OH_Drawing_PathAddRoundRect(nullptr, roundRect,
        static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE));
    OH_Drawing_PathAddRoundRect(path, nullptr,
        static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE));
    OH_Drawing_PathAddRoundRect(path, roundRect,
        static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE));

    OH_Drawing_PathAddOvalWithInitialPoint(nullptr, rect, GetObject<uint32_t>(),
        static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE));
    OH_Drawing_PathAddOvalWithInitialPoint(path, nullptr, GetObject<uint32_t>(),
        static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE));
    OH_Drawing_PathAddOvalWithInitialPoint(path, rect, GetObject<uint32_t>(),
        static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE));

    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RoundRectDestroy(roundRect);
}

void NativeDrawingPathTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    float startAngle = GetObject<float>();
    float sweepAngle = GetObject<float>();
    float scaleX = GetObject<float>();
    float skewX = GetObject<float>();
    float transX = GetObject<float>();
    float skewY = GetObject<float>();
    float scaleY = GetObject<float>();
    float transY = GetObject<float>();
    float persp0 = GetObject<float>();
    float persp1 = GetObject<float>();
    float persp2 = GetObject<float>();

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(left, top, right, bottom);
    OH_Drawing_PathAddArc(nullptr, rect, startAngle, sweepAngle);
    OH_Drawing_PathAddArc(path, nullptr, GetObject<float>(), GetObject<float>());
    OH_Drawing_PathAddArc(path, rect, GetObject<float>(), GetObject<float>());
    
    OH_Drawing_Path* destPath = OH_Drawing_PathCreate();
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);

    OH_Drawing_PathAddPath(nullptr, path, matrix);
    OH_Drawing_PathAddPath(destPath, nullptr, matrix);
    OH_Drawing_PathAddPath(destPath, path, nullptr);
    OH_Drawing_PathAddPath(destPath, path, matrix);

    OH_Drawing_PathAddPathWithMatrixAndMode(nullptr, path, matrix,
        static_cast<OH_Drawing_PathAddMode>(GetObject<uint32_t>() % PATH_ADD_MODE_ENUM_SIZE));
    OH_Drawing_PathAddPathWithMatrixAndMode(destPath, nullptr, matrix,
        static_cast<OH_Drawing_PathAddMode>(GetObject<uint32_t>() % PATH_ADD_MODE_ENUM_SIZE));
    OH_Drawing_PathAddPathWithMatrixAndMode(destPath, path, nullptr,
        static_cast<OH_Drawing_PathAddMode>(GetObject<uint32_t>() % PATH_ADD_MODE_ENUM_SIZE));
    OH_Drawing_PathAddPathWithMatrixAndMode(destPath, path, matrix,
        static_cast<OH_Drawing_PathAddMode>(GetObject<uint32_t>() % PATH_ADD_MODE_ENUM_SIZE));

    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(destPath);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_MatrixDestroy(matrix);
}

void NativeDrawingPathTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    float dx = GetObject<float>();
    float dy = GetObject<float>();
    uint32_t pathDirection = GetObject<uint32_t>();

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Path* dstPath = OH_Drawing_PathCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(left, top, right, bottom);

    OH_Drawing_PathAddOval(nullptr, rect,
        static_cast<OH_Drawing_PathDirection>(pathDirection % PATH_DIRECTION_ENUM_SIZE));
    OH_Drawing_PathAddOval(path, nullptr,
        static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE));
    OH_Drawing_PathAddOval(path, rect,
        static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE));

    OH_Drawing_PathAddPathWithMode(nullptr, path,
        static_cast<OH_Drawing_PathAddMode>(GetObject<uint32_t>() % PATH_ADD_MODE_ENUM_SIZE));
    OH_Drawing_PathAddPathWithMode(dstPath, nullptr,
        static_cast<OH_Drawing_PathAddMode>(GetObject<uint32_t>() % PATH_ADD_MODE_ENUM_SIZE));
    OH_Drawing_PathAddPathWithMode(dstPath, path,
        static_cast<OH_Drawing_PathAddMode>(GetObject<uint32_t>() % PATH_ADD_MODE_ENUM_SIZE));

    OH_Drawing_PathAddPathWithOffsetAndMode(nullptr, path, dx, dy,
        static_cast<OH_Drawing_PathAddMode>(GetObject<uint32_t>() % PATH_ADD_MODE_ENUM_SIZE));
    OH_Drawing_PathAddPathWithOffsetAndMode(dstPath, nullptr, dx, dy,
        static_cast<OH_Drawing_PathAddMode>(GetObject<uint32_t>() % PATH_ADD_MODE_ENUM_SIZE));
    OH_Drawing_PathAddPathWithOffsetAndMode(dstPath, path, dx, dy,
        static_cast<OH_Drawing_PathAddMode>(GetObject<uint32_t>() % PATH_ADD_MODE_ENUM_SIZE));

    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(dstPath);
    OH_Drawing_RectDestroy(rect);
}

void NativeDrawingPathTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t count = GetObject<uint32_t>() % MAX_ARRAY_SIZE + 1;
    OH_Drawing_Point2D* point = new OH_Drawing_Point2D[count];
    for (size_t i = 0; i < count; i++) {
        point[i] = {GetObject<float>(), GetObject<float>()};
    }
    char* str = new char[count];
    for (size_t i = 0; i < count; i++) {
        str[i] =  GetObject<char>();
    }
    str[count - 1] = '\0';
    float x = GetObject<float>();
    float y = GetObject<float>();
    OH_Drawing_Path* path = OH_Drawing_PathCreate();

    OH_Drawing_PathAddPolygon(nullptr, point, count, GetObject<bool>());
    OH_Drawing_PathAddPolygon(path, nullptr, count, GetObject<bool>());
    OH_Drawing_PathAddPolygon(path, point, 0, GetObject<bool>());
    OH_Drawing_PathAddPolygon(path, point, count, GetObject<bool>());
    if (point != nullptr) {
        delete[] point;
        point = nullptr;
    }

    OH_Drawing_PathAddCircle(nullptr, x, y, GetObject<float>(),
        static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE));
    OH_Drawing_PathAddCircle(nullptr, GetObject<float>(), GetObject<float>(), 0,
        static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE));
    OH_Drawing_PathAddCircle(path, GetObject<float>(), GetObject<float>(), GetObject<float>(),
        static_cast<OH_Drawing_PathDirection>(GetObject<uint32_t>() % PATH_DIRECTION_ENUM_SIZE));

    OH_Drawing_PathBuildFromSvgString(nullptr, str);
    OH_Drawing_PathBuildFromSvgString(path, nullptr);
    OH_Drawing_PathBuildFromSvgString(path, str);
    if (str != nullptr) {
        delete[] str;
        str = nullptr;
    }
    OH_Drawing_PathDestroy(path);
}

void NativeDrawingPathTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    float x = GetObject<float>();
    float y = GetObject<float>();
    bool isValue = GetObject<bool>();
    float scaleX = GetObject<float>();
    float skewX = GetObject<float>();
    float transX = GetObject<float>();
    float skewY = GetObject<float>();
    float scaleY = GetObject<float>();
    float transY = GetObject<float>();
    float persp0 = GetObject<float>();
    float persp1 = GetObject<float>();
    float persp2 = GetObject<float>();
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathContains(nullptr, x, y);
    OH_Drawing_PathContains(path, GetObject<float>(), GetObject<float>());

    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);
    OH_Drawing_PathTransform(nullptr, matrix);
    OH_Drawing_PathTransform(path, nullptr);
    OH_Drawing_PathTransform(path, matrix);

    OH_Drawing_Path* dstPath = OH_Drawing_PathCreate();
    OH_Drawing_PathTransformWithPerspectiveClip(nullptr, matrix, dstPath, isValue);
    OH_Drawing_PathTransformWithPerspectiveClip(path, nullptr, dstPath, GetObject<bool>());
    OH_Drawing_PathTransformWithPerspectiveClip(path, matrix, dstPath, GetObject<bool>());

    OH_Drawing_PathSetFillType(nullptr, static_cast<OH_Drawing_PathFillType>(GetObject<uint32_t>() % PATH_FILL_TYPE_ENUM_SIZE));
    OH_Drawing_PathSetFillType(path, static_cast<OH_Drawing_PathFillType>(GetObject<uint32_t>()));
    OH_Drawing_PathSetFillType(path, static_cast<OH_Drawing_PathFillType>(GetObject<uint32_t>() % PATH_FILL_TYPE_ENUM_SIZE));

    OH_Drawing_PathGetLength(nullptr, GetObject<bool>());
    OH_Drawing_PathGetLength(path, GetObject<bool>());
    OH_Drawing_PathClose(nullptr);
    OH_Drawing_PathClose(path);

    OH_Drawing_PathOffset(nullptr, dstPath, GetObject<float>(), GetObject<float>());
    OH_Drawing_PathOffset(path, dstPath, GetObject<float>(), GetObject<float>());

    OH_Drawing_PathReset(nullptr);
    OH_Drawing_PathReset(path);

    OH_Drawing_PathDestroy(dstPath);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_MatrixDestroy(matrix);
}

void NativeDrawingPathTest008(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(left, top, right, bottom);
    OH_Drawing_PathGetBounds(nullptr, rect);
    OH_Drawing_PathGetBounds(path, nullptr);
    OH_Drawing_PathGetBounds(path, rect);

    OH_Drawing_PathIsClosed(nullptr, GetObject<bool>());
    OH_Drawing_PathIsClosed(path, GetObject<bool>());

    OH_Drawing_Path* pathGetPosition = OH_Drawing_PathCreate();
    OH_Drawing_Point2D tangent;
    OH_Drawing_Point2D position;
    OH_Drawing_PathGetPositionTangent(nullptr, GetObject<bool>(), GetObject<float>(), &position, &tangent);
    OH_Drawing_PathGetPositionTangent(pathGetPosition, GetObject<bool>(), GetObject<float>(), &position, &tangent);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(pathGetPosition);
    OH_Drawing_RectDestroy(rect);
}

void NativeDrawingPathTest009(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint32_t pathOpMode = GetObject<uint32_t>();
    uint32_t pathMatrixFlags = GetObject<uint32_t>();
    float scaleX = GetObject<float>();
    float skewX = GetObject<float>();
    float transX = GetObject<float>();
    float skewY = GetObject<float>();
    float scaleY = GetObject<float>();
    float transY = GetObject<float>();
    float persp0 = GetObject<float>();
    float persp1 = GetObject<float>();
    float persp2 = GetObject<float>();
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Path* srcPath = OH_Drawing_PathCreate();

    OH_Drawing_PathOp(nullptr, srcPath, static_cast<OH_Drawing_PathOpMode>(pathOpMode % PATH_OP_MODE_ENUM_SIZE));
    OH_Drawing_PathOp(path, srcPath, static_cast<OH_Drawing_PathOpMode>(pathOpMode % PATH_OP_MODE_ENUM_SIZE));

    OH_Drawing_Path* pathGetMatrix = OH_Drawing_PathCreate();
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, scaleX, skewX, transX, skewY, scaleY, transY, persp0, persp1, persp2);

    OH_Drawing_PathGetMatrix(nullptr, GetObject<bool>(), GetObject<float>(),
        matrix, static_cast<OH_Drawing_PathMeasureMatrixFlags>(pathMatrixFlags % PATH_MATRIX_FLAGS_ENUM_SIZE));
    OH_Drawing_PathGetMatrix(pathGetMatrix, GetObject<bool>(), GetObject<float>(),
        matrix, static_cast<OH_Drawing_PathMeasureMatrixFlags>(pathMatrixFlags % PATH_MATRIX_FLAGS_ENUM_SIZE));

    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(srcPath);
    OH_Drawing_PathDestroy(pathGetMatrix);
    OH_Drawing_MatrixDestroy(matrix);
}

void NativeDrawingPathTest010(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, GetObject<float>(), GetObject<float>());
    OH_Drawing_PathLineTo(path, GetObject<float>(), GetObject<float>());
    OH_Drawing_PathLineTo(path, GetObject<float>(), GetObject<float>());
    OH_Drawing_Path* newPath = OH_Drawing_PathCreate();
    bool result = false;

    OH_Drawing_PathGetSegment(path, GetObject<bool>(), GetObject<float>(), GetObject<float>(), GetObject<bool>(),
        newPath, &result);
    OH_Drawing_PathGetSegment(nullptr, GetObject<bool>(), GetObject<float>(), GetObject<float>(), GetObject<bool>(),
        newPath, &result);
    OH_Drawing_PathGetSegment(path, GetObject<bool>(), GetObject<float>(), GetObject<float>(), GetObject<bool>(),
        nullptr, &result);
    OH_Drawing_PathGetSegment(path, GetObject<bool>(), GetObject<float>(), GetObject<float>(), GetObject<bool>(),
        newPath, nullptr);

    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(newPath);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingPathTest001(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathTest002(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathTest003(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathTest004(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathTest005(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathTest006(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathTest007(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathTest008(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathTest009(data, size);
    OHOS::Rosen::Drawing::NativeDrawingPathTest010(data, size);
    return 0;
}
