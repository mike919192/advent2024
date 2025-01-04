
#pragma once

#include <algorithm>
#include <cctype>
#include <locale>
#include <utility>

template <typename t_t, typename u_t>
auto operator+(const std::pair<t_t, u_t> &l, const std::pair<t_t, u_t> &r)
{
    return std::pair<t_t, u_t>{ l.first + r.first, l.second + r.second };
}

template <typename t_t, typename u_t>
auto operator-(const std::pair<t_t, u_t> &l, const std::pair<t_t, u_t> &r)
{
    return std::pair<t_t, u_t>{ l.first - r.first, l.second - r.second };
}

template <typename t_t, typename u_t>
auto operator-(const std::pair<t_t, u_t> &l)
{
    return std::pair<t_t, u_t>{ -l.first, -l.second };
}

using xy_pos_t = std::pair<int, int>;

inline bool is_pos_on_map(xy_pos_t pos, xy_pos_t dim)
{
    return !(pos.first < 0 || pos.second < 0 || pos.first >= dim.first || pos.second >= dim.second);
}

// trim from start (in place)
inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

inline bool string_contains(const std::string & s1, const std::string & s2)
{
    return s1.find(s2) != std::string::npos;
}
