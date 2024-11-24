//
// Created by QUASARITY on 06.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_TABLE_H
#define DATABASE_CONTROLLER_HSE_TABLE_H

#include <map>
#include <vector>
#include <string>
#include <functional>
#include "../../types.h"
#include "../../query_language/AST/SQLStatement.h"

namespace database {

    class Table {
    public:
        Table() {}

        Table(const std::string& name, const std::vector<ColumnDefinition>& columns)
            : name_(name), scheme_(columns) {
        }

        size_t size() const {
            return rows_.size();
        }

        std::vector<RowType> get_rows() const {
            return rows_;
        }

        std::vector<ColumnDefinition> get_scheme() const {
            return scheme_;
        }

        std::string get_name() const {
            return name_;
        }

        void insert_row(const RowType& row) {
            if (row.size() != scheme_.size()) {
                throw std::runtime_error("Number of values does not match number of columns.");
            }
            rows_.push_back(row);
        }
        std::vector<RowType> filter(const std::function<bool(const RowType&)>& predicate);

        void update_many(
                const std::function<void(std::vector<DBType>&)>& updater,
                const std::function<bool(const std::vector<DBType>&)>& predicate
        );

        void remove_many(
            const std::function<bool(const std::vector<DBType>&)>& predicate
        );

        std::string convert_to_byte_buffer();

        void load_from_byte_buffer(const std::string& buffer);
    private:
        std::string name_;
        std::vector<ColumnDefinition> scheme_;
        std::vector<RowType> rows_;
        std::vector<size_t> row_sizes_;
    };

} // database

#endif //DATABASE_CONTROLLER_HSE_TABLE_H
