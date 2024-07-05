#include "table/descriptors.h"

#include <utility>


namespace starrocks {
    SlotDescriptor::SlotDescriptor(
            SlotId id,
            TypeDescriptor& type,
            TupleId parent,
            std::string& col_name,
            int slot_idx,
            int slot_size,
            bool is_materialized)
        : _id(id),
          _type(std::move(type)),
          _parent(parent),
          _col_name(std::move(col_name)),
          _slot_idx(slot_idx),
          _slot_size(slot_size),
          _is_materialized(is_materialized) {}

    std::string SlotDescriptor::debug_string() const {
        std::stringstream out;
        out << "Slot(id=" << _id << " type=" << _type << " name=" << _col_name
            << ")";
        return out.str();
    }


    TableDescriptor::TableDescriptor(const starrocks::MockTTableDescriptor &mt_tbl) : _name(mt_tbl._name),
                                                                                _database(mt_tbl._database),
                                                                                _id(mt_tbl._id) {
    }
    std::string TableDescriptor::debug_string() const {
        std::stringstream out;
        out << "#name=" << _name;
        return out.str();
    }


    JDBCTableDescriptor::JDBCTableDescriptor(const starrocks::MockTTableDescriptor &mt_tbl) : TableDescriptor(mt_tbl),
                                                                                        _jdbc_driver_name(mt_tbl._name),
                                                                                        _jdbc_driver_url(mt_tbl._jdbc_table._jdbc_driver_url),
                                                                                        _jdbc_driver_checksum(mt_tbl._jdbc_table._jdbc_driver_checksum),
                                                                                        _jdbc_driver_class(mt_tbl._jdbc_table._jdbc_driver_class),
                                                                                        _jdbc_url(mt_tbl._jdbc_table._jdbc_url),
                                                                                        _jdbc_table(mt_tbl._jdbc_table._jdbc_table),
                                                                                        _jdbc_user(mt_tbl._jdbc_table._jdbc_user),
                                                                                        _jdbc_passwd(mt_tbl._jdbc_table._jdbc_passwd) {
    }

    std::string JDBCTableDescriptor::debug_string() const {
        std::stringstream out;
        out << "JDBCTable(" << TableDescriptor::debug_string() << " jdbc_driver_name=" << _jdbc_driver_name
            << " jdbc_driver_url=" << _jdbc_driver_url << " jdbc_driver_checksum=" << _jdbc_driver_checksum
            << " jdbc_driver_class=" << _jdbc_driver_class << " jdbc_url=" << _jdbc_url << " jdbc_table=" << _jdbc_table
            << " jdbc_user=" << _jdbc_user << " jdbc_passwd=" << _jdbc_passwd << "}";
        return out.str();
    }

    TupleDescriptor::TupleDescriptor(TupleId id, starrocks::TableDescriptor *table_descriptor, int byte_size) : _id(id),
                                                                                                                _table_desc(table_descriptor),
                                                                                                                _byte_size(byte_size) {}

    void TupleDescriptor::add_slot(starrocks::SlotDescriptor *slot) {
        _slots.push_back(slot);
        _decoded_slots.push_back(slot);
    }

    std::string TupleDescriptor::debug_string() const {
        std::stringstream out;
        out << "Tuple(id=" << _id << " size=" << _byte_size;
        if (_table_desc != nullptr) {
            //out << " " << _table_desc->debug_string();
        }

        out << " slots=[";
        for (size_t i = 0; i < _slots.size(); ++i) {
            if (i > 0) {
                out << ", ";
            }
            out << _slots[i]->debug_string();
        }

        out << "]";
        out << ")";
        return out.str();
    }


    Status DescriptorTbl::create(starrocks::RuntimeState *state, const MockTTableDescriptor &mt_tbl, starrocks::DescriptorTbl **tbl, int32_t chunk_size) {
        // todo JDBC only
        TableDescriptor *desc = nullptr;

        switch (mt_tbl._table_type) {
            case TableType::JDBC_TABLE:
                desc = new JDBCTableDescriptor(mt_tbl);
                break;
            default:
                break;
        }
    }

    TableDescriptor *DescriptorTbl::get_table_descriptor(TableId id) const {
        auto i = _tbl_desc_map.find(id);
        if (i == _tbl_desc_map.end()) {
            return nullptr;
        } else {
            return i->second;
        }
    }

    TupleDescriptor *DescriptorTbl::get_tuple_descriptor(TupleId id) const {
        auto i = _tuple_desc_map.find(id);
        if (i == _tuple_desc_map.end()) {
            return nullptr;
        } else {
            return i->second;
        }
    }

    SlotDescriptor *DescriptorTbl::get_slot_descriptor(SlotId id) const {
        auto i = _slot_desc_map.find(id);

        if (i == _slot_desc_map.end()) {
            return nullptr;
        } else {
            return i->second;
        }
    }

    void DescriptorTbl::get_tuple_descs(std::vector<TupleDescriptor *> *descs) const {
        descs->clear();

        for (auto i: _tuple_desc_map) {
            descs->push_back(i.second);
        }
    }
    std::string DescriptorTbl::debug_string() const {
        std::stringstream out;
        out << "tuples:\n";

        for (auto i: _tuple_desc_map) {
            out << i.second->debug_string() << '\n';
        }

        return out.str();
    }

}// namespace starrocks