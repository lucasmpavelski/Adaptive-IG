#include <string>
#include <unordered_map>
#include <vector>

#include "problems/FSPData.hpp"
#include "FSPProblemFactory.hpp"

std::unordered_map<std::string, FSPData> FSPProblemFactory::cache;
std::string FSPProblemFactory::data_folder;
std::vector<std::unordered_map<std::string, std::string>>
    FSPProblemFactory::lower_bounds_data;
