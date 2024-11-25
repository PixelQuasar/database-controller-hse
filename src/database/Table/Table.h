//
// Created by QUASARITY on 06.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_TABLE_H
#define DATABASE_CONTROLLER_HSE_TABLE_H

#include <map>
#include <vector>
#include <string>
#include <functional>
#include <set>
#include <cstring>
#include "../../types.h"
#include "../../query_language/AST/SQLStatement.h"

namespace database {
    class Table {
    public:
        Table() {}

        Table(const std::string& name, const SchemeType& columns)
            : name_(name), scheme_(columns) {
            for (int i = 0; i < columns.size(); i++) {
                column_to_row_offset_[columns[i].name] = i;
            }
            row_sizes_.resize(columns.size());
            // TODO Initialize row_sizes_
        }

        size_t size() const {
            return rows_.size();
        }

        std::vector<RowType> get_rows() const {
            return rows_;
        }

        SchemeType get_scheme() const {
            return scheme_;
        }

        std::map<std::string, size_t> get_column_to_row_offset() const {
            return column_to_row_offset_;
        }

        std::string get_name() const {
            return name_;
        }

        void insert_row(RowType row);

        void addAutoIncrement(const std::string& columnName);
        void addUniqueConstraint(const std::string& columnName);
        void addKeyConstraint(const std::string& columnName);

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
        SchemeType scheme_;
        std::vector<RowType> rows_;
        std::vector<size_t> row_sizes_;
        std::map<std::string, size_t> column_to_row_offset_;
        std::vector<std::string> checkConditions_;
        bool hasCheckCondition_ = false;
        std::map<std::string, int> autoIncrementValues_;
        std::map<std::string, std::set<DBType>> indixes_;
    };

} // database

#endif //DATABASE_CONTROLLER_HSE_TABLE_H
