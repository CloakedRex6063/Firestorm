#pragma once

namespace FS::VK
{
    class Command;
    class Device;
    class Queue;
    class Model;
    class ModelManager
    {
    public:
        ModelManager(const std::shared_ptr<Device>& device, const std::shared_ptr<Queue>& queue);
        void UploadMeshes();

        std::unordered_map<std::string, Model>& GetModels() { return mModels; }

    private:
        std::unique_ptr<Command> mCommand;
        std::shared_ptr<Device> mDevice;
        std::shared_ptr<Queue> mTransferQueue;
        std::unordered_map<std::string, Model> mModels;
    };
}