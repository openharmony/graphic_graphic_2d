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

#include "vertices_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "utils/vertices.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t ARRAY_MAX_SIZE = 5000;
constexpr size_t MATH_THREE = 3;
constexpr size_t MATH_TWO = 2;
} // namespace

namespace Drawing {
void VerticesFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t mode = GetObject<uint32_t>();
    uint32_t arr_size = GetObject<uint32_t>() % ARRAY_MAX_SIZE;
    scalar xRad = GetObject<scalar>();
    scalar yRad = GetObject<scalar>();

    Vertices ver;
    Vertices ver1 = Vertices(nullptr);
    Point* positions = new Point[arr_size];
    Point* texs = new Point[arr_size];
    ColorQuad* colors = new ColorQuad[arr_size];
    uint16_t* indices = new uint16_t[arr_size];
    for (size_t i = 0; i < arr_size; i++) {
        positions[i].Set(xRad, yRad);
        texs[i].Set(xRad, yRad);
        colors[i] = GetObject<ColorQuad>();
        indices[i] = GetObject<uint16_t>();
    }
    ver.MakeCopy(static_cast<VertexMode>(mode % MATH_THREE), MATH_TWO, positions, texs, colors, arr_size, indices);
    for (size_t i = 0; i < arr_size; i++) {
        positions[i].Set(xRad, yRad);
        texs[i].Set(xRad, yRad);
        colors[i] = GetObject<ColorQuad>();
        indices[i] = GetObject<uint16_t>();
    }
    ver1.MakeCopy(static_cast<VertexMode>(mode % MATH_THREE), MATH_TWO, positions, texs, colors, arr_size, indices);
    for (size_t i = 0; i < arr_size; i++) {
        positions[i].Set(xRad, yRad);
        texs[i].Set(xRad, yRad);
        colors[i] = GetObject<ColorQuad>();
    }
    ver.MakeCopy(static_cast<VertexMode>(mode % MATH_THREE), MATH_TWO, positions, texs, colors);
    for (size_t i = 0; i < arr_size; i++) {
        positions[i].Set(xRad, yRad);
        texs[i].Set(xRad, yRad);
        colors[i] = GetObject<ColorQuad>();
    }
    ver1.MakeCopy(static_cast<VertexMode>(mode % MATH_THREE), MATH_TWO, positions, texs, colors);
    std::shared_ptr<Data> data1 = ver.Serialize();
    ver.Deserialize(data1);
    if (positions != nullptr) {
        delete [] positions;
        positions = nullptr;
    }
    if (texs != nullptr) {
        delete [] texs;
        texs = nullptr;
    }
    if (colors != nullptr) {
        delete [] colors;
        colors = nullptr;
    }
    if (indices != nullptr) {
        delete [] indices;
        indices = nullptr;
    }
}

void VerticesFuzzTest001(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    int vertexCount = GetObject<int>() % ARRAY_MAX_SIZE;
    int indexCount = GetObject<int>() % ARRAY_MAX_SIZE;
    uint32_t flags = GetObject<uint32_t>() % ARRAY_MAX_SIZE;
    uint32_t mode = GetObject<uint32_t>();

    Vertices::Builder builder = Vertices::Builder(static_cast<VertexMode>(mode % MATH_THREE), vertexCount, indexCount,
        flags);
    builder.IsValid();
    builder.Positions();
    builder.Indices();
    builder.TexCoords();
    builder.Colors();
    builder.Detach();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::VerticesFuzzTest000(data, size);
    OHOS::Rosen::Drawing::VerticesFuzzTest001(data, size);

    return 0;
}