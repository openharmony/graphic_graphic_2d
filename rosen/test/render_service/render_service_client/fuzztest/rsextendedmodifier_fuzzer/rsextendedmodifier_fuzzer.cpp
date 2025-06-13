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

#include "rsextendedmodifier_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <securec.h>

#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_render_modifier.h"
#include "modifier/rs_modifier_type.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_recording_canvas.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool DoCreateDrawingContext(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::weak_ptr<RSCanvasNode> node = RSCanvasNode::Create(false, false);
    auto ctx = RSExtendedModifierHelper::CreateDrawingContext(node);
    delete ctx.canvas;
    ctx.canvas = nullptr;
    return true;
}

bool DoCreateRenderModifier(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::weak_ptr<RSCanvasNode> node = RSCanvasNode::Create(false, false);
    auto ctx = RSExtendedModifierHelper::CreateDrawingContext(node);
    PropertyId propertyId = GetData<PropertyId>();
    RSModifierType type = GetData<RSModifierType>();
    int16_t index = GetData<int16_t>();
    RSExtendedModifierHelper::CreateRenderModifier(ctx, propertyId, type, index);
    return true;
}

bool DoFinishDrawing(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // test
    std::weak_ptr<RSCanvasNode> node = RSCanvasNode::Create(false, false);
    auto ctx = RSExtendedModifierHelper::CreateDrawingContext(node);
    RSExtendedModifierHelper::FinishDrawing(ctx);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoCreateDrawingContext(data, size);
    OHOS::Rosen::DoCreateRenderModifier(data, size);
    OHOS::Rosen::DoFinishDrawing(data, size);
    return 0;
}

