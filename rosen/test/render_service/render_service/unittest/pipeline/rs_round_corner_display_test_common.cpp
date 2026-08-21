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

#include "rs_round_corner_display_test_common.h"

namespace OHOS::Rosen {
void RSRoundCornerDisplayTest::SetUpTestCase() {}

void RSRoundCornerDisplayTest::TearDownTestCase()
{
    auto& mainThread = *RSMainThread::Instance();
    if (mainThread.renderEngine_) {
        if (mainThread.renderEngine_->renderContext_) {
            mainThread.renderEngine_->renderContext_->drGPUContext_ = nullptr;
            mainThread.renderEngine_->renderContext_ = nullptr;
        }
        if (mainThread.renderEngine_->protectedRenderContext_) {
            mainThread.renderEngine_->protectedRenderContext_->drGPUContext_ = nullptr;
        }
        mainThread.renderEngine_->protectedRenderContext_ = nullptr;
        mainThread.renderEngine_ = nullptr;
    }
    auto& rtThread = RSUniRenderThread::Instance();
    if (rtThread.uniRenderEngine_) {
        if (rtThread.uniRenderEngine_->renderContext_) {
            rtThread.uniRenderEngine_->renderContext_->drGPUContext_ = nullptr;
            rtThread.uniRenderEngine_->renderContext_ = nullptr;
        }
        if (rtThread.uniRenderEngine_->protectedRenderContext_) {
            rtThread.uniRenderEngine_->protectedRenderContext_->drGPUContext_ = nullptr;
        }
        rtThread.uniRenderEngine_->protectedRenderContext_ = nullptr;
        rtThread.uniRenderEngine_ = nullptr;
    }
}

void RSRoundCornerDisplayTest::SetUp() {}

void RSRoundCornerDisplayTest::TearDown() {}

xmlNodePtr CreateNodeWithProperty(const std::string& nodeName, const XMLProperty& property)
{
    auto xmlptr = xmlNewNode(NULL, BAD_CAST(nodeName.c_str()));
    xmlNewProp(xmlptr, BAD_CAST(property.name.c_str()), BAD_CAST(property.value.c_str()));
    return xmlptr;
}

xmlNodePtr CreateNodeWithProperties(const std::string& nodeName, const std::vector<XMLProperty>& properties)
{
    auto xmlptr = xmlNewNode(NULL, BAD_CAST(nodeName.c_str()));
    for (auto& property : properties) {
        xmlNewProp(xmlptr, BAD_CAST(property.name.c_str()), BAD_CAST(property.value.c_str()));
    }
    return xmlptr;
}

xmlNodePtr CreateRCDLayer(const std::string& nodeName, const rs_rcd::RoundCornerLayer& layer)
{
    std::vector<XMLProperty> properties = {
        {rs_rcd::ATTR_FILENAME, layer.fileName},
        {rs_rcd::ATTR_OFFSET_X, std::to_string(layer.offsetX)},
        {rs_rcd::ATTR_OFFSET_Y, std::to_string(layer.offsetY)},
        {rs_rcd::ATTR_BINFILENAME, layer.binFileName},
        {rs_rcd::ATTR_BUFFERSIZE, std::to_string(layer.bufferSize)},
        {rs_rcd::ATTR_CLDWIDTH, std::to_string(layer.cldWidth)},
        {rs_rcd::ATTR_CLDHEIGHT, std::to_string(layer.cldHeight)}
    };
    return CreateNodeWithProperties(nodeName, properties);
}

bool LoadBitmapFromFile(const char* path, Drawing::Bitmap& bitmap)
{
    if (path == nullptr) {
        return false;
    }

    std::shared_ptr<Drawing::Image> image;
    auto& rcdInstance = RSSingleton<RoundCornerDisplay>::GetInstance();
    rcdInstance.Init();
    rcdInstance.LoadImg(path, image);
    if (image == nullptr) {
        std::cout << "LoadBitmapFromFile: current os no rcd source" << std::endl;
        return false;
    }

    return rcdInstance.DecodeBitmap(image, bitmap);
}

void InitRcdRenderParamsInvalid01(HardwareLayerInfo* layerInfo, RcdExtInfo* extInfo)
{
    if (layerInfo != nullptr) {
        layerInfo->pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
        layerInfo->bufferSize = -1;  // invalid buffer size
        layerInfo->cldWidth = 1;
        layerInfo->cldHeight = 1;
    }
    if (extInfo != nullptr) {
        extInfo->srcRect_ = RectI(0, 0, 1, 1);
        extInfo->dstRect_ = RectI(0, 0, 1, 1);
    }
}

void InitRcdRenderParamsInvalid02(HardwareLayerInfo* layerInfo, RcdExtInfo* extInfo)
{
    if (layerInfo != nullptr) {
        layerInfo->pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
        layerInfo->bufferSize = 1;
        layerInfo->cldWidth = -1;  // invalid cld height
        layerInfo->cldHeight = 1;
    }
    if (extInfo != nullptr) {
        extInfo->srcRect_ = RectI(0, 0, 1, 1);
        extInfo->dstRect_ = RectI(0, 0, 1, 1);
    }
}

void InitRcdRenderParamsInvalid03(HardwareLayerInfo* layerInfo, RcdExtInfo* extInfo)
{
    if (layerInfo != nullptr) {
        layerInfo->pathBin = "";  // Invalid path
        layerInfo->bufferSize = 1;
        layerInfo->cldWidth = 1;
        layerInfo->cldHeight = 1;
    }
    if (extInfo != nullptr) {
        extInfo->srcRect_ = RectI(0, 0, 1, 1);
        extInfo->dstRect_ = RectI(0, 0, 1, 1);
    }
}

void InitRcdRenderParamsInvalid04(HardwareLayerInfo* layerInfo, RcdExtInfo* extInfo)
{
    if (layerInfo != nullptr) {
        layerInfo->pathBin = "/sys_prod/etc/display/RoundCornerDisplay/test.bin";
        layerInfo->bufferSize = 1;
        layerInfo->cldWidth = 1;
        layerInfo->cldHeight = 1;
    }
    if (extInfo != nullptr) {
        extInfo->srcRect_ = RectI(0, 0, 1, 1);
        extInfo->dstRect_ = RectI(0, 0, 1, 1);
    }
}
} // OHOS::Rosen
