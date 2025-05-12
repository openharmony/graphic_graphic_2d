/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rscrossrendernode01_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_occlusion_config.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/sk_resource_manager.h"

namespace OHOS {
namespace Rosen {

namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(DATA) which size is according to sizeof(T)
 * tips: only support basic type
 */
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

/*
 * get a string from DATA
 */
std::string GetStringFromData(int strlen)
{
    if (strlen <= 0) {
        return "fuzz";
    }
    char cstr[strlen];
    cstr[strlen - 1] = '\0';
    for (int i = 0; i < strlen - 1; i++) {
        char tmp = GetData<char>();
        if (tmp == '\0') {
            tmp = '1';
        }
        cstr[i] = tmp;
    }
    std::string str(cstr);
    return str;
}

struct DrawBuffer {
    int32_t w;
    int32_t h;
};

bool RSCrossRenderNode01FuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    NodeId id = GetData<NodeId>();
    uint64_t screenId = GetData<uint64_t>();
    bool isMirrored = GetData<bool>();
    NodeId mirrorNodeId = GetData<NodeId>();
    bool isSync = GetData<bool>();
    RSDisplayNodeConfig config = { screenId, isMirrored, mirrorNodeId, isSync };
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    RSDisplayRenderNode displayNode(id, config, context);

    NodeId childId = GetData<NodeId>();
    NodeId cloneNodeId = GetData<NodeId>();
    int index = GetData<int>();
    RSSurfaceRenderNode::SharedPtr child = std::make_shared<RSSurfaceRenderNode>(childId, context);
    displayNode.AddCrossScreenChild(child, cloneNodeId, index);
    displayNode.RemoveCrossScreenChild(child);

    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSCrossRenderNode01FuzzTest(data, size);
    return 0;
}