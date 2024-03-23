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
#include "test_common.h"
#include <fcntl.h>
#include "image/image.h"
#include "image_packer.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
void TestCommon::Log(std::string info)
{
    LOGE("%{public}s", info.c_str());
    std::cout << info.c_str() << std::endl;
}

int TestCommon::PackingPixmap(std::shared_ptr<OHOS::Media::PixelMap> pixmap, std::string fileName)
{
    if (pixmap == nullptr || fileName == "") {
        Log("invalied param");
        return RET_FAILED;
    }
    OHOS::Media::ImagePacker imagePacker;
    std::set<std::string> formats;
    uint32_t ret = imagePacker.GetSupportedFormats(formats);
    if (ret != 0) {
        Log("failed to GetSupportedFormats");
        return RET_FAILED;
    }

    std::string path = "/data/test/" + fileName + ".jpg";
    int32_t fileDes = open(path.c_str(), O_CREAT | O_RDWR);
    if (fileDes <= 0) {
        Log("failed to open");
        return RET_FAILED;
    }

    OHOS::Media::PackOption option;
    option.format = "image/jpeg";
    uint32_t result = imagePacker.StartPacking(fileDes, option);
    if (result != 0) {
        Log("failed to StartPacking");
        close(fileDes);
        return RET_FAILED;
    }
    result = imagePacker.AddImage(*pixmap);
    if (result != 0) {
        Log("failed to AddImage");
        close(fileDes);
        return RET_FAILED;
    }
    int64_t packedSize = 0;
    result = imagePacker.FinalizePacking(packedSize);
    if (result != 0) {
        Log("failed to FinalizePacking");
        close(fileDes);
        return RET_FAILED;
    }

    close(fileDes);
    return RET_OK;
}
std::shared_ptr<Drawing::ColorSpace> TestCommon::ColorSpaceToDrawingColorSpace(OHOS::Media::ColorSpace colorSpace)
{
    switch (colorSpace) {
        case OHOS::Media::ColorSpace::DISPLAY_P3:
            return Drawing::ColorSpace::CreateRGB(
                Drawing::CMSTransferFuncType::SRGB, Drawing::CMSMatrixType::DCIP3);
        case OHOS::Media::ColorSpace::LINEAR_SRGB:
            return Drawing::ColorSpace::CreateSRGBLinear();
        case OHOS::Media::ColorSpace::SRGB:
        default:
            return Drawing::ColorSpace::CreateSRGB();
    }
}
Drawing::ColorType TestCommon::PixelFormatToDrawingColorType(OHOS::Media::PixelFormat pixelFormat)
{
    switch (pixelFormat) {
        case OHOS::Media::PixelFormat::RGB_565:
            return Drawing::ColorType::COLORTYPE_RGB_565;
        case OHOS::Media::PixelFormat::RGBA_8888:
            return Drawing::ColorType::COLORTYPE_RGBA_8888;
        case OHOS::Media::PixelFormat::BGRA_8888:
            return Drawing::ColorType::COLORTYPE_BGRA_8888;
        case OHOS::Media::PixelFormat::ALPHA_8:
            return Drawing::ColorType::COLORTYPE_ALPHA_8;
        case OHOS::Media::PixelFormat::RGBA_F16:
            return Drawing::ColorType::COLORTYPE_RGBA_F16;
        case OHOS::Media::PixelFormat::UNKNOWN:
        case OHOS::Media::PixelFormat::ARGB_8888:
        case OHOS::Media::PixelFormat::RGB_888:
        case OHOS::Media::PixelFormat::NV21:
        case OHOS::Media::PixelFormat::NV12:
        case OHOS::Media::PixelFormat::CMYK:
        default:
            return Drawing::ColorType::COLORTYPE_UNKNOWN;
    }
}
Drawing::AlphaType TestCommon::AlphaTypeToDrawingAlphaType(OHOS::Media::AlphaType alphaType)
{
    switch (alphaType) {
        case OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
        case OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE:
            return Drawing::AlphaType::ALPHATYPE_OPAQUE;
        case OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL:
            return Drawing::AlphaType::ALPHATYPE_PREMUL;
        case OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL:
            return Drawing::AlphaType::ALPHATYPE_UNPREMUL;
        default:
            return Drawing::AlphaType::ALPHATYPE_UNKNOWN;
    }
}
} // namespace Rosen
} // namespace OHOS