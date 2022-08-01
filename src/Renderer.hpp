#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <array>
#include <vector>
#include <unordered_map>

#include "AllocatedImage.hpp"
#include "DebugMessenger.hpp"
#include "DeletionQueue.hpp"
#include "QueueFamilyData.hpp"
#include "SurfaceDetails.hpp"
#include "PipelineBuilder.hpp"
#include "UniformData.hpp"
#include "vk_mem_alloc.h"
#include "FrameData.hpp"
#include "Common.hpp"
#include "Vertex.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "RenderObject.hpp"

#include <vulkan/vulkan_core.h>

/**
 * @brief Renderer class to handle all rendering operations
 */
class Renderer {
public:
    /**
     * @brief Renderer constructor.
     * @param window [in] to render to.
     */
    Renderer(SDL_Window *window);

    // cleanup all vulkan structures
    void cleanup();

    // draw to given window
    void draw();

    // tell renderer if a resize operation is being done on the given window
    // if window is resizable and this function isn't called on resize,
    // it'll result in crash
    void windowResized();

    // uniform data sent to shaders per frame
    UniformData uniformData;

    // array of renderable objects
    std::vector<RenderObject> renderables;
    std::unordered_map<std::string, Material> materials;
    std::unordered_map<std::string, Mesh> meshes;

    // create material and add it to the map
    Material* createMaterial(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name);

    // find material by name
    // returns nullptr if material cannot be found
    Material* getMaterial(const std::string& name);

    // find mesh by name
    // returns nullptr if it can't be found
    Mesh* getMesh(const std::string& name);

    // draw method
    void drawObjects(VkCommandBuffer cmd, RenderObject* first, size_t count);
private:
    // sdl window to render images to
    SDL_Window *window;

    // deletors added to this queue are required to
    // be executed on recreation of swapchain
    DeletionQueue swapchainDeletionQueue;
    // main deletion queue will take anything else
    DeletionQueue mainDeletionQueue;

    // vulkan instance is our key to talk to vulkan drivers
    VkInstance instance = VK_NULL_HANDLE;
    // creates an instance for this renderer class
    void createInstance();

    // debug messenger to get validation messages
    DebugMessenger debugMessenger;

    // surface handle
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    // create surface for the given window
    void createSurface();

    // physical device
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    // queue family data for selected physical device
    QueueFamilyData queueFamilyData;
    // device properties
    VkPhysicalDeviceProperties physicalDeviceProperties;
    // select physical device
    void selectPhysicalDevice();

    // logical device
    VkDevice device = VK_NULL_HANDLE;
    // required device extensions
    std::vector<const char*> deviceExtensions = {
       VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    // graphics queue handle
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    // surface support queue handle
    VkQueue presentQueue = VK_NULL_HANDLE;
    // transfer operations queue
    VkQueue transferQueue = VK_NULL_HANDLE;
    // create logical device from selected physical device
    void createLogicalDevice();

    // allocator
    VmaAllocator allocator;
    // create allocator
    void createAllocator();

    // swapchain
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    // surface details
    SurfaceDetails surfaceDetails;
    // swapchain image extent, same as window size
    VkExtent2D swapchainImageExtent;
    // selected swapchain image format
    VkSurfaceFormatKHR swapchainImageFormat;
    // number of images in swapchain
    uint32_t swapchainImageCount;
    // images created by swapchain
    std::vector<VkImage> swapchainImages;
    // create swapchain
    void createSwapchain();

    // depth image format
    VkFormat depthImageFormat;
    // depth iamge
    AllocatedImage depthImage;
    // depth iamge view
    VkImageView depthImageView;
    // setup depth image
    void createDepthImage();

    // image views for images in swapchain
    std::vector<VkImageView> swapchainImageViews;
    // create image views
    void createImageViews();

    // buffering size for rendering
    // like double or triple buffering etc...
    static constexpr uint32_t bufferingSize = 3;

    // frame data
    std::array<FrameData, bufferingSize> frames;
    // get current frame
    FrameData& getCurrentFrame() { return frames[frameNumber % bufferingSize]; }

    // command buffer and pool for transfer operations
    VkCommandPool transferCommandPool;
    VkCommandBuffer transferCommandBuffer;

    // initialize command buffers and stuffs
    void createCommandPool();
    void allocateCommandBuffers();
    void initCommands();

    // copy buffer from cpu memory to gpu memory
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    // renderpass
    VkRenderPass renderPass;
    // create renderpass
    void initRenderPass();

    // framebuffers
    std::vector<VkFramebuffer> framebuffers;
    // create framebuffers
    void createFramebuffers();

    // init sync structures
    void initSyncStructures();

    // keep count of frame number
    size_t frameNumber = 0;

    // mesh to be loaded
    Mesh mesh;
    // load meshes
    void loadMeshes();
    // upload mesh to wherever we want
    void uploadMesh(Mesh& mesh);

    // descriptor set layout
    VkDescriptorSetLayout globalDescriptorSetLayout;
    // descriptor pool to allocate sets from
    VkDescriptorPool descriptorPool;
    // initialize descriptor sets
    void initDescriptors();

    // shader modules
    VkShaderModule meshVS, meshFS;
    // pipeline layout
    VkPipelineLayout meshPipelineLayout;
    // pipeline
    VkPipeline meshPipeline;
    // pipeline builder
    PipelineBuilder pipelineBuilder;
    // initialize graphics pipeline
    void initGraphicsPipeline();

    // flag to keep track of window resizes, to be flagged by user
    bool framebufferResized = false;
    // recreate swapchain when swapchain becomes incompatibl
    // with window used by renderer
    void recreateSwapchain();

    // cleanup stuffs that depend of swapchain
    void cleanupSwapchain();

    // create buffer of given size and usage
    AllocatedBuffer createBuffer(size_t allocSize, VkBufferUsageFlags usageFlags, VmaMemoryUsage memoryUsage);

};

#endif//RENDERER_HPP
