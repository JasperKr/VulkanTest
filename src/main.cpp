#include <main.hpp>

namespace Main
{
    GLFWwindow *window;
    uint32_t Width = 800;
    uint32_t Height = 600;

    glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_forwards = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    const int MAX_FRAMES_IN_FLIGHT = 2;

    const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"};

    const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    void run()
    {
        VulkanSetup::window = window;

        VulkanSetup::init();

        mainLoop();
        cleanup();
    }

    void mainLoop()
    {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(VulkanSetup::device);
    }

    void cleanupSwapChain()
    {
        vkDestroyImageView(VulkanSetup::device, VulkanSetup::depthImageView, nullptr);
        vkDestroyImage(VulkanSetup::device, VulkanSetup::depthImage, nullptr);
        vkFreeMemory(VulkanSetup::device, VulkanSetup::depthImageMemory, nullptr);

        for (auto framebuffer : VulkanSetup::swapChainFramebuffers)
        {
            vkDestroyFramebuffer(VulkanSetup::device, framebuffer, nullptr);
        }

        for (auto imageView : VulkanSetup::swapChainImageViews)
        {
            vkDestroyImageView(VulkanSetup::device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(VulkanSetup::device, VulkanSetup::swapChain, nullptr);
    }

    void cleanup()
    {
        cleanupSwapChain();

        vkDestroyPipeline(VulkanSetup::device, VulkanSetup::graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(VulkanSetup::device, VulkanSetup::pipelineLayout, nullptr);
        vkDestroyRenderPass(VulkanSetup::device, VulkanSetup::renderPass, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroyBuffer(VulkanSetup::device, VulkanSetup::uniformBuffers[i], nullptr);
            vkFreeMemory(VulkanSetup::device, VulkanSetup::uniformBuffersMemory[i], nullptr);
        }

        vkDestroyDescriptorPool(VulkanSetup::device, VulkanSetup::descriptorPool, nullptr);

        for (size_t modelIndex = 0; modelIndex < VulkanSetup::models.size(); modelIndex++)
        {
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                vkDestroyBuffer(VulkanSetup::device, VulkanSetup::models[modelIndex].uniformBuffers[i], nullptr);
                vkFreeMemory(VulkanSetup::device, VulkanSetup::models[modelIndex].uniformBuffersMemory[i], nullptr);
            }
            Application_Model::Application_Model model = VulkanSetup::models[modelIndex];
            for (size_t i = 0; i < model.textureImages.size(); i++)
            {
                vkDestroySampler(VulkanSetup::device, model.textureSamplers[i], nullptr);
                vkDestroyImageView(VulkanSetup::device, model.textureImageViews[i], nullptr);
                vkDestroyImage(VulkanSetup::device, model.textureImages[i], nullptr);
                vkFreeMemory(VulkanSetup::device, model.textureImagesMemory[i], nullptr);
            }
            vkDestroyDescriptorPool(VulkanSetup::device, model.descriptorPool, nullptr);

            for (size_t i = 0; i < model.Vertices.size(); i++)
            {
                vkDestroyBuffer(VulkanSetup::device, model.indexBuffers[i], nullptr);
                vkFreeMemory(VulkanSetup::device, model.indexBuffersMemory[i], nullptr);

                vkDestroyBuffer(VulkanSetup::device, model.vertexBuffers[i], nullptr);
                vkFreeMemory(VulkanSetup::device, model.vertexBuffersMemory[i], nullptr);
            }
        }

        vkDestroyDescriptorSetLayout(VulkanSetup::device, Application_Model::uboDescriptorSetLayout, nullptr);
        vkDestroyDescriptorSetLayout(VulkanSetup::device, Application_Model::textureDescriptorSetLayout, nullptr);
        vkDestroyDescriptorSetLayout(VulkanSetup::device, VulkanSetup::descriptorSetLayout, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(VulkanSetup::device, VulkanSetup::renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(VulkanSetup::device, VulkanSetup::imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(VulkanSetup::device, VulkanSetup::inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(VulkanSetup::device, VulkanSetup::commandPool, nullptr);

        vkDestroyDevice(VulkanSetup::device, nullptr);

        if (enableValidationLayers)
        {
            VulkanSetup::DestroyDebugUtilsMessengerEXT(VulkanSetup::instance, VulkanSetup::debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(VulkanSetup::instance, VulkanSetup::surface, nullptr);
        vkDestroyInstance(VulkanSetup::instance, nullptr);

        glfwDestroyWindow(VulkanSetup::window);

        glfwTerminate();
    }

    void recreateSwapChain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(VulkanSetup::window, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(VulkanSetup::window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(VulkanSetup::device);

        cleanupSwapChain();

        VulkanSetup::createSwapChain();
        VulkanSetup::createImageViews();
        VulkanSetup::createDepthResources();
        VulkanSetup::createFramebuffers();
    }

    void updateUniformBuffer(uint32_t currentImage)
    {
        CameraUBO ubo{};

        ubo.view = glm::lookAt(camera_position, camera_position + camera_forwards, camera_up);
        ubo.proj = glm::perspective(glm::radians(45.0f), VulkanSetup::swapChainExtent.width / (float)VulkanSetup::swapChainExtent.height, 0.1f, 1000.0f);
        ubo.proj[1][1] *= -1;

        memcpy(VulkanSetup::uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
    }

    void updateCameraRotation()
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        static double lastX = xpos, lastY = ypos;
        static float yaw = 0, pitch = 0;

        float sensitivity = 0.1f;
        yaw += (xpos - lastX) * sensitivity;
        pitch += (lastY - ypos) * sensitivity;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camera_forwards = glm::normalize(front);

        lastX = xpos;
        lastY = ypos;
    }

    void updateCamera()
    {
        const float cameraSpeed = 0.05f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            camera_position += cameraSpeed * camera_forwards;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            camera_position -= cameraSpeed * camera_forwards;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camera_position -= glm::normalize(glm::cross(camera_forwards, camera_up)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera_position += glm::normalize(glm::cross(camera_forwards, camera_up)) * cameraSpeed;
        }

        updateCameraRotation();
    }

    void drawFrame()
    {
        vkWaitForFences(VulkanSetup::device, 1, &VulkanSetup::inFlightFences[VulkanSetup::currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(VulkanSetup::device, VulkanSetup::swapChain, UINT64_MAX, VulkanSetup::imageAvailableSemaphores[VulkanSetup::currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        updateCamera();
        updateUniformBuffer(VulkanSetup::currentFrame);

        vkResetFences(VulkanSetup::device, 1, &VulkanSetup::inFlightFences[VulkanSetup::currentFrame]);

        vkResetCommandBuffer(VulkanSetup::commandBuffers[VulkanSetup::currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
        VulkanSetup::recordCommandBuffer(VulkanSetup::commandBuffers[VulkanSetup::currentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {VulkanSetup::imageAvailableSemaphores[VulkanSetup::currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &VulkanSetup::commandBuffers[VulkanSetup::currentFrame];

        VkSemaphore signalSemaphores[] = {VulkanSetup::renderFinishedSemaphores[VulkanSetup::currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(VulkanSetup::graphicsQueue, 1, &submitInfo, VulkanSetup::inFlightFences[VulkanSetup::currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {VulkanSetup::swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(VulkanSetup::presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
        {
            framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        VulkanSetup::currentFrame = (VulkanSetup::currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
};

class Application
{
public:
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height)
    {
        Main::framebufferResized = true;
    }

    void initWindow(uint32_t Width, uint32_t Height)
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        Main::window = glfwCreateWindow(Width, Height, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(Main::window, this);
        glfwSetFramebufferSizeCallback(Main::window, framebufferResizeCallback);
    }
};

int main()
{
    try
    {
        Application app;
        app.initWindow(Main::Width, Main::Height);

        Main::run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
