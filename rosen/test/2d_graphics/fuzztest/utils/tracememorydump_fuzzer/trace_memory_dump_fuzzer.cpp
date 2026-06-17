/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "trace_memory_dump_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "get_object.h"
#include "skia_adapter/skia_canvas.h"
#include "utils/memory_stream.h"
#include "image/trace_memory_dump.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t MATH_ONE = 1;
constexpr size_t ARRAY_MAX_SIZE = 5000;
} // namespace

namespace Drawing {
void TraceMemoryDumpFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    uint32_t strSize = GetObject<uint32_t>() % ARRAY_MAX_SIZE + MATH_ONE;
    std::vector<char> dumpName(strSize);
    std::vector<char> categoryKey(strSize);
    std::vector<char> valueName(strSize);
    std::vector<char> units(strSize);
    for (size_t i = 0; i < strSize; i++) {
        dumpName[i] = GetObject<char>();
        categoryKey[i] = GetObject<char>();
        valueName[i] = GetObject<char>();
        units[i] = GetObject<char>();
    }
    categoryKey[strSize - MATH_ONE] = '\0';
    dumpName[strSize - MATH_ONE] = '\0';
    valueName[strSize - MATH_ONE] = '\0';
    units[strSize - MATH_ONE] = '\0';
    bool itemizeType = GetObject<bool>();
    TraceMemoryDump dump(categoryKey.data(), itemizeType);
    dump.DumpNumericValue(dumpName.data(), valueName.data(), units.data(), GetObject<uint64_t>());
    dump.DumpStringValue(dumpName.data(), valueName.data(), units.data());
    OHOS::Rosen::DfxString log;
    dump.LogOutput(log);
    dump.LogTotals(log);
    // DDGRGPUContext::DumpGpuStats
    std::unique_ptr<GPUContext> context = std::make_unique<GPUContext>();
    std::string str("hello");
    context->DumpGpuStats(str);
    // DDGRGPUContext::DumpMemoryStatistics
    TraceMemoryDump traceMemoryDump("category", true);
    traceMemoryDump.DumpNumericValue(dumpName.data(), valueName.data(), units.data(), GetObject<uint64_t>());
    traceMemoryDump.DumpStringValue(dumpName.data(), valueName.data(), units.data());
    traceMemoryDump.LogOutput(log);
    traceMemoryDump.LogTotals(log);
    context->DumpMemoryStatistics(&traceMemoryDump);
    // DDGRGPUContext::DumpMemoryStatisticsByTag
    GPUResourceTag tag(0, 0, 0, 0, "tag");
    context->DumpMemoryStatisticsByTag(&traceMemoryDump, tag);
}

void TraceMemoryDumpFuzzTest001(const uint8_t* data, size_t size)
{
    // DDGRBuilder::Init
    BuilderFlags flags = BuilderFlags::HAS_COLORS_BUILDER_FLAG;
    int vertCounts = GetObject<uint32_t>() % ARRAY_MAX_SIZE + MATH_ONE;
    int indexCount = GetObject<uint32_t>() % ARRAY_MAX_SIZE + MATH_ONE;
    Vertices::Builder builder(VertexMode::TRIANGLEFAN_VERTEXMODE, vertCounts, indexCount, static_cast<uint32_t>(flags));
    // MakeCopy
    std::unique_ptr<Point[]> points = std::make_unique<Point[]>(vertCounts);
    std::unique_ptr<Point[]> texs = std::make_unique<Point[]>(vertCounts);
    std::unique_ptr<ColorQuad[]> colors = std::make_unique<ColorQuad[]>(vertCounts);
    for (int i = 0; i < vertCounts; i++) {
        points[i] = {GetObject<scalar>(), GetObject<scalar>()};
        texs[i] = {GetObject<scalar>(), GetObject<scalar>()};
        colors[i] = GetObject<ColorQuad>();
    }
    auto ver = builder.Detach();
    ver->MakeCopy(VertexMode::TRIANGLEFAN_VERTEXMODE, vertCounts, points.get(), texs.get(), colors.get());
    std::unique_ptr<uint16_t[]> indices = std::make_unique<uint16_t[]>(indexCount);
    for (int i = 0; i < indexCount; i++) {
        indices[i] = GetObject<uint16_t>();
    }
    ver->MakeCopy(VertexMode::TRIANGLEFAN_VERTEXMODE, vertCounts, points.get(), texs.get(), colors.get(),
        indexCount, indices.get());
    // Deserialize
    std::shared_ptr<Data> dataPtr = std::make_shared<Data>();
    ver->Deserialize(dataPtr);

    // DDGRData::BuildFromOHNativeBuffer
    OH_NativeBuffer *NativeBuffer = nullptr;
    size_t pandaSize = GetObject<size_t>();
    std::shared_ptr<Data> imageData = std::make_shared<Data>();
    imageData->BuildFromOHNativeBuffer(NativeBuffer, pandaSize);
    // DDGRData::BuildUninitialized
    Bitmap bmp;
    int width = GetObject<int>() % ARRAY_MAX_SIZE;
    int height = GetObject<int>() % ARRAY_MAX_SIZE;
    BitmapFormat format {COLORTYPE_RGBA_8888, ALPHATYPE_OPAQUE};
    bmp.Build(width, height, format); // bitmap width and height
    bmp.ClearWithColor(GetObject<ColorQuad>());
    Image image;
    image.BuildFromBitmap(bmp);
    size_t rowBytes = image.GetImageInfo().GetBytesPerPixel() * image.GetWidth();
    dataPtr->BuildUninitialized(rowBytes * image.GetHeight());
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // initialize
    OHOS::Rosen::Drawing::g_data = data;
    OHOS::Rosen::Drawing::g_size = size;
    OHOS::Rosen::Drawing::g_pos = 0;

    /* Run your code on data */
    OHOS::Rosen::Drawing::TraceMemoryDumpFuzzTest000(data, size);
    OHOS::Rosen::Drawing::TraceMemoryDumpFuzzTest001(data, size);
    return 0;
}