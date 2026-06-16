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

#include "runtime_effect_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"
#include "recording/recording_canvas.h"

#include "effect/runtime_blender_builder.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "image/image.h"
#include "image/image_info.h"
#include "utils/matrix44.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr int32_t MAX_SIZE = 5000;
static Matrix GetFuzzMatrix()
{
    Matrix matrix;
    matrix.SetMatrix(
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>()
    );
    return matrix;
}
static Matrix44 GetFuzzMatrix44()
{
    Matrix44 matrix44;
    Matrix44::Buffer buffer;
    for (int i = 0; i < Matrix44::MATRIX44_SIZE; i++) {
        buffer[i] = GetObject<scalar>();
    }
    matrix44.SetMatrix44ColMajor(buffer);
    return matrix44;
}
bool RuntimeEffectFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* text = new char[length];
    for (size_t i = 0; i < length; i++) {
        text[i] = GetObject<char>();
    }
    text[length - 1] = '\0';
    std::shared_ptr<RuntimeEffect> runtimeEffect = RuntimeEffect::CreateForShader(std::string(text));
    runtimeEffect->GetDrawingType();
    bool isOpaque = GetObject<bool>();
    auto dataVal = std::make_shared<Data>();
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    dataVal->BuildWithoutCopy(dataText, length - 1);
    runtimeEffect->MakeShader(nullptr, nullptr, 0, nullptr, isOpaque);
    Matrix matrix;
    matrix.SetMatrix(
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>()
    );
    runtimeEffect->MakeShader(dataVal, nullptr, 0, &matrix, isOpaque);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }

    return true;
}

bool RuntimeEffectShaderWithOptionsFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* text = new char[length];
    for (size_t i = 0; i < length; i++) {
        text[i] = GetObject<char>();
    }
    text[length - 1] = '\0';
    std::string sl(text);
    RuntimeEffectOptions options;
    options.forceNoInline = GetObject<bool>();
    options.useAF = GetObject<bool>();
    options.useHighpLocalCoords = GetObject<bool>();
    options.needDrawingslToSksl = GetObject<bool>();
    std::shared_ptr<RuntimeEffect> runtimeEffect = RuntimeEffect::CreateForShader(sl, options);
    runtimeEffect->GetDrawingType();
    bool isOpaque = GetObject<bool>();
    runtimeEffect->MakeShader(nullptr, nullptr, 0, nullptr, isOpaque);
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }

    return true;
}

bool RuntimeEffectES3ShaderFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* text = new char[length];
    for (size_t i = 0; i < length; i++) {
        text[i] = GetObject<char>();
    }
    text[length - 1] = '\0';
    std::string sl(text);
    std::shared_ptr<RuntimeEffect> runtimeEffect = RuntimeEffect::CreateForES3Shader(sl);
    runtimeEffect->GetDrawingType();
    bool isOpaque = GetObject<bool>();
    runtimeEffect->MakeShader(nullptr, nullptr, 0, nullptr, isOpaque);
    if (text != nullptr) {
        delete [] text;
        text = nullptr;
    }

    return true;
}

bool RuntimeShaderFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    static constexpr char prog[] = R"(
        uniform shader imageInput;
        uniform float2 in_blurOffset;
        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            c += imageInput.eval(float2(in_blurOffset.x + xy.x, in_blurOffset.y + xy.y));
            c += imageInput.eval(float2(in_blurOffset.x + xy.x, -in_blurOffset.y + xy.y));
            c += imageInput.eval(float2(-in_blurOffset.x + xy.x, in_blurOffset.y + xy.y));
            c += imageInput.eval(float2(-in_blurOffset.x + xy.x, -in_blurOffset.y + xy.y));
            return half4(c.rgba * 0.2);
        }
    )";

    std::shared_ptr<Drawing::RuntimeEffect> runtimeshader =
        Drawing::RuntimeEffect::CreateForShader(prog);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(runtimeshader);
    Rosen::Drawing::Image image;
    auto encodeData = Drawing::Data::MakeFromFileName("mandrill_256.png");
    image.MakeFromEncoded(encodeData);

    Drawing::Matrix matrix;
    matrix.SetMatrix(
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(),
        GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>()
    );
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), matrix);

    builder->SetChild("imageShader", imageShader);

    float val1 = GetObject<float>();
    float val2 = GetObject<float>();
    builder->SetUniform("coefficient1", val1);
    builder->SetUniform("coefficient2", val2);

    bool isOpaque = GetObject<bool>();
    std::shared_ptr<Drawing::ShaderEffect> shader = builder->MakeShader(&matrix, isOpaque);

    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    Drawing::RecordingCanvas playbackCanvas_ = Drawing::RecordingCanvas(GetObject<int32_t>(), GetObject<int32_t>());
    playbackCanvas_.AttachBrush(brush);
    playbackCanvas_.DrawRect(Drawing::Rect(GetObject<float>(),
        GetObject<float>(), GetObject<float>(), GetObject<float>()));
    playbackCanvas_.DetachBrush();
    return true;
}

bool RuntimeShaderBuilderFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    static constexpr char prog[] = R"(
        uniform shader imageInput;
        uniform float2 in_blurOffset;
        uniform float3 in_coeff3;
        uniform float4 in_coeff4;
        half4 main(float2 xy) {
            half4 c = imageInput.eval(xy);
            c += imageInput.eval(float2(in_blurOffset.x + xy.x, in_blurOffset.y + xy.y));
            c += half4(in_coeff3.x, in_coeff3.y, in_coeff3.z, in_coeff4.w);
            return half4(c.rgba * 0.2);
        }
    )";

    std::shared_ptr<Drawing::RuntimeEffect> runtimeEffect = Drawing::RuntimeEffect::CreateForShader(prog);
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(runtimeEffect);

    Drawing::Matrix matrix = GetFuzzMatrix();

    bool mipmapped = GetObject<bool>();
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Drawing::ImageInfo resultInfo = Drawing::ImageInfo::MakeN32Premul(width, height);

    builder->MakeImage(nullptr, &matrix, resultInfo, mipmapped);

    Drawing::RecordingCanvas recordingCanvas(GetObject<int32_t>(), GetObject<int32_t>());
    builder->MakeImage(recordingCanvas, &matrix, resultInfo, mipmapped);

    builder->SetUniform("in_blurOffset", GetObject<float>(), GetObject<float>());
    builder->SetUniform("in_coeff3", GetObject<float>(), GetObject<float>(), GetObject<float>());
    builder->SetUniform("in_coeff4", GetObject<float>(), GetObject<float>(),
        GetObject<float>(), GetObject<float>());

    size_t arrSize = GetObject<size_t>() % MAX_SIZE + 1;
    std::vector<float> values(arrSize);
    for (size_t i = 0; i < arrSize; i++) {
        values[i] = GetObject<float>();
    }
    builder->SetUniform("in_blurOffset", values.data(), arrSize);

    Drawing::Matrix uniformMatrix33 = GetFuzzMatrix();
    builder->SetUniform("uniformMatrix33", uniformMatrix33);

    Drawing::Matrix44 uniformMatrix44 = GetFuzzMatrix44();
    builder->SetUniform("uniformMatrix44", uniformMatrix44);

    builder->SetUniformVec4("in_color", GetObject<float>(), GetObject<float>(),
        GetObject<float>(), GetObject<float>());

    return true;
}

bool RuntimeBlenderFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    static constexpr char prog[] = R"(
        uniform float dynamicLightUpRate;
        uniform float dynamicLightUpDeg;

        vec4 main(vec4 drawing_src, vec4 drawing_dst) {
            float x = 0.299 * drawing_dst.r + 0.587 * drawing_dst.g + 0.114 * drawing_dst.b;
            float y = (0 - dynamicLightUpRate) * x + dynamicLightUpDeg;
            float R = clamp((drawing_dst.r + y), 0.0, 1.0);
            float G = clamp((drawing_dst.g + y), 0.0, 1.0);
            float B = clamp((drawing_dst.b + y), 0.0, 1.0);
            return vec4(R, G, B, 1.0);
        }
    )";

    std::shared_ptr<Drawing::RuntimeEffect> runtimeshader =
        Drawing::RuntimeEffect::CreateForBlender(prog);
    std::shared_ptr<Drawing::RuntimeBlenderBuilder> builder =
        std::make_shared<Drawing::RuntimeBlenderBuilder>(runtimeshader);

    float val1 = GetObject<float>();
    float val2 = GetObject<float>();
    builder->SetUniform("dynamicLightUpRate", val1);
    builder->SetUniform("dynamicLightUpDeg", val2);

    auto blender = builder->MakeBlender();

    Rosen::Drawing::Image image;
    auto encodeData = Drawing::Data::MakeFromFileName("mandrill_256.png");
    image.MakeFromEncoded(encodeData);

    Drawing::Brush brush;
    brush.SetColor(GetObject<uint32_t>());
    brush.SetBlender(blender);
    Drawing::RecordingCanvas playbackCanvas_ = Drawing::RecordingCanvas(GetObject<int32_t>(), GetObject<int32_t>());
    playbackCanvas_.AttachBrush(brush);
    playbackCanvas_.DrawRect(Drawing::Rect(GetObject<float>(),
        GetObject<float>(), GetObject<float>(), GetObject<float>()));
    playbackCanvas_.DetachBrush();

    return true;
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
    OHOS::Rosen::Drawing::RuntimeEffectFuzzTest(data, size);
    OHOS::Rosen::Drawing::RuntimeEffectShaderWithOptionsFuzzTest(data, size);
    OHOS::Rosen::Drawing::RuntimeEffectES3ShaderFuzzTest(data, size);
    OHOS::Rosen::Drawing::RuntimeShaderFuzzTest(data, size);
    OHOS::Rosen::Drawing::RuntimeShaderBuilderFuzzTest(data, size);
    OHOS::Rosen::Drawing::RuntimeBlenderFuzzTest(data, size);
    return 0;
}
