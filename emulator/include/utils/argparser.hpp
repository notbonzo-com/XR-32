#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <string>
#include <unordered_map>
#include <optional>
#include <vector>
#include <iostream>
#include <functional>

class ArgParser {
public:
    using OptionHandler = std::function<void(std::optional<std::string>)>;

    ArgParser(std::initializer_list<std::pair<std::string_view, OptionHandler>> validOptions) {
        for (const auto& [option, handler] : validOptions) {
            options_.emplace(option, handler);
        }
    }

    void parse(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string_view arg = argv[i];

            if (arg.starts_with("-")) {
                auto equalPos = arg.find('=');
                std::string_view optionName = (equalPos != std::string_view::npos) ? arg.substr(0, equalPos) : arg;

                auto it = options_.find(optionName);
                if (it != options_.end()) {
                    std::optional<std::string> optionValue = std::nullopt;
                    if (equalPos != std::string_view::npos) {
                        optionValue = std::string(arg.substr(equalPos + 1));
                    } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                        optionValue = argv[++i];
                    }
                    it->second(optionValue);
                } else {
                    std::cerr << "Unknown option: " << optionName << "\n";
                }
            } else {
                positionalArgs_.emplace_back(arg);
            }
        }
    }

    const std::vector<std::string>& getPositionalArguments() const {
        return positionalArgs_;
    }

private:
    std::unordered_map<std::string_view, OptionHandler> options_;
    std::vector<std::string> positionalArgs_;
};

#endif
