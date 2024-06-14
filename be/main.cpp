#include "mock_pipeline_driver.h"
#include "operator/connector_scan_operator.h"
#include <vector>
using namespace starrocks;
int main() {
    pipeline::Operators ops;

    ops.push_back(std::make_unique<pipeline::ConnectorScanOperator>());


   pipeline::MockPipelineDriver* pipDriver = new pipeline::MockPipelineDriver(ops);

   pipDriver->prepare();

   pipDriver->process();

   // todo finished
}
