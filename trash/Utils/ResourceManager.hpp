/**
 * @file      ResourceManager.hpp
 * @brief     Manages resources and provides methods to easily get those resources.
 * @date      Thu Dec  2 10:14:11 2021
 * @author    Siddharth Mishra
 */

#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <vector>
#include <cstring>
#include <utility>
#include <array>

template<typename ResourceType>
class ResourceManager{
public:
    /// resource destructor callback function pointer typedef
    typedef void(*PFN_resoureDestructorCallback)(ResourceType& resource, void* userData);

    /// resource information struct
    struct Resource{
        const char* name;
        ResourceType handle;
    };

    /**
     * @brief Create a resource manager.
     * @param[in] resoureDestructorCallback A pointer to resource destructor function.
     */
    ResourceManager(PFN_resoureDestructorCallback resourceDestructor);

    /**
     * @brief Release all resources.
     * @param[in] userData to passed to resource destructor callback.
     */
    void releaseResources(void* userData);

   /**
    * @brief Add a new resource to the resource manager. If resource already
    * exists then the previous one is destroyed and then assigned to the new one.
    * @param[in] resourceName that'll be used to get the resource in future.
    * @param[in] resource to be added
    * @param[in] Optional. This will be given to destructor for destroying previous
    * resource if a resource with similar name exists.
    */
    void add(const char* resourceName, const ResourceType& resource, void* userData = nullptr);

    /**
     * @brief Get a previously added resource.
     * @param[in] resourceName.
     * @return Pointer to resource if found or nullptr if not found.
     */
    ResourceType* get(const char* resourceName);

    /**
     * @brief Discard (destroy) a resource that is no longer needed.
     * @param[in] resourceName of resource to be discarded.
     * @param[in] userData is data to be passed to resource destructor callback.
     *   In case of vulkan, destructor functions require extra parameters. That
     *   extra parameters can pe passed as user data here and then be converted
     *   back to something that makes sense in the callback function and then
     *   be used in the destructor.
     */
    void discard(const char* resourceName, void* userData);

    /// resources data
    std::vector<Resource*> resources;
private:
    // resource destructor callback function.
    // this is called whenever we need to discard a resource.
    // this is to make sure that the resource is actually free'd and not
    // in system memory for long period of time.
    PFN_resoureDestructorCallback resourceDestructorCallback = nullptr;

    // maximum size of indices to store before a realloc happens in the resources vector.
    // this'll reduce the chance of frequent allocations and deallocations whenever a
    // reource is discarded.
    constexpr static inline size_t maxSizeBeforeRealloc = 20;

    // number of current discards done.
    // if this counter reaches maxSizeBeforeRealloc, then a realloc happens
    size_t numDiscards;

    // indices of resources that were free'd using discard
    // instead of using std::erase and functions that cause frequent memory allocations
    // and deallocations, we'd reather use the already allocated memory.
    std::array<size_t, maxSizeBeforeRealloc> freeIndices;
};

#include "ResourceManagerImpl.hpp"

#endif//RESOURCE_MANAGER_HPP
