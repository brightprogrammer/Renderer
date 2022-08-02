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

    /**
     * @brief Upload mesh to GPU for rendering.
     * Mesh must be uploaded to gpu before drawing it to GPU.
     * Mesh must be uploaded only once.
     *
     * Uploading only makes Mesh drawable, it does not draw the mesh.
     * To draw meshes to screen, one must create RenderObject and add
     * it to list of objects to be drawn.
     *
     * @param mesh to be uploaded to GPU.
     * */
    void uploadMesh(Mesh& mesh);

    /**
     * @brief Draws all RenderObject objects present in renderObjects vector.
     * To draw multiple objects in one frame, push RenderObjects to list of objects
     * to be drawn (renderObjects) and then call this draw command at once.
     * */
    void draw();

    /**
     * @brief Tell renderer if a resize operation is being done on the given window
     * if window is resizable and this function isn't called on resize,
     * it can result in crash or some undefined behaviour like image distortion etc...
     */
    void windowResized();

    /**
     * @brief Uniform data sent to shaders.
     * This must be updated regularly whenever needed.
     * */
    UniformData uniformData;

    /**
     * @brief Contains list of objects to be drawn in single frame.
     * One can clear this list every frame if list of objects is dynamic
     * and recreate this list every frame.
     * Or keep this list constant everyframe and just keep rendering objects.
     * Usage of this vector depends on the user mostly.
     * */
    std::vector<RenderObject> renderObjects;

    /**
     * @brief Add given render object to renderObjects vector
     * */
    inline void addRenderObject(const RenderObject& obj) { renderObjects.push_back(obj); }

    /**
     * @brief Clear renderObjects vector.
     * */
    inline void clearRenderObjects() { renderObjects.clear(); }

    /**
     * @brief Register materials by name in renderer. Since this keeps a copy
     * of material and creating a RenderObject requires pointer to material,
     * you can get pointer to copied material using getMaterial() function
     * and easily create a RenderObject.
     * */
    std::unordered_map<std::string, Material> materials;

     /**
     * @brief Register meshes by name in renderer. Since this keeps a copy
     * of material and creating a RenderObject requires pointer to mesh,
     * you can get pointer to copied mesh using getMesh() function
     * and easily create a RenderObject.
     * */
    std::unordered_map<std::string, Mesh> meshes;

    /// create material and add it to the map
    Material* createMaterial(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name);

    /// Find material by name.
    /// Returns nullptr if material cannot be found.
    Material* getMaterial(const std::string& name);

    // Find mesh by name.
    // Returns nullptr if it can't be found.
    Mesh* getMesh(const std::string& name);
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

    // load meshes
    void loadMeshes();
    // upload data to gpu using staging buffer
    AllocatedBuffer uploadDataToGPU(void* data, size_t size, VkBufferUsageFlags flags);
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

    // record draw commands for drawing multiple objects
    void drawObjects(VkCommandBuffer cmd, RenderObject* first, size_t count);
};

#endif//RENDERER_HPP
