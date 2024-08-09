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

#include "drawcmd_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"
#include "recording/draw_cmd.h"
#include "recording/draw_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t BLENDMODE_SIZE = 29;
constexpr size_t MAX_SIZE = 5000;
constexpr size_t PIONTMODE_SIZE = 3;
} // namespace
namespace Drawing {
bool DrawCmdFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    BrushHandle brushHandle;
    brushHandle.colorFilterHandle.size = GetObject<size_t>();
    brushHandle.colorSpaceHandle.size = GetObject<size_t>();
    brushHandle.shaderEffectHandle.size = GetObject<size_t>();
    brushHandle.imageFilterHandle.size = GetObject<size_t>();
    brushHandle.maskFilterHandle.size = GetObject<size_t>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    Brush brush;
    DrawOpItem::BrushHandleToBrush(brushHandle, *drawCmdList, brush);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    BrushHandle brushHandle;
    brushHandle.colorFilterHandle.size = GetObject<size_t>();
    brushHandle.colorSpaceHandle.size = GetObject<size_t>();
    brushHandle.shaderEffectHandle.size = GetObject<size_t>();
    brushHandle.imageFilterHandle.size = GetObject<size_t>();
    brushHandle.maskFilterHandle.size = GetObject<size_t>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Brush brush = Brush(color);
    uint32_t mode = GetObject<uint32_t>();
    bool isAntiAlias = GetObject<bool>();
    brush.SetBlendMode(static_cast<BlendMode>(mode % BLENDMODE_SIZE));
    brush.SetAntiAlias(isAntiAlias);
    DrawOpItem::BrushToBrushHandle(brush, *drawCmdList, brushHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.blenderEnabled = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    paintHandle.miterLimit = GetObject<scalar>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawOpItem::GeneratePaintFromHandle(paintHandle, *drawCmdList, paint);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.blenderEnabled = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    paintHandle.miterLimit = GetObject<scalar>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    scalar width = GetObject<scalar>();
    uint32_t mode = GetObject<uint32_t>();
    bool isAntiAlias = GetObject<bool>();
    paint.SetWidth(width);
    paint.SetBlendMode(static_cast<BlendMode>(mode % BLENDMODE_SIZE));
    paint.SetAntiAlias(isAntiAlias);
    DrawOpItem::GenerateHandleFromPaint(*drawCmdList, paint, paintHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    uint32_t type = GetObject<uint32_t>();
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    scalar width = GetObject<scalar>();
    uint32_t mode = GetObject<uint32_t>();
    bool isAntiAlias = GetObject<bool>();
    paint.SetWidth(width);
    paint.SetBlendMode(static_cast<BlendMode>(mode % BLENDMODE_SIZE));
    paint.SetAntiAlias(isAntiAlias);
    DrawWithPaintOpItem drawWithPaintOpItem = DrawWithPaintOpItem(paint, type);
    drawWithPaintOpItem.Marshalling(*drawCmdList);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    uint32_t type = GetObject<uint32_t>();
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.blenderEnabled = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    paintHandle.miterLimit = GetObject<scalar>();
    DrawWithPaintOpItem drawWithPaintOpItem = DrawWithPaintOpItem(*drawCmdList, paintHandle, type);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawWithPaintOpItem.Playback(&canvas, &rect);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.blenderEnabled = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    paintHandle.miterLimit = GetObject<scalar>();
    int32_t x = GetObject<int32_t>();
    int32_t y = GetObject<int32_t>();
    Point point { x, y };
    DrawPointOpItem::ConstructorHandle constructorHandle = DrawPointOpItem::ConstructorHandle(point, paintHandle);
    DrawPointOpItem drawPointOpItem = DrawPointOpItem(*drawCmdList, &constructorHandle);
    drawPointOpItem.Marshalling(*drawCmdList);
    DrawPointOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest008(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    int32_t x = GetObject<int32_t>();
    int32_t y = GetObject<int32_t>();
    Point point { x, y };
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawPointOpItem drawPointOpItem = DrawPointOpItem(point, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawPointOpItem.Playback(&canvas, &rect);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest009(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::vector<Point> points;
    scalar ptOneX = GetObject<scalar>();
    scalar ptOneY = GetObject<scalar>();
    scalar ptTwoX = GetObject<scalar>();
    scalar ptTwoY = GetObject<scalar>();
    Point ptOne { ptOneX, ptOneY };
    Point ptTwo { ptTwoX, ptTwoY };
    points.push_back(ptOne);
    points.push_back(ptTwo);
    uint32_t pointMode = GetObject<uint32_t>();
    Color color = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    Paint paint = Paint(color);
    DrawPointsOpItem drawPointsOpItem = DrawPointsOpItem(static_cast<PointMode>(pointMode % PIONTMODE_SIZE), points,
        paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawPointsOpItem.Playback(&canvas, &rect);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    std::pair<uint32_t, size_t> pts = { GetObject<uint32_t>(), length };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.blenderEnabled = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    paintHandle.miterLimit = GetObject<scalar>();
    DrawPointsOpItem::ConstructorHandle constructorHandle = DrawPointsOpItem::ConstructorHandle(
        static_cast<PointMode>(pointMode % PIONTMODE_SIZE), pts, paintHandle);
    drawPointsOpItem.Marshalling(*drawCmdList);
    DrawPointsOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    DrawPointsOpItem drawPointsOpItemTwo = DrawPointsOpItem(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest010(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path;
    uint32_t strCount = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* str = new char[strCount];
    for (size_t i = 0; i < strCount; i++) {
        str[i] = GetObject<char>();
    }
    str[strCount - 1] = '\0';
    path.BuildFromSVGString(str);
    Color color = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    Paint paint = Paint(color);
    DrawPathOpItem drawPathOpItem = DrawPathOpItem(path, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawPathOpItem.Playback(&canvas, &rectT);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle pathT { GetObject<uint32_t>(), GetObject<size_t>() };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.blenderEnabled = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    paintHandle.miterLimit = GetObject<scalar>();
    DrawPathOpItem::ConstructorHandle constructorHandle = DrawPathOpItem::ConstructorHandle(pathT, paintHandle);
    drawPathOpItem.Marshalling(*drawCmdList);
    DrawPathOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    DrawPathOpItem drawPathOpItemTwo = DrawPathOpItem(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    if (str != nullptr) {
        delete [] str;
        str = nullptr;
    }
    return true;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::DrawCmdFuzzTest001(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest002(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest003(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest004(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest005(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest006(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest007(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest008(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest009(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest010(data, size);

    return 0;
}
