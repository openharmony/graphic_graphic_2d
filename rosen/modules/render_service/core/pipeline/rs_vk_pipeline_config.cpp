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

#include "rs_vk_pipeline_config.h"

#include <cstdint>
#include <fstream>
#include <memory>

#include "config_policy_utils.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace RDC {
static const std::streamsize SHADER_MAX_SIZE = 102400; // 100K
static constexpr int DECIMAL_BASE = 10;

xmlNodePtr XMLReader::FindChildNodeByPropName(const xmlNodePtr& src, const std::string& index)
{
    if (src == nullptr) {
        RS_LOGE(
            "RDC [%{public}s] can not found node by prop name %{public}s, src is null! \n", __func__, index.c_str());
        return nullptr;
    }
    xmlNodePtr startPtr = src->children;
    while (startPtr != nullptr) {
        if (XMLReader::ReadAttrStr(startPtr, std::string("name")) == index) {
            return startPtr;
        }
        startPtr = startPtr->next;
    }
    RS_LOGE("RDC [%{public}s] can not found node by prop name %{public}s! \n", __func__, index.c_str());
    return nullptr;
}

std::string XMLReader::ReadNodeValue(const xmlNodePtr& node)
{
    if (node == nullptr) {
        RS_LOGE("RDC read xml node error, node is null");
        return "";
    }
    xmlChar* content = xmlNodeGetContent(node);
    if (content == nullptr) {
        RS_LOGE("RDC read xml node error, content is null");
        return "";
    }

    std::string contentStr = reinterpret_cast<const char*>(content);
    xmlFree(content);

    return contentStr;
}

uint32_t XMLReader::ReadNodeValueInt(const xmlNodePtr& node)
{
    std::string nodeValueStr = ReadNodeValue(node);
    if (nodeValueStr.empty()) {
        RS_LOGE("RDC read xml node error, read node value is empty");
        return 0;
    }
    char* end;
    unsigned long long value = std::strtoull(nodeValueStr.c_str(), &end, DECIMAL_BASE);
    if (*end != '\0') {
        RS_LOGE("RDC read xml node error, read node value is not end");
        return 0;
    }
    if (value > UINT32_MAX) {
        RS_LOGE("RDC read xml node error, read node value is overflow");
        return 0;
    }
    return static_cast<uint32_t>(value);
}

std::string XMLReader::ReadAttrStr(const xmlNodePtr& src, const std::string& attr)
{
    auto result = reinterpret_cast<char*>((xmlGetProp(src, BAD_CAST(attr.c_str()))));
    if (result == nullptr) {
        RS_LOGE("RDC [%{public}s] can not found attribute %{public}s! \n", __func__, attr.c_str());
        return std::string("");
    }
    std::string str(result);
    xmlFree(result);
    return str;
}

std::string XMLReader::GetConfigPath(const std::string& configFileName)
{
    char buf[PATH_MAX];
    char* configPath = GetOneCfgFile(configFileName.c_str(), buf, PATH_MAX);
    char tmpPath[PATH_MAX] = { 0 };
    if (!configPath || strlen(configPath) == 0 || strlen(configPath) >= PATH_MAX || !realpath(configPath, tmpPath)) {
        RS_LOGI("RDC can not get customization config file");
        return "/system/" + configFileName;
    }
    return std::string(tmpPath);
}

std::shared_ptr<VkRenderPassCreateInfoModel> RDCConfig::GetRenderPassModelByResourceId(uint32_t resourceId)
{
    auto iter = std::find_if(renderPassModels.begin(), renderPassModels.end(),
        [&resourceId](auto model) { return model != nullptr && model->resourceId == resourceId; });
    if (iter == renderPassModels.end()) {
        RS_LOGE("RDC [%{public}s] can not found resourceId:%{public}d! \n", __func__, resourceId);
        return nullptr;
    }
    return *iter;
}

std::shared_ptr<VkDescriptorSetLayoutCreateInfoModel> RDCConfig::GetDescriptorSetLayoutModelByResourceId(
    uint32_t resourceId)
{
    auto iter = std::find_if(descriptorSetLayoutModels.begin(), descriptorSetLayoutModels.end(),
        [&resourceId](auto model) { return model != nullptr && model->resourceId == resourceId; });
    if (iter == descriptorSetLayoutModels.end()) {
        RS_LOGE("RDC [%{public}s] can not found resourceId:%{public}d! \n", __func__, resourceId);
        return nullptr;
    }
    return *iter;
}

std::shared_ptr<VkPipelineLayoutCreateInfoModel> RDCConfig::GetPipelineLayoutModelByResourceId(uint32_t resourceId)
{
    auto iter = std::find_if(pipelineLayoutModels.begin(), pipelineLayoutModels.end(),
        [&resourceId](auto model) { return model != nullptr && model->resourceId == resourceId; });
    if (iter == pipelineLayoutModels.end()) {
        RS_LOGE("RDC [%{public}s] can not found resourceId:%{public}d! \n", __func__, resourceId);
        return nullptr;
    }
    return *iter;
}

void RDCConfig::LoadRenderPassModels(const xmlNodePtr& chunkPtr)
{
    auto chunIndexStr = XMLReader::ReadAttrStr(chunkPtr, std::string("index"));
    RS_LOGI("RDC Begin read one vkCreateRenderPass, index!:[%{public}s]} \n", chunIndexStr.c_str());

    auto createInfo = XMLReader::FindChildNodeByPropName(chunkPtr, std::string("CreateInfo"));
    if (!createInfo) {
        RS_LOGE("RDC read createInfo fail, createInfo is null! \n");
        return;
    }
    std::shared_ptr<VkRenderPassCreateInfoModel> createInfoModel = std::make_shared<VkRenderPassCreateInfoModel>();
    if (createInfoModel->ReadXmlNode(createInfo)) {
        renderPassModels.push_back(createInfoModel);
        auto renderPassPtr = XMLReader::FindChildNodeByPropName(chunkPtr, std::string("RenderPass"));
        if (renderPassPtr == nullptr) {
            RS_LOGE("RDC [%{public}s] input renderPassPtr node is null! \n", __func__);
        } else {
            createInfoModel->resourceId = XMLReader::ReadNodeValueInt(renderPassPtr);
        }
    } else {
        RS_LOGE("RDC read createInfoModel fail! \n");
    }
}

void RDCConfig::LoadDescriptorSetLayoutModels(const xmlNodePtr& chunkPtr)
{
    auto chunIndexStr = XMLReader::ReadAttrStr(chunkPtr, std::string("index"));
    RS_LOGI("RDC Begin read one vkCreateDescriptorSetLayout, index!:[%{public}s]} \n", chunIndexStr.c_str());

    auto createInfo = XMLReader::FindChildNodeByPropName(chunkPtr, std::string("CreateInfo"));
    if (!createInfo) {
        RS_LOGE("RDC read createInfo fail, createInfo is null! \n");
        return;
    }
    std::shared_ptr<VkDescriptorSetLayoutCreateInfoModel> createInfoModel =
        std::make_shared<VkDescriptorSetLayoutCreateInfoModel>();
    if (createInfoModel->ReadXmlNode(createInfo)) {
        descriptorSetLayoutModels.push_back(createInfoModel);
        auto setLayoutPtr = XMLReader::FindChildNodeByPropName(chunkPtr, std::string("SetLayout"));
        if (setLayoutPtr == nullptr) {
            RS_LOGE("RDC [%{public}s] input setLayoutPtr node is null! \n", __func__);
        } else {
            createInfoModel->resourceId = XMLReader::ReadNodeValueInt(setLayoutPtr);
        }
    } else {
        RS_LOGE("RDC read createInfoModel fail! \n");
    }
}

void RDCConfig::LoadPipelineLayoutModels(const xmlNodePtr& chunkPtr)
{
    auto chunIndexStr = XMLReader::ReadAttrStr(chunkPtr, std::string("index"));
    RS_LOGI("RDC Begin read one vkCreatePipelineLayout, index!:[%{public}s]} \n", chunIndexStr.c_str());

    auto createInfo = XMLReader::FindChildNodeByPropName(chunkPtr, std::string("CreateInfo"));
    if (!createInfo) {
        RS_LOGE("RDC read createInfo fail, createInfo is null! \n");
        return;
    }
    std::shared_ptr<VkPipelineLayoutCreateInfoModel> createInfoModel =
        std::make_shared<VkPipelineLayoutCreateInfoModel>();
    if (createInfoModel->ReadXmlNode(createInfo)) {
        pipelineLayoutModels.push_back(createInfoModel);
        auto pipelineLayoutPtr = XMLReader::FindChildNodeByPropName(chunkPtr, std::string("PipelineLayout"));
        if (pipelineLayoutPtr == nullptr) {
            RS_LOGE("RDC [%{public}s] input pipelineLayoutPtr node is null! \n", __func__);
        } else {
            createInfoModel->resourceId = XMLReader::ReadNodeValueInt(pipelineLayoutPtr);
        }
    } else {
        RS_LOGE("RDC read createInfoModel fail! \n");
    }
}

void RDCConfig::LoadGraphicsPipelineModels(const xmlNodePtr& chunkPtr)
{
    auto chunIndexStr = XMLReader::ReadAttrStr(chunkPtr, std::string("index"));
    RS_LOGI("RDC Begin read one vkCreateGraphicsPipelines, index!:[%{public}s]} \n", chunIndexStr.c_str());

    auto createInfo = XMLReader::FindChildNodeByPropName(chunkPtr, std::string("CreateInfo"));
    if (!createInfo) {
        RS_LOGE("RDC read createInfo fail, createInfo is null! \n");
        return;
    }
    std::shared_ptr<VkGraphicsPipelineCreateInfoModel> createInfoModel =
        std::make_shared<VkGraphicsPipelineCreateInfoModel>();
    if (createInfoModel->ReadXmlNode(createInfo)) {
        createInfoModel->chunkIndex = chunIndexStr;
        graphicsPipelineModels.push_back(createInfoModel);
    } else {
        RS_LOGE("RDC read createInfoModel fail! \n");
    }
}

bool RDCConfig::LoadAndAnalyze(const std::string& configFile)
{
    RS_TRACE_FUNC();
    if (OHOS::Rosen::RsVulkanContext::GetSingleton().GetRsVulkanInterface().GetDevice() == VK_NULL_HANDLE) {
        RS_LOGE("RDC LoadAndAnalyze failed device is null. \n");
        return false;
    }
    auto configFilePath = XMLReader::GetConfigPath(configFile);
    std::lock_guard<std::mutex> lock(xmlMut);
    xmlKeepBlanksDefault(0);
    pDoc = xmlReadFile(configFilePath.c_str(), "", XML_PARSE_RECOVER);
    if (pDoc == nullptr) {
        RS_LOGE("RDC read xml failed \n");
        CloseXML();
        return false;
    }
    pRoot = xmlDocGetRootElement(pDoc);
    if (pRoot == nullptr) {
        RS_LOGE("RDC get xml root failed \n");
        CloseXML();
        return false;
    }
    xmlNodePtr chunksPtr = pRoot->children;
    while (chunksPtr != nullptr) {
        auto name = chunksPtr->name;
        if (xmlStrEqual(name, BAD_CAST(NODE_CHUNKS)) == 1) {
            break;
        }
        chunksPtr = chunksPtr->next;
    }
    if (chunksPtr == nullptr) {
        RS_LOGE("RDC get xml chunksPtr failed \n");
        CloseXML();
        return false;
    }
    LoadChunks(chunksPtr->children);
    CreatePipelines();
    CloseXML();
    RS_LOGI("RDC read xml finish");
    return true;
}

void RDCConfig::LoadChunks(xmlNodePtr& chunkPtr)
{
    while (chunkPtr != nullptr) {
        if (XMLReader::ReadAttrStr(chunkPtr, std::string("name")) == "vkCreateRenderPass") {
            LoadRenderPassModels(chunkPtr);
        } else if (XMLReader::ReadAttrStr(chunkPtr, std::string("name")) == "vkCreateDescriptorSetLayout") {
            LoadDescriptorSetLayoutModels(chunkPtr);
        } else if (XMLReader::ReadAttrStr(chunkPtr, std::string("name")) == "vkCreatePipelineLayout") {
            LoadPipelineLayoutModels(chunkPtr);
        } else if (XMLReader::ReadAttrStr(chunkPtr, std::string("name")) == "vkCreateGraphicsPipelines") {
            LoadGraphicsPipelineModels(chunkPtr);
        }
        chunkPtr = chunkPtr->next;
    }
}

void RDCConfig::CreatePipelines()
{
    RS_TRACE_FUNC();
    for (auto pipelineModel : graphicsPipelineModels) {
        auto renderPassModel = GetRenderPassModelByResourceId(pipelineModel->renderPassResourceId);
        if (renderPassModel == nullptr) {
            continue;
        }
        pipelineModel->createInfo.renderPass = renderPassModel->renderPass;
        auto layoutModel = GetPipelineLayoutModelByResourceId(pipelineModel->layoutResourceId);
        if (layoutModel == nullptr) {
            continue;
        }
        std::vector<VkDescriptorSetLayout> descriptorSetLayout(layoutModel->setLayoutResourceIds.size());
        for (size_t i = 0; i < layoutModel->setLayoutResourceIds.size(); ++i) {
            auto model = GetDescriptorSetLayoutModelByResourceId(layoutModel->setLayoutResourceIds[i]);
            if (model == nullptr) {
                RS_LOGE("RDC [%{public}s] input model is null! \n", __func__);
                continue;
            }
            descriptorSetLayout[i] = model->descriptorSetLayout;
        }
        layoutModel->createInfo.pSetLayouts = descriptorSetLayout.data();
        auto& vkContext = OHOS::Rosen::RsVulkanContext::GetSingleton().GetRsVulkanInterface();
        VkDevice device = vkContext.GetDevice();
        auto result = vkCreatePipelineLayout(device, &(layoutModel->createInfo), nullptr, &pipelineLayout);
        if (result != VK_SUCCESS) {
            RS_LOGE("RDC [%{public}s] vkCreatePipelineLayout fail, result: %{public}d", __func__, result);
            continue;
        }
        pipelineModel->createInfo.layout = pipelineLayout;
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages(pipelineModel->createInfo.stageCount);
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = LoadSpirvShader(pipelineModel->vertexShaderFilePath);
        shaderStages[0].pName = "main";
        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = LoadSpirvShader(pipelineModel->fragShaderFilePath);
        shaderStages[1].pName = "main";
        pipelineModel->createInfo.pStages = shaderStages.data();
        result = vkCreateGraphicsPipelines(device, nullptr, 1, &(pipelineModel->createInfo), nullptr, &pipeline);

        vkDestroyShaderModule(device, shaderStages[0].module, nullptr);
        vkDestroyShaderModule(device, shaderStages[1].module, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        vkDestroyPipeline(device, pipeline, nullptr);
        if (result != VK_SUCCESS) {
            RS_LOGE("RDC CreatePipelines fail, result: %{public}d", result);
        }
    }
}

void VkRenderPassCreateInfoModel::ReadAttachments(const xmlNodePtr& node, std::vector<VkAttachmentDescription>& vec)
{
    auto pAttachmentsPtr = XMLReader::FindChildNodeByPropName(node, std::string("pAttachments"));
    if (pAttachmentsPtr == nullptr) {
        RS_LOGE("RDC [%{public}s] pAttachmentsPtr is nullptr.", __func__);
        return;
    }
    auto VkAttachmentDescriptionPtr = pAttachmentsPtr->children;
    for (size_t i = 0; i < vec.size(); i++) {
        if (VkAttachmentDescriptionPtr == nullptr) {
            RS_LOGE("RDC [%{public}s] VkAttachmentDescriptionPtr is nullptr.", __func__);
            return;
        }
        auto flagsPtr = XMLReader::FindChildNodeByPropName(VkAttachmentDescriptionPtr, std::string("flags"));
        vec[i].flags = static_cast<VkAttachmentDescriptionFlags>(XMLReader::ReadNodeValueInt(flagsPtr));

        auto formatPtr = XMLReader::FindChildNodeByPropName(VkAttachmentDescriptionPtr, std::string("format"));
        vec[i].format = static_cast<VkFormat>(XMLReader::ReadNodeValueInt(formatPtr));

        auto samplesPtr = XMLReader::FindChildNodeByPropName(VkAttachmentDescriptionPtr, std::string("samples"));
        vec[i].samples = static_cast<VkSampleCountFlagBits>(XMLReader::ReadNodeValueInt(samplesPtr));

        auto loadOpPtr = XMLReader::FindChildNodeByPropName(VkAttachmentDescriptionPtr, std::string("loadOp"));
        vec[i].loadOp = static_cast<VkAttachmentLoadOp>(XMLReader::ReadNodeValueInt(loadOpPtr));

        auto storeOpPtr = XMLReader::FindChildNodeByPropName(VkAttachmentDescriptionPtr, std::string("storeOp"));
        vec[i].storeOp = static_cast<VkAttachmentStoreOp>(XMLReader::ReadNodeValueInt(storeOpPtr));

        auto stencilLoadOpPtr =
            XMLReader::FindChildNodeByPropName(VkAttachmentDescriptionPtr, std::string("stencilLoadOp"));
        vec[i].stencilLoadOp = static_cast<VkAttachmentLoadOp>(XMLReader::ReadNodeValueInt(stencilLoadOpPtr));

        auto stencilStoreOpPtr =
            XMLReader::FindChildNodeByPropName(VkAttachmentDescriptionPtr, std::string("stencilStoreOp"));
        vec[i].stencilStoreOp = static_cast<VkAttachmentStoreOp>(XMLReader::ReadNodeValueInt(stencilStoreOpPtr));

        auto initialLayoutPtr =
            XMLReader::FindChildNodeByPropName(VkAttachmentDescriptionPtr, std::string("initialLayout"));
        vec[i].initialLayout = static_cast<VkImageLayout>(XMLReader::ReadNodeValueInt(initialLayoutPtr));

        auto finalLayoutPtr =
            XMLReader::FindChildNodeByPropName(VkAttachmentDescriptionPtr, std::string("finalLayout"));
        vec[i].finalLayout = static_cast<VkImageLayout>(XMLReader::ReadNodeValueInt(finalLayoutPtr));

        VkAttachmentDescriptionPtr = VkAttachmentDescriptionPtr->next;
    }
}

VkAttachmentReference* VkRenderPassCreateInfoModel::ReadColorReference(const xmlNodePtr& node)
{
    VkAttachmentReference* colorReference = new VkAttachmentReference();
    auto pColorAttachmentsPtr = XMLReader::FindChildNodeByPropName(node, std::string("pColorAttachments"));
    if (pColorAttachmentsPtr == nullptr) {
        delete colorReference;
        colorReference = nullptr;
        return nullptr;
    }
    auto colorAttachmentPtr =
        XMLReader::FindChildNodeByPropName(pColorAttachmentsPtr->children, std::string("attachment"));
    if (colorAttachmentPtr == nullptr) {
        delete colorReference;
        colorReference = nullptr;
        return nullptr;
    }
    colorReference->attachment = XMLReader::ReadNodeValueInt(colorAttachmentPtr);
    auto colorLayoutPtr = XMLReader::FindChildNodeByPropName(pColorAttachmentsPtr->children, std::string("layout"));
    if (colorLayoutPtr == nullptr) {
        delete colorReference;
        colorReference = nullptr;
        return nullptr;
    }
    colorReference->layout = static_cast<VkImageLayout>(XMLReader::ReadNodeValueInt(colorLayoutPtr));
    return colorReference;
}

VkAttachmentReference* VkRenderPassCreateInfoModel::ReadDepthReference(const xmlNodePtr& node)
{
    VkAttachmentReference* depthReference = new VkAttachmentReference();
    auto pDepthStencilAttachmentsPtr = XMLReader::FindChildNodeByPropName(node, std::string("pDepthStencilAttachment"));
    if (pDepthStencilAttachmentsPtr == nullptr) {
        delete depthReference;
        depthReference = nullptr;
        return nullptr;
    }
    auto attachmentPtr = XMLReader::FindChildNodeByPropName(pDepthStencilAttachmentsPtr, std::string("attachment"));
    if (attachmentPtr == nullptr) {
        delete depthReference;
        depthReference = nullptr;
        return nullptr;
    }
    depthReference->attachment = XMLReader::ReadNodeValueInt(attachmentPtr);

    auto layoutPtr = XMLReader::FindChildNodeByPropName(pDepthStencilAttachmentsPtr, std::string("layout"));
    if (layoutPtr == nullptr) {
        delete depthReference;
        depthReference = nullptr;
        return nullptr;
    }
    depthReference->layout = static_cast<VkImageLayout>(XMLReader::ReadNodeValueInt(layoutPtr));
    return depthReference;
}

void VkRenderPassCreateInfoModel::ReadSubPass(const xmlNodePtr& node, std::vector<VkSubpassDescription>& vec)
{
    auto pSubpassesPtr = XMLReader::FindChildNodeByPropName(node, std::string("pSubpasses"));
    if (pSubpassesPtr == nullptr) {
        RS_LOGE("RDC [%{public}s] pSubpassesPtr is nullptr.", __func__);
        return;
    }
    auto VkSubpassDescriptionPtr = pSubpassesPtr->children;
    for (size_t i = 0; i < vec.size(); i++) {
        if (VkSubpassDescriptionPtr == nullptr) {
            RS_LOGE("RDC [%{public}s] VkSubpassDescriptionPtr is nullptr.", __func__);
            return;
        }
        auto flagsPtr = XMLReader::FindChildNodeByPropName(VkSubpassDescriptionPtr, std::string("flags"));
        vec[i].flags = static_cast<VkSubpassDescriptionFlags>(XMLReader::ReadNodeValueInt(flagsPtr));

        auto pipelineBindPointPtr =
            XMLReader::FindChildNodeByPropName(VkSubpassDescriptionPtr, std::string("pipelineBindPoint"));
        vec[i].pipelineBindPoint = static_cast<VkPipelineBindPoint>(XMLReader::ReadNodeValueInt(pipelineBindPointPtr));

        auto inputAttachmentCountPtr =
            XMLReader::FindChildNodeByPropName(VkSubpassDescriptionPtr, std::string("inputAttachmentCount"));
        vec[i].inputAttachmentCount =
            static_cast<VkPipelineBindPoint>(XMLReader::ReadNodeValueInt(inputAttachmentCountPtr));

        auto colorAttachmentCountPtr =
            XMLReader::FindChildNodeByPropName(VkSubpassDescriptionPtr, std::string("colorAttachmentCount"));
        vec[i].colorAttachmentCount =
            static_cast<VkPipelineBindPoint>(XMLReader::ReadNodeValueInt(colorAttachmentCountPtr));

        vec[i].pColorAttachments = ReadColorReference(VkSubpassDescriptionPtr);
        vec[i].pDepthStencilAttachment = ReadDepthReference(VkSubpassDescriptionPtr);

        auto preserveAttachmentCountPtr =
            XMLReader::FindChildNodeByPropName(VkSubpassDescriptionPtr, std::string("preserveAttachmentCount"));
        if (preserveAttachmentCountPtr == nullptr) {
            continue;
        }
        vec[i].preserveAttachmentCount =
            static_cast<VkImageLayout>(XMLReader::ReadNodeValueInt(preserveAttachmentCountPtr));

        VkSubpassDescriptionPtr = VkSubpassDescriptionPtr->next;
    }
}

bool VkRenderPassCreateInfoModel::ReadXmlNode(const xmlNodePtr& createInfoNodePtr)
{
    VkRenderPassCreateInfo renderPassCI {};
    renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    auto attachmentCountPtr = XMLReader::FindChildNodeByPropName(createInfoNodePtr, std::string("attachmentCount"));
    if (attachmentCountPtr == nullptr) {
        return false;
    }
    renderPassCI.attachmentCount = XMLReader::ReadNodeValueInt(attachmentCountPtr);
    std::vector<VkAttachmentDescription> attachments(renderPassCI.attachmentCount);
    ReadAttachments(createInfoNodePtr, attachments);
    renderPassCI.pAttachments = attachments.data();

    auto subpassCountPtr = XMLReader::FindChildNodeByPropName(createInfoNodePtr, std::string("subpassCount"));
    if (subpassCountPtr == nullptr) {
        return false;
    }
    renderPassCI.subpassCount = XMLReader::ReadNodeValueInt(subpassCountPtr);
    std::vector<VkSubpassDescription> subpassDescription(renderPassCI.subpassCount);
    ReadSubPass(createInfoNodePtr, subpassDescription);
    renderPassCI.pSubpasses = subpassDescription.data();

    renderPassCI.dependencyCount = 0;

    auto& vkContext = OHOS::Rosen::RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    VkDevice device = vkContext.GetDevice();
    auto result = vkCreateRenderPass(device, &renderPassCI, nullptr, &renderPass);
    for (auto subPass : subpassDescription) {
        if (subPass.pColorAttachments) {
            delete subPass.pColorAttachments;
            subPass.pColorAttachments = nullptr;
        }
        if (subPass.pDepthStencilAttachment) {
            delete subPass.pDepthStencilAttachment;
            subPass.pDepthStencilAttachment = nullptr;
        }
    }
    if (result != VK_SUCCESS) {
        RS_LOGE("RDC [%{public}s] vkCreateRenderPass fail, result: %{public}d! \n", __func__, result);
        return false;
    }
    return true;
}

bool VkDescriptorSetLayoutCreateInfoModel::ReadXmlNode(const xmlNodePtr& createInfoNodePtr)
{
    VkDescriptorSetLayoutCreateInfo descriptorLayoutCI {};
    descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayoutCI.pNext = nullptr;
    descriptorLayoutCI.flags = 0;

    auto bindingCountPtr = XMLReader::FindChildNodeByPropName(createInfoNodePtr, std::string("bindingCount"));
    if (bindingCountPtr == nullptr) {
        return false;
    }
    descriptorLayoutCI.bindingCount = XMLReader::ReadNodeValueInt(bindingCountPtr);
    std::vector<VkDescriptorSetLayoutBinding> layoutBinding(descriptorLayoutCI.bindingCount);

    auto pBindingsPtr = XMLReader::FindChildNodeByPropName(createInfoNodePtr, std::string("pBindings"));
    if (pBindingsPtr == nullptr) {
        RS_LOGE("RDC [%{public}s] pBindingsPtr is nullptr.", __func__);
        return false;
    }
    auto VkDescriptorSetLayoutBindingPtr = pBindingsPtr->children;
    for (size_t i = 0; i < descriptorLayoutCI.bindingCount; i++) {
        if (VkDescriptorSetLayoutBindingPtr == nullptr) {
            RS_LOGE("RDC [%{public}s] VkDescriptorSetLayoutBindingPtr is nullptr.", __func__);
            return false;
        }
        auto bindingPtr = XMLReader::FindChildNodeByPropName(VkDescriptorSetLayoutBindingPtr, std::string("binding"));
        layoutBinding[i].binding = XMLReader::ReadNodeValueInt(bindingPtr);

        auto descriptorTypePtr =
            XMLReader::FindChildNodeByPropName(VkDescriptorSetLayoutBindingPtr, std::string("descriptorType"));
        layoutBinding[i].descriptorType = static_cast<VkDescriptorType>(XMLReader::ReadNodeValueInt(descriptorTypePtr));

        auto descriptorCountPtr =
            XMLReader::FindChildNodeByPropName(VkDescriptorSetLayoutBindingPtr, std::string("descriptorCount"));
        layoutBinding[i].descriptorCount =
            static_cast<VkDescriptorType>(XMLReader::ReadNodeValueInt(descriptorTypePtr));

        auto stageFlagsPtr =
            XMLReader::FindChildNodeByPropName(VkDescriptorSetLayoutBindingPtr, std::string("stageFlags"));
        layoutBinding[i].stageFlags = static_cast<VkShaderStageFlags>(XMLReader::ReadNodeValueInt(stageFlagsPtr));

        layoutBinding[i].pImmutableSamplers = nullptr;
        VkDescriptorSetLayoutBindingPtr = VkDescriptorSetLayoutBindingPtr->next;
    }
    descriptorLayoutCI.pBindings = layoutBinding.data();

    auto& vkContext = OHOS::Rosen::RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    VkDevice device = vkContext.GetDevice();
    auto result = vkCreateDescriptorSetLayout(device, &descriptorLayoutCI, nullptr, &descriptorSetLayout);
    if (result != VK_SUCCESS) {
        RS_LOGE("RDC [%{public}s] vkCreateDescriptorSetLayout fail, result: %{public}d! \n", __func__, result);
        return false;
    }
    return true;
}

bool VkPipelineLayoutCreateInfoModel::ReadXmlNode(const xmlNodePtr& createInfoNodePtr)
{
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.pushConstantRangeCount = 0;
    createInfo.pPushConstantRanges = nullptr;

    auto setLayoutCountPtr = XMLReader::FindChildNodeByPropName(createInfoNodePtr, std::string("setLayoutCount"));
    if (setLayoutCountPtr == nullptr) {
        RS_LOGE("RDC [%{public}s] input setLayoutCountPtr node is null! \n", __func__);
        return false;
    }
    createInfo.setLayoutCount = XMLReader::ReadNodeValueInt(setLayoutCountPtr);
    RS_LOGI("RDC [%{public}s] read setLayoutCount succ: %{public}d! \n", __func__, createInfo.setLayoutCount);

    auto pSetLayoutsPtr = XMLReader::FindChildNodeByPropName(createInfoNodePtr, std::string("pSetLayouts"));
    if (pSetLayoutsPtr == nullptr) {
        RS_LOGE("RDC [%{public}s] input pSetLayoutsPtr node is null! \n", __func__);
        return false;
    }
    auto layoutResourceIdPtr = pSetLayoutsPtr->children;
    for (size_t i = 0; i < createInfo.setLayoutCount; i++) {
        if (layoutResourceIdPtr == nullptr) {
            RS_LOGE("RDC [%{public}s] input layoutResourceIdPtr node is null! \n", __func__);
            return false;
        }
        setLayoutResourceIds.push_back(XMLReader::ReadNodeValueInt(layoutResourceIdPtr));
        layoutResourceIdPtr = layoutResourceIdPtr->next;
    }
    return true;
}

VkShaderModule RDCConfig::LoadSpirvShader(std::string fileName)
{
    auto& vkContext = OHOS::Rosen::RsVulkanContext::GetSingleton().GetRsVulkanInterface();
    VkDevice device = vkContext.GetDevice();
    std::streamsize shaderSize;
    char* shaderCode { nullptr };

    char realPath[PATH_MAX] = { 0 };
    if (fileName.length() == 0 || fileName.length() >= PATH_MAX || !realpath(fileName.c_str(), realPath)) {
        RS_LOGE("RDC can not get spirv shader file.");
        return VK_NULL_HANDLE;
    }
    std::ifstream is(realPath, std::ios::binary | std::ios::in | std::ios::ate);

    if (is.is_open()) {
        shaderSize = is.tellg();
        if (shaderSize <= 0 || shaderSize >= SHADER_MAX_SIZE) {
            RS_LOGE("RDC [%{public}s] fail,shaderSize is unvalid size: %{public}zu \n", __func__, shaderSize);
            is.close();
            return VK_NULL_HANDLE;
        }
        is.seekg(0, std::ios::beg);
        // Copy file contents into a buffer
        shaderCode = new char[shaderSize];
        is.read(shaderCode, shaderSize);
        is.close();
    }

    if (shaderCode) {
        // Create a new shader module that will be used for pipeline creation
        VkShaderModuleCreateInfo shaderModuleCI {};
        shaderModuleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCI.codeSize = static_cast<size_t>(shaderSize);
        shaderModuleCI.pCode = (uint32_t*)shaderCode;

        VkShaderModule shaderModule;
        auto result = vkCreateShaderModule(device, &shaderModuleCI, nullptr, &shaderModule);
        if (result != VK_SUCCESS) {
            RS_LOGE("RDC [%{public}s] vkCreateShaderModule fail, result: %{public}d! \n", __func__, result);
        }

        delete[] shaderCode;
        return shaderModule;
    } else {
        RS_LOGE("RDC [%{public}s] LoadSpirvShader fail, file: %{public}s. \n", __func__, realPath);
        return VK_NULL_HANDLE;
    }
}

void VkGraphicsPipelineCreateInfoModel::ReadVertexInputBinding(const xmlNodePtr& node)
{
    auto vertexBindingDescriptionCountPtr =
        XMLReader::FindChildNodeByPropName(node, std::string("vertexBindingDescriptionCount"));
    if (vertexBindingDescriptionCountPtr == nullptr) {
        return;
    }
    vertexInputStateCI.vertexBindingDescriptionCount = XMLReader::ReadNodeValueInt(vertexBindingDescriptionCountPtr);
    vertexInputBinding.resize(vertexInputStateCI.vertexBindingDescriptionCount);
    auto pVertexBindingDescriptionsPtr =
        XMLReader::FindChildNodeByPropName(node, std::string("pVertexBindingDescriptions"));
    if (pVertexBindingDescriptionsPtr == nullptr) {
        RS_LOGE("RDC [%{public}s] pVertexBindingDescriptionsPtr is nullptr! \n", __func__);
        return;
    }
    auto VkVertexInputBindingDescriptionPtr = pVertexBindingDescriptionsPtr->children;
    for (size_t i = 0; i < vertexInputStateCI.vertexBindingDescriptionCount; i++) {
        if (VkVertexInputBindingDescriptionPtr == nullptr) {
            RS_LOGE("RDC [%{public}s] VkVertexInputBindingDescriptionPtr is nullptr.", __func__);
            return;
        }
        auto bindingPtr =
            XMLReader::FindChildNodeByPropName(VkVertexInputBindingDescriptionPtr, std::string("binding"));
        vertexInputBinding[i].binding = XMLReader::ReadNodeValueInt(bindingPtr);

        auto stridePtr = XMLReader::FindChildNodeByPropName(VkVertexInputBindingDescriptionPtr, std::string("stride"));
        vertexInputBinding[i].stride = XMLReader::ReadNodeValueInt(stridePtr);

        auto inputRatePtr =
            XMLReader::FindChildNodeByPropName(VkVertexInputBindingDescriptionPtr, std::string("inputRate"));
        vertexInputBinding[i].inputRate = static_cast<VkVertexInputRate>(XMLReader::ReadNodeValueInt(inputRatePtr));

        VkVertexInputBindingDescriptionPtr = VkVertexInputBindingDescriptionPtr->next;
    }
    vertexInputStateCI.pVertexBindingDescriptions = vertexInputBinding.data();
}

void VkGraphicsPipelineCreateInfoModel::ReadVertexInputAttributs(const xmlNodePtr& node)
{
    auto vertexAttributeDescriptionCountPtr =
        XMLReader::FindChildNodeByPropName(node, std::string("vertexAttributeDescriptionCount"));
    if (vertexAttributeDescriptionCountPtr == nullptr) {
        return;
    }
    vertexInputStateCI.vertexAttributeDescriptionCount =
        XMLReader::ReadNodeValueInt(vertexAttributeDescriptionCountPtr);
    vertexInputAttributs.resize(vertexInputStateCI.vertexAttributeDescriptionCount);
    auto pVertexAttributeDescriptionsPtr =
        XMLReader::FindChildNodeByPropName(node, std::string("pVertexAttributeDescriptions"));
    if (pVertexAttributeDescriptionsPtr == nullptr) {
        RS_LOGE("RDC [%{public}s] pVertexAttributeDescriptionsPtr is nullptr! \n", __func__);
        return;
    }
    auto VkVertexInputAttributeDescriptionPtr = pVertexAttributeDescriptionsPtr->children;
    for (size_t i = 0; i < vertexInputStateCI.vertexAttributeDescriptionCount; i++) {
        if (VkVertexInputAttributeDescriptionPtr == nullptr) {
            RS_LOGE("RDC [%{public}s] VkVertexInputAttributeDescriptionPtr is nullptr.", __func__);
            return;
        }
        auto locationPtr =
            XMLReader::FindChildNodeByPropName(VkVertexInputAttributeDescriptionPtr, std::string("location"));
        vertexInputAttributs[i].location = XMLReader::ReadNodeValueInt(locationPtr);

        auto bindingPtr =
            XMLReader::FindChildNodeByPropName(VkVertexInputAttributeDescriptionPtr, std::string("binding"));
        vertexInputAttributs[i].binding = XMLReader::ReadNodeValueInt(bindingPtr);

        auto formatPtr =
            XMLReader::FindChildNodeByPropName(VkVertexInputAttributeDescriptionPtr, std::string("format"));
        vertexInputAttributs[i].format = static_cast<VkFormat>(XMLReader::ReadNodeValueInt(formatPtr));

        auto offsetPtr =
            XMLReader::FindChildNodeByPropName(VkVertexInputAttributeDescriptionPtr, std::string("offset"));
        vertexInputAttributs[i].offset = XMLReader::ReadNodeValueInt(offsetPtr);

        VkVertexInputAttributeDescriptionPtr = VkVertexInputAttributeDescriptionPtr->next;
    }
    vertexInputStateCI.pVertexAttributeDescriptions = vertexInputAttributs.data();
}

void VkGraphicsPipelineCreateInfoModel::ReadVertexInputState(const xmlNodePtr& node)
{
    vertexInputStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    auto pVertexInputStatePtr = XMLReader::FindChildNodeByPropName(node, std::string("pVertexInputState"));
    if (pVertexInputStatePtr == nullptr) {
        return;
    }
    ReadVertexInputBinding(pVertexInputStatePtr);
    ReadVertexInputAttributs(pVertexInputStatePtr);
    createInfo.pVertexInputState = &vertexInputStateCI;
}

void VkGraphicsPipelineCreateInfoModel::ReadColorBlendState(const xmlNodePtr& node)
{
    colorBlendStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    auto pColorBlendStatePtr = XMLReader::FindChildNodeByPropName(node, std::string("pColorBlendState"));
    auto attachmentCountPtr = XMLReader::FindChildNodeByPropName(pColorBlendStatePtr, std::string("attachmentCount"));
    colorBlendStateCI.attachmentCount = XMLReader::ReadNodeValueInt(attachmentCountPtr);
    if (attachmentCountPtr == nullptr || colorBlendStateCI.attachmentCount == 0) {
        return;
    }
    auto pAttachmentsPtr = XMLReader::FindChildNodeByPropName(pColorBlendStatePtr, std::string("pAttachments"));
    if (pAttachmentsPtr == nullptr) {
        return;
    }
    auto pAttachmentsStatePtr = pAttachmentsPtr->children;
    blendAttachmentState.resize(colorBlendStateCI.attachmentCount);
    for (size_t i = 0; i < colorBlendStateCI.attachmentCount; i++) {
        if (pAttachmentsStatePtr == nullptr) {
            RS_LOGE("RDC [%{public}s] input pAttachmentsStatePtr is null! \n", __func__);
            return;
        }
        auto blendEnablePtr = XMLReader::FindChildNodeByPropName(pAttachmentsStatePtr, std::string("blendEnable"));
        blendAttachmentState[i].blendEnable = static_cast<VkBool32>(XMLReader::ReadNodeValueInt(blendEnablePtr));

        auto srcColorBlendFactorPtr =
            XMLReader::FindChildNodeByPropName(pAttachmentsStatePtr, std::string("srcColorBlendFactor"));
        blendAttachmentState[i].srcColorBlendFactor =
            static_cast<VkBlendFactor>(XMLReader::ReadNodeValueInt(srcColorBlendFactorPtr));

        auto dstColorBlendFactorPtr =
            XMLReader::FindChildNodeByPropName(pAttachmentsStatePtr, std::string("dstColorBlendFactor"));
        blendAttachmentState[i].dstColorBlendFactor =
            static_cast<VkBlendFactor>(XMLReader::ReadNodeValueInt(dstColorBlendFactorPtr));

        auto colorBlendOpPtr = XMLReader::FindChildNodeByPropName(pAttachmentsStatePtr, std::string("colorBlendOp"));
        blendAttachmentState[i].colorBlendOp = static_cast<VkBlendOp>(XMLReader::ReadNodeValueInt(colorBlendOpPtr));

        auto srcAlphaBlendFactorPtr =
            XMLReader::FindChildNodeByPropName(pAttachmentsStatePtr, std::string("srcAlphaBlendFactor"));
        blendAttachmentState[i].srcAlphaBlendFactor =
            static_cast<VkBlendFactor>(XMLReader::ReadNodeValueInt(srcAlphaBlendFactorPtr));

        auto dstAlphaBlendFactorPtr =
            XMLReader::FindChildNodeByPropName(pAttachmentsStatePtr, std::string("dstAlphaBlendFactor"));
        blendAttachmentState[i].dstAlphaBlendFactor =
            static_cast<VkBlendFactor>(XMLReader::ReadNodeValueInt(dstAlphaBlendFactorPtr));

        auto alphaBlendOpPtr = XMLReader::FindChildNodeByPropName(pAttachmentsStatePtr, std::string("alphaBlendOp"));
        blendAttachmentState[i].alphaBlendOp = static_cast<VkBlendOp>(XMLReader::ReadNodeValueInt(alphaBlendOpPtr));

        auto colorWriteMaskPtr =
            XMLReader::FindChildNodeByPropName(pAttachmentsStatePtr, std::string("colorWriteMask"));
        blendAttachmentState[i].colorWriteMask =
            static_cast<VkColorComponentFlags>(XMLReader::ReadNodeValueInt(colorWriteMaskPtr));

        pAttachmentsStatePtr = pAttachmentsStatePtr->next;
    }
    colorBlendStateCI.pAttachments = blendAttachmentState.data();
    createInfo.pColorBlendState = &colorBlendStateCI;
}

void VkGraphicsPipelineCreateInfoModel::ReadRasterizationState(const xmlNodePtr& node)
{
    rasterizationStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCI.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCI.cullMode = VK_CULL_MODE_NONE;
    rasterizationStateCI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCI.depthClampEnable = VK_FALSE;
    rasterizationStateCI.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCI.depthBiasEnable = VK_FALSE;
    rasterizationStateCI.lineWidth = 1.0f;
    createInfo.pRasterizationState = &rasterizationStateCI;
}

void VkGraphicsPipelineCreateInfoModel::ReadMultiSampleState(const xmlNodePtr& node)
{
    multisampleStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateCI.pSampleMask = nullptr;
    createInfo.pMultisampleState = &multisampleStateCI;
}

bool VkGraphicsPipelineCreateInfoModel::ReadXmlNode(const xmlNodePtr& createInfoNodePtr)
{
    if (createInfoNodePtr == nullptr) {
        RS_LOGE("RDC [%{public}s] input createInfoNodePtr is null! \n", __func__);
        return false;
    }
    auto stageCountPtr = XMLReader::FindChildNodeByPropName(createInfoNodePtr, std::string("stageCount"));
    if (stageCountPtr == nullptr) {
        RS_LOGE("RDC [%{public}s] stageCountPtr is null! \n", __func__);
        return false;
    }
    createInfo.stageCount = XMLReader::ReadNodeValueInt(stageCountPtr);
    auto pStagesPtr = XMLReader::FindChildNodeByPropName(createInfoNodePtr, std::string("pStages"));
    if (pStagesPtr == nullptr) {
        RS_LOGE("RDC [%{public}s] pStagesPtr is null! \n", __func__);
        return false;
    }
    auto vertexShaderPtr = pStagesPtr->children;
    if (vertexShaderPtr == nullptr) {
        RS_LOGE("RDC [%{public}s] vertexShaderPtr is null! \n", __func__);
        return false;
    }
    auto vsModulePtr = XMLReader::FindChildNodeByPropName(vertexShaderPtr, std::string("module"));
    auto vsResourceIdStr = XMLReader::ReadNodeValue(vsModulePtr);
    vertexShaderFilePath = XMLReader::GetConfigPath(std::string(PATH_CONFIG_DIR) + vsResourceIdStr + "vs.spv");
    auto fragShaderPtr = vertexShaderPtr->next;
    auto fsModulePtr = XMLReader::FindChildNodeByPropName(fragShaderPtr, std::string("module"));
    auto fsResourceIdStr = XMLReader::ReadNodeValue(fsModulePtr);
    fragShaderFilePath = XMLReader::GetConfigPath(std::string(PATH_CONFIG_DIR) + fsResourceIdStr + "fs.spv");

    auto layoutPtr = XMLReader::FindChildNodeByPropName(createInfoNodePtr, std::string("layout"));
    if (layoutPtr == nullptr) {
        RS_LOGE("RDC [%{public}s] layoutPtr is null! \n", __func__);
        return false;
    }
    layoutResourceId = XMLReader::ReadNodeValueInt(layoutPtr);

    auto renderPassPtr = XMLReader::FindChildNodeByPropName(createInfoNodePtr, std::string("renderPass"));
    if (renderPassPtr == nullptr) {
        RS_LOGE("RDC [%{public}s] renderPassPtr is null! \n", __func__);
        return false;
    }
    renderPassResourceId = XMLReader::ReadNodeValueInt(renderPassPtr);

    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    ReadVertexInputState(createInfoNodePtr);
    ReadRasterizationState(createInfoNodePtr);
    ReadMultiSampleState(createInfoNodePtr);
    ReadColorBlendState(createInfoNodePtr);

    return true;
}

void RDCConfig::CloseXML()
{
    if (pDoc != nullptr) {
        xmlFreeDoc(pDoc);
        pDoc = nullptr;
    }
}

} // namespace RDC
} // namespace Rosen
} // namespace OHOS
