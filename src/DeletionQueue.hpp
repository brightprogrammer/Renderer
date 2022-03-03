#ifndef DELETION_QUEUE_HPP
#define DELETION_QUEUE_HPP

#include <deque>
#include <functional>

struct DeletionQueue{
    std::deque<std::function<void()>> deletors;

    void push_function(std::function<void()>&& function) {
        deletors.push_back(function);
    }

    void flush() {
        // reverse iterate the deletion queue to execute all the functions
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
            (*it)(); //call the function
        }

        deletors.clear();
    }
};


#endif//DELETION_QUEUE_HPP
