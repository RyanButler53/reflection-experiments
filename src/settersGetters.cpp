#include <meta>
#include <print>
#include <structs.hpp>
#include <vector>
#include <functional>
#include <flat_map>

class SetterGetter;
class SubSubPacket_;

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
constexpr void printMembers(){
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr std::meta::info mem : std::define_static_array(std::meta::nonstatic_data_members_of(^^T, ctx))){
        std::println("{} {}",  std::meta::display_string_of(std::meta::type_of(mem)), std::meta::identifier_of(mem));
    }
}


template <std::meta::info mainStruct>
consteval void getDataMembers(std::vector<std::meta::info>& dataMembers, std::string_view name = ""){
    constexpr auto ctx = std::meta::access_context::current();
    template for (constexpr std::meta::info mem : std::define_static_array(std::meta::nonstatic_data_members_of(mainStruct, ctx))){
        constexpr auto memType = std::meta::type_of(mem);
        if constexpr (std::meta::is_fundamental_type(memType)){
            std::string memName = concat(name, std::meta::identifier_of(mem));
            // Raw data member
            dataMembers.push_back(std::meta::data_member_spec(memType, {.name=memName}));
        
            // Getter: 
            using mem_t = typename [:memType:];
            auto getter = concat(std::string_view("get_"), std::string_view(memName));
            dataMembers.push_back(std::meta::data_member_spec(^^std::function<mem_t()>, {.name=getter}));
            // dataMembers.push_back(std::meta::data_member_spec(^^mem_t(*)(), {.name=getter}));

            auto setter = concat(std::string_view("set_"), std::string_view(memName));
            dataMembers.push_back(std::meta::data_member_spec(^^std::function<void(mem_t)>, {.name=setter}));
            // dataMembers.push_back(std::meta::data_member_spec(^^void(*)(mem_t), {.name=setter}));
        } else if constexpr (std::meta::is_array_type(memType)){
            std::string memName = concat(name, std::meta::identifier_of(mem));
            dataMembers.push_back(std::meta::data_member_spec(memType, {.name=memName}));
        } else if constexpr (std::meta::is_class_type(memType)){
            std::string underscored = add_underscore(std::meta::identifier_of(mem));
            getDataMembers<memType>(dataMembers, concat(name, std::string_view(underscored)));
        }
    }
}

template <typename OriginalClass, typename IncompleteType>
consteval void makeSubpacket(){
    std::vector<std::meta::info> members;
    getDataMembers<^^OriginalClass>(members, "");
    std::meta::define_aggregate(^^IncompleteType, members);

}

consteval {

makeSubpacket<structs::SubSubPacket, SubSubPacket_>();

}


namespace detail {
    consteval std::meta::info find_member(std::meta::info type, std::string_view name) {
        for (std::meta::info m : std::meta::nonstatic_data_members_of(
                 type, std::meta::access_context::unchecked()))
            if (std::meta::has_identifier(m) && std::meta::identifier_of(m) == name)
                return m;
        throw "find_member: no such data member";   // compile error if reached
    }
    }

template <typename T>
void bind_accessors(T& obj) {
    template for (constexpr auto m : std::define_static_array(
                      std::meta::nonstatic_data_members_of(
                          ^^T, std::meta::access_context::unchecked()))) {
        if constexpr (std::meta::has_identifier(m)) {
            constexpr std::string_view id = std::meta::identifier_of(m);

            if constexpr (id.starts_with("get_")) {
                constexpr auto target = detail::find_member(^^T, id.substr(4));
                obj.[:m:] = [&obj]() { return obj.[:target:]; };

            } else if constexpr (id.starts_with("set_")) {
                constexpr auto target = detail::find_member(^^T, id.substr(4));
                using V = [:std::meta::type_of(target):];
                obj.[:m:] = [&obj](V v) { obj.[:target:] = std::move(v); };
            }
        }
    }
}

struct SubSubPacket : public SubSubPacket_ {
    constexpr SubSubPacket() {
        bind_accessors<SubSubPacket_>(static_cast<SubSubPacket_&>(*this)); 
    }

};

template <typename Generated>
class GetterSetter : public Generated {
    constexpr GetterSetter(){
        bind_accessors<Generated>(static_cast<Generated&>(*this)); 

    }
};


int main(){

    printMembers<SubSubPacket>();
    SubSubPacket pkt;
    printMembers<SubSubPacket_>();
    pkt.set_moreData(15.0);
    std::println("{}", pkt.get_moreData());

}