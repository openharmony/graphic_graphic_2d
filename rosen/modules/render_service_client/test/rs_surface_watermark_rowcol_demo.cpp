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

#include <iostream>
#include <unistd.h>
#include <surface.h>

#include "foundation/window/window_manager/interfaces/innerkits/wm/window.h"

#include "image_source.h"

#include "pixel_map.h"
#include "common/rs_common_def.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_root_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

#define MY_LOGE(...) Println("[ERR]", "(", __FUNCTION__, ":", __LINE__, ")", ":", ##__VA_ARGS__)
template<class... Args>

void Println(Args&&... args)
{
    (cout << ... << std::forward<Args>(args)) << "\n";
}

[[maybe_unused]] shared_ptr<Media::PixelMap> DecodePicture(const string& pathName,
    const Media::AllocatorType allocatorType = Media::AllocatorType::SHARE_MEM_ALLOC)
{
    MY_LOGE("decode start: ------------ ", pathName);
    MY_LOGE("decode 1: CreateImageSource");
    uint32_t errCode = 0;
    unique_ptr<Media::ImageSource> imageSource =
        Media::ImageSource::CreateImageSource(pathName, Media::SourceOptions(), errCode);
    if (imageSource == nullptr || errCode != 0) {
        cout << "imageSource : " << (imageSource != nullptr) << ", err:" << errCode << endl;
        return nullptr;
    }

    cout << "decode 2: CreatePixelMap" << endl;
    Media::DecodeOptions decodeOpt;
    decodeOpt.allocatorType = allocatorType;
    shared_ptr<Media::PixelMap> pixelmap = imageSource->CreatePixelMap(decodeOpt, errCode);
    if (pixelmap == nullptr || errCode != 0) {
        cout << "pixelmap == nullptr, err:" << errCode << endl;
        return nullptr;
    }

    MY_LOGE("w x h: ", pixelmap->GetWidth(), "x", pixelmap->GetHeight());
    MY_LOGE("AllocatorType: ", (int)pixelmap->GetAllocatorType());
    MY_LOGE("fd: ", (!pixelmap->GetFd() ? "null" : to_string(*(int*) pixelmap->GetFd())));
    MY_LOGE("decode success: ------------");
    return pixelmap;
}

shared_ptr<Media::PixelMap> CreateDefaultPixelMap()
{
    constexpr uint32_t color[1] = { 0x6f0000ff };
    uint32_t colorLength = sizeof(color) / sizeof(color[0]);
    constexpr int32_t offset = 0;
    Media::InitializationOptions opts;
    opts.size.width = 1;
    opts.size.height = 1;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    int32_t stride = opts.size.width;
    return std::shared_ptr<Media::PixelMap>(Media::PixelMap::Create(color, colorLength, offset, stride, opts));
}

constexpr float MAX_ZORDER = 1e5f;
constexpr size_t VIEWPORT_WIDTH = 500;
constexpr size_t VIEWPORT_HEIGHT = 200;
constexpr uint32_t BACKGROUND_COLOR = 0XFFFF0000;
constexpr uint32_t SLEEP_TWO_SECOND = 2;

std::pair<vector<NodeId>, vector<std::shared_ptr<RSSurfaceNode>>> CreateRSSurfaceNodeList(
    int32_t num, ScreenId screenId, int width = VIEWPORT_WIDTH, int height = VIEWPORT_HEIGHT, int32_t interval = 0)
{
    std::pair<vector<NodeId>, vector<std::shared_ptr<RSSurfaceNode>>> nodeList;
    for (int i = 0; i < num; i++) {
        auto rsUiDirector = RSUIDirector::Create();
        if (rsUiDirector == nullptr) {
            MY_LOGE("rsUiDirector is nullptr");
            return {};
        }
        RSSurfaceNodeConfig config {
            .SurfaceNodeName = "SurfaceWatermarkRowColTest" + std::to_string(num),
            .isSync = true,
        };
        RSSurfaceNodeType type = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
        auto rsUIContext = rsUiDirector->GetRSUIContext();
        auto surfaceNode = RSSurfaceNode::Create(config, type, true, false, rsUIContext);
        if (surfaceNode == nullptr) {
            MY_LOGE("surfaceNode is nullptr");
            return {};
        }
        surfaceNode->SetPositionZ(MAX_ZORDER - num);
        surfaceNode->SetBounds({0, 0, width + interval * num, height + interval * num});
        surfaceNode->SetBackgroundColor(BACKGROUND_COLOR);
        surfaceNode->SetFrameGravity(Gravity::RESIZE_ASPECT);
        RSTransaction::FlushImplicitTransaction();

        surfaceNode->AttachToDisplay(screenId);
        RSTransaction::FlushImplicitTransaction();
        nodeList.first.push_back(surfaceNode->GetId());
        nodeList.second.push_back(surfaceNode);
        std::cout << "surfaeNodeInit:" + std::to_string(num) << std::endl;
    }
    return nodeList;
}

int main()
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    auto screenId = rsInterfaces.GetDefaultScreenId();
    if (screenId == INVALID_SCREEN_ID) {
        MY_LOGE("GetDefaultScreenId failed");
        screenId = 0;
    }

    // init watermark pixelmap
    shared_ptr<Media::PixelMap> pixelmap;
    constexpr auto isUsePicture = true;
    constexpr const char* picturePath = "/data/test.png";
    if constexpr (isUsePicture) {
        pixelmap = DecodePicture(picturePath);
    }
    if (pixelmap == nullptr) {
        pixelmap = CreateDefaultPixelMap();
    }

    // ==================== Test SetWatermark ====================
    std::cout << "========== Test SetWatermark ==========" << std::endl;
    uint32_t rowCount = 1;
    uint32_t colCount = 1;
    std::cout << "Please enter watermark row count for SetWatermark:";
    std::cin >> rowCount;
    std::cout << std::endl;
    std::cout << "Please enter watermark col count for SetWatermark:";
    std::cin >> colCount;
    std::cout << std::endl;

    string watermarkName = "RowColWatermarkTest";
    bool watermarkRes = rsInterfaces.SetWatermark(watermarkName, pixelmap,
        SaSurfaceWatermarkMaxSize::SA_WATER_MARK_DEFAULT_SIZE, rowCount, colCount);
    std::cout << "Test SetWatermark with row=" << rowCount << ", col=" << colCount
              << ", result=" << watermarkRes << std::endl;
    sleep(SLEEP_TWO_SECOND);

    // Set SetWatermark again with different row/col
    std::cout << "Please enter new watermark row count for SetWatermark:";
    std::cin >> rowCount;
    std::cout << std::endl;
    std::cout << "Please enter new watermark col count for SetWatermark:";
    std::cin >> colCount;
    std::cout << std::endl;

    watermarkRes = rsInterfaces.SetWatermark(watermarkName, pixelmap,
        SaSurfaceWatermarkMaxSize::SA_WATER_MARK_DEFAULT_SIZE, rowCount, colCount);
    std::cout << "Test SetWatermark with row=" << rowCount << ", col=" << colCount
              << ", result=" << watermarkRes << std::endl;
    sleep(SLEEP_TWO_SECOND);

    // ==================== Test SetSurfaceWatermark ====================
    std::cout << "========== Test SetSurfaceWatermark ==========" << std::endl;

    // SetNodeNum
    int32_t nodeNum = 1;
    int32_t width = VIEWPORT_WIDTH;
     
    int32_t mode = 1;
    std::cout << "If you want to set the number of nodes, please enter 1; for the default mode, enter 2.";
    std::cin >> mode;
    std::cout << std::endl;
    if (mode > 1) {
        std::cout << "Set surfaceNode num:";
        std::cin >> nodeNum;
        std::cout << std::endl;
        std::cout << "Set surface Width:";
        std::cin >> width;
        std::cout << std::endl;
        std::cout << "Set surface Height:";
        std::cin >> height;
        std::cout << std::endl;
    }

    // Create SurfaceNode
    std::pair<vector<NodeId>, vector<std::shared_ptr<RSSurfaceNode>>> nodeList =
        CreateRSSurfaceNodeList(nodeNum, screenId, width, height);

    if (nodeList.first.size() == 0) {
        return EXIT_FAILURE;
    }

    // Input row and col for surface watermark
    std::cout << "Please enter watermark row count for SetSurfaceWatermark:";
    std::cin >> rowCount;
    std::cout << std::endl;
    std::cout << "Please enter watermark col count for SetSurfaceWatermark:";
    std::cin >> colCount;
    std::cout << std::endl;

    // SET SurfaceWatermark with user-defined row and col
    string surfaceWatermarkName = "SurfaceWatermarkRowColTest";
    auto res = rsInterfaces.SetSurfaceWatermark(getpid(), surfaceWatermarkName,
        pixelmap, nodeList.first, SurfaceWatermarkType::CUSTOM_WATER_MARK, rowCount, colCount);
    std::cout << "Test SetSurfaceWatermark with row=" << rowCount << ", col=" << colCount
              << ", result=" << res << std::endl;
    sleep(SLEEP_TWO_SECOND);

    // Test clear surface watermark
    std::cout << "Clearing surface watermark..." << std::endl;
    rsInterfaces.ClearSurfaceWatermarkForNodes(getpid(), surfaceWatermarkName, nodeList.first);
    sleep(SLEEP_TWO_SECOND);

    // Test setting surface watermark again with different row/col
    std::cout << "Please enter new watermark row count for SetSurfaceWatermark:";
    std::cin >> rowCount;
    std::cout << std::endl;
    std::cout << "Please enter new watermark col count for SetSurfaceWatermark:";
    std::cin >> colCount;
    std::cout << std::endl;

    res = rsInterfaces.SetSurfaceWatermark(getpid(), surfaceWatermarkName + "New",
        pixelmap, nodeList.first, SurfaceWatermarkType::CUSTOM_WATER_MARK, rowCount, colCount);
    std::cout << "Test SetSurfaceWatermark with row=" << rowCount << ", col=" << colCount
              << ", result=" << res << std::endl;
    sleep(SLEEP_TWO_SECOND);

    return EXIT_SUCCESS;
}
