#include <print>
#include <meta>
#include <string>
#include <vector>
#include <ranges>
#include <iostream>
#include <format>
#include <string_view>
#include <fstream>
#include <cstdlib>
#include <array>


// Must declare the incomplete type up front to get populated from the CSV header. 
struct Car;

constexpr char csv_text[]{
    #embed "car_stats.csv"
};

constexpr std::string_view csv{std::define_static_string(csv_text), sizeof(csv_text)};
constexpr size_t N = std::ranges::distance(csv | std::views::split('\n')) - 1;

consteval auto getMembers(){
    return csv
    | std::views::split('\n')
    | std::views::take(1)
    | std::views::transform([](auto row) { return std::string_view{row} | std::views::split(','); })
    | std::views::join
    | std::views::transform([](auto&& r){ return std::string_view{r}; });
}

// Ai-Generated double parsing function
constexpr double parse_double(std::string_view s) {
    std::size_t i = 0;
    bool negative = false;

    if (i < s.size() && (s[i] == '-' || s[i] == '+')) {
        negative = (s[i] == '-');
        ++i;
    }

    std::uint64_t mantissa = 0;
    int digits = 0;        // digits accumulated into mantissa
    int frac_digits = 0;   // how many of those were after the '.'
    bool seen_dot = false;
    bool any_digit = false;

    for (; i < s.size(); ++i) {
        const char c = s[i];
        if (c == '.') {
            if (seen_dot) throw "parse_double: second decimal point";
            seen_dot = true;
        } else if (c >= '0' && c <= '9') {
            any_digit = true;
            if (digits < 18) {                 // stay inside uint64_t
                mantissa = mantissa * 10 + static_cast<unsigned>(c - '0');
                ++digits;
                if (seen_dot) ++frac_digits;
            } else if (!seen_dot) {
                throw "parse_double: integer part too large";
            }
            // extra fractional digits beyond 18 are dropped
        } else {
            throw "parse_double: unexpected character";
        }
    }

    if (!any_digit) throw "parse_double: no digits";

    double scale = 1.0;
    for (int k = 0; k < frac_digits; ++k) scale *= 10.0;

    const double result = static_cast<double>(mantissa) / scale;
    return negative ? -result : result;
}

// Populate the Car class
consteval {

    auto members = getMembers();

    auto data_members = members
    | std::views::transform([](auto&& member){return std::meta::data_member_spec(^^double, {.name=std::string_view(member)});})
    | std::ranges::to<std::vector<std::meta::info>>();

    std::meta::define_aggregate(^^Car, data_members);
}

consteval std::array<Car, N> getCars(){
    std::array<Car, N> cars{};
    auto rows = csv 
                | std::views::split('\n')
                | std::views::drop(1)  // Drop the header row
                | std::views::transform([](auto row) { return std::string_view{row} | std::views::split(','); });
                // Each row is split by comma

    static constexpr auto members = std::define_static_array(std::meta::nonstatic_data_members_of(^^Car, std::meta::access_context::current()));
    for (auto [i, row]: std::views::enumerate(rows)){
        auto fieldIterator = row.begin();
        template for (constexpr std::meta::info dataMem : members){
            cars[i].[:dataMem:] = parse_double(std::string_view{*fieldIterator});
            ++fieldIterator;
        }
    }
    return cars;
}
constexpr auto cars = getCars();

int main(){

    for (const Car& c : cars){
        std::println("Car {}: a = {}, p = {}, vdes = {}", c.id, c.a, c.p, c.vdes);
    }

}