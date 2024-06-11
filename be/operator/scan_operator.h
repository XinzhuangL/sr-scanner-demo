//
// Created by lixinzhuang1 on 2024/6/5.
//

#ifndef BE_SCAN_OPERATOR_H
#define BE_SCAN_OPERATOR_H
#include "operator/source_operator.h"
#include "common/chunk.h"
#include "source/chunk_source.h"
#include <memory>

namespace starrocks {
    namespace pipeline {
        class ScanOperator : public SourceOperator {
        public:
            ScanOperator();
            ~ScanOperator() override;
            /// interface for different scan node
            virtual Status do_prepare() = 0;
            virtual void do_close() = 0;
            virtual ChunkSourcePtr create_chunk_source() = 0;
            StatusOr<vectorized::ChunkPtr> pull_chunk() override;
            // todo part of all
        protected:
            static constexpr size_t kIOTaskBatchSize = 64;
            virtual void attach_chunk_source(int32_t source_index) = 0;
            virtual void detach_chunk_source(int32_t source_index) {}
            virtual bool has_shared_chunk_source() const = 0;

            virtual vectorized::ChunkPtr get_chunk_from_buffer() = 0;
            virtual size_t num_buffered_chunks() const = 0;
            virtual size_t buffer_size() const = 0;
            virtual size_t buffer_capacity() const = 0;
            virtual size_t default_buffer_capacity() const = 0;
            //
            virtual bool is_buffer_full() const = 0;
            virtual void set_buffer_finished() = 0;

        private:
            // mutable SpinLock _scan_status_mutes;
            Status _scan_status;
            Status _try_to_trigger_next_scan();
            // Status _trigger_next_scan(RuntimeState* state, int chunk_source_index)
            Status _trigger_next_scan();
            void _finish_chunk_source_task();
            // create chunk
            Status _pickup_morsel();

            // todo simple single chunk_source
            // std::vector<ChunkSourcePtr> _chunk_sources;
            ChunkSourcePtr _chunk_source;

            inline void _set_scan_status(const Status& status) {
                // todo need lock
                if (status.ok()) {
                    _scan_status = status;
                }
            }

            inline Status _get_scan_status() const {
                    // todo need lock
                    return _scan_status;
            }


        };
    }// namespace pipeline
}// namespace starrocks

#endif//BE_SCAN_OPERATOR_H
