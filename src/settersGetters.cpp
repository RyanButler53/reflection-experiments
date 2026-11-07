#include <meta>
#include <print>
#include <structs.hpp>
#include <vector>

class SetterGetter;

// Iterate over the members of DataPacket

constexpr std::string capitalize_first(std::string_view s) {
    std::string out{s};
    if (!out.empty() && out[0] >= 'a' && out[0] <= 'z')
        out[0] = static_cast<char>(out[0] - 'a' + 'A');
    return out;
}

constexpr std::string add_underscore(std::string_view s){
    std::string out{s};
    return out + '_';
}

constexpr std::string concat(std::string_view s1, std::string_view s2){
    std::string str1(s1);
    std::string str2(s2);
    return str1 + str2;
}

template <std::meta::info mainStruct, std::meta::info incompleteType>
constexpr void flattenStruct(std::string_view name = ""){
    constexpr auto ctx = std::meta::access_context::current();
    template for (constexpr std::meta::info mem : std::define_static_array(std::meta::nonstatic_data_members_of(mainStruct, ctx))){
        constexpr auto memType = std::meta::type_of(mem);
        if constexpr (std::meta::is_fundamental_type(memType)){
            std::println("Primitive: {}{}",name, std::meta::identifier_of(mem));
        }else if constexpr (std::meta::is_array_type(memType)){
            std::println("Array: {}{}", name, std::meta::identifier_of(mem));
        } else if constexpr (std::meta::is_class_type(memType)){
            std::string underscored = add_underscore(std::meta::identifier_of(mem));
            flattenStruct<memType, incompleteType>(std::string_view(underscored));
        } else {
            std::println("Something else:: {}", std::meta::display_string_of(mem));
        }
    }
}

template <typename T>
constexpr void printStruct(){
    constexpr auto ctx = std::meta::access_context::current();
    template for (constexpr std::meta::info mem : std::define_static_array(std::meta::nonstatic_data_members_of(^^T, ctx))){
        std::println("{}", std::meta::identifier_of(mem));
    }
}


template <std::meta::info mainStruct>
consteval void getDataMembers(std::vector<std::meta::info>& dataMembers, std::string_view name = ""){
    constexpr auto ctx = std::meta::access_context::current();
    template for (constexpr std::meta::info mem : std::define_static_array(std::meta::nonstatic_data_members_of(mainStruct, ctx))){
        constexpr auto memType = std::meta::type_of(mem);
        if constexpr (std::meta::is_fundamental_type(memType)){
            dataMembers.push_back(std::meta::data_member_spec(memType, {.name=concat(name, std::meta::identifier_of(mem))}));
        }else if constexpr (std::meta::is_array_type(memType)){
            dataMembers.push_back(std::meta::data_member_spec(memType, {.name=concat(name, std::meta::identifier_of(mem))}));
        } else if constexpr (std::meta::is_class_type(memType)){
            std::string underscored = add_underscore(std::meta::identifier_of(mem));
            getDataMembers<memType>(dataMembers, std::string_view(underscored));
        }
    }
}

consteval {

std::vector<std::meta::info> dataMems;
getDataMembers<^^DataPacket>(dataMems, "");

std::meta::define_aggregate(^^SetterGetter, dataMems);
}





int main(){

    printStruct<SetterGetter>();
    return 0;
}