/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <string>
#include <securec.h>
#include <sys/time.h>

#include "png.h"
#include "feature/capture/rs_surface_capture_task.h"
#include "transaction/rs_interfaces.h"

using namespace OHOS::Rosen;
using namespace OHOS::Media;
using namespace std;

class SurfaceCaptureFuture : public SurfaceCaptureCallback {
    public:
         SurfaceCaptureFuture() = default;
        ~SurfaceCaptureFuture() {};
        void OnSurfaceCapture(shared_ptr<PixelMap> pixelmap) override
        {
            pixelMap_ = pixelmap;
        }
        void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelMap,
            std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}
        shared_ptr<PixelMap> GetPixelMap()
        {
            return pixelMap_;
        }
    private:
        shared_ptr<PixelMap> pixelMap_ = nullptr;
};

const char *SNAPSHOT_PATH = "/data/surface_";
const char *SNAPSHOT_SUFFIX = ".png";
constexpr int MAX_TIME_STR_LEN = 40;
constexpr int YEAR_SINCE = 1900;
constexpr int BITMAP_DEPTH = 8;

string GenerateFileName()
{
    timeval tv;
    string fileName = SNAPSHOT_PATH;

    if (gettimeofday(&tv, nullptr) == 0) {
        struct tm *tmVal = localtime(&tv.tv_sec);
        if (tmVal != nullptr) {
            char timeStr[MAX_TIME_STR_LEN] = { 0 };
            snprintf_s(timeStr, sizeof(timeStr), sizeof(timeStr) - 1,
                "%04d-%02d-%02d_%02d-%02d-%02d",
                tmVal->tm_year + YEAR_SINCE, tmVal->tm_mon + 1, tmVal->tm_mday,
                tmVal->tm_hour, tmVal->tm_min, tmVal->tm_sec);
            fileName += timeStr;
        }
    }
    fileName += SNAPSHOT_SUFFIX;
    return fileName;
}

bool WriteToPng(const std::string &filename, const WriteToPngParam &param)
{
    if (filename.empty()) {
        RS_LOGI("WriteToPng filename is empty");
        return false;
    }
    RS_LOGI("WriteToPng filename = %{public}s", filename.c_str());
    png_structp pngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (pngStruct == nullptr) {
        return false;
    }
    png_infop pngInfo = png_create_info_struct(pngStruct);
    if (pngInfo == nullptr) {
        png_destroy_write_struct(&pngStruct, nullptr);
        return false;
    }

    FILE *fp = fopen(filename.c_str(), "wb");
    if (fp == nullptr) {
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
    return ret == 0;
}

bool WritePixelMapToPng(PixelMap& pixelMap, std::string filename)
{
    struct timeval now;
    int64_t nowVal = 0;
    if (gettimeofday(&now, nullptr) == 0) {
        constexpr int secToUsec = 1000 * 1000;
        nowVal =  static_cast<int64_t>(now.tv_sec) * secToUsec + static_cast<int64_t>(now.tv_usec);
    }
    if (filename.size() == 0) {
        filename = "/data/PixelMap_" + std::to_string(nowVal) + ".png";
    }

    WriteToPngParam param;
    param.width = static_cast<uint32_t>(pixelMap.GetWidth());
    param.height = static_cast<uint32_t>(pixelMap.GetHeight());
    param.data = pixelMap.GetPixels();
    param.stride = static_cast<uint32_t>(pixelMap.GetRowBytes());
    param.bitDepth = BITMAP_DEPTH;

    return WriteToPng(filename, param);
}

int main()
{
    cout << "Please input surfacenode id: ";
    uint64_t input;
    cin >> input;
    shared_ptr<SurfaceCaptureFuture> callback = make_shared<SurfaceCaptureFuture>();
    RSSurfaceCaptureConfig captureConfig;
    RSInterfaces::GetInstance().TakeSurfaceCapture(input, callback, captureConfig);
    sleep(2);
    shared_ptr<PixelMap> pixelmap = callback->GetPixelMap();
    if (pixelmap == nullptr) {
        cout << "pixelmap is nullptr" << endl;
        return 0;
    }
    string filename = GenerateFileName();
    bool ret = WritePixelMapToPng(*pixelmap, filename);
    if (ret) {
        cout << "success: snapshot surface " << input << " , write to " << filename.c_str() << endl;
    } else {
        cout << "error: snapshot surface " << input << endl;
    }
    return 0;
}