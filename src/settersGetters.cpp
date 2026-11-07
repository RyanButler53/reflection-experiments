#include <meta>
#include <print>
#include <structs.hpp>

class SetterGetter;

// Iterate over the members of DataPacket

// consteval std::string_view capitalize_first(std::string_view s) {
//     std::string out{s};
//     if (!out.empty() && out[0] >= 'a' && out[0] <= 'z')
//         out[0] = static_cast<char>(out[0] - 'a' + 'A');

//     return std::string_view{std::define_static_string(out), out.size()};
// }

constexpr std::string capitalize_first(std::string_view s) {
    std::string out{s};
    if (!out.empty() && out[0] >= 'a' && out[0] <= 'z')
        out[0] = static_cast<char>(out[0] - 'a' + 'A');
    return out;
}

template <std::meta::info mainStruct, std::meta::info incompleteType>
constexpr void flattenStruct(std::string_view name = ""){
    constexpr auto ctx = std::meta::access_context::current();
    template for (constexpr std::meta::info mem : std::define_static_array(std::meta::nonstatic_data_members_of(mainStruct, ctx))){
        constexpr auto memType = std::meta::type_of(mem);
        std::string_view capitalName = capitalize_first(name);
        if constexpr (std::meta::is_fundamental_type(memType)){

            std::println("Primitive: {}{}",capitalName, capitalize_first(std::meta::identifier_of(mem)));
        }else if constexpr (std::meta::is_array_type(memType)){
            std::println("Array: {}{}", capitalName, capitalize_first(std::meta::identifier_of(mem)));
        } else if constexpr (std::meta::is_class_type(memType)){
            flattenStruct<memType, incompleteType>(capitalize_first(std::meta::identifier_of(mem)));
        } else {
            std::println("Something else:: {}", std::meta::display_string_of(mem));
        }
    }
}




int main(){

    flattenStruct<^^DataPacket, ^^SetterGetter>();

    return 0;
}