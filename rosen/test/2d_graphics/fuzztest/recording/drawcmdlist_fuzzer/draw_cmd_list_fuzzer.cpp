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

#include "draw_cmd_list_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd.h"
#include "recording/draw_cmd_list.h"
#include "recording/recording_canvas.h"

#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MAX_SIZE = 5000;
} // namespace

namespace Drawing {
void DrawCmdListFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int32_t width = GetObject<int32_t>() % MAX_SIZE;
    int32_t height = GetObject<int32_t>() % MAX_SIZE;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* obj = new char[length];
    for (size_t i = 0; i < length; i++) {
        obj[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(obj);
    cmdListData.second = length;
    bool isCopy = GetObject<bool>();
    static std::shared_ptr<DrawCmdList> drawCmdList = DrawCmdList::CreateFromData(cmdListData, isCopy);
    drawCmdList->GetType();
    drawCmdList->ClearOp();
    drawCmdList->GetOpItemSize();
    drawCmdList->GetOpsWithDesc();
    drawCmdList->MarshallingDrawOps();
    drawCmdList->UnmarshallingDrawOps();
    drawCmdList->IsEmpty();
    drawCmdList->SetWidth(width);
    drawCmdList->SetHeight(height);
    drawCmdList->GetWidth();
    drawCmdList->GetHeight();
    drawCmdList->SetIsCache(static_cast<bool>(isCopy));
    drawCmdList->GetIsCache();
    drawCmdList->SetCachedHighContrast(static_cast<bool>(isCopy));
    drawCmdList->GetCachedHighContrast();
    if (obj != nullptr) {
        delete [] obj;
        obj = nullptr;
    }
}

void DrawCmdListFuzzTest001(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t width = GetObject<int32_t>();
    uint32_t height = GetObject<int32_t>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    scalar l = GetObject<scalar>();
    scalar t = GetObject<scalar>();
    scalar r = GetObject<scalar>();
    scalar b = GetObject<scalar>();
    NodeId nodeId = GetObject<NodeId>();
    char* obj = new char[length];
    for (size_t i = 0; i < length; i++) {
        obj[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(obj);
    cmdListData.second = length;
    bool isCopy = GetObject<bool>();
    static std::shared_ptr<DrawCmdList> drawCmdList = DrawCmdList::CreateFromData(cmdListData, isCopy);
    Canvas canvas(width, height);
    Rect rect(l, t, r, b);
    drawCmdList->Playback(canvas);
    drawCmdList->Playback(canvas, &rect);
    drawCmdList->GenerateCache();
    drawCmdList->GenerateCache(&canvas);
    drawCmdList->GenerateCache(&canvas, &rect);
    drawCmdList->UpdateNodeIdToPicture(nodeId);
    std::vector<std::pair<size_t, size_t>> replacedOpList = drawCmdList->GetReplacedOpList();
    drawCmdList->SetReplacedOpList(replacedOpList);
    drawCmdList->GetBounds(rect);
    drawCmdList->ClearOp();
    drawCmdList->ClearCache();
    std::string out;
    drawCmdList->Dump(out);
    if (obj != nullptr) {
        delete [] obj;
        obj = nullptr;
    }
}

void DrawCmdListFuzzTest002(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t width = GetObject<int32_t>();
    uint32_t height = GetObject<int32_t>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    scalar l = GetObject<scalar>();
    scalar t = GetObject<scalar>();
    scalar r = GetObject<scalar>();
    scalar b = GetObject<scalar>();
    NodeId nodeId = GetObject<NodeId>();
    char* obj = new char[length];
    for (size_t i = 0; i < length; i++) {
        obj[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(obj);
    cmdListData.second = length;
    bool isCopy = GetObject<bool>();
    static std::shared_ptr<DrawCmdList> drawCmdList = DrawCmdList::CreateFromData(cmdListData, isCopy);
    Canvas canvas(width, height);
    Rect rect(l, t, r, b);
    Brush brush;
    drawCmdList->AddDrawOp(std::make_shared<DrawBackgroundOpItem>(brush));
    drawCmdList->Playback(canvas);
    drawCmdList->Playback(canvas, &rect);
    drawCmdList->GenerateCache();
    drawCmdList->GenerateCache(&canvas);
    drawCmdList->GenerateCache(&canvas, &rect);
    drawCmdList->UpdateNodeIdToPicture(nodeId);
    std::vector<std::pair<size_t, size_t>> replacedOpList = drawCmdList->GetReplacedOpList();
    drawCmdList->SetReplacedOpList(replacedOpList);
    drawCmdList->ClearOp();
    drawCmdList->ClearCache();
    std::string out;
    drawCmdList->Dump(out);
    drawCmdList->Purge();
    if (obj != nullptr) {
        delete [] obj;
        obj = nullptr;
    }
}

void DrawCmdListFuzzTest003(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    scalar l = GetObject<scalar>();
    scalar t = GetObject<scalar>();
    scalar r = GetObject<scalar>();
    scalar b = GetObject<scalar>();
    NodeId nodeId = GetObject<NodeId>();
    char* obj = new char[length];
    for (size_t i = 0; i < length; i++) {
        obj[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(obj);
    cmdListData.second = length;
    bool isCopy = GetObject<bool>();
    static std::shared_ptr<DrawCmdList> drawCmdList = DrawCmdList::CreateFromData(cmdListData, isCopy);
    drawCmdList->SetIsCache(true);
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 10); // 10: width, height
    Rect rect(l, t, r, b);
    drawCmdList->Playback(*recordingCanvas, &rect);
    drawCmdList->GenerateCache(recordingCanvas.get(), &rect);
    drawCmdList->SetIsCache(false);
    drawCmdList->GenerateCache(recordingCanvas.get(), &rect);
    drawCmdList->AddDrawOp(nullptr);
    drawCmdList->GetOpItemSize();
    drawCmdList->GetOpsWithDesc();
    drawCmdList->MarshallingDrawOps();
    drawCmdList->UnmarshallingDrawOps();
    drawCmdList->UpdateNodeIdToPicture(nodeId);
    drawCmdList->ClearOp();
    drawCmdList->ClearCache();
    std::string out;
    drawCmdList->Dump(out);
    if (obj != nullptr) {
        delete [] obj;
        obj = nullptr;
    }
}

void DrawCmdListFuzzTest004(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* obj = new char[length];
    for (size_t i = 0; i < length; i++) {
        obj[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(obj);
    cmdListData.second = length;
    bool isCopy = GetObject<bool>();
    static std::shared_ptr<DrawCmdList> drawCmdList = DrawCmdList::CreateFromData(cmdListData, isCopy);
    drawCmdList->SetIsCache(true);
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 10); // 10: width, height
    Rect rect(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    drawCmdList->Playback(*recordingCanvas, &rect);
    drawCmdList->GenerateCache(recordingCanvas.get(), &rect);
    drawCmdList->SetIsCache(false);
    drawCmdList->GenerateCache(recordingCanvas.get(), &rect);
    Font font;
    auto textBlob = TextBlob::MakeFromString("11", font, TextEncoding::UTF8);
    Paint paint;
    auto space = std::make_shared<ColorSpace>();
    Color4f color;
    color.alphaF_ = 0;
    color.blueF_ = 1;
    paint.SetColor(color, space);
    auto opItem = std::make_shared<DrawTextBlobOpItem>(textBlob.get(), 0, 0, paint);
    drawCmdList->AddDrawOp(opItem);
    drawCmdList->SetIsCache(true);
    drawCmdList->GenerateCache(recordingCanvas.get(), &rect);
    drawCmdList->PatchTypefaceIds();
    drawCmdList->GetCachedHighContrast();
    drawCmdList->GetOpItemSize();
    drawCmdList->GetOpsWithDesc();
    drawCmdList->MarshallingDrawOps();
    drawCmdList->UnmarshallingDrawOps();
    drawCmdList->ClearOp();
    drawCmdList->ClearCache();
    if (obj != nullptr) {
        delete [] obj;
        obj = nullptr;
    }
}

void DrawCmdListFuzzTest005(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    scalar l = GetObject<scalar>();
    scalar t = GetObject<scalar>();
    scalar r = GetObject<scalar>();
    scalar b = GetObject<scalar>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* obj = new char[length];
    for (size_t i = 0; i < length; i++) {
        obj[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(obj);
    cmdListData.second = length;
    bool isCopy = GetObject<bool>();
    static std::shared_ptr<DrawCmdList> drawCmdList = DrawCmdList::CreateFromData(cmdListData, isCopy);
    drawCmdList->SetIsCache(true);
    drawCmdList->SetCachedHighContrast(true);
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 10); // 10: width, height
    Rect rect(l, t, r, b);
    drawCmdList->Playback(*recordingCanvas, &rect);
    drawCmdList->GenerateCache(recordingCanvas.get(), &rect);
    Brush brush;
    drawCmdList->AddDrawOp(std::make_shared<DrawBackgroundOpItem>(brush));

    drawCmdList->SetIsCache(true);
    drawCmdList->SetCachedHighContrast(false);
    drawCmdList->GenerateCache(recordingCanvas.get(), &rect);
    drawCmdList->ClearOp();
    drawCmdList->ClearCache();
    if (obj != nullptr) {
        delete [] obj;
        obj = nullptr;
    }
}

void DrawCmdListFuzzTest006(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t width = GetObject<int32_t>();
    uint32_t height = GetObject<int32_t>();
    scalar l = GetObject<scalar>();
    scalar t = GetObject<scalar>();
    scalar r = GetObject<scalar>();
    scalar b = GetObject<scalar>();
    Canvas canvas(width, height);
    Rect rect(l, t, r, b);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* obj = new char[length];
    for (size_t i = 0; i < length; i++) {
        obj[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(obj);
    cmdListData.second = length;
    bool isCopy = GetObject<bool>();
    static std::shared_ptr<DrawCmdList> drawCmdList = DrawCmdList::CreateFromData(cmdListData, isCopy);
    drawCmdList->MarshallingDrawOps();
    drawCmdList->CaculatePerformanceOpType();
    drawCmdList->ClearCache();
    drawCmdList->GenerateCacheByBuffer(&canvas, &rect);
    std::pair<const void*, size_t> cmdListData2;
    cmdListData2.first = static_cast<const void*>(obj);
    cmdListData2.second = length - 1;
    auto drawCmdListTwo = DrawCmdList::CreateFromData(cmdListData2, isCopy);
    drawCmdList->PlaybackToDrawCmdList(drawCmdListTwo);
    drawCmdList->ClearOp();
    drawCmdList->ClearCache();
    drawCmdListTwo->ClearOp();
    drawCmdListTwo->ClearCache();
    if (obj != nullptr) {
        delete [] obj;
        obj = nullptr;
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::DrawCmdListFuzzTest000(data, size);
    OHOS::Rosen::Drawing::DrawCmdListFuzzTest001(data, size);
    OHOS::Rosen::Drawing::DrawCmdListFuzzTest002(data, size);
    OHOS::Rosen::Drawing::DrawCmdListFuzzTest003(data, size);
    OHOS::Rosen::Drawing::DrawCmdListFuzzTest004(data, size);
    OHOS::Rosen::Drawing::DrawCmdListFuzzTest005(data, size);
    OHOS::Rosen::Drawing::DrawCmdListFuzzTest006(data, size);
    return 0;
}