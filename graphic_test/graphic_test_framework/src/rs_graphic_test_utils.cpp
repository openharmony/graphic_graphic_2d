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

#include "png.h"
#include "rs_graphic_log.h"
#include "rs_graphic_test_utils.h"

static bool WriteToPng(const std::string &filename, const WriteToPngParam &param)
{
    if (filename.empty()) {
        LOGI("RSBaseRenderUtil::WriteToPng filename is empty");
        return false;
    }
    LOGI("RSBaseRenderUtil::WriteToPng filename = %{public}s", filename.c_str());
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
        LOGE("WriteToPng file: %s open file failed, errno: %d", filename.c_str(), errno);
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

bool WriteToPngWithPixelMap(const std::string& fileName, OHOS::Media::PixelMap& pixelMap)
{
    constexpr int bitDepth = 8;

    WriteToPngParam param;
    param.width = static_cast<uint32_t>(pixelMap.GetWidth());
    param.height = static_cast<uint32_t>(pixelMap.GetHeight());
    param.data = pixelMap.GetPixels();
    param.stride = static_cast<uint32_t>(pixelMap.GetRowBytes());
    param.bitDepth = bitDepth;
    return WriteToPng(fileName, param);
}

void WaitTimeout(int ms)
{
    auto time = std::chrono::milliseconds(ms);
    std::this_thread::sleep_for(time);
}
