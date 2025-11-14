#ifndef RESOURCES_CACHE_INCLUDE__H
#define RESOURCES_CACHE_INCLUDE__H


#include <unordered_map>
#include <string>
#include <functional>

template<typename T>
class ResourcesCache 
{
public:
    T* GetOrLoad(const std::string& key, std::function<T* (void)> loader) 
    {
        auto it = m_cache.find(key);
        if (it != m_cache.end()) return it->second;
        T* res = loader();
        m_cache[key] = res;
        return res;
    };

    void Release()
    {
        for (auto& p : m_cache)
        {
            delete p.second;
            p.second = nullptr;
        }
    }

private:
    std::unordered_map<std::string, T*> m_cache;
};


#endif // !RESOURCES_CACHE_INCLUDE__H
