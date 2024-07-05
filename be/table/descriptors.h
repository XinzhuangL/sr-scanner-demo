//
// Created by lixinzhuang1 on 2024/6/5.
//
#pragma once
#ifndef BE_DESCRIPTORS_H
#define BE_DESCRIPTORS_H
#include "runtime/types.h"
#include "common/global_types.h"
#include "common/status.h"
#include "runtime/runtime_state.h"
#include "common/mock_t_table_descriptor.h"

namespace starrocks {
    class SlotDescriptor {
    public:
        SlotDescriptor(SlotId id,
                       TypeDescriptor& type,
                       TupleId parent,
                       std::string& col_name,
                       int slot_idx,
                       int slot_size,
                       bool is_materialized);
        SlotId id() const { return _id; }
        const TypeDescriptor& type() const { return _type; }
        TypeDescriptor& type() { return _type; }
        TupleId parent() const { return _parent; }
        bool is_materialized() const { return _is_materialized; }
        // is_nullable

        int slot_size() const { return _slot_size; }

        const std::string& col_name() const { return _col_name; }

        std::string debug_string() const;


    private:
        friend class DescriptorTbl;
        friend class TupleDescriptor;
        // SchemaScanner;
        // OlapTableSchemaParam;
        const SlotId _id;
        TypeDescriptor _type;
        const TupleId _parent;
        // NullIndicatorOffset
        const std::string _col_name;

        // the idx of the slot in the tuple descriptor (0-based).
        // this is provided by the fe
        const int _slot_idx;

        // the byte size of this slot.
        const int _slot_size;

        const bool _is_materialized;

        // constructor
    };
    class TableDescriptor {
    public:
        // TableDescriptor(thrift)
        TableDescriptor(const starrocks::MockTTableDescriptor &mt_tbl);
        virtual ~TableDescriptor() = default;
        TableId table_id() const { return _id; }
        virtual std::string debug_string() const;

        const std::string& name() const { return _name; }
        const std::string& database() const { return _database; }

    private:
        std::string _name;
        std::string _database;
        TableId _id;
    };

    class JDBCTableDescriptor : public TableDescriptor {
    public:
        JDBCTableDescriptor(const MockTTableDescriptor& mt_tbl);
        std::string debug_string() const override;
        const std::string jdbc_driver_name() const { return _jdbc_driver_name; }
        const std::string jdbc_driver_url() const { return _jdbc_driver_url; }
        const std::string jdbc_driver_checksum() const { return _jdbc_driver_checksum; }
        const std::string jdbc_driver_class() const { return _jdbc_driver_class; }
        const std::string jdbc_url() const { return _jdbc_url; }
        const std::string jdbc_table() const { return _jdbc_table; }
        const std::string jdbc_user() const { return _jdbc_user; }
        const std::string jdbc_passwd() const { return _jdbc_passwd; }
    private:
        std::string _jdbc_driver_name;
        std::string _jdbc_driver_url;
        std::string _jdbc_driver_checksum;
        std::string _jdbc_driver_class;

        std::string _jdbc_url;
        std::string _jdbc_table;
        std::string _jdbc_user;
        std::string _jdbc_passwd;

    };

    class TupleDescriptor {
    public:
        TupleDescriptor(TupleId id, TableDescriptor* table_descriptor, int byte_size);
        int byte_size() const { return _byte_size; }
        const std::vector<SlotDescriptor*>& slots() const { return _slots; }
        std::vector<SlotDescriptor*>& slots() { return _slots; }
        const std::vector<SlotDescriptor*>& decoded_slots() const { return _decoded_slots; }
        std::vector<SlotDescriptor*>& decoded_slots() { return _decoded_slots; }
        const TableDescriptor* table_desc() const { return _table_desc; }
        void set_table_desc(TableDescriptor* table_desc) { _table_desc = table_desc; }
        TupleId id() const { return _id; }

        std::string debug_string() const;

    private:
        friend class DescriptorTbl;
        // OlapTableSchemaParam
        const TupleId _id;
        TableDescriptor* _table_desc;
        int _byte_size;
        std::vector<SlotDescriptor*> _slots; // contains all slots
        // For a low cardinality string column with global dict;
        // The type in _slots is int, in _decode_slots is varchar
        std::vector<SlotDescriptor*> _decoded_slots;

        void add_slot(SlotDescriptor* slot);


    };
    class DescriptorTbl {
    public:
        // Creates a descriptor tbl with in 'pool' from thrift_tbl and returns it via 'tbl'.
        // Returns OK on success, otherwise error (int which case 'tbl' will be unset).
        //
        // ObjectPool, TDescriptorTable, DescriptorTbl,
        static Status create(RuntimeState* state,  const MockTTableDescriptor& mt_tbl, DescriptorTbl** tbl, int32_t chunk_size);

        TableDescriptor* get_table_descriptor(TableId id) const;
        TupleDescriptor* get_tuple_descriptor(TupleId id) const;
        SlotDescriptor* get_slot_descriptor(SlotId id) const;

        // return all registered tuple descriptors
        void get_tuple_descs(std::vector<TupleDescriptor*>* descs) const;

        std::string debug_string() const;

    private:
        typedef std::unordered_map<TableId, TableDescriptor*> TableDescriptorMap;
        typedef std::unordered_map<TupleId, TupleDescriptor*> TupleDescriptorMap;
        typedef std::unordered_map<SlotId, SlotDescriptor*> SlotDescriptorMap;

        TableDescriptorMap _tbl_desc_map;
        TupleDescriptorMap _tuple_desc_map;
        SlotDescriptorMap _slot_desc_map;

        DescriptorTbl() {}

    };

    // todo impl
    class RowDescriptor {};


} // namespace starrocks

#endif//BE_DESCRIPTORS_H
