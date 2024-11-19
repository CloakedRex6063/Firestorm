#pragma once
#include "Descriptor.h"
#include "Core/Render/Vulkan/Resources/Model.h"

namespace FS::VK
{
    class Context;
    class Command;
    class ModelManager 
    {
    public:
        ModelManager(const std::shared_ptr<Context>& context);
        ~ModelManager();
        
        void UploadModels(std::unordered_map<std::string, FS::Model>& models);
        [[nodiscard]] std::span<Model> GetModels() { return mModels; }
        [[nodiscard]] Descriptor& GetDescriptor() { return mDescriptor; }
        
    private:
        std::shared_ptr<Context> mContext;
        std::unique_ptr<Command> mTransferCommand;
        std::vector<Model> mModels;

        Descriptor mDescriptor;
        VkSampler mSampler{};

        uint16_t mIncrementor = 0;
    };
}