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

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include "render_context/render_context.h"
#include "surface_type.h"
#include "feature/capture/rs_ui_capture.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_display_node.h"
#include "ui/rs_ui_director.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_surface_extractor.h"
#include "window.h"
#include "pixel_map.h"
#include "png.h"
using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;
using namespace std;

static const string FILE_DIR_IN_THE_SANDBOX = "/data/local/";

using WriteToPngParam = struct {
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t bitDepth;
    const uint8_t *data;
};

constexpr int BITMAP_DEPTH = 8;

std::string GetPixelMapName(shared_ptr<Media::PixelMap> pixelMap)
{
    if (!pixelMap) {
        cout << "ImageUtils::GetPixelMapName error, pixelMap is null" << endl;
        return "";
    }
#ifdef IOS_PLATFORM
    std::string pixelMapStr = "_pixelMap_w" + std::to_string(pixelMap->GetWidth()) +
        "_h" + std::to_string(pixelMap->GetHeight()) +
        "_rowStride" + std::to_string(pixelMap->GetRowStride()) +
        "_total" + std::to_string(pixelMap->GetRowStride() * pixelMap->GetHeight()) +
        "_pid" + std::to_string(getpid()) +
        "_tid" + std::to_string(syscall(SYS_thread_selfid));
#else
    std::string yuvInfoStr = "";
    if (pixelMap->GetPixelFormat() == Media::PixelFormat::NV12 ||
        pixelMap->GetPixelFormat() == Media::PixelFormat::NV21) {
        Media::YUVDataInfo yuvInfo;
        pixelMap->GetImageYUVInfo(yuvInfo);
        yuvInfoStr += "_yWidth" + std::to_string(yuvInfo.yWidth) +
            "_yHeight" + std::to_string(yuvInfo.yHeight) +
            "_yStride" + std::to_string(yuvInfo.yStride) +
            "_yOffset" + std::to_string(yuvInfo.yOffset) +
            "_uvWidth" + std::to_string(yuvInfo.uvWidth) +
            "_uvHeight" + std::to_string(yuvInfo.uvHeight) +
            "_uvStride" + std::to_string(yuvInfo.uvStride) +
            "_uvOffset" + std::to_string(yuvInfo.uvOffset);
    }
    std::string pixelMapStr = "_pixelMap_w" + std::to_string(pixelMap->GetWidth()) +
        "_h" + std::to_string(pixelMap->GetHeight()) +
        "_rowStride" + std::to_string(pixelMap->GetRowStride()) +
        "_pixelFormat" + std::to_string((int32_t)pixelMap->GetPixelFormat()) +
        yuvInfoStr +
        "_total" + std::to_string(pixelMap->GetRowStride() * pixelMap->GetHeight());
#endif
    return pixelMapStr;
}

uint32_t SaveDataToFile(const std::string& fileName, const char* data, const size_t& totalSize)
{
    std::ofstream outFile(fileName, std::ofstream::out);
    if (!outFile.is_open()) {
        cout << "SaveDataToFile write error, path=" << fileName.c_str() <<endl;
        return 100;
    }
    if (data == nullptr) {
        cout << "SaveDataToFile data is nullptr" << endl;
        return 100;
    }
    outFile.write(data, totalSize);
    return SUCCESS;
}

bool WriteToPng(const string &fileName, const WriteToPngParam &param)
{
    png_structp pngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (pngStruct == nullptr) {
        cout << "png_create_write_struct error, nullptr!" << endl;
        return false;
    }
    png_infop pngInfo = png_create_info_struct(pngStruct);
    if (pngInfo == nullptr) {
        cout << "png_create_info_struct error, nullptr!" <<endl;
        png_destroy_write_struct(&pngStruct, nullptr);
        return false;
    }
    FILE *fp = fopen(fileName.c_str(), "wb");
    if (fp == nullptr) {
        cout << "open file error, nullptr!" << endl;
        png_destroy_write_struct(&pngStruct, &pngInfo);
        return false;
    }
    png_init_io(pngStruct, fp);

    // set png header
    png_set_IHDR(pngStruct, pngInfo,
        param.width, param.height,
        param.bitDepth,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE,
        PNG_FILTER_TYPE_BASE);
    png_set_packing(pngStruct); // set packing info
    png_write_info(pngStruct, pngInfo); // write to header

    for (uint32_t i = 0; i < param.height; i++) {
        png_write_row(pngStruct, param.data + (i * param.stride));
    }

    png_write_end(pngStruct, pngInfo);

    // free
    png_destroy_write_struct(&pngStruct, &pngInfo);
    int ret = fclose(fp);
    if (ret != 0) {
        cout << "close fp failed" << endl;
    }
    return true;
}

bool WriteToPngWithPixelMap(const string &fileName, Media::PixelMap &pixelMap)
{
    WriteToPngParam param;
    param.width = static_cast<uint32_t>(pixelMap.GetWidth());
    param.height = static_cast<uint32_t>(pixelMap.GetHeight());
    param.data = pixelMap.GetPixels();
    param.stride = static_cast<uint32_t>(pixelMap.GetRowStride());
    param.bitDepth = BITMAP_DEPTH;
    return WriteToPng(fileName, param);
}

int main(int argc, char* argv[])
{
    pid_t pid;
    char* end;
    if (argc <= 1) {
        cout << "please input data" << endl;
        return 0;
    }

    strtol(argv[1], &end, 10);
    if (*end != '\0') {
        cout << "Invalid input" << endl;
        return 0;
    } else {
        pid = atoi(argv[1]);
        cout << "pid = " << pid << endl;
    }

    if (argc == 2 || strcmp(argv[2], "once") == 0) {
        sleep(3);
        std::vector<PixelMapInfo> pixelMapInfoVector;
        // Get current time point
        auto start = std::chrono::high_resolution_clock::now();
        RSInterfaces::GetInstance().GetPixelMapByProcessId(pixelMapInfoVector, pid);

        // Get current time point again
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "duration is " << duration << " ms" << std::endl;

        for (uint32_t i = 0; i < pixelMapInfoVector.size(); i++) {
            if (pixelMapInfoVector[i].pixelMap == nullptr) {
                cout << "GetPixelMapByProcessId failed to get pixelmap, return nullptr!" << endl;
                continue;
            }

            cout << "rs local surface demo drawPNG" << endl;
            std::string fileName =
                FILE_DIR_IN_THE_SANDBOX + to_string(i) + GetPixelMapName(pixelMapInfoVector[i].pixelMap) + ".png";
            int ret = WriteToPngWithPixelMap(fileName, *(pixelMapInfoVector[i].pixelMap));
            if (!ret) {
                cout << "pixelmap write to png failed" << endl;
            }
            cout << "pixelmap write to png sucess " << fileName << endl;
            cout << "pixelmap x: " << pixelMapInfoVector[i].location.x << " y: " << pixelMapInfoVector[i].location.y
                 << " z: " << pixelMapInfoVector[i].location.z << " width: " << pixelMapInfoVector[i].location.width
                 << " height: " << pixelMapInfoVector[i].location.height
                 << " nodeName: " << pixelMapInfoVector[i].nodeName << endl;
        }
        if (pixelMapInfoVector.size() == 0) {
            cout << "this pid doesn't have selfdrawingnode" << endl;
        }
    } else if (strcmp(argv[2], "loop") == 0) {
        while(true) {
            sleep(10);
            std::vector<PixelMapInfo> pixelMapInfoVector;
            // Get current time point
            auto start = std::chrono::high_resolution_clock::now();
            RSInterfaces::GetInstance().GetPixelMapByProcessId(pixelMapInfoVector, pid);

            // Get current time point again
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            std::cout << "duration is " << duration << " ms" << std::endl;
            for (uint32_t i = 0; i < pixelMapInfoVector.size(); i++) {
                if (pixelMapInfoVector[i].pixelMap == nullptr) {
                    cout << "GetPixelMapByProcessId failed to get pixelmap, return nullptr!" << endl;
                    return 0;
                }
                cout << "rs local surface demo drawPNG" << endl;
                std::string fileName =
                    FILE_DIR_IN_THE_SANDBOX + to_string(i) + GetPixelMapName(pixelMapInfoVector[i].pixelMap) + ".png";
                int ret = WriteToPngWithPixelMap(fileName, *(pixelMapInfoVector[i].pixelMap));
                if (!ret) {
                    cout << "pixelmap write to png failed" << endl;
                }
                cout << "pixelmap write to png sucess " << fileName << endl;
                cout << "pixelmap x: " << pixelMapInfoVector[i].location.x << " y: " << pixelMapInfoVector[i].location.y
                     << " z: " << pixelMapInfoVector[i].location.z << " width: " << pixelMapInfoVector[i].location.width
                     << " height: " << pixelMapInfoVector[i].location.height
                     << " nodeName: " << pixelMapInfoVector[i].nodeName << endl;
            }
            if (pixelMapInfoVector.size() == 0) {
                cout << "this pid doesn't have selfdrawingnode" << endl;
            }
        }
    } else {
        cout << "invalid mode" << endl;
    }
    return 0;
}
