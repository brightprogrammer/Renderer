#include "Renderer.hpp"
#include "Device.hpp"
#include "Config.hpp"
#include "GPUCameraData.hpp"
#include "Initializers.hpp"
#include "QueueFamilyData.hpp"
#include "ReturnCode.hpp"
#include "Shader.hpp"
#include "SurfaceDetails.hpp"
#include "VertexInputDescription.hpp"
#include "VkResultString.hpp"
#include "Swapchain.hpp"
#include "Shader.hpp"
#include "MeshPushConstants.hpp"
#include "Math.hpp"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_vulkan.h>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>
#include <glm/gtx/transform.hpp>

#include <ios>
#include <set>
#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include <vector>
#include <cstdint>

// renderer constructor
Renderer::Renderer(SDL_Window *window)
     : window(window) {
    // create vulkan instance
    createInstance();

    // setup debug utils messenger
    if(validationEnabled){
        if(debugMessenger.create(instance) != SUCCESS){
            std::cerr << "[ERROR] failed to create debug messenger";
            exit(1);
        }
    }

    // create surface
    createSurface();

    // select physical device
    selectPhysicalDevice();

    // create logical device
    createLogicalDevice();

    // create allocator
    createAllocator();

    // create swapchain
    createSwapchain();

    // create image views
    createImageViews();

    // create depth image and image view
    createDepthImage();

    // create command pool and spawn some command buffers
    initCommands();

    // init render pass
    initRenderPass();

    // create framebuffers
    createFramebuffers();

    // init sync structures
    initSyncStructures();

    // initialize descriptor sets
    initDescriptors();

    // init graphics pipeline
    initGraphicsPipeline();

    // load meshes
    loadMeshes();
}

// destroy renderer
void Renderer::cleanup(){
    // wait for all device operations to complete before destroying
    vkDeviceWaitIdle(device);

    // swapchain deletion queue
    swapchainDeletionQueue.flush();

    // delete objects created after logical device creation
    mainDeletionQueue.flush();

    // destroy swapchain
    vkDestroySwapchainKHR(device, swapchain, nullptr);

    // destroy logical device
    vkDestroyDevice(device, nullptr);

    // destroy surface
    vkDestroySurfaceKHR(instance, surface, nullptr);

    // destroy debug messenger if validation enabled
    if(validationEnabled){
        if(debugMessenger.destroy(instance) != SUCCESS){
            std::cerr << "[ERROR] failed to destroy debug messenger";
        }
    }

    // destroy instance
    vkDestroyInstance(instance, nullptr);
    instance = VK_NULL_HANDLE;
}

// create vulkan instance for renderer
void Renderer::createInstance(){
    // get required instance layer and extensions
    std::vector<const char*> layerNames, extNames;
    getExtensionsAndLayers(window, layerNames, extNames);

    // get debug messenger create info and pass it as pnext in instance create info
    // this will create a debug messenger before creating instance
    // this will help us log instance creation and destruction
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = DebugMessenger::defaultCreateInfo();

    // give information to vulkan on how to create instance
    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = STYPE(INSTANCE_CREATE_INFO),
        .pNext = &debugCreateInfo,
        .pApplicationInfo = nullptr,
        .enabledLayerCount = static_cast<uint32_t>(layerNames.size()),
        .ppEnabledLayerNames = layerNames.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extNames.size()),
        .ppEnabledExtensionNames = extNames.data(),
    };

    // create instance
    VKCHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));
}

// create surface for the given window
void Renderer::createSurface(){
    // this function returns SDL_FALSE on failure
    if(SDL_Vulkan_CreateSurface(window, instance, &surface) == SDL_FALSE){
        std::cerr << "[ERROR] Failed to create rendering surface for window. "
                  << SDL_GetError(); // reason
        exit(1);
    }
}

// select physical device
// just select the first device in the list that satisfies all requirements
// in future, we'd add a method to change device from gui itself
void Renderer::selectPhysicalDevice(){
    // get physical devices
    std::vector<VkPhysicalDevice> physicalDevices;
    ReturnCode retCode = enumeratePhysicalDevices(instance, physicalDevices);
    if(retCode == FAILED){
        std::cerr << "[ERROR] Failed to enumerate physical devices on host" << std::endl;
        exit(1);
    }

    // check for compatibility
    for(VkPhysicalDevice phyDev : physicalDevices){
        // get physical device information
        VkPhysicalDeviceProperties phyDevProperties;
        vkGetPhysicalDeviceProperties(phyDev, &phyDevProperties);

        bool deviceIsSuitable = false;

        // get queue family data
        queueFamilyData = QueueFamilyData(phyDev, surface);
        if((queueFamilyData.graphicsQueueIdx == -1) ||
           (queueFamilyData.presentQueueIdx == -1) ||
           (queueFamilyData.transferQueueIdx == -1)){
            // device is not suitable since the required queue families are not present
            deviceIsSuitable = false;
        }else{
            // device is suitable since the required queue families are present
            deviceIsSuitable = true;
        }

        // check if desired extensions are available
        if(deviceIsSuitable){
            if(checkDeviceExtensionSupport(phyDev, deviceExtensions) == SUCCESS)
                deviceIsSuitable = true;
            else deviceIsSuitable = false;
        }

        // check for surface present modes
        if(deviceIsSuitable){
            // get surface present mode count
            uint32_t count = 0;
            VKCHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(phyDev, surface, &count, nullptr));

            // check if present modes are present or not
            if(count == 0){
                deviceIsSuitable = false;
            }
        }

        // check for surface formats
        if(deviceIsSuitable){
            // get surface format count
            uint32_t count = 0;
            VKCHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(phyDev, surface, &count, nullptr));

            // check if formats are present or not
            if(count == 0){
                deviceIsSuitable = false;
            }
        }

        // if still device is suitable then select this device and exit
        if(deviceIsSuitable){
            physicalDevice = phyDev;
            break;
        }else{
            std::cout << "[INFO] Physical device \"" << phyDevProperties.deviceName << "\" failed test" << std::endl;
        }
    }

    // check if we got our device or not
    if(physicalDevice == VK_NULL_HANDLE){
        std::cerr << "[ERROR] Failed to find any compatible Physical Device on host" << std::endl;
        exit(-1);
    } else {
        // get physical device information
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        // print device name
        std::cout << "[INFO] Selected Physical Device "
                  <<"\"" << physicalDeviceProperties.deviceName << "\"" << std::endl;
    }
}

// create device
void Renderer::createLogicalDevice(){
    // queue priority of all the queues created from a queue family
    // for now we have only one queue so...
    float qPriority = 1.f;

    // default queue create info
    VkDeviceQueueCreateInfo queueCreateInfo = {
        .sType = STYPE(DEVICE_QUEUE_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .queueCount = 1,
        .pQueuePriorities = &qPriority
    };

    std::set<uint32_t> uniqueQueueIndices = {
        static_cast<uint32_t>(queueFamilyData.graphicsQueueIdx),
        static_cast<uint32_t>(queueFamilyData.presentQueueIdx),
        static_cast<uint32_t>(queueFamilyData.transferQueueIdx)
    };

    // create queue create infos for unique queues
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};
    for(uint32_t idx : uniqueQueueIndices){
        queueCreateInfo.queueFamilyIndex = idx;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // device create info
    VkDeviceCreateInfo createInfo = {
        .sType = STYPE(DEVICE_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
        .pEnabledFeatures = nullptr
    };

    // create device
    VKCHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device));

    // get graphics and surface support queue
    vkGetDeviceQueue(device, queueFamilyData.graphicsQueueIdx, 0, &graphicsQueue);
    vkGetDeviceQueue(device, queueFamilyData.presentQueueIdx, 0, &presentQueue);
    vkGetDeviceQueue(device, queueFamilyData.transferQueueIdx, 0, &transferQueue);
}

void Renderer::createAllocator(){
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.device = device;
    allocatorInfo.instance = instance;
    allocatorInfo.physicalDevice = physicalDevice;
    VKCHECK(vmaCreateAllocator(&allocatorInfo, &allocator));

    mainDeletionQueue.push_function([=](){
        vmaDestroyAllocator(allocator);
    });
}

uint32_t clamp(uint32_t x, uint32_t y, uint32_t z){
    if(x>z) return z;
    if(x<y) return y;
    return x;
}

// create swapchain
void Renderer::createSwapchain(){
    // get surface details
    getSurfaceDetails(physicalDevice, surface, surfaceDetails);

    // get swapchain image extent
    int32_t w, h;
    SDL_GetWindowSize(window, &w, &h);

    swapchainImageExtent.width = clamp(w, surfaceDetails.capabilities.minImageExtent.width, surfaceDetails.capabilities.maxImageExtent.width);
    swapchainImageExtent.height = clamp(h, surfaceDetails.capabilities.minImageExtent.height, surfaceDetails.capabilities.maxImageExtent.height);

    // get min image count
    uint32_t swapchainMinImgCount = surfaceDetails.capabilities.minImageCount + 1;
    // max image count = 0 means no upper limit
    if(swapchainMinImgCount > surfaceDetails.capabilities.maxImageCount &&
        surfaceDetails.capabilities.maxImageCount != 0){
        swapchainMinImgCount = surfaceDetails.capabilities.maxImageCount;
    }

    // select surface format
    // by default select the first in the list
    swapchainImageFormat = surfaceDetails.formats[0];
    for(const auto& sf : surfaceDetails.formats){
        if(sf.format == VK_FORMAT_B8G8R8A8_SRGB &&
           sf.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
            swapchainImageFormat = sf;
        }
    }

    // just use fifo for now as it's guaranteed to be present
    // also fifo doesn't have tearing
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

    // queue family indices and count
    std::vector<uint32_t> queueFamilyIndices = {
        static_cast<uint32_t>(queueFamilyData.graphicsQueueIdx)
    };

    // sharing mode
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if(queueFamilyData.graphicsQueueIdx != queueFamilyData.presentQueueIdx){
        sharingMode = VK_SHARING_MODE_CONCURRENT;
        queueFamilyIndices.push_back(queueFamilyData.presentQueueIdx);
    }

    // make use of already created swapchain
    VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE;
    if(swapchain != VK_NULL_HANDLE)
        oldSwapchain = swapchain;

    
    // swapchain create info
    VkSwapchainCreateInfoKHR createInfo = {
        .sType = STYPE(SWAPCHAIN_CREATE_INFO_KHR),
        .pNext = nullptr,
        .flags = 0,
        .surface = surface,
        .minImageCount = swapchainMinImgCount,
        .imageFormat = swapchainImageFormat.format,
        .imageColorSpace = swapchainImageFormat.colorSpace,
        .imageExtent = swapchainImageExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = sharingMode,
        .queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size()),
        .pQueueFamilyIndices = queueFamilyIndices.data(),
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE, // NOTE : Notorius setting here
        .oldSwapchain = oldSwapchain
    };

    // create swapchain
    VKCHECK(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain));

    // no need to destroy old swapchain
    // destroying it means destroying images too
    // as the previous images are reused here!

    // get swapchain images
    ReturnCode retCode = getSwapchainImages(device, swapchain, swapchainImages);
    if(retCode == SUCCESS){
        swapchainImageCount = swapchainImages.size();
    }else{
        std::cerr << "[ERROR] Failed to get swapchain images" << std::endl;
        exit(1);
    }

    // swapchain will be explicitly destroyed
}

// create depth image
void Renderer::createDepthImage(){
    VkExtent3D depthImageExtent {
        swapchainImageExtent.width,
        swapchainImageExtent.height,
        1
    };

    depthImageFormat = VK_FORMAT_D32_SFLOAT;

    // get default iamge create info
    VkImageCreateInfo depthImageInfo = defaultImageCreateInfo(depthImageFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

    // allocation info
    VmaAllocationCreateInfo allocInfo {
        .usage = VMA_MEMORY_USAGE_GPU_ONLY,
        .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    };

    // create image
    VKCHECK(vmaCreateImage(allocator, &depthImageInfo, &allocInfo, &depthImage.image, &depthImage.allocation, nullptr));

    // build an image-view for the depth image to use for rendering
    VkImageViewCreateInfo depthImageViewInfo = defaultImageViewCreateInfo(depthImageFormat, depthImage.image, VK_IMAGE_ASPECT_DEPTH_BIT);

    // create image view
    VKCHECK(vkCreateImageView(device, &depthImageViewInfo, nullptr, &depthImageView));

    // add to deletion queues
    swapchainDeletionQueue.push_function([=]() {
        vkDestroyImageView(device, depthImageView, nullptr);
        vmaDestroyImage(allocator, depthImage.image, depthImage.allocation);
    });
}

// create image views for images in swapchain
void Renderer::createImageViews(){
    VkComponentMapping componentMapping{
        .r = VK_COMPONENT_SWIZZLE_R,
        .g = VK_COMPONENT_SWIZZLE_G,
        .b = VK_COMPONENT_SWIZZLE_B,
        .a = VK_COMPONENT_SWIZZLE_A,
    };

    VkImageSubresourceRange subresourceRange{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
    };

    VkImageViewCreateInfo createInfo = {
        .sType = STYPE(IMAGE_VIEW_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .image = VK_NULL_HANDLE,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = swapchainImageFormat.format,
        .components = componentMapping,
        .subresourceRange = subresourceRange
    };

    // resize image views vector
    swapchainImageViews.resize(swapchainImageCount);

    // create image views
    for(uint32_t i = 0; i < swapchainImageCount; i++){
        createInfo.image = swapchainImages[i];
        VKCHECK(vkCreateImageView(device, &createInfo, nullptr, &swapchainImageViews[i]));
    }

    // add to deletion queue
    swapchainDeletionQueue.push_function([=](){
        // destroy image views
        for(VkImageView imageView : swapchainImageViews){
            vkDestroyImageView(device, imageView, nullptr);
        }
    });
}

// create command pool
void Renderer::createCommandPool(){
    // common command pool create info
    VkCommandPoolCreateInfo commandPoolInfo =
        defaultCommandPoolCreateInfo(queueFamilyData.graphicsQueueIdx,
                                     VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    // for each frame in flight
    for(int i = 0; i < bufferingSize; i++){
        // create command pool
        VKCHECK(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &frames[i].commandPool));

        // add to deletion queue
        mainDeletionQueue.push_function([=](){
            vkDestroyCommandPool(device, frames[i].commandPool, nullptr);
        });
    }

    // create command pool for transfer operation
    commandPoolInfo.queueFamilyIndex = queueFamilyData.transferQueueIdx;
    VKCHECK(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &transferCommandPool));


    // add to deletion queue
    mainDeletionQueue.push_function([=](){
        vkDestroyCommandPool(device, transferCommandPool, nullptr);
    });
}

// allocate command buffers for each frame
void Renderer::allocateCommandBuffers(){
    for(uint32_t i = 0; i < bufferingSize; i++){
        //allocate the default command buffer that we will use for rendering
        VkCommandBufferAllocateInfo cmdAllocInfo = defaultCommandBufferAllocateInfo(frames[i].commandPool, 1);
        // allocate cmd buffers
        VKCHECK(vkAllocateCommandBuffers(device, &cmdAllocInfo, &frames[i].commandBuffer));
    }

    // allocate command buffer for transfer operations
    VkCommandBufferAllocateInfo cmdAllocInfo = defaultCommandBufferAllocateInfo(transferCommandPool, 1);
    VKCHECK(vkAllocateCommandBuffers(device, &cmdAllocInfo, &transferCommandBuffer));
}

// create command pool and allocate command buffers for each frame in flight
void Renderer::initCommands(){
    createCommandPool();
    allocateCommandBuffers();
}

// copy buffer from cpu to gpu
void Renderer::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = STYPE(COMMAND_BUFFER_BEGIN_INFO);
    beginInfo.pNext = nullptr;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    // shorter name
    VkCommandBuffer& cmd = transferCommandBuffer;

    // begin recording command buffer
    VKCHECK(vkBeginCommandBuffer(cmd, &beginInfo));

    // copy info
    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;

    // copy command
    vkCmdCopyBuffer(cmd, src, dst, 1, &copyRegion);

    // end recording
    VKCHECK(vkEndCommandBuffer(cmd));

    // submit command buffer
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = STYPE(SUBMIT_INFO);
    submitInfo.pNext = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &transferCommandBuffer;
    // we are not waiting or signalling any other operation

    // submit
    vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(transferQueue);

    // reset command buffer so that it can be used again
    VKCHECK(vkResetCommandBuffer(transferCommandBuffer, 0));
}

// initialize a default renderpass
void Renderer::initRenderPass(){
    // will define the properties of image we'll render to
    VkAttachmentDescription colorAttachment = {
        .flags = 0,
        .format = swapchainImageFormat.format,
        .samples = VK_SAMPLE_COUNT_1_BIT, // no multisampling
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, // clear screen when renderpass begines
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE, // store image when renderpass ends
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, // we don't know initial layout
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR // we want image to be presentable
    };

    // references are used by subpasses for getting images
    VkAttachmentReference colorAttachmentRef {
        // index of attachment in VkRenderPassCreateInfo::pAttachments array
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    // attachment description for depth image
    VkAttachmentDescription depthAttachment {
        .flags = 0,
        .format = depthImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .stencilStoreOp =  VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    // attachment reference
    VkAttachmentReference depthAttachmentRef {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkAttachmentDescription attachments[2] = {colorAttachment, depthAttachment};

    // subpass within renderpass
    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
        .pDepthStencilAttachment = &depthAttachmentRef
    };

    // render pass data
    VkRenderPassCreateInfo createInfo = {
        .sType = STYPE(RENDER_PASS_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .attachmentCount = 2,
        .pAttachments = attachments,
        .subpassCount = 1,
        .pSubpasses = &subpass
    };

    VKCHECK(vkCreateRenderPass(device, &createInfo, nullptr, &renderPass));

    // destroy renderpass
    swapchainDeletionQueue.push_function([=](){
        vkDestroyRenderPass(device, renderPass, nullptr);
    });
}

// create frambuffers for rendering images into them
void Renderer::createFramebuffers(){
    VkFramebufferCreateInfo createInfo = {
        .sType = STYPE(FRAMEBUFFER_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .renderPass = renderPass,
        .attachmentCount = 2,
        .width = swapchainImageExtent.width,
        .height = swapchainImageExtent.height,
        .layers = 1
    };

    // resize
    framebuffers.resize(swapchainImageCount);

    // create framebuffers
    for(uint32_t i = 0; i < swapchainImageCount; i++){
        VkImageView attachments[2] = {swapchainImageViews[i], depthImageView};
        createInfo.pAttachments = attachments;
        VKCHECK(vkCreateFramebuffer(device, &createInfo, nullptr, &framebuffers[i]));
    }

    // add to deletion queue
    swapchainDeletionQueue.push_function([=](){
        // destroy framebuffers
        for(uint32_t i = 0; i < framebuffers.size(); i++)
            vkDestroyFramebuffer(device, framebuffers[i], nullptr);
    });
}

void Renderer::initSyncStructures(){
    for(auto& frame : frames){
        VkSemaphoreCreateInfo semaphoreCreateInfo = defaultSemaphoreCreateInfo();

        VKCHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &frame.renderSemaphore));
        VKCHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &frame.presentSemaphore));

        // we want to create the fence with the Create Signaled flag, so we can
        // wait on it before using it on a GPU command (for the first frame)
        VkFenceCreateInfo fenceCreateInfo = defaultFenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

        // create fence
        VKCHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &frame.renderFence));

        // destruction
        mainDeletionQueue.push_function([=](){
            vkDestroySemaphore(device, frame.renderSemaphore, nullptr);
            vkDestroySemaphore(device, frame.presentSemaphore, nullptr);
            vkDestroyFence(device, frame.renderFence, nullptr);
        });
    }
}

// load mesh
void Renderer::loadMeshes(){
    mesh.loadFromObj("../assets/InteriorTest.obj");
    uploadMesh(mesh);

    meshes["car"] = mesh;

    glm::vec3 pos = {-10, 0, 10};
    glm::vec3 scale = {4, 4, 4};
    glm::vec3 rotAxis = {1, 0, 0};
    float rotAngle = 0;

    RenderObject obj(getMesh("car"), getMaterial("defaultMaterial"),
                     pos, scale, rotAxis, rotAngle);
    renderables.push_back(obj);


    // change object attributes
    // rotAxis = {0, 1, 1};
    // rotAngle = 37;
    // pos = {40, 0, -40};
    // obj.setRotation(rotAxis, rotAngle);
    // obj.setScale({1, 1, 1});
    // obj.setPosition(pos);
    // renderables.push_back(obj);

    // create sphere mesh
    meshes["sphere"] = Mesh{};
    Mesh& sphere = meshes["sphere"];

    // create sphere mesh
    uint32_t slices = 100, circles = 100;
    float radius = 1.f;
    createSphereMesh(sphere, slices, circles, radius, {1, 0.2, 0.8});

    // upload mesh data to gpu
    uploadMesh(sphere);

    // create renderable object
    pos = {0, 5, 0};
    RenderObject sphereObj(getMesh("sphere"), getMaterial("defaultMaterial"));
    sphereObj.setPosition(pos);

    renderables.push_back(sphereObj);

    // create a plane
    meshes["plane"] = Mesh{};
    Mesh& plane = meshes["plane"];

    float width = 100;
    float height = 100;
    createRectangleMesh(plane, width, height);

    // upload mesh data
    uploadMesh(plane);

    // create renderable object
    RenderObject planeObj(getMesh("plane"), getMaterial("defaultMaterial"));
    planeObj.setRotation({1, 0 ,0}, -90); // rotate about x axis 90 degrees
    planeObj.setPosition({0, -10, 0});

    renderables.push_back(planeObj);
};

void Renderer::uploadMesh(Mesh &mesh) {
    // calculate buffer size
    size_t bufferSize = mesh.vertices.size() * sizeof(Vertex);

    // create staging buffer (in cpu ram)
    AllocatedBuffer stagingBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    // move last pushed function
    mainDeletionQueue.deletors.pop_back();

    //copy vertex data to staging buffer
    void* data;
    vmaMapMemory(allocator, stagingBuffer.allocation, &data);
    memcpy(data, mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex));
    vmaUnmapMemory(allocator, stagingBuffer.allocation);

    // create vertex buffer (in gpu ram)
    mesh.vertexBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

    // copy data from staging buffer (in cpu ram) to vertex buffer (in gpu ram)
    copyBuffer(stagingBuffer.buffer, mesh.vertexBuffer.buffer, bufferSize);

    // destroy staging buffer
    vmaDestroyBuffer(allocator, stagingBuffer.buffer, stagingBuffer.allocation);
}

// draw on screen
void Renderer::draw(){
    // update camera data every frame
    // camera data is modified per frame in the main loop depending on the events triggered
    void* data;
    vmaMapMemory(allocator, getCurrentFrame().cameraBuffer.allocation, &data);
    memcpy(data, &cameraData, sizeof(GPUCameraData));
    vmaUnmapMemory(allocator, getCurrentFrame().cameraBuffer.allocation);

    // wait for 1 seconds max
    uint64_t timeout = 1e9;

    // get current frame to render image to
    FrameData& currentFrame = getCurrentFrame();

    // wait for gpu to finish rendering and signal us on render fence
    // timeout is 1 second
    VKCHECK(vkWaitForFences(device, 1, &currentFrame.renderFence, VK_TRUE, timeout));
    // fence must be reset before use again
    VKCHECK(vkResetFences(device, 1, &currentFrame.renderFence));

    // get image index
    uint32_t swapchainImageIndex;
    VkResult res = vkAcquireNextImageKHR(device, swapchain, timeout, currentFrame.presentSemaphore, VK_NULL_HANDLE, &swapchainImageIndex);

    do {
        if(res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || framebufferResized){
            // swapchain is no longer compatible
            recreateSwapchain();
            framebufferResized = false;

            // acquire image index again
            // previous swapchain image index is invalid
            res = vkAcquireNextImageKHR(device, swapchain, timeout, currentFrame.presentSemaphore, VK_NULL_HANDLE, &swapchainImageIndex);
        }else if(res != VK_SUCCESS){
            std::cerr << "[ERROR] Failed to get next image index" << std::endl;
            abort();
        }
    } while((res == VK_ERROR_OUT_OF_DATE_KHR) || (res == VK_SUBOPTIMAL_KHR) || res != VK_SUCCESS);

    // now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
    VKCHECK(vkResetCommandBuffer(currentFrame.commandBuffer, 0));

    // shorter name
    VkCommandBuffer& cmd = currentFrame.commandBuffer;

    // we'll use command buffer extactly one, record, submit and reset
    VkCommandBufferBeginInfo beginInfo{
        .sType = STYPE(COMMAND_BUFFER_BEGIN_INFO),
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr
    };

    // begin command buffer recording
    VKCHECK(vkBeginCommandBuffer(cmd, &beginInfo));

    // set a clear value to clear the screen with
    VkClearValue colorClear{
        .color = VkClearColorValue{
            .float32 = {0.01, 0.01, 0.01, 1.f} // dark gray
        }
    };

    // depth clear value
    VkClearValue depthClear{
        .depthStencil{
            .depth = 1.f
        }
    };

    // clear values
    VkClearValue clearValues[2] = {colorClear, depthClear};

    // render pass begin info
    VkRenderPassBeginInfo rpBeginInfo{
        .sType = STYPE(RENDER_PASS_BEGIN_INFO),
        .pNext = nullptr,
        .renderPass = renderPass,
        .framebuffer = framebuffers[swapchainImageIndex],
        .renderArea = {{0,0}, swapchainImageExtent},
        .clearValueCount = 2,
        .pClearValues = clearValues
    };

    // begin render pass
    vkCmdBeginRenderPass(cmd, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // bind descriptor set
    // this is to send camera data per frame
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, meshPipelineLayout, 0, 1, &currentFrame.globalDescriptorSet, 0, nullptr);

    // // get updated swapchain image extent
    // int w,h;
    // SDL_GetWindowSize(window, &w, &h);
    // swapchainImageExtent.width = w;
    // swapchainImageExtent.height = h;

    // set dynamic viewport
    VkViewport viewport = {
        .x = 0.f,
        .y = 0.f,
        .width = static_cast<float>(swapchainImageExtent.width),
        .height = static_cast<float>(swapchainImageExtent.height),
        .minDepth = 0.f,
        .maxDepth = 1.f
    };
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    // scissor to cut off part we don't need to render
    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = swapchainImageExtent
    };
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    // draw renderables
    drawObjects(cmd, renderables.data(), renderables.size());

    // end renderpass
    vkCmdEndRenderPass(cmd);

    // end commnad buffer recording
    VKCHECK(vkEndCommandBuffer(cmd));

    // submit commands to render queue now

    // at what stage to start rendering
    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    // prepare the submission to the queue.
    // we want to wait on the presentSemaphore, as that semaphore is signaled when the swapchain is ready
    // we will signal the renderSemaphore, to signal that rendering has finished
    VkSubmitInfo submitInfo{
      .sType = STYPE(SUBMIT_INFO),
      .pNext = nullptr,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &currentFrame.presentSemaphore,
      .pWaitDstStageMask = &waitStage,
      .commandBufferCount = 1,
      .pCommandBuffers = &cmd,
      .signalSemaphoreCount = 1,
      .pSignalSemaphores = &currentFrame.renderSemaphore
    };

    //submit command buffer to the queue and execute it.
    // renderFence will now block until the graphic commands finish execution
    VKCHECK(vkQueueSubmit(graphicsQueue, 1, &submitInfo, currentFrame.renderFence));

    // present info
    VkPresentInfoKHR presentInfo {
        .sType = STYPE(PRESENT_INFO_KHR),
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &currentFrame.renderSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &swapchainImageIndex,
        .pResults = nullptr
    };

    // submit for presentation to surface
    res = vkQueuePresentKHR(presentQueue, &presentInfo);
    if(res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || framebufferResized){
        // swapchain is no longer compatible
        recreateSwapchain();
        framebufferResized = false;
        return;
    }else if(res != VK_SUCCESS){
        std::cerr << "[ERROR] Failed to present rendered image" << std::endl;
        abort();
    }

    frameNumber++;
}

// initialize descriptor sets
void Renderer::initDescriptors(){
    // ALGORTIHM :
    // create descriptor pool (with different sizes for each descriptor set)
    // allocate descriptor sets (can allocate any number, can bind only 4 at a time)
    // define descriptor set layout for each descriptor set
    // pass these descriptor set layouts in pipeline layout

    // create a descriptor pool that'll hold 10 uniform buffers
    std::vector<VkDescriptorPoolSize> sizes = {
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10}
    };

    // descriptor pool create info
    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = STYPE(DESCRIPTOR_POOL_CREATE_INFO);
    poolInfo.flags = 0;
    // maximum 10 descriptor sets
    poolInfo.maxSets = 10;
    // total number of descriptors that can be allocated by all descriptor sets combined
    poolInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
    poolInfo.pPoolSizes = sizes.data();

    // create descriptor pool
    VKCHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool));

    // add to deletion queue
    mainDeletionQueue.push_function([=](){
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    });

    // give information about which binding to use to r/w data
    VkDescriptorSetLayoutBinding camDataBinding = {};
    // send data through binding 0
    camDataBinding.binding = 0;
    // each binding can send an array of descriptors
    // we are sending only one camera data and not an array
    camDataBinding.descriptorCount = 1;
    // what type of data will be r/w from this binding
    camDataBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // which shader stage will read from this binding?
    camDataBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    // give infromation about how the descriptors will be passed to shaders
    VkDescriptorSetLayoutCreateInfo setLayoutInfo = {};
    setLayoutInfo.sType = STYPE(DESCRIPTOR_SET_LAYOUT_CREATE_INFO);
    setLayoutInfo.pNext = nullptr;
    // no flags
    setLayoutInfo.flags = 0;
    // we are sending data through only one binding
    setLayoutInfo.bindingCount = 1;
    // point to camera buffer binding
    setLayoutInfo.pBindings = &camDataBinding;

    // create descriptor set layout
    VKCHECK(vkCreateDescriptorSetLayout(device, &setLayoutInfo, nullptr, &globalDescriptorSetLayout));

    // add to deletion queue
    mainDeletionQueue.push_function([=](){
        vkDestroyDescriptorSetLayout(device, globalDescriptorSetLayout, nullptr);
    });

    // allocate buffers, allocate one descriptor set per frame and write to each descriptor set with camera data
    for(uint32_t i = 0; i < bufferingSize; i++){
        // create buffer that the camera descriptor will point to
        frames[i].cameraBuffer = createBuffer(sizeof(GPUCameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

        // allocate descriptor set per frame data
        VkDescriptorSetAllocateInfo setAllocInfo = {};
        setAllocInfo.sType = STYPE(DESCRIPTOR_SET_ALLOCATE_INFO);
        setAllocInfo.pNext = nullptr;
        // which descriptor pool to use
        setAllocInfo.descriptorPool = descriptorPool;
        // only one descriptor set to allocate
        setAllocInfo.descriptorSetCount = 1;
        // what is the layout of each set?
        setAllocInfo.pSetLayouts = &globalDescriptorSetLayout;
        // allocate
        VKCHECK(vkAllocateDescriptorSets(device, &setAllocInfo, &frames[i].globalDescriptorSet));

        // the descriptor doesn't point to any data yet
        // for that we'll need to do a vkUpdateDescriptorSet which requries a write struct

        // data about which buffer to point to
        VkDescriptorBufferInfo bufferInfo = {};
        // point to this buffer
        bufferInfo.buffer = frames[i].cameraBuffer.buffer;
        // what offset in this buffer?
        // this means we can allocate a large buffer and write at different places
        bufferInfo.offset = 0; // beginning
        // size of buffer after offset
        bufferInfo.range = sizeof(GPUCameraData);

        // where, how and what to write?
        VkWriteDescriptorSet setWrite = {};
        setWrite.sType = STYPE(WRITE_DESCRIPTOR_SET);
        setWrite.pNext = nullptr;
        // which binding to write to?
        // so if we had 10 bindings, then this will change correspondingly!
        setWrite.dstBinding = 0;
        // and which set to write to
        setWrite.dstSet = frames[i].globalDescriptorSet;
        // how many descriptors to write to ( one buffer is written to one descriptor )
        setWrite.descriptorCount = 1; // same as size of pBufferInfo
        // what tyoe of descriptor to write to?
        setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // infos of buffers to point to
        setWrite.pBufferInfo = &bufferInfo;

        // update descriptor sets
        vkUpdateDescriptorSets(device, 1, &setWrite, 0, nullptr);

        // next we just need to update the buffers as the descriptors point to that buffer
        // after updating the buffers we can bind these descriptor sets and then run the draw commands
    }
}

// create graphics pipeline
void Renderer::initGraphicsPipeline(){
    // load fragmment shader
    if(loadShaderModule("../shaders/compiled/mesh_shader.frag.spv", device, meshFS) != SUCCESS){
        std::cerr << "[ERROR] Failed to create fragment shader module" << std::endl;
    }

    // load vertex shader
    if(loadShaderModule("../shaders/compiled/mesh_shader.vert.spv", device, meshVS) != SUCCESS){
        std::cerr << "[ERROR] Failed to create vertex shader module" << std::endl;
    }

    // build the pipeline layout that controls the inputs/outputs of the shader
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = defaultPipelineLayoutCreateInfo();

    // setup push constants
    // this push constant will be sent at vertex stage
    VkPushConstantRange pushConstant = {};
    pushConstant.offset = 0;
    pushConstant.size = sizeof(MeshPushConstants);
    pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstant;

    // using only one descriptor set for now, for camera data
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &globalDescriptorSetLayout;

    // create pipeline layout
    VKCHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &meshPipelineLayout));

    // set shader stages
    pipelineBuilder.shaderStages = {
        defaultPipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, meshVS),
        defaultPipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, meshFS)
    };

    // get vertex input binding descriptions and attribute descriptions
    // make this static as we'll need to create pipeline again and builder
    // stores only pointer to this!
    static VertexInputDescription vertexDescription = Vertex::getVertexDescription();

    // vertex input controls how vertex is read from vertex buffers
    pipelineBuilder.vertexInputInfo = defaultPipelineVertexInputStateCreateInfo();
    // connect the pipeline builder vertex input info to the one we get from Vertex
    pipelineBuilder.vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();
    pipelineBuilder.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexDescription.attributes.size());
    pipelineBuilder.vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();
    pipelineBuilder.vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexDescription.bindings.size());

    // input assembly is how to use given input and assemble to draw something
    // we are use input array as a list triangles
    pipelineBuilder.inputAssembly = defaultPipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    /*
     * Dynamic is using dynamic viewport and scissors at the moment.
     * This means that below data will be ignored.
     * Also, it is updating viewport and scissor every frame.

     // defines visible volume
     pipelineBuilder.viewport = {
     .x = 0.f,
     .y = 0.f,
     .width = static_cast<float>(swapchainImageExtent.width),
     .height = static_cast<float>(swapchainImageExtent.height),
     .minDepth = 0.f,
     .maxDepth = 1.f
     };

     // scissor to cut off part we don't need to render
     pipelineBuilder.scissor = {
     .offset = {0, 0},
     .extent = swapchainImageExtent
     };
     */

    // dynamic by default uses dynamic viewport and scissor states
    std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    pipelineBuilder.dynamicState = defaultPipelineDynamicStateCreateInfo(dynamicStateEnables);

    // configure rasterizer to draw filled triangles
    pipelineBuilder.rasterizer = defaultPipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);

    // multisampling and antialiasing is off
    pipelineBuilder.multisampling = defaultPipelineMultisampleStateCreateInfo();

    // depth stencil state create info
    pipelineBuilder.depthStencil = defaultPipelineDepthStencilStateCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);

    // single blend attachment with no blending and direct write to rgba
    pipelineBuilder.colorBlendAttachment = defaultPipelineColorBlendAttachmentState();

    // triangle pipeline layout
    pipelineBuilder.pipelineLayout = meshPipelineLayout;

    // build pipeline
    meshPipeline = pipelineBuilder.buildPipeline(device, renderPass);
    assert(meshPipeline && "FAILED TO CREATE PIPELINE");

    // destroy shader modules
    vkDestroyShaderModule(device, meshVS, nullptr);
    vkDestroyShaderModule(device, meshFS, nullptr);

    // these objects will be destroyed in the end
    mainDeletionQueue.push_function([=](){
        // destroy pipeline layout
        vkDestroyPipelineLayout(device, meshPipelineLayout, nullptr);
        // destroy pipeline
        vkDestroyPipeline(device, meshPipeline, nullptr);
    });

    createMaterial(meshPipeline, meshPipelineLayout, "defaultMaterial");
}

// recreate swapchain on window resize
void Renderer::recreateSwapchain(){
    // wait for all device operations to complete
    vkDeviceWaitIdle(device);

    // first create new swapchain
    // this will make use of current swapchain to create a new one
    createSwapchain();

    // cleanup relevan objects
    cleanupSwapchain();

    // create relevant objects
    createImageViews();
    createDepthImage();
    initRenderPass();
    createFramebuffers();
}

// cleanup vulkan structures that need to recreated for every swapchain recreate call
void Renderer::cleanupSwapchain(){
    // reset command pool
    for(uint32_t i = 0; i < bufferingSize; i++){
        vkResetCommandPool(device, frames[i].commandPool, 0);
    }

    // no need to destroy old swapchain as we'll be using it
    // for new swapchain creation and this means most of the resources will be reused
    // this means images and other things too
    // if we destroy old swapchain then the images and other resources will get destroyed too!

    // execute all deletors on swapchain deletion queue
    swapchainDeletionQueue.flush();
}

// when window is resized
void Renderer::windowResized(){
    recreateSwapchain();
}

// allocate buffer
AllocatedBuffer Renderer::createBuffer(size_t allocSize, VkBufferUsageFlags usageFlags, VmaMemoryUsage memoryUsage){
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = STYPE(BUFFER_CREATE_INFO);
    bufferInfo.pNext = nullptr;
    bufferInfo.flags = 0;
    bufferInfo.size = allocSize;
    bufferInfo.usage = usageFlags;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = memoryUsage;

    AllocatedBuffer allocatedBuffer;

    // allocate buffer
    VKCHECK(vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &allocatedBuffer.buffer, &allocatedBuffer.allocation, nullptr));

    // add to deletion queue
    mainDeletionQueue.push_function([=](){
        vmaDestroyBuffer(allocator, allocatedBuffer.buffer, allocatedBuffer.allocation);
    });

    return allocatedBuffer;
}

// create material and return address
Material* Renderer::createMaterial(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name){
    Material m;
    m.pipeline = pipeline;
    m.pipelineLayout = layout;

    materials[name] = m;
    return &materials[name];
}

// get material by name
Material* Renderer::getMaterial(const std::string& name){
    auto it = materials.find(name);
    if(it == materials.end()){
        return nullptr;
    }

    return &(*it).second;
}

// get mesh by name
Mesh* Renderer::getMesh(const std::string& name){
     auto it = meshes.find(name);
    if(it == meshes.end()){
        return nullptr;
    }

    return &(*it).second;
}

// draw a list of renderables
void Renderer::drawObjects(VkCommandBuffer cmd, RenderObject* first, size_t count){
    // store last mesh and last material to reduce total number of bindings in for loop
    Mesh* lastMesh = nullptr;
    Material* lastMaterial = nullptr;

    // data to be sent for every object
    MeshPushConstants pushConstants;

    for(size_t i = 0; i < count; i++){
        // get object data to be drawn
        RenderObject& object = first[i];

        // bind new pipeline if and only if it doesn't match the previous one
        if(object.getMaterial() != lastMaterial){
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.getMaterial()->pipeline);
            lastMaterial = object.getMaterial();
        }

        // send object model matrix for every object
        pushConstants.objectModelMatrix = object.getModelMatrix();
        vkCmdPushConstants(cmd, meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &pushConstants);

        // bind mesh only if it's different from last one
        if(object.getMesh() != lastMesh){
            VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(cmd, 0, 1, &object.getMesh()->vertexBuffer.buffer, &offset);
            lastMesh = object.getMesh();
        }

        // finally draw this object
        vkCmdDraw(cmd, object.getMesh()->vertices.size(), 1, 0, 0);
    }
}
