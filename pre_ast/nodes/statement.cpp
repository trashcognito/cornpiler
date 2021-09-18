#include "statement.hpp"

ast_types::statement::statement() { act = act_type::statement; }
std::string ast_types::statement::print_node() { return "\"statement\": { \"name\":" +
                                                        this->name.print_node() +
                                                        ",\"args\": " + this->args.print_node() + "}"; }