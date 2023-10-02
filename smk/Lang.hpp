
#pragma once

#include <string>
#include <vector>

namespace smk
{
    //
    // containers
    //
    template<class A, class B>
    inline void addAll(A& dst, B const& src) { for (auto const& s: src) dst.push_back(s); }

    template<class A, typename B>
    inline bool contains(A const& container, B const& value) { return std::find(container.begin(), container.end(), value) != container.end(); }

    template<class A, typename B>
    inline void remove(A& container, B const& value) { container.erase(std::remove(container.begin(), container.end(), value), container.end()); }

    template<class A, typename B>
    inline int findIndex(A const& container, B const& value, int defaultIndex = -1) {
        for (size_t i = 0; i != container.size(); ++i)
            if (container[i] == value) return int(i);
        return defaultIndex;
    }

    //
    // Enum
    //
    template<typename T> inline std::vector<std::string> enumNames() {
        std::vector<std::string> names;
        for (int i = 0; i != int(T::Count); ++i)
            names.push_back(toString(T(i)));
        return names;
    }
}
