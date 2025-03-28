/*
* 
* This contains the abstraction of the descriptors set. These are used to send data to the shaders.
* 
* Author: Brendan Galea
* Retrieved from: https://pastebin.com/yU7dMAxt
*/
#pragma once

#include "weEngineDevice.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace weEngine {

    /*
    * Describes the layout of the descriptor sets, aka their binding, their type, shader stage and so on
    */
    class weEngineDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(weEngineDevice& weEngineDevice) : weEngineDevice{ weEngineDevice } {}
            /*
            * Adds Binding for a descriptor set layout
            */
            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<weEngineDescriptorSetLayout> build() const;

        private:
            weEngineDevice& weEngineDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        weEngineDescriptorSetLayout(
            weEngineDevice& weEngineDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~weEngineDescriptorSetLayout();
        weEngineDescriptorSetLayout(const weEngineDescriptorSetLayout&) = delete;
        weEngineDescriptorSetLayout& operator=(const weEngineDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        weEngineDevice& weEngineDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class weEngineDescriptorWriter;
    };
    /*
    * Allocates and free memory from the VKDescriptorPool for Descriptor set buffers
    */
    class weEngineDescriptorPool
    {
    public:
        class Builder {
        public:
            Builder(weEngineDevice& weEngineDevice) : weEngineDevice{ weEngineDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<weEngineDescriptorPool> build() const;

        private:
            weEngineDevice& weEngineDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        weEngineDescriptorPool(
            weEngineDevice& weEngineDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~weEngineDescriptorPool();
        weEngineDescriptorPool(const weEngineDescriptorPool&) = delete;
        weEngineDescriptorPool& operator=(const weEngineDescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        weEngineDevice& weEngineDevice;
        VkDescriptorPool descriptorPool;

        friend class weEngineDescriptorWriter;
    };

    /*
    * Describes how the data are written into the descriptor set
    */
    class weEngineDescriptorWriter {
    public:
        weEngineDescriptorWriter(weEngineDescriptorSetLayout& setLayout, weEngineDescriptorPool& pool);

        weEngineDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        weEngineDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        weEngineDescriptorSetLayout& setLayout;
        weEngineDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };
}
