#include "operators/utils/memory_data_op.h"
#include "utils/op_kernel.h"

namespace dragon {

template <class Context> template <typename Tx, typename Ty>
void MemoryDataOp<Context>::RunWithType() {
    auto* Xdata = input(0).template data<Tx, Context>();
    auto* Ydata = output(0)->template mutable_data<Ty, Context>();
    kernel::MemoryData<Tx, Ty, Context>(output(0)->count(), 
                                        output(0)->dim(0),
                                        output(0)->dim(1), 
                                        output(0)->dim(2), 
                                        output(0)->dim(3), 
                                        Xdata, Ydata);
}

template <class Context>
void MemoryDataOp<Context>::RunOnDevice() {
    vector<TIndex> dims({ input(0).dim(0), input(0).dim(3),
                          input(0).dim(1), input(0).dim(2) });
    output(0)->Reshape(dims);

    if (input(0).template IsType<float>()) {
        if (data_type == TensorProto_DataType_FLOAT) RunWithType<float, float>();
        else if (data_type == TensorProto_DataType_FLOAT16) RunWithType<float, float16>();
        else LOG(FATAL) << "unsupported input types.";
    } 
    else if (input(0).template IsType<uint8_t>()) {
        if (data_type == TensorProto_DataType_FLOAT) RunWithType<uint8_t, float>();
        if (data_type == TensorProto_DataType_FLOAT16) RunWithType<uint8_t, float16>();
    } 
    else { LOG(FATAL) << "unsupported input types."; }
}

DEPLOY_CPU(MemoryData);
#ifdef WITH_CUDA
DEPLOY_CUDA(MemoryData);
#endif
OPERATOR_SCHEMA(MemoryData).NumInputs(1).NumOutputs(1);

NO_GRADIENT(MemoryData);

}    // namespace dragon