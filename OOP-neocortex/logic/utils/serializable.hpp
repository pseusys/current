#ifndef LOGIC_SERIALIZABLE_HPP
#define LOGIC_SERIALIZABLE_HPP

#include "../libs/json.hpp"

using json = nlohmann::json;

class serializable {
protected:
    serializable() = default;
    explicit serializable(json& package) {}

public:
    virtual std::shared_ptr<json> pack(int serializer) = 0;

    template<typename T>
    std::shared_ptr<json> pack_vector(std::vector<T>& vec, int serializer);

    template<typename T>
    std::shared_ptr<std::vector<T>> unpack_vector(json& package);
};

template<typename T>
std::shared_ptr<json> serializable::pack_vector(std::vector<T>& vec, int serializer) {
    static_assert(std::is_convertible<T*, serializable*>::value, "vector class may only contain serializable objects");
    std::shared_ptr<json> package = std::make_shared<json>();
    (*package)["size"] = vec.size();
    (*package)["data"] = json::array();
    for (int i = 0; i < vec.size(); ++i) {
        (*package)["data"].push_back(*(vec[i].pack(serializer)));
    }
    return package;
}

template<typename T>
std::shared_ptr<std::vector<T>> serializable::unpack_vector(json& package) {
    static_assert(std::is_convertible<T*, serializable*>::value, "vector class may only contain serializable objects");
    std::shared_ptr<std::vector<T>> vec = std::make_shared<std::vector<T>>();
    int size = package["size"];
    for (int i = 0; i < size; ++i) {
        vec->push_back(T(package["data"].back()));
    }
    return vec;
}


#endif //LOGIC_SERIALIZABLE_HPP
