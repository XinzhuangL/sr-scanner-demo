//
// Created by lixinzhuang1 on 2024/6/5.
//

#ifndef BE_SOURCE_OPERATOR_H
#define BE_SOURCE_OPERATOR_H

#include "common/chunk.h"
#include "common/status.h"
#include "common/statusor.h"
#include <vector>

namespace starrocks {
    namespace pipeline {
        class Operator {
        public:
            Operator(int32_t id, std::string name)
                : _id(id),
                  _name(name) {}
            virtual ~Operator() = default;

            // prepare is used to do the initialization work
            // It's one of the stages of the operator life cycle
            // (prepare -> finishing -> finished -> [cancelled] -> closed)
            // This method will be exactly invoked once in the whole life cycle
            virtual Status prepare();

            // Notifies the operator that no more input chunk will be added.

            // The operator should finish processing

            // The method should be idempotent, because it may be triggered
            // multiple times in the entire life cycle


            // finish function is used to finish the following operator of the current operator that
            // encounters its EOS and has no data to push into its following operator, but the operator
            // is not finished until its buffered data inside is processed
            virtual Status set_finishing() { return Status::OK(); }

            // set_finished is used to shutdown both input and output stream of an operator and after its invocation
            // buffered data inside the operator is cleared.

            // This function is used to shutdown preceding operators of the current operator if it is finished in advance,
            // when the query or fragment instance is canceled, set_finished is also called to shutdown unfinished operators.

            // A complex source operator that interacts with the corresponding sink operator in its preceding drivers via
            // an implementation-specific context should override set_finished function, such as LocalExchangeSourceOperator.

            // It's one of the stages of the operator life cycle (prepare -> finishing -> finished -> [cancelled] -> closed)
            // This method will be exactly invoked once in the whole life cycle
            virtual Status set_finished() { return Status::OK(); }

            // It's one of the stages of the operator life cycle (prepare -> finishing -> finished -> [cancelled] -> closed)
            // - When the fragment exits abnormally, the stage operator will become to CANCELLED between FINISHED and CLOSE.
            // - When the fragment exits normally, there isn't CANCELLED stage for the drivers.
            // SomeTimes, the operator need to realize it is cancelled to stop earlier than normal, such as ExchangeSink.
            virtual Status set_cancelled() { return Status::OK(); }

            // when local runtime filters are ready, the operator should bound its corresponding runtime in-filters.
            virtual void set_precondition_ready();

            // close is used to do the cleanup work
            // It's one of the stages of the operator life cycle (prepare -> finishing -> finished -> [cancelled] -> closed)
            // This method will be exactly invoked once in the whole life cycle
            virtual void close();

            // Whether we could pull chunk from this operator
            virtual bool has_output() const = 0;


            // Whether we could push chunk to this operator
            virtual bool need_input() const = 0;

            // Is this operator completely finished processing and no more
            // output chunks will be produced
            virtual bool is_finished() const = 0;

            // pending_finish returns whether this operator still has reference to the object owned by the
            // operator or FragmentContext. It can ONLY be called after calling set_finished().

            // When a driver's sink operator is finished, the driver should wait for pending i/o task completion.

            // Otherwise, pending tasks shall reference to destructed objects in the operator or FragmentContext,
            // since FragmentContext is unregistered prematurely after all the drivers are finalized.

            // Only source and sink operator may return true, and other operators always return false.
            virtual bool pending_finish() const { return false; }


            // Pull chunk from this operator
            // Use shared_ptr, because in some cases (local broadcast exchange),
            // the chunk need to be shared
            virtual StatusOr<vectorized::ChunkPtr> pull_chunk() = 0;

            // Push chunk to this operator
            virtual Status push_chunk(const vectorized::ChunkPtr& chunk) = 0;

            // reset_state is used by MultilaneOperator in cache mechanism, because lanes in MultilaneOperator are
            // re-used by tablets, before the lane serves for the current tablet, it must invoke reset_state to re-prepare
            // the operators (such as: Project, ChunkAccumulate, DictDecode, Aggregate) that is decorated by MultilaneOperator
            // and clear the garbage that previous tablet has produced.

            // In multi-version cache, when cache is hit partially, the partial-hit cache value should be refilled back
            // to the pre-cache operator(e.g. pre-cache Agg operator) that precedes CacheOperator immediately, the Rowsets of delta
            // version and the partial-hit cache value will be merged in this pre-cache operator.

            // which operators should override this functions?
            // 1. operators not decorated by MultiOperator: not required
            // 2. operators decorated by MultilaneOperator and it precedes CacheOperator immediately: required, and must refill back
            // partial-hit cache value via the `chunks` parameter, e.g.
            // MultilaneOperator<ConjugateOperator<AggregateBlockingSinkOperator, AggregateBlockingSourceOperator>>
            // 3. operators decorated by MultilaneOperator except case 2: e.g. ProjectOperator, Chunk AccumulateOperator and etc.
            virtual Status reset_state(const std::vector<vectorized::ChunkPtr>& refill_chunks) {
                return Status::OK();
            }

            int32_t get_id() const { return _id; }

            virtual std::string get_name() const {
                return std::to_string(_id)  + "_" + _name;
            }





        private:
            const int32_t _id;
            const std::string _name;
            // _plan_node_id;
        };
        class SourceOperator : public Operator {
        public:
            SourceOperator(int32_t id, const std::string& name)
                : Operator(id, name) {}
            ~SourceOperator() override = default;
            bool need_input() const override { return false; }
            Status push_chunk(const vectorized::ChunkPtr &chunk) override {
                return Status::InternalError("Shouldn't push chunk to source operator");
            }
        };
    }// namespace pipeline
}// namespace starrocks

#endif//BE_SOURCE_OPERATOR_H
