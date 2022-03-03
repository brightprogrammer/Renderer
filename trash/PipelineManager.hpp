#ifndef PIPELINE_MANAGER_HPP
#define PIPELINE_MANAGER_HPP

#include "Common.hpp"
#include "PipelineConfig.hpp"
#include "Utils/ResourceManager.hpp"

struct PipelineManager{
    /**
     * @brief Create a new pipeline manager for the given device.
     * @param[in] device that this pipeline manager will create pipelines for.
     */
    PipelineManager(VkDevice device);

    /**
     * @brief Get pipeline handle by pipeline name.
     *        Pipeline name is same as pipeline config name.
     * @param[in] name The name of the pipeline config used to create pipeline.
     * @return VkPipeline handle. Program crashes if no such pipeline exists.
     */
    VkPipeline getPipeline(const char* name);

    /**
     * @brief Get pipeline config by name.
     * @param[in] name of the pipeline config.
     * @return Pointer to queried pipeline config. Program crashes if wrong resource is queried.
     */
    PipelineConfig* getConfig(const char* name);

    /**
     * @brief Add a new config to pipeline manager.
     * @param [in] configName will be used to query the given config in future.
     * @param [in] config is const reference to new PipelineConfig.
     */
    void addConfig(const char* configName, const PipelineConfig& config);

    /**
     * @brief Build render pipeline after filling all information in the builder.
     * @param[in] renderPass to create pipeline for.
     * @return VkPipeline handle of created pipeline.
     */
    VkPipeline buildPipeline(const char* configName);

    /**
     * @brief Build all pipelines using all the configs in pipeline configs.
     */
    void buildPipelines();

    /**
     * @brief Destory pipeline.
     * @param[in] pipelineName
     */
    void destroyPipeline(const char* pipelineName);

    /**
     * @brief Delete a config that was added previously.
     * @param[in] cfgName is the name of config to be deleted.
     */
    void deleteConfig(const char* cfgName);

    // all pipeline configs managed by resource manager
    ResourceManager<PipelineConfig> configs;

    // pipelines
    ResourceManager<VkPipeline> pipelines;
private:
    // pipeline destructor callback for 
    static void pipelineDestructorCallback(VkPipeline pipeline, void* userData);

    // Each pipeline manager handles pipelines for a single device.
    VkDevice device;

    // create pipeline create info using the given config
    void preparePipelineCreateInfo(const PipelineConfig* config, VkGraphicsPipelineCreateInfo& createInfo);
};

#endif//PIPELINE_MANAGER_HPP
