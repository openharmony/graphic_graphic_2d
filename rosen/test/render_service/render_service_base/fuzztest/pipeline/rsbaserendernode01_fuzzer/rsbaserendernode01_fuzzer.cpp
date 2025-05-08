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

#include "rsbaserendernode01_fuzzer.h"

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

bool RSBaseRenderNode01FuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // get data
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    NodeId id = GetData<NodeId>();
    NodeId idChild = GetData<NodeId>();
    NodeId idChildOne = GetData<NodeId>();
    NodeId idChildTwo = GetData<NodeId>();
    RSBaseRenderNode::SharedPtr child = std::make_shared<RSCanvasRenderNode>(idChild, context);
    RSBaseRenderNode::SharedPtr childOne = std::make_shared<RSCanvasRenderNode>(idChildOne, context);
    RSBaseRenderNode::SharedPtr childTwo = std::make_shared<RSCanvasRenderNode>(idChildTwo, context);

    int indexChild = GetData<int>();
    int indexChildOne = GetData<int>();
    int indexChildTwo = GetData<int>();

    bool skipTransition = GetData<bool>();
    std::vector<RSBaseRenderNode::SharedPtr> vec = { child, childOne, childTwo };
    bool isUniRender = GetData<bool>();
    int64_t timestamp = GetData<int64_t>();
    int64_t delayTime = GetData<int64_t>();
    bool flag = GetData<bool>();
    bool recursive = GetData<bool>();
    bool change = GetData<bool>();
    bool onlyFirstLevel = GetData<bool>();
    bool isContainBootAnimation = GetData<bool>();
    bool isBootAnimation = GetData<bool>();

    // test
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(id, true, context);

    // add child
    node->AddChild(child, indexChild);
    node->AddChild(childOne, indexChildOne);
    node->AddChild(childTwo, indexChildTwo);
    node->CollectSurface(child, vec, isUniRender, onlyFirstLevel);
    // move child
    node->MoveChild(child, indexChild + 1);
    // remove child
    node->RemoveChild(child, skipTransition);
    // remove fromtree
    node->RemoveFromTree(skipTransition);
    node->RemoveCrossParentChild(childOne, childTwo);
    node->RemoveUIExtensionChild(child);
    node->AddCrossParentChild(child, indexChild);
    node->SetContainBootAnimation(isContainBootAnimation);
    node->SetBootAnimation(isBootAnimation);
    node->Animate(timestamp, delayTime);
    node->HasDisappearingTransition(recursive);
    node->SetTunnelHandleChange(change);
    node->UpdateChildrenOutOfRectFlag(flag);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSBaseRenderNode01FuzzTest(data, size);
    return 0;
}