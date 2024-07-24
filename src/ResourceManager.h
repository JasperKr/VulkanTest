#include <source.h>
#include <config.h>

class ResourceManager
{
public:
    void createDescriptorSetLayout(VkDevice device);
    void createDescriptorPool(VkDevice device);
    void createDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool);
    void updateDescriptorSets(VkDevice device);

    VkDescriptorSetLayout getDescriptorSetLayout() const;
    std::vector<VkDescriptorSet> getDescriptorSets() const;

private:
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;
};