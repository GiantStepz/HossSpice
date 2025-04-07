// grammar file to be used for PEGTL parsing
#pragma once

#include <tao/pegtl.hpp>
#include <string>
#include <vector>
#include <unordered_map>

namespace pegtl = tao::pegtl;

namespace hoss
{

    // Grammar rules
    struct identifier : pegtl::plus<pegtl::alnum>
    {
    };
    struct equals : pegtl::one<'='>
    {
    };
    struct value : pegtl::plus<pegtl::not_one<' '>>
    {
    };

    struct resistor_kw : pegtl::string<'r', 'e', 's', 'i', 's', 't', 'o', 'r'>
    {
    };
    struct capacitor_kw : pegtl::string<'c', 'a', 'p', 'a', 'c', 'i', 't', 'o', 'r'>
    {
    };
    struct voltage_kw : pegtl::string<'v', 'o', 'l', 't', 'a', 'g', 'e'>
    {
    };
    struct ground_kw : pegtl::string<'g', 'r', 'o', 'u', 'n', 'd'>
    {
    };

    struct component_type : pegtl::sor<resistor_kw, capacitor_kw, voltage_kw, ground_kw>
    {
    };

    struct between : pegtl::seq<
                         pegtl::string<'b', 'e', 't', 'w', 'e', 'e', 'n'>,
                         pegtl::plus<pegtl::space>, identifier,
                         pegtl::plus<pegtl::space>, pegtl::string<'a', 'n', 'd'>,
                         pegtl::plus<pegtl::space>, identifier>
    {
    };

    struct component_line : pegtl::seq<
                                identifier, pegtl::plus<pegtl::space>,
                                equals, pegtl::plus<pegtl::space>,
                                component_type,
                                pegtl::opt<pegtl::plus<pegtl::space>, value>,
                                pegtl::opt<pegtl::plus<pegtl::space>, between>>
    {
    };

    struct grammar : pegtl::must<component_line>
    {
    };

    // collects results during parsing
    struct parsed_component
    {
        std::string name;
        std::string type;
        std::string value;
        std::vector<std::string> nodes;
    };

    // action handlers
    template <typename Rule>
    struct action : pegtl::nothing<Rule>
    {
    };

    template <>
    struct action<identifier>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            if (comp.name.empty())
                comp.name = in.string();
            else
                comp.nodes.push_back(in.string());
        }
    };

    template <>
    struct action<resistor_kw>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            comp.type = "resistor";
        }
    };
    template <>
    struct action<capacitor_kw>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            comp.type = "capacitor";
        }
    };
    template <>
    struct action<voltage_kw>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            comp.type = "voltage";
        }
    };
    template <>
    struct action<ground_kw>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            comp.type = "ground";
        }
    };

    template <>
    struct action<value>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            comp.value = in.string();
        }
    };

    // template <>
    // struct action<simple_component_line>
    // {
    //     template <typename Input>
    //     static void apply(const Input &in, parsed_component &comp)
    //     {
    //         comp.type = "ground";
    //         comp.value = "0V";
    //         // Use name from identifier action
    //         // No nodes needed, or just default to the component name
    //         if (comp.nodes.empty())
    //         {
    //             comp.nodes.push_back(comp.name);
    //         }
    //     }
    // };
}
