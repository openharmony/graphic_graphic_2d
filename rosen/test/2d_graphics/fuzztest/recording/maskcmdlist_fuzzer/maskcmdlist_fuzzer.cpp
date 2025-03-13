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

#include "maskcmdlist_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"
#include "recording/mask_cmd_list.h"
#include "recording/draw_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t BLENDMODE_SIZE = 29;
constexpr size_t MAX_SIZE = 5000;
constexpr size_t TYPE_SIZE = 4;
} // namespace
namespace Drawing {
bool MaskCmdListFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<Path> pathPtr = std::make_shared<Path>();
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Brush brush = Brush(color);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    bool isCopy = GetObject<bool>();
    static std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, isCopy);
    MaskPlayer maskPlayer = MaskPlayer(pathPtr, brush, *cmdList);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool MaskCmdListFuzzTest002(const uint8_t* data, size_t size)
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
    CmdListData cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    bool isCopy = GetObject<bool>();
    static std::shared_ptr<MaskCmdList> maskCmdList = MaskCmdList::CreateFromData(cmdListData, isCopy);
    maskCmdList->GetType();
    std::shared_ptr<Path> pathPtr = std::make_shared<Path>();
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Brush brush = Brush(color);
    maskCmdList->Playback(pathPtr, brush);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool MaskCmdListFuzzTest003(const uint8_t* data, size_t size)
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
    CmdListData cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    bool isCopy = GetObject<bool>();
    static std::shared_ptr<MaskCmdList> maskCmdList = MaskCmdList::CreateFromData(cmdListData, isCopy);
    std::shared_ptr<Path> pathPtr = std::make_shared<Path>();
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Brush brush = Brush(color);
    Pen pen = Pen(color);
    maskCmdList->Playback(pathPtr, pen, brush);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool MaskCmdListFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    BrushHandle brushHandle;
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    brushHandle.colorFilterHandle.size = GetObject<size_t>();
    brushHandle.colorSpaceHandle.size = GetObject<size_t>();
    brushHandle.shaderEffectHandle.size = GetObject<size_t>();
    brushHandle.imageFilterHandle.size = GetObject<size_t>();
    brushHandle.maskFilterHandle.size = GetObject<size_t>();
    brushHandle.isAntiAlias = GetObject<bool>();
    brushHandle.blenderEnabled = GetObject<bool>();
    uint32_t blendMode = GetObject<uint32_t>();
    brushHandle.mode = static_cast<BlendMode>(blendMode % BLENDMODE_SIZE);
    brushHandle.color = color;
    MaskBrushOpItem maskBrushOpItem = MaskBrushOpItem(brushHandle);
    std::shared_ptr<Path> pathPtr = std::make_shared<Path>();
    Brush brush = Brush(color);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    if (dataText == nullptr) {
        return false;
    }
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    bool isCopy = GetObject<bool>();
    static std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, isCopy);
    MaskPlayer maskPlayer = MaskPlayer(pathPtr, brush, *cmdList);
    size_t leftOpAllocatorSize = GetObject<size_t>();
    MaskBrushOpItem::Playback(maskPlayer, nullptr, leftOpAllocatorSize);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool MaskCmdListFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    PenHandle penHandle;
    penHandle.width = GetObject<scalar>();
    penHandle.miterLimit = GetObject<scalar>();
    MaskPenOpItem maskPenOpItem = MaskPenOpItem(penHandle);
    std::shared_ptr<Path> pathPtr = std::make_shared<Path>();
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Brush brush = Brush(color);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    if (dataText == nullptr) {
        return false;
    }
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    bool isCopy = GetObject<bool>();
    static std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, isCopy);
    MaskPlayer maskPlayer = MaskPlayer(pathPtr, brush, *cmdList);
    size_t leftOpAllocatorSize = GetObject<size_t>();
    MaskPenOpItem::Playback(maskPlayer, &maskPenOpItem, leftOpAllocatorSize);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool MaskCmdListFuzzTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    PenHandle penHandle;
    penHandle.width = GetObject<scalar>();
    penHandle.miterLimit = GetObject<scalar>();
    MaskPenOpItem maskPenOpItem = MaskPenOpItem(penHandle);
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Pen pen = Pen(color);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    maskPenOpItem.Playback(pen, *cmdList);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool MaskCmdListFuzzTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    OpDataHandle pathHandle;
    pathHandle.size = GetObject<size_t>();
    pathHandle.offset = GetObject<uint32_t>();
    MaskPathOpItem maskPathOpItem = MaskPathOpItem(pathHandle);
    std::shared_ptr<Path> pathPtr = std::make_shared<Path>();
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Brush brush = Brush(color);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    MaskPlayer maskPlayer = MaskPlayer(pathPtr, brush, *cmdList);
    size_t leftOpAllocatorSize = GetObject<size_t>();
    MaskPathOpItem::Playback(maskPlayer, &maskPathOpItem, leftOpAllocatorSize);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool MaskCmdListFuzzTest008(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    OpDataHandle pathHandle;
    pathHandle.size = GetObject<size_t>();
    pathHandle.offset = GetObject<uint32_t>();
    MaskPathOpItem maskPathOpItem = MaskPathOpItem(pathHandle);
    std::shared_ptr<Path> pathPtr = std::make_shared<Path>();
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Pen pen = Pen(color);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    maskPathOpItem.Playback(pathPtr, *cmdList);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool MaskCmdListFuzzTest009(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<Path> pathPtr = std::make_shared<Path>();
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Brush brush = Brush(color);
    Pen pen = Pen(color);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    MaskPlayer maskPlayer = MaskPlayer(pathPtr, brush, pen, *cmdList);
    uint32_t type = GetObject<uint32_t>() % TYPE_SIZE;
    size_t leftOpAllocatorSize = GetObject<size_t>();
    BrushHandle brushHandle;
    brushHandle.colorFilterHandle.size = GetObject<size_t>();
    brushHandle.colorSpaceHandle.size = GetObject<size_t>();
    brushHandle.shaderEffectHandle.size = GetObject<size_t>();
    brushHandle.imageFilterHandle.size = GetObject<size_t>();
    brushHandle.maskFilterHandle.size = GetObject<size_t>();
    brushHandle.isAntiAlias = GetObject<bool>();
    brushHandle.blenderEnabled = GetObject<bool>();
    brushHandle.mode = GetObject<BlendMode>();
    brushHandle.color = color;
    brushHandle.filterQuality = GetObject<Filter::FilterQuality>();
    MaskBrushOpItem maskBrushOpItem = MaskBrushOpItem(brushHandle);
    OpDataHandle pathHandle;
    pathHandle.size = GetObject<size_t>();
    pathHandle.offset = GetObject<uint32_t>();
    MaskPathOpItem maskPathOpItem = MaskPathOpItem(pathHandle);
    PenHandle penHandle;
    penHandle.width = GetObject<scalar>();
    penHandle.miterLimit = GetObject<scalar>();
    MaskPenOpItem maskPenOpItem = MaskPenOpItem(penHandle);
    std::unordered_map<uint32_t, void*> opItemMap = { { MaskOpItem::MASK_BRUSH_OPITEM, &maskBrushOpItem },
        { MaskOpItem::MASK_PATH_OPITEM, &maskPathOpItem }, { MaskOpItem::MASK_PEN_OPITEM, &maskPenOpItem } };
    if (type == MaskOpItem::OPITEM_HEAD) {
        if (dataText != nullptr) {
            delete [] dataText;
            dataText = nullptr;
        }
        return false;
    }
    auto it = opItemMap.find(type);
    maskPlayer.Playback(type, it->second, leftOpAllocatorSize);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool MaskCmdListFuzzTest010(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    BrushHandle brushHandle;
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    brushHandle.colorFilterHandle.size = GetObject<size_t>();
    brushHandle.colorSpaceHandle.size = GetObject<size_t>();
    brushHandle.shaderEffectHandle.size = GetObject<size_t>();
    brushHandle.imageFilterHandle.size = GetObject<size_t>();
    brushHandle.maskFilterHandle.size = GetObject<size_t>();
    brushHandle.isAntiAlias = GetObject<bool>();
    brushHandle.blenderEnabled = GetObject<bool>();
    brushHandle.mode = GetObject<BlendMode>();
    brushHandle.color = color;
    brushHandle.filterQuality = GetObject<Filter::FilterQuality>();
    MaskBrushOpItem maskBrushOpItem = MaskBrushOpItem(brushHandle);
    Brush brush = Brush(color);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    std::shared_ptr<CmdList> cmdList = MaskCmdList::CreateFromData(cmdListData, false);
    maskBrushOpItem.Playback(brush, *cmdList);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
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
    OHOS::Rosen::Drawing::MaskCmdListFuzzTest001(data, size);
    OHOS::Rosen::Drawing::MaskCmdListFuzzTest002(data, size);
    OHOS::Rosen::Drawing::MaskCmdListFuzzTest003(data, size);
    OHOS::Rosen::Drawing::MaskCmdListFuzzTest004(data, size);
    OHOS::Rosen::Drawing::MaskCmdListFuzzTest005(data, size);
    OHOS::Rosen::Drawing::MaskCmdListFuzzTest006(data, size);
    OHOS::Rosen::Drawing::MaskCmdListFuzzTest007(data, size);
    OHOS::Rosen::Drawing::MaskCmdListFuzzTest008(data, size);
    OHOS::Rosen::Drawing::MaskCmdListFuzzTest009(data, size);
    OHOS::Rosen::Drawing::MaskCmdListFuzzTest010(data, size);
    return 0;
}
