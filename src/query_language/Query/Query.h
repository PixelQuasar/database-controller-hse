//
// Created by QUASARITY on 12.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_QUERY_H
#define DATABASE_CONTROLLER_HSE_QUERY_H

#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace database {
    enum QueryType {
        SELECT,
        SELECT_WHERE,
        INSERT,
        DELETE_WHERE,
        UPDATE_SET_WHERE,
        JOIN_ON,
        CREATE_TABLE
    };

    class Query {
    public:
        explicit Query(std::string& raw_string) {
            size_t pos = 0;
            std::string token;
            std::string key_words;
            std::string current_param;
            while ((pos = raw_string.find(' ')) != std::string::npos) {
                token = raw_string.substr(0, pos);
                raw_string.erase(0, pos + 1);
                if (std::find(possible_key_words.begin(), possible_key_words.end(), token) != possible_key_words.end()) {
                    if (!key_words.empty() && !current_param.empty()) {
                        if (current_param.back() == ' ' || current_param.back() == ';') {
                            current_param.pop_back();
                        }
                        raw_params.push_back(current_param);
                    }
                    key_words += token + " ";
                    current_param.clear();
                } else {
                     current_param += token + " ";
                }
            }
            current_param += raw_string.substr(0, pos);
            if (current_param.back() == ' ' || current_param.back() == ';') {
                current_param.pop_back();
            }

            if (query_templates.contains(key_words)) {
                throw std::invalid_argument("Invalid query");
            }

            m_query_type = query_templates.at(key_words);

            std::cout << key_words << std::endl;
            for (const auto& param : raw_params) {
                std::cout << param << std::endl;
            }
        }

        QueryType get_query_type() {
            return m_query_type;
        }

        std::vector<std::string> get_params() {
            return raw_params;
        }

    private:
        QueryType m_query_type;

        std::vector<std::string> raw_params;

        static const std::map<std::string, QueryType> query_templates;

        static const std::vector<std::string> possible_key_words;
    };
}

#endif //DATABASE_CONTROLLER_HSE_QUERY_H
