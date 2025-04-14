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
    struct macro_kw : pegtl::string<'m', 'a', 'c', 'r', 'o'>
    {
    };
    // component types
    //  resistor, capacitor, voltage, ground, inductor, current, diode, transistors
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
    struct inductor_kw : pegtl::string<'i', 'n', 'd', 'u', 'c', 't', 'o', 'r'>
    {
    };
    struct current_kw : pegtl::string<'c', 'u', 'r', 'r', 'e', 'n', 't'>
    {
    };
    struct diode_kw : pegtl::string<'d', 'i', 'o', 'd', 'e'>
    {
    };
    struct transistor_type : pegtl::sor<
                                 pegtl::string<'n', 'm', 'o', 's'>,
                                 pegtl::string<'p', 'm', 'o', 's'>,
                                 pegtl::string<'n', 'p', 'n'>,
                                 pegtl::string<'p', 'n', 'p'>>
    {
    };
    struct component_type : pegtl::sor<resistor_kw, capacitor_kw, voltage_kw, ground_kw, inductor_kw, current_kw, diode_kw, transistor_type>
    {
    };

    struct between : pegtl::seq<
                         pegtl::string<'b', 'e', 't', 'w', 'e', 'e', 'n'>,
                         pegtl::plus<pegtl::space>, identifier,
                         pegtl::opt<pegtl::plus<pegtl::space>, pegtl::string<'a', 'n', 'd'>>,
                         pegtl::plus<pegtl::space>, identifier,
                         pegtl::opt<pegtl::plus<pegtl::space>, identifier>,
                         pegtl::opt<pegtl::plus<pegtl::space>, identifier>>
    {
    };
    struct component_line : pegtl::seq<
                                identifier, pegtl::plus<pegtl::space>,
                                equals, pegtl::plus<pegtl::space>,
                                component_type,
                                pegtl::opt<pegtl::plus<pegtl::space>, value>,
                                pegtl::opt<pegtl::plus<pegtl::space>, between>,    // check for between first before extra nodes
                                pegtl::opt<pegtl::plus<pegtl::space>, identifier>, // drain
                                pegtl::opt<pegtl::plus<pegtl::space>, identifier>, // gate
                                pegtl::opt<pegtl::plus<pegtl::space>, identifier>, // source
                                pegtl::opt<pegtl::plus<pegtl::space>, identifier>> // maybe bulk
    {
    };
    // parentheses and braces for macro definitions
    struct lparen : pegtl::one<'('>
    {
    };
    struct rparen : pegtl::one<')'>
    {
    };
    struct lbrace : pegtl::one<'{'>
    {
    };
    struct rbrace : pegtl::one<'}'>
    {
    };
    struct comma : pegtl::one<','>
    {
    };
    // macro definitions below up until grammar line
    struct macro_name : identifier
    {
    };
    struct macro_arg : identifier
    {
    };
    struct macro_args : pegtl::list<macro_arg, pegtl::pad<comma, pegtl::space>>
    {
    };
    struct macro_header : pegtl::seq<
                              macro_kw, pegtl::plus<pegtl::space>, macro_name,
                              lparen, pegtl::opt<macro_args>, rparen>
    {
    };
    using opt_ws = pegtl::star<pegtl::sor<pegtl::space, pegtl::eol>>;
    // just catch the full line of raw text
    struct macro_body_line : pegtl::until<pegtl::eol, pegtl::any>
    {
    };
    struct macro_body : pegtl::star<macro_body_line>
    {
    };

    struct macro_definition : pegtl::seq<
                                  macro_header,
                                  opt_ws,
                                  lbrace,
                                  opt_ws,
                                  macro_body,
                                  opt_ws,
                                  rbrace>
    {
    };
    // macro calls: my_instance = my_macro(arg1, arg2, ...)
    struct instance_name : identifier
    {
    };
    struct macro_ref_name : identifier
    {
    };
    struct macro_call_arg : identifier
    {
    };
    struct macro_call_args : pegtl::list<macro_call_arg, pegtl::pad<comma, pegtl::space>>
    {
    };
    struct macro_instantiation : pegtl::seq<
                                     instance_name, pegtl::plus<pegtl::space>,
                                     equals, pegtl::plus<pegtl::space>,
                                     macro_ref_name, lparen,
                                     pegtl::opt<macro_call_args>, rparen>
    {
    };

    // *Top level grammar rule*
    struct grammar : pegtl::must<pegtl::sor<component_line, macro_definition, macro_instantiation>>
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
    struct parsed_macro
    {
        std::string name;
        std::vector<std::string> args;
        std::vector<std::string> raw_lines; // using raw lines for macro body lines instead of parsed components
        std::vector<parsed_component> body;
    };

    inline std::unordered_map<std::string, parsed_macro> macro_table;

    struct current_macro
    {
        parsed_macro macro;
        std::vector<std::string> arg_temp;
    };

    struct macro_call
    {
        std::string instance_name;
        std::string macro_name;
        std::vector<std::string> call_args;
    };

    // action handlers for component lines
    template <typename Rule>
    struct component_action : pegtl::nothing<Rule>
    {
    };

    template <>
    struct component_action<identifier>
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
    struct component_action<value>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            comp.value = in.string();
        }
    };
    template <>
    struct component_action<resistor_kw>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            comp.type = "resistor";
        }
    };
    template <>
    struct component_action<capacitor_kw>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            comp.type = "capacitor";
        }
    };
    template <>
    struct component_action<voltage_kw>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            comp.type = "voltage";
        }
    };
    template <>
    struct component_action<ground_kw>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            comp.type = "ground";
        }
    };
    template <>
    struct component_action<inductor_kw>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            comp.type = "inductor";
        }
    };
    template <>
    struct component_action<current_kw>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            comp.type = "current";
        }
    };
    template <>
    struct component_action<diode_kw>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            comp.type = "diode";
        }
    };
    template <>
    struct component_action<transistor_type>
    {
        template <typename Input>
        static void apply(const Input &in, parsed_component &comp)
        {
            comp.type = in.string();
        }
    };

    // action handlers for macro lines
    template <typename Rule>
    struct macro_action : pegtl::nothing<Rule>
    {
    };

    template <>
    struct macro_action<macro_name>
    {
        template <typename Input>
        static void apply(const Input &in, current_macro &state, parsed_component &comp)
        {
            state.macro.name = in.string();
        }
    };
    template <>
    struct macro_action<macro_arg>
    {
        template <typename Input>
        static void apply(const Input &in, current_macro &state, parsed_component &)
        {
            state.arg_temp.push_back(in.string());
        }
    };
    // For debugging purposes
    // template <>
    // struct macro_action<macro_args>
    // {
    //     template <typename Input>
    //     static void apply(const Input &in, current_macro &, parsed_component &)
    //     {
    //         std::cout << "[+] Matched args list: " << in.string() << "\n";
    //     }
    // };
    // // Also for debugging purposes
    // template <>
    // struct macro_action<macro_header>
    // {
    //     template <typename Input>
    //     static void apply(const Input &in, current_macro &, parsed_component &)
    //     {
    //         std::cout << "[+] Matched macro header: " << in.string() << "\n";
    //     }
    // };
    template <>
    struct macro_action<macro_body_line>
    {
        template <typename Input>
        static void apply(const Input &in, current_macro &state, parsed_component &comp)
        {
            std::string line = in.string();
            state.macro.raw_lines.push_back(line);
        }
    };
    // Debugging only:
    // template <>
    // struct macro_action<macro_body>
    // {
    //     template <typename Input>
    //     static void apply(const Input &in, current_macro &, parsed_component &)
    //     {
    //         std::cout << "[+] Matched macro body: " << in.string() << "\n";
    //     }
    // };
    template <>
    struct macro_action<macro_definition>
    {
        template <typename Input>
        static void apply(const Input &in, current_macro &state, parsed_component &comp)
        {
            state.macro.args = std::move(state.arg_temp);
            macro_table[state.macro.name] = state.macro;
            state = {};
        }
    };

    // action handlers for macro calls
    template <typename Rule>
    struct macro_instantiation_action : pegtl::nothing<Rule>
    {
    };

    template <>
    struct macro_instantiation_action<instance_name>
    {
        template <typename Input>
        static void apply(const Input &in, macro_call &call)
        {
            call.instance_name = in.string();
        }
    };
    template <>
    struct macro_instantiation_action<macro_ref_name>
    {
        template <typename Input>
        static void apply(const Input &in, macro_call &call)
        {
            call.macro_name = in.string();
        }
    };
    template <>
    struct macro_instantiation_action<macro_call_arg>
    {
        template <typename Input>
        static void apply(const Input &in, macro_call &call)
        {
            call.call_args.push_back(in.string());
        }
    };
    template <>
    struct macro_instantiation_action<macro_instantiation>
    {
        template <typename Input>
        static void apply(const Input &in, macro_call &call)
        {
            // this is optional but i think it would be good for user debugging
            if (macro_table.find(call.macro_name) == macro_table.end())
            {
                throw std::runtime_error("Macro not defined: " + call.macro_name);
            }

            // print it out for debugging
            // std::cout << "[Macro] Call: " << call.instance_name
            //           << " = " << call.macro_name << "(";
            // for (auto &arg : call.call_args)
            //     std::cout << arg << " ";
            // std::cout << ")\n";
        }
    };
}
