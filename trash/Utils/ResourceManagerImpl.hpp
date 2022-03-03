/**
 * @file      ResourceManagerImpl.hpp
 * @brief     Implementation of ResourceManager
 * @date      Thu Dec  2 10:25:57 2021
 * @author    Siddharth Mishra
 */

#ifndef RESOURCE_MANAGER_IMPL_HPP
#define RESOURCE_MANAGER_IMPL_HPP

#include "ResourceManager.hpp"

#include <cstdlib>
#include <cassert>
#include <algorithm>

template <typename ResourceType>
ResourceManager<ResourceType>::ResourceManager(PFN_resoureDestructorCallback resourceDestructor){
    assert(resourceDestructor && "INVALID RESOURCE DESTRUCTOR CALLBACK FUNCTION USED TO CREATE RESOURCE MANAGER");
    resourceDestructorCallback = resourceDestructor;
}

// destroy
template <typename ResourceType>
void ResourceManager<ResourceType>::releaseResources(void* userData){
    for(Resource* res : resources){
        if(res->name) {
            // free name
            free((void*)res->name);
            res->name = nullptr;

            // destroy resource
            resourceDestructorCallback(res->handle, userData);
        }

        // free memory allocated for Resource struct
        free((void*)res);
    }
}

// add resource
template<typename ResourceType>
void ResourceManager<ResourceType>::add(const char* resName, const ResourceType& resource, void* userData){
    int32_t idx = -1;
    for(size_t i = 0; i < resources.size(); i++){
        if(strcmp(resources[i]->name, resName) == 0) idx = i;
    }

    // add resource
    if(idx >= 0){
        // free previous resource
        resourceDestructorCallback(resources[idx]->handle, userData);

        // set resource handle to new resource
        resources[idx]->handle = resource;
    } else {
        // allocate resource
        // We do it this way because this would save us from copying/moving
        // data again and again whenever a resoures realloc is done.
        // In this solution, we'd just have to copy/move the value of pointer to
        // newly allocated vector.
        Resource* resPtr = reinterpret_cast<Resource*>(malloc(sizeof(Resource)));
        resPtr->name = strcpy((char*)malloc(strlen(resName)), resName);
        resPtr->handle = resource;

        resources.push_back(resPtr);
    }
}

// get resource
template<typename ResourceType>
ResourceType* ResourceManager<ResourceType>::get(const char* resName){
    for(auto& res : resources){
        if(strcmp(res->name, resName) == 0){
            return &res->handle;
        }
    }

    // return nullptr if resource doesn't exist
    return nullptr;
}

// discard a previously added resource
template<typename ResourceType>
void ResourceManager<ResourceType>::discard(const char* resName, void* userData){
    // if resource exists then add it to free indices
    for(size_t i = 0; i < resources.size(); i++){
        if(strcmp(resources[i]->name, resName) == 0){
            // free the name and resource
            free((void*)resources[i]->name);

            // call destructor
            resourceDestructorCallback(resources[i]->handle, userData);

            // store index of free'd resource
            freeIndices[numDiscards++] = i;
        }
    }

    // if we hit the top
    if(numDiscards == maxSizeBeforeRealloc){
        // reallocate
        std::vector<Resource*> newResources(resources.size() - maxSizeBeforeRealloc);

        // sort free'd indices
        // we'd skip these indices while deallocating them
        std::sort(freeIndices.begin(), freeIndices.end());

        // index in freeIndices array
        size_t freeIdx = 0;

        // index in newResources vector
        size_t j = 0;

        // go through each element
        for(size_t i = 0 ; i < resources.size(); i++){
            // if resource at this index was discarded
            if(i == freeIndices[freeIdx]){
                // free space allocated for resources
                free((void*)resources[i]);
                freeIdx++;
            }else{
                newResources[j] = i;
                j++;
            }
        }
    }

    // reset
    numDiscards = 0;
}

#endif//RESOURCE_MANAGER_IMPL_HPP
