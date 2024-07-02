#include "table/descriptors.h"


namespace starrocks {
    SlotDescriptor::SlotDescriptor(
            SlotId id,
            TypeDescriptor type,
            TupleId parent,
            std::string col_name,
            int slot_idx,
            int slot_size,
            bool is_materialized)
        : _id(id),
          _type(type),
          _parent(parent),
          _col_name(col_name),
          _slot_idx(slot_idx),
          _slot_size(slot_size),
          _is_materialized(is_materialized) {}

    std::string SlotDescriptor::debug_string() const {
        std::stringstream out;
        out << "Slot(id=" << _id << " type=" << _type << " name=" << _col_name
            << ")";
        return out.str();
    }

    TableDescriptor::TableDescriptor(std::string name, std::string database, TableId id) : _name(name),
                                                                                           _database(database),
                                                                                           _id(id) {}
    std::string TableDescriptor::debug_string() const {
        std::stringstream out;
        out << "#name=" << _name;
        return out.str();
    }

    JDBCTableDescriptor::JDBCTableDescriptor(std::string name, std::string database, TableId id, std::string jdbc_driver_name, std::string jdbc_driver_url, std::string jdbc_driver_checksum, std::string jdbc_driver_class, std::string jdbc_url, std::string jdbc_table, std::string jdbc_user, std::string jdbc_passwd) :
                                                                                                                                                                                                                                                                                                                             TableDescriptor(name, database, id),
    _jdbc_driver_name(name),
    _jdbc_driver_url(jdbc_driver_url),
    _jdbc_driver_checksum(jdbc_driver_checksum),
    _jdbc_driver_class(jdbc_driver_class),
    _jdbc_url(jdbc_url),
    _jdbc_table(jdbc_table),
    _jdbc_user(jdbc_user),
    _jdbc_passwd(jdbc_passwd){}

    std::string JDBCTableDescriptor::debug_string() const {
        std::stringstream out;
        out << "JDBCTable(" << TableDescriptor::debug_string() << " jdbc_driver_name=" << _jdbc_driver_name
            << " jdbc_driver_url=" << _jdbc_driver_url << " jdbc_driver_checksum=" << _jdbc_driver_checksum
            << " jdbc_driver_class=" << _jdbc_driver_class << " jdbc_url=" << _jdbc_url << " jdbc_table=" << _jdbc_table
            << " jdbc_user=" << _jdbc_user << " jdbc_passwd=" << _jdbc_passwd << "}";
        return out.str();
    }

    TupleDescriptor::TupleDescriptor(TupleId id, starrocks::TableDescriptor *table_descriptor, int byte_size):
    _id(id),
    _table_desc(table_descriptor),
    _byte_size(byte_size){}

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


    // todo Status DescriptorTbl::create(RuntimeState* state, ObjectPool* pool, const TDescriptorTable& thrift_tbl,
    //                             DescriptorTbl** tbl, int32_t chunk_size)


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

    void DescriptorTbl::get_tuple_descs(std::vector<TupleDescriptor*>* descs) const {
        descs->clear();

        for (auto i : _tuple_desc_map) {
            descs->push_back(i.second);
        }
    }
    std::string DescriptorTbl::debug_string() const {
        std::stringstream out;
        out << "tuples:\n";

        for (auto i : _tuple_desc_map) {
            out << i.second->debug_string() << '\n';
        }

        return out.str();
    }

}// namespace starrocks