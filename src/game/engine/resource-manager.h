#ifndef RESOURCE_MANAGER_H__
#define RESOURCE_MANAGER_H__

#include <unordered_map>
#include <string>
#include <string_view>
#include <optional>
#include <expected>

#include "tuple-helpers.h"
#include "shader.h"
#include "texture.h"
#include "sprite.h"

// singleton pattern
// potential issue with destruction order fiasco

auto compile_shaders()
    -> void;
namespace engine {
using managable_resources = std::tuple<shader, texture, vao>;

template<typename>
struct resources;
template<template<typename...> typename Tuple, typename... Ts>
    requires (not has_duplicate_v<std::tuple<Ts...>>)
struct resources<Tuple<Ts...>> {
    using resource_types = std::tuple<Ts...>;
    template<typename T> 
        requires (has_type_v<T, resource_types>)
    auto get() const noexcept 
        -> std::unordered_map<std::string, T> const& {
        return std::get<type_index_v<T, resource_types>>(boxes);
    }
    template<typename T> 
        requires (has_type_v<T, resource_types>)
    auto get() noexcept 
        -> std::unordered_map<std::string, T>& {
        return std::get<type_index_v<T, resource_types>>(boxes);
    }
    template<typename T>
        requires (has_type_v<T, resource_types>)
    auto free_one(std::unordered_map<std::string, T>& one) noexcept {
        for (auto& elem : one) {
            elem.second.free();
        }
        one.clear();
    }
    template<size_t... Is>
    auto free_all_impl(std::index_sequence<Is...>) noexcept -> void {
        ((static_cast<void>(Is), free_one(std::get<Is>(boxes))),...);
    }
    auto free_all() noexcept -> void {
        free_all_impl(std::index_sequence_for<Ts...>{});
    }
    std::tuple<std::unordered_map<std::string, Ts>...> boxes;
};

template<typename T>
concept Resource = requires(T&& resource) {
    resource.free();
    {T::create_fallback()} noexcept;
    {T()} noexcept;
};

template<typename T>
concept ManagableResource = has_type_v<T, managable_resources> && Resource<T>;
class resource_manager {
private:
    resource_manager() {
        create_fallbacks();
    }
    resource_manager(resource_manager const&) = delete;
    resource_manager& operator=(resource_manager const) = delete;
public:
    static auto get_instance() 
        -> resource_manager& {
        static resource_manager* instance = new resource_manager;
        return *instance;
    }
    ~resource_manager() {
        m_resources.free_all();
    }
    /**
     * @brief requests resources from resource_manager instance
     * @tparam T ManagableResource, one that inherits from resource<T> and belongs "managable_resources"
     * @param handle resource handle
     * @return Empty optional if failed to locate the resource
     */
    template<ManagableResource T>
    [[nodiscard]] auto request(std::string const& handle) const noexcept 
        -> T {
        auto const& map = m_resources.get<T>();
        auto it = map.find(handle); 
        if (it != map.end()) return it->second;
        return std::get<type_index_v<T, managable_resources>>(fallbacks);
    }
    template<ManagableResource T>
    [[nodiscard]] auto request() const noexcept 
        -> T { return std::get<type_index_v<T, managable_resources>>(fallbacks); }
    template<ManagableResource T>
    auto submit(
        std::string const& handle, 
        T&& resource
    ) -> void {
        auto& map = m_resources.get<T>();
        auto [it, success] = map.insert({handle, resource});
    }
    template<ManagableResource T, typename... Ts>
    auto submit_emplace(std::string const& handle, Ts&&... args)
        -> void {
        auto& map = m_resources.get<T>();
        auto [it, success] = map.emplace(handle, std::forward<Ts>(args)...);
    }
    template<typename T>
    auto free() noexcept -> void {
        auto& map = m_resources.get<T>();
        for (auto& elem : map) {
            elem.second.free();
        }
        map.clear();
    }
    auto free_all() noexcept -> void {
        m_resources.free_all();
    }
private:
    auto create_fallbacks()
        -> void {
        for_each(
            fallbacks, 
            [](auto&& fallback) {
                using T = std::remove_cvref_t<decltype(fallback)>;
                T::create_fallback(); 
            }
        );
    }
private:
    managable_resources fallbacks;
    resources<managable_resources> m_resources;
};
}

#endif