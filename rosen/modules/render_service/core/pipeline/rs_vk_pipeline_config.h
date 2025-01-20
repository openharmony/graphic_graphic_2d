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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_RDC_CONFIG_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_RDC_CONFIG_H

#include <cstring>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <mutex>
#include <vector>

#include "platform/ohos/backend/rs_vulkan_context.h"

namespace OHOS {
namespace Rosen {
namespace RDC {

const char NODE_CHUNKS[] = "chunks";
const char CONFIG_XML_FILE[] = "/etc/vkpipeline/config.xml";
const char PATH_CONFIG_DIR[] = "etc/vkpipeline/";

struct XMLReader {
    static std::string ReadAttrStr(const xmlNodePtr& src, const std::string& attr);
    static xmlNodePtr FindChildNodeByPropName(const xmlNodePtr& src, const std::string& index);
    static std::string ReadNodeValue(const xmlNodePtr& node);
    static uint32_t ReadNodeValueInt(const xmlNodePtr& node);
    static std::string GetConfigPath(const std::string& configFileName);
};

struct VkRenderPassCreateInfoModel {
    bool ReadXmlNode(const xmlNodePtr& createInfoNodePtr);
    uint32_t resourceId;
    VkRenderPass renderPass;

private:
    void ReadAttachments(const xmlNodePtr& node, std::vector<VkAttachmentDescription>& vec);
    void ReadSubPass(const xmlNodePtr& node, std::vector<VkSubpassDescription>& vec);
    VkAttachmentReference* ReadColorReference(const xmlNodePtr& node);
    VkAttachmentReference* ReadDepthReference(const xmlNodePtr& node);
};

struct VkDescriptorSetLayoutCreateInfoModel {
    bool ReadXmlNode(const xmlNodePtr& createInfoNodePtr);
    VkDescriptorSetLayout descriptorSetLayout;
    uint32_t resourceId;
};

struct VkPipelineLayoutCreateInfoModel {
    bool ReadXmlNode(const xmlNodePtr& createInfoNodePtr);
    VkPipelineLayoutCreateInfo createInfo;
    uint32_t resourceId;
    std::vector<uint32_t> setLayoutResourceIds;
};

struct VkGraphicsPipelineCreateInfoModel {
    bool ReadXmlNode(const xmlNodePtr& createInfoNodePtr);
    VkGraphicsPipelineCreateInfo createInfo;
    uint32_t renderPassResourceId;
    uint32_t layoutResourceId;
    std::string chunkIndex;
    std::string vertexShaderFilePath;
    std::string fragShaderFilePath;

private:
    void ReadVertexInputAttributs(const xmlNodePtr& node);
    void ReadVertexInputBinding(const xmlNodePtr& node);
    void ReadVertexInputState(const xmlNodePtr& node);
    void ReadRasterizationState(const xmlNodePtr& node);
    void ReadMultiSampleState(const xmlNodePtr& node);
    void ReadColorBlendState(const xmlNodePtr& node);

    VkPipelineVertexInputStateCreateInfo vertexInputStateCI;
    VkPipelineRasterizationStateCreateInfo rasterizationStateCI;
    VkPipelineColorBlendStateCreateInfo colorBlendStateCI;
    VkPipelineMultisampleStateCreateInfo multisampleStateCI;
    std::vector<VkVertexInputBindingDescription> vertexInputBinding;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributs;
    std::vector<VkPipelineColorBlendAttachmentState> blendAttachmentState;
};

struct RDCConfig {
    RDCConfig() {}
    virtual ~RDCConfig() = default;
    bool LoadAndAnalyze(const std::string& configFile);

private:
    void LoadChunks(xmlNodePtr& chunkPtr);
    void LoadRenderPassModels(const xmlNodePtr& chunksPtr);
    void LoadDescriptorSetLayoutModels(const xmlNodePtr& chunksPtr);
    void LoadPipelineLayoutModels(const xmlNodePtr& chunksPtr);
    void LoadGraphicsPipelineModels(const xmlNodePtr& chunksPtr);
    void CreatePipelines();
    VkShaderModule LoadSpirvShader(std::string fileName);
    std::shared_ptr<VkRenderPassCreateInfoModel> GetRenderPassModelByResourceId(uint32_t resourceId);
    std::shared_ptr<VkDescriptorSetLayoutCreateInfoModel> GetDescriptorSetLayoutModelByResourceId(uint32_t resourceId);
    std::shared_ptr<VkPipelineLayoutCreateInfoModel> GetPipelineLayoutModelByResourceId(uint32_t resourceId);
    std::vector<std::shared_ptr<VkRenderPassCreateInfoModel>> renderPassModels;
    std::vector<std::shared_ptr<VkDescriptorSetLayoutCreateInfoModel>> descriptorSetLayoutModels;
    std::vector<std::shared_ptr<VkPipelineLayoutCreateInfoModel>> pipelineLayoutModels;
    std::vector<std::shared_ptr<VkGraphicsPipelineCreateInfoModel>> graphicsPipelineModels;
    void CloseXML();
    xmlDocPtr pDoc = nullptr;
    xmlNodePtr pRoot = nullptr;
    std::mutex xmlMut;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
};
} // namespace RDC
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CORE_PIPELINE_RS_RDC_CONFIG_H
