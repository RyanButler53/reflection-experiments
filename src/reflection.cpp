#include <print>
#include <meta>
#include <string>
#include <vector>
#include <ranges>
#include <iostream>
#include <format>
#include <fstream>
#include <cstdlib>
struct Nested {
    std::string nested_str1;
    float nested_flt;
};

struct S {
    int num;
    std::string str;
    Nested n;

    void print(){
        std::println("string: {}, number: {}", str, num);
    }
};



enum class JobStatus : uint8_t {
    INVALID = 0, // Jobs that can't parse
    QUEUED = 1,
    RUNNING = 2,
    DONE = 3,
    ERROR = 4 // Jobs that throw runtime errors
};



template <typename T>
constexpr void printMembers(){
    constexpr std::meta::info r = ^^T;
    std::println("Struct: {}", std::meta::display_string_of(r));
    constexpr auto ctx1 = std::meta::access_context::current();
    // constexpr auto r_members = 
    template for (constexpr std::meta::info member : std::define_static_array(std::meta::nonstatic_data_members_of(r, ctx1))){
        // if constexpr (std::meta::is_class_type(member)){
        using MemberType = typename [: std::meta::type_of(member) :];
        if constexpr (std::is_aggregate_v<MemberType> && !std::is_same_v<MemberType, std::string>){

            printMembers<MemberType>();
        }
        if constexpr (std::meta::has_identifier(member)){
            std::cout << "Name: " << std::meta::identifier_of(member) << "\n";
        }
    }
}

template <typename T>
constexpr void printEnum() {
    constexpr std::meta::info r = ^^T;
    template for (constexpr std::meta::info meta : std::define_static_array(std::meta::enumerators_of(r))){
        // if constexpr (std::meta::has_identifier(meta)){
            std::cout << "Name: " << std::meta::identifier_of(meta) << "\n";
        // }
    }
}

/**
 * @brief Serialize a POD struct 
 * 
 * @tparam S type of a structure to serialize
 */
template <typename S>
constexpr std::vector<std::string> serialize(const S& s){
    // write to a file called "serialize.txt"
    constexpr std::meta::info r = ^^S;
    std::vector<std::string> serialization;

    static constexpr std::meta::access_context ctx = std::meta::access_context::unchecked();
    static constexpr auto members = std::define_static_array(std::meta::nonstatic_data_members_of(r, ctx));
    template for (constexpr std::meta::info m : members){
        serialization.push_back(std::format("{}", s.[:m:]));
    } 
    return serialization;
}

constexpr std::vector<std::string> splitString(std::string line, std::string delim){
    return std::views::split(line, delim) | std::ranges::to<std::vector<std::string>>();
}

// template <typename S>
// constexpr S deserialize(const std::vector<std::string>& serialized){
//     S des;
//     constexpr std::meta::info r = ^^des;
//     static constexpr std::meta::access_context ctx = std::meta::access_context::unchecked();
//     static constexpr auto members = std::define_static_array(std::meta::nonstatic_data_members_of(r, ctx));
//     template for (constexpr auto [serial, reflect] : std::views::zip(serialized, r)){
//         using MemberType = [: std::meta::type_of(reflect) :];
//         // Entire string is a line. 
//         MemberType t;
//         if constexpr (std::is_same_v<MemberType, int>){
//             des.[:reflect:] = std::stoi(serial);
//         } else if constexpr(std::is_same_v<MemberType, float>){
//             des.[:reflect:] = std::stof(serial);
//         } else if constexpr (std::is_same_v<MemberType, std::string>){
//             des.[:reflect:] = serial;
//         }
//     }
//     return des;
// }

struct Base{
    virtual void printType() = 0;
};

struct Factory{};

constexpr Factory AbstractFactory;

// Is there a way to type annotate the types so they don't have to live in a specific namespace? 
struct [[=AbstractFactory]] Derived1 : public Base {
    void printType() override {
        constexpr std::meta::info r = ^^Derived1;
        std::println("Class name: {}", std::meta::display_string_of(r));
    }
};

struct [[=AbstractFactory]] Derived2 : public Base {
    void printType() override {
        constexpr std::meta::info r = ^^Derived2;
        std::println("Class name: {}", std::meta::display_string_of(r));
    }
};

struct NotDerived {
    void printType(){
        constexpr std::meta::info r = ^^NotDerived;
        std::println("Class name of not derived class: {}", std::meta::display_string_of(r));    }
};

struct Incomplete;

void findClass(){
    constexpr std::meta::info n = ^^::; // global namespace reflectiom
    constexpr auto ctx1 = std::meta::access_context::current();
    template for (constexpr std::meta::info cls : std::define_static_array(std::meta::members_of(n, ctx1))){
        if constexpr (std::meta::is_type(cls)){
            constexpr auto annotations = std::define_static_array(std::meta::annotations_of(cls));
            if constexpr (!annotations.empty()){
                std::println("{} Has annotations!", std::meta::display_string_of(cls));
            }
        }

    }
}

// combine this with ^^:: to reflect on whe global namespace
// Base* makeBasePtr(std::string_view name){
//     constexpr std::meta::info n = ^^::; // global namespace reflectiom
//     constexpr auto ctx1 = std::meta::access_context::current();
//     template for (constexpr std::meta::info cls : std::define_static_array(std::meta::members_of(n, ctx1))){
        
//         std::println("Class: {}", std::meta::display_string_of(cls));
//         using T = typename [:cls:];
//         // constexpr std::string_view clsname = std::meta::identifier_of(cls);
//         if (name == std::meta::display_string_of(cls)){
//             return new T();
//         }
//     }
//     return nullptr;
// }

// How can we get the annotations on a type? 


int main(){

    // FactoryPattern::Derived1 d;
    // d.printType();
    // Base* d1 = makeBasePtr("FactoryPattern::Derived1");
    // d1 ->printType();
    // Base* d2 = makeBasePtr("FactoryPattern::Derived2");
    // d2->printType();
    findClass();
    // delete d1;
    // delete d2;
}
    // constexpr S structure;
    // printMembers<S>();
    // printEnum<JobStatus>();

    // for (int i : std::views::iota(0, 3)){
    //     Nested n;
    //     n.
    // }
    // Nested n1{"nested1", 4.5};
    // Nested n2{"nested2", 5.5};
    // Nested n3{"nested3", 6.5};

    // n1.nested_str1 ="nested1";
    // n1.nested_flt = 4.5;

    // Nested n2;
    // n1.nested_str1 ="nested2";
    // n1.nested_flt = 5.5;

    // Nested n3;
    // n1.nested_str1 ="nested3";
    // n1.nested_flt = 6.5;

    // std::vector<std::string> s1 = serialize(n1);
    // for (auto x : s1){
    //     std::println("{}", x);
    // }
    // Nested r1 = deserialize<Nested>(s1);
    
    // std::println("Hello c++ 26!");
// }