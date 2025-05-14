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

#include "rscanvasrendernode01_fuzzer.h"

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
constexpr size_t STR_LEN = 10;
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

bool RSCanvasRenderNode01FuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    // getdata
    Drawing::Canvas tmpCanvas;
    float alpha = GetData<float>();
    RSPaintFilterCanvas canvas(&tmpCanvas, alpha);

    Drawing::Canvas canvasChildren;
    float alphaChildren = GetData<float>();
    RSPaintFilterCanvas canvasChi(&canvasChildren, alphaChildren);

    Drawing::Canvas canvasContents;
    float alphaContents = GetData<float>();
    RSPaintFilterCanvas canvasCon(&canvasContents, alphaContents);

    Drawing::Canvas canvasTransition;
    float alphaTransition = GetData<float>();
    RSPaintFilterCanvas canvasTra(&canvasTransition, alphaTransition);

    Drawing::Canvas canvasProperty;
    float alphaProperty = GetData<float>();
    RSPaintFilterCanvas canvasPro(&canvasProperty, alphaProperty);

    NodeId id = GetData<NodeId>();
    RSCanvasRenderNode node(id);
    bool includeProperty = GetData<bool>();
    std::shared_ptr<RSNodeVisitor> visitor;
    std::shared_ptr<RSNodeVisitor> visitorQuick;
    std::shared_ptr<RSNodeVisitor> visitorProcess;

    // create drawCmdList
    size_t length = GetData<size_t>() % STR_LEN + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetData<char>();
    }
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    bool isCopy = GetData<bool>();
    static std::shared_ptr<Drawing::DrawCmdList> drawCmds = Drawing::DrawCmdList::CreateFromData(cmdListData, isCopy);
    RSModifierType type = GetData<RSModifierType>();
    // test
    node.UpdateRecording(drawCmds, type);
    node.ProcessRenderAfterChildren(canvas);
    node.ProcessRenderBeforeChildren(canvasChi);
    node.ProcessRenderContents(canvasCon);
    node.ProcessTransitionBeforeChildren(canvasTra);
    node.PropertyDrawableRender(canvasPro, includeProperty);
    node.Prepare(visitor);
    node.QuickPrepare(visitorQuick);
    node.Prepare(visitorProcess);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSCanvasRenderNode01FuzzTest(data, size);
    return 0;
}
