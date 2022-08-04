# DYNAMIC DESIGN IDEAS AND RESOURCES

#### Designing Render Systems
Implement multiple render systems with different `VkPipleines` and shaders.
So the basic renderer we have now will have to be separated as an independent render system and
there will be multiple render systems. Each render system will draw meshes differently.

A render pass contains multiple subpasses (atleast one). Each subpass will use a different pipeline. Since each pipeline corresponds to a different method of rendering, we'll have to create subpasses corresponding to different render systems. These subpasses will be utilized by renderpass.

RenderPass is collection of attachment descriptions. Each attachment description describes what happens with each image in framebuffer.
Subpassess refer to these attachments and pipelines specify which subpass
they are made for. This means subpasses specify which image attachment
will be used and how it will be loaded or stored after rendering is done.

We can create two different system. One is RenderPassSystem and one is RenderSystem itself. RenderPass system will make use of RenderSystem to
create multiple subpasses and all. So we can create RenderSystems for different effects too and RenderPass can utilize these effects which paves the way for post processing effects.

After this there will be CommandBuffer interface to record draw commands.

Also all the basic handles like VkInstance, VkDevice, VkPhysicalDevice, Window, VkSwapchainKHR
etc... are to separated in a separate `Context` struct.
