#include <mutex>
#include <iostream>

#include "sgraph.hpp"

namespace sae {
namespace streaming {

GraphFormatMap *graph_format_map = nullptr;
static std::mutex graph_format_map_lock;

GraphFormatRegisterer::GraphFormatRegisterer(const char *name, StreamingGraphCreator&& creator) {
	graph_format_map_lock.lock();
	if (graph_format_map == nullptr) {
		graph_format_map = new std::map<std::string, StreamingGraphCreator>;
	}
    graph_format_map->emplace(name, creator);
    graph_format_map_lock.unlock();
}

}
}
