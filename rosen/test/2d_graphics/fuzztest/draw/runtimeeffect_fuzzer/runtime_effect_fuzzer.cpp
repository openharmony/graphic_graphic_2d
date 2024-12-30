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

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr int32_t MAX_SIZE = 5000;
bool RuntimeEffectFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

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

bool RuntimeShaderFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

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

bool RuntimeBlenderFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

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
    /* Run your code on data */
    OHOS::Rosen::Drawing::RuntimeEffectFuzzTest(data, size);
    OHOS::Rosen::Drawing::RuntimeShaderFuzzTest(data, size);
    OHOS::Rosen::Drawing::RuntimeBlenderFuzzTest(data, size);
    return 0;
}
