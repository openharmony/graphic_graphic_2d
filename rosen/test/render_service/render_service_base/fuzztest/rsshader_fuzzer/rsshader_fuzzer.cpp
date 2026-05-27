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

#include "rsshader_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <securec.h>

#include "effect/shader_effect.h"
#include "message_parcel.h"
#include "render/rs_shader.h"

namespace OHOS {
namespace Rosen {

namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr size_t SHADER_TYPE_SIZE = 4;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoCreateRSShader(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    auto shader1 = RSShader::CreateRSShader();
    if (shader1 == nullptr) {
        return false;
    }

    shader1->GetShaderType();
    shader1->GetDrawingShader();
    return true;
}

bool DoCreateRSShaderWithType(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    uint32_t typeValue = GetData<uint32_t>();
    RSShader::ShaderType type = static_cast<RSShader::ShaderType>(typeValue % SHADER_TYPE_SIZE);
    auto shader = RSShader::CreateRSShader(type);
    if (shader == nullptr) {
        return true;
    }

    shader->GetShaderType();
    shader->GetDrawingShader();
    return true;
}

bool DoSetDrawingShader(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    auto shader = RSShader::CreateRSShader();
    if (shader == nullptr) {
        return false;
    }

    uint32_t colorValue = GetData<uint32_t>();
    auto shaderEffect = Drawing::ShaderEffect::CreateColorShader(colorValue);
    shader->SetDrawingShader(shaderEffect);
    shader->GetDrawingShader();
    return true;
}

bool DoMakeDrawingShader(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    auto shader = RSShader::CreateRSShader();
    if (shader == nullptr) {
        return false;
    }

    float left = GetData<float>();
    float top = GetData<float>();
    float width = GetData<float>();
    float height = GetData<float>();
    RectF rect(left, top, width, height);
    float progress = GetData<float>();
    shader->MakeDrawingShader(rect, progress);
    return true;
}

bool DoMarshalling(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    auto shader = RSShader::CreateRSShader();
    if (shader == nullptr) {
        return false;
    }

    MessageParcel parcel;
    shader->Marshalling(parcel);
    return true;
}

bool DoUnmarshalling(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    auto shader = RSShader::CreateRSShader();
    if (shader == nullptr) {
        return false;
    }

    MessageParcel parcel;
    shader->Marshalling(parcel);

    bool needReset = false;
    auto newShader = RSShader::CreateRSShader();
    if (newShader != nullptr) {
        newShader->Unmarshalling(parcel, needReset);
    }
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::Rosen::DATA = data;
    OHOS::Rosen::g_size = size;
    OHOS::Rosen::g_pos = 0;
    
    OHOS::Rosen::DoCreateRSShader(data, size);
    OHOS::Rosen::DoCreateRSShaderWithType(data, size);
    OHOS::Rosen::DoSetDrawingShader(data, size);
    OHOS::Rosen::DoMakeDrawingShader(data, size);
    OHOS::Rosen::DoMarshalling(data, size);
    OHOS::Rosen::DoUnmarshalling(data, size);
    return 0;
}