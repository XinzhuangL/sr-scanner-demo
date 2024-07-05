#include "mock_pipeline_driver.h"
#include "operator/connector_scan_operator.h"
#include "runtime/jdbc_driver_manager.h"
#include <vector>
#include "runtime/runtime_state.h"
using namespace starrocks;
int main() {


    JDBCDriverManager::getInstance() ->init("/Users/lixinzhuang1/IdeaProjects/sr-scanner-demo/be/lib/jdbc_drivers");

    pipeline::Operators ops;

    RuntimeState state;

    ops.push_back(std::make_unique<pipeline::ConnectorScanOperator>());





   pipeline::MockPipelineDriver* pipDriver = new pipeline::MockPipelineDriver(ops);

   pipDriver->prepare();

   // create state
   pipDriver->process(&state);

   // todo finished
}
