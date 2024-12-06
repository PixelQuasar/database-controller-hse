//
// Created by QUASARITY on 06.11.2024.
//

#ifndef DATABASE_CONTROLLER_HSE_TABLE_H
#define DATABASE_CONTROLLER_HSE_TABLE_H

#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <variant>

#include "../../query_language/AST/SQLStatement.h"
#include "../../types.h"

namespace database {

struct Index {
    IndexType type;
    std::vector<std::string> columns;

    std::multimap<std::string, size_t> orderedIndex;

    std::unordered_map<std::string, std::unordered_set<size_t>> unorderedIndex;
};

class Table {
   public:
    Table() {}

    Table(const std::string& name, const SchemeType& columns)
        : name_(name), scheme_(columns) {
        for (size_t i = 0; i < columns.size(); i++) {
            column_to_row_offset_[columns[i].name] = i;
        }
        row_sizes_.resize(columns.size());
        // TODO Initialize row_sizes_
    }

    size_t size() const { return rows_.size(); }

    std::vector<RowType> get_rows() const { return rows_; }

    SchemeType get_scheme() const { return scheme_; }

    std::map<std::string, size_t> get_column_to_row_offset() const {
        return column_to_row_offset_;
    }

    std::string get_name() const { return name_; }

    void insert_row(RowType row);

    void addAutoIncrement(const std::string& columnName);
    void addUniqueConstraint(const std::string& columnName);
    void addKeyConstraint(const std::string& columnName);

    std::vector<RowType> filter(
        const std::function<bool(const RowType&)>& predicate);

    void update_many(
        const std::function<void(std::vector<DBType>&)>& updater,
        const std::function<bool(const std::vector<DBType>&)>& predicate);

    void remove_many(
        const std::function<bool(const std::vector<DBType>&)>& predicate);

    void drop_rows() { rows_ = {}; }

    std::string convert_to_byte_buffer();

    void load_from_byte_buffer(const std::string& buffer);

    void createIndex(const std::string& indexTypeStr, const std::vector<std::string>& columns);

    std::string columnsToKey(const std::vector<std::string>& columns) const;

    bool useIndexForQuery(const std::string& columnName);

    const std::unordered_map<std::string, Index>& getIndexes() const {
        return indexes_;
    }

   private:
    std::string name_;
    SchemeType scheme_;
    std::vector<RowType> rows_;
    std::vector<size_t> row_sizes_;
    std::map<std::string, size_t> column_to_row_offset_;
    std::vector<std::string> checkConditions_;
    std::map<std::string, int> autoIncrementValues_;
    std::unordered_map<std::string, Index> indexes_;
};

}  // namespace database

#endif  // DATABASE_CONTROLLER_HSE_TABLE_H
