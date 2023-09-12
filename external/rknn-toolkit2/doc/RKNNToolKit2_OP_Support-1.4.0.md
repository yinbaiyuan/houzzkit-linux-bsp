# RKNNToolkit2 OPs Support

## Explanation of terms:

**Remarks**:

    Operators' specifications must meet the remarks' requirements.

**Broadcast rule**: 

- per-layer： 

        shape(A) = (2, 3, 4, 5), shape(B) = (,), i.e. B is a scalar ==> shape(result) = (2, 3, 4, 5)

- per-channel: 
      
        shape(A) = (2, 3, 4, 5), shape(B) = (3,), ==> shape(result) = (2, 3, 4, 5)
        
        shape(A) = (2, 3, 4, 5), shape(B) = (1,3,1,1), ==> shape(result) = (2, 3, 4, 5)
    
- per-element: 

        shape(A) = (2, 3, 4, 5), shape(B) = (2,3,4,5) ==> shape(result) = (2, 3, 4, 5)

- other:

        shape(A) = (2, 3, 4, 5), shape(B) = (5,) ==> shape(result) = (2, 3, 4, 5)

**Input Size Restrictions Description**


Assuming that input size is [N,H,W,C] (layout is NHWC)

- Case 1: the first layer is **Convolution**, whose kernel size is [kernel_height, kernel_width]

  **W * kernel_height < 7168**

  **kernel_height * kernel_width < 128**


- Case 2: first layer is not Convolution, and C == 1 or C == 3 or C == 4

  **W < 7168**

- others:
  
  **No Restrictions**

  
  
  


## ONNX OPs supported by RKNN Toolkit2

According to [ONNX official instructions](https://github.com/microsoft/onnxruntime/blob/master/docs/Versioning.md 'ONNX Version Description'), the corresponding ONNX opset version is 12.  
The list of ONNX OPs supported by RKNN Toolkit2 is as follows:
<br>(For more restrictions, please refer to [RKNN_Compiler_Support_Operator_List.pdf](https://github.com/rockchip-linux/rknpu2/tree/master/doc))

| **Operators**         | **Remarks**                                                                                                                                                                                    |
| --------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Abs                   | Not Supported                                                                                                                                                                                  |
| Acos                  | Not Supported                                                                                                                                                                                  |
| Acosh                 | Not Supported                                                                                                                                                                                  |
| Add                   |                                                                                                                                                                                                |
| And                   | Not Supported                                                                                                                                                                                  |
| ArgMax                |                                                                                                                                                                                                |
| ArgMin                |                                                                                                                                                                                                |
| Asin                  | Not Supported                                                                                                                                                                                  |
| Asinh                 | Not Supported                                                                                                                                                                                  |
| Atan                  | Not Supported                                                                                                                                                                                  |
| Atanh                 | Not Supported                                                                                                                                                                                  |
| AveragePool           | **NPU Limit:**<br />channel: [1, 8192]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]<br />auto_pad: NOTSET<br />count_include_pad: 1 <br />ceil_mode: 0              |
| BatchNormalization    | **NPU Limit:**<br />channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                            |
| BitShift              | Not Supported                                                                                                                                                                                  |
| Cast                  | only support bool/int8/float                                                                                                                                                                   |
| Ceil                  | Not Supported                                                                                                                                                                                  |
| Celu                  | Not Supported                                                                                                                                                                                  |
| Clip                  |                                                                                                                                                                                                |
| Compress              | Not Supported                                                                                                                                                                                  |
| Concat                |                                                                                                                                                                                                |
| ConcatFromSequence    | Not Supported                                                                                                                                                                                  |
| Constant              |                                                                                                                                                                                                |
| ConstantOfShape       |                                                                                                                                                                                                |
| Conv                  | **NPU Limit:**<br />kernel height/width: [1, 31]<br />stride height/width: [1, 7]<br />pad left/right/top/bottom: [0, 15]                                                                      |
| ConvInteger           | Not Supported                                                                                                                                                                                  |
| ConvTranspose         | **NPU Limit:**<br />kernel height/width: [1, 31]<br />stride height/width: 2, 4, 8<br />pad left/right/top/bottom: [0, 15]                                                                     |
| Cos                   | Not Supported                                                                                                                                                                                  |
| Cosh                  | Not Supported                                                                                                                                                                                  |
| CumSum                | Not Supported                                                                                                                                                                                  |
| DepthToSpace          |                                                                                                                                                                                                |
| DequantizeLinear      |                                                                                                                                                                                                |
| Det                   |                                                                                                                                                                                                |
| Div                   | **NPU Limit:**<br />support broadcast rule: per-element/other                                                                                                                                  |
| Dropout               |                                                                                                                                                                                                |
| Einsum                | Not Supported                                                                                                                                                                                  |
| Elu                   | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />                                                                                                                          |
| Equal                 |                                                                                                                                                                                                |
| Erf                   | Not Supported                                                                                                                                                                                  |
| Exp                   |                                                                                                                                                                                                |
| Expand                | Not Supported                                                                                                                                                                                  |
| EyeLike               | only support constant input                                                                                                                                                                    |
| Flatten               |                                                                                                                                                                                                |
| Floor                 | Not Supported                                                                                                                                                                                  |
| GRU                   | batchsize: 1                                                                                                                                                                                   |
| Gather                |                                                                                                                                                                                                |
| GatherElements        | Not Supported                                                                                                                                                                                  |
| GatherND              | Not Supported                                                                                                                                                                                  |
| Gemm                  |                                                                                                                                                                                                |
| GlobalAveragePool     | channel: [1, 8192]<br />kernel height/width: [1, 343]<br />                                                                                                                                    |
| GlobalLpPool          | Not Supported                                                                                                                                                                                  |
| GlobalMaxPool         | channel: [1, 8192]<br />kernel height/width: [1, 343]<br />                                                                                                                                    |
| Greater               | **NPU Limit:**<br />support broadcast rule: per-element/other                                                                                                                                  |
| GreaterOrEqual        |                                                                                                                                                                                                |
| HardSigmoid           |                                                                                                                                                                                                |
| HardSwish             |                                                                                                                                                                                                |
| Hardmax               | Not Supported                                                                                                                                                                                  |
| Identity              |                                                                                                                                                                                                |
| If                    | only support constant input                                                                                                                                                                    |
| InstanceNormalization |                                                                                                                                                                                                |
| IsInf                 | Not Supported                                                                                                                                                                                  |
| IsNaN                 | Not Supported                                                                                                                                                                                  |
| LRN                   |                                                                                                                                                                                                |
| LSTM                  | batchsize: 1<br />input_forget: 0                                                                                                                                                              |
| LeakyRelu             |                                                                                                                                                                                                |
| Less                  | **NPU Limit:**<br />support broadcast rule: per-element/other                                                                                                                                  |
| LessOrEqual           |                                                                                                                                                                                                |
| Log                   | Not Supported                                                                                                                                                                                  |
| LogSoftmax            | batchsize: 1                                                                                                                                                                                   |
| Loop                  | Not Supported                                                                                                                                                                                  |
| LpNormalization       |                                                                                                                                                                                                |
| LpPool                | Not Supported                                                                                                                                                                                  |
| MatMul                |                                                                                                                                                                                                |
| MatMulInteger         | Not Supported                                                                                                                                                                                  |
| Max                   | **NPU Limit:**<br />channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                            |
| MaxPool               | **NPU Limit:**<br />channel: [1, 8192]<br />stride height/width: [1, 8]<br />pad left/right/top/bottom: [0, 7]<br />auto_pad: NOTSET<br />ceil_mode: 0<br />dilations: 1<br />storage_order: 0 |
| MaxRoiPool            |                                                                                                                                                                                                |
| MaxUnpool             |                                                                                                                                                                                                |
| Mean                  | Not Supported                                                                                                                                                                                  |
| Min                   | **NPU Limit:**<br />channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                            |
| Mod                   | Not Supported                                                                                                                                                                                  |
| Mul                   | **NPU Limit:**<br />support broadcast rule: per-layer/channel/element                                                                                                                          |
| Multinomial           | Not Supported                                                                                                                                                                                  |
| Neg                   | Not Supported                                                                                                                                                                                  |
| NonMaxSuppression     | Not Supported                                                                                                                                                                                  |
| NonZero               | Not Supported                                                                                                                                                                                  |
| Not                   | Not Supported                                                                                                                                                                                  |
| OneHot                | Not Supported                                                                                                                                                                                  |
| Or                    | Not Supported                                                                                                                                                                                  |
| PRelu                 | slope support broadcast rule: per-layer/channel                                                                                                                                                |
| Pad                   | **NPU Limit:**<br />width: [1, 8176]<br />mode: constant<br />pads n_begin/n_end/c_begin/c_end: 1                                                                                              |
| Pow                   |                                                                                                                                                                                                |
| QLinearConv           | Not Supported                                                                                                                                                                                  |
| QLinearMatMul         | Not Supported                                                                                                                                                                                  |
| QuantizeLinear        |                                                                                                                                                                                                |
| RNN                   | Not Supported                                                                                                                                                                                  |
| RandomNormal          | Not Supported                                                                                                                                                                                  |
| RandomNormalLike      | Not Supported                                                                                                                                                                                  |
| RandomUniform         | Not Supported                                                                                                                                                                                  |
| RandomUniformLike     | Not Supported                                                                                                                                                                                  |
| Range                 | Not Supported                                                                                                                                                                                  |
| Reciprocal            | Not Supported                                                                                                                                                                                  |
| ReduceL1              | Not Supported                                                                                                                                                                                  |
| ReduceL2              | Not Supported                                                                                                                                                                                  |
| ReduceLogSum          | Not Supported                                                                                                                                                                                  |
| ReduceLogSumExp       | Not Supported                                                                                                                                                                                  |
| ReduceMax             |                                                                                                                                                                                                |
| ReduceMean            | **NPU Limit:**<br />channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8192]                                                                                                            |
| ReduceMin             |                                                                                                                                                                                                |
| ReduceProd            | Not Supported                                                                                                                                                                                  |
| ReduceSum             | **NPU Limit:**<br />channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8192]                                                                                                            |
| ReduceSumSquare       |                                                                                                                                                                                                |
| Relu                  |                                                                                                                                                                                                |
| Reshape               | **NPU Limit:**<br />channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                            |
| Resize                | **NPU Limit:**<br />channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]<br />scales: [1, 8]                                                                                        |
| ReverseSequence       |                                                                                                                                                                                                |
| RoiAlign              | pool type: average<br />batchsize: 1                                                                                                                                                           |
| Round                 | Not Supported                                                                                                                                                                                  |
| Scan                  | Not Supported                                                                                                                                                                                  |
| ScatterElements       | Not Supported                                                                                                                                                                                  |
| ScatterND             | Not Supported                                                                                                                                                                                  |
| Selu                  | Not Supported                                                                                                                                                                                  |
| SequenceAt            | Not Supported                                                                                                                                                                                  |
| SequenceConstruct     | Not Supported                                                                                                                                                                                  |
| SequenceEmpty         | Not Supported                                                                                                                                                                                  |
| SequenceErase         | Not Supported                                                                                                                                                                                  |
| SequenceInsert        | Not Supported                                                                                                                                                                                  |
| SequenceLength        | Not Supported                                                                                                                                                                                  |
| Shape                 |                                                                                                                                                                                                |
| Shrink                | Not Supported                                                                                                                                                                                  |
| Sigmoid               |                                                                                                                                                                                                |
| Sign                  | Not Supported                                                                                                                                                                                  |
| Sin                   | Not Supported                                                                                                                                                                                  |
| Sinh                  | Not Supported                                                                                                                                                                                  |
| Size                  |                                                                                                                                                                                                |
| Slice                 | batchsize: 1<br />**NPU Limit:**<br />steps: 1                                                                                                                                                 |
| Softmax               | batchsize: 1<br />**NPU Limit:**<br />channel: [1, 8192]<br />axis: 1                                                                                                                          |
| Softplus              |                                                                                                                                                                                                |
| Softsign              | Not Supported                                                                                                                                                                                  |
| SpaceToDepth          |                                                                                                                                                                                                |
| Split                 |                                                                                                                                                                                                |
| SplitToSequence       | Not Supported                                                                                                                                                                                  |
| Sqrt                  |                                                                                                                                                                                                |
| Squeeze               |                                                                                                                                                                                                |
| StringNormalizer      | Not Supported                                                                                                                                                                                  |
| Sub                   | **NPU Limit:**<br />support broadcast rule: per-layer/channel/element                                                                                                                          |
| Sum                   | Not Supported                                                                                                                                                                                  |
| Tan                   | Not Supported                                                                                                                                                                                  |
| Tanh                  |                                                                                                                                                                                                |
| TfIdfVectorizer       | Not Supported                                                                                                                                                                                  |
| ThresholdedRelu       | Not Supported                                                                                                                                                                                  |
| Tile                  | batchsize: 1<br />not support broadcast                                                                                                                                                        |
| TopK                  | Not Supported                                                                                                                                                                                  |
| Transpose             | **NPU Limit:**<br />channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176]                                                                                                            |
| Trilu                 | Not Supported                                                                                                                                                                                  |
| Unique                | Not Supported                                                                                                                                                                                  |
| Unsqueeze             |                                                                                                                                                                                                |
| Where                 |                                                                                                                                                                                                |
| Xor                   | Not Supported                                                                                                                                                                                  |  |  |


## Caffe OPs supported by RKNN Toolkit2

Caffe protocols RKNN Toolkit2 uses only based on the officially modified protocol of berkeley.
The protocol based on the official revision of berkeley comes from [berkeley caffe](https://github.com/BVLC/caffe/tree/master/src/caffe/proto 'Berkeley Caffe'), commit hash is 21d0608. On this basis RKNN Toolkit2 have added some OPs.  
Based on this protocol, the list of Caffe OPs supported by RKNN Toolkit2 is as follows:

| **Operators**          | **Remarks**                                                                                                   |
| ---------------------- | ------------------------------------------------------------------------------------------------------------- |
| BatchNorm              | same as onnx BatchNormalization                                                                               |
| bn (BatchNorm + Scale) | same as onnx BatchNormalization according to https://github.com/TimoSaemann/caffe-segnet-cudnn5               |
| BNLL                   |                                                                                                               |
| Concat                 | same as onnx Concat                                                                                           |
| Convolution            | same as onnx Conv                                                                                             |
| ConvolutionDepthwise   | kernel height/width: [1, 8]<br />others same as onnx Conv                                                     |
| Crop                   |                                                                                                               |
| Deconvolution          | same as ConvTranspose                                                                                         |
| Dropout                |                                                                                                               |
| Eltwise                | support broadcast rule: per-layer/channel/element                                                             |
| Flatten                |                                                                                                               |
| HardSigmoid            |                                                                                                               |
| InnerProduct           | same as onnx Gemm                                                                                             |
| LRN                    | same as onnx LRN                                                                                              |
| Lstm                   | same as onnx LSTM according to https://github.com/xmfbit/warpctc-caffe                                        |
| Normalize              |                                                                                                               |
| Permute                | same as onnx Transpose                                                                                        |
| Power                  |                                                                                                               |
| Pooling                | same as onnx pooling                                                                                          |
| PRelu                  | same as onnx PRelu                                                                                            |
| Proposal               | batch: 1                                                                                                      |
| Reduction              | output dims <= 4                                                                                              |
| Relu                   | same as onnx Relu                                                                                             |
| Relu6                  | same as onnx Clip                                                                                             |
| Reorg                  |                                                                                                               |
| Reshape                | same as onnx Reshape                                                                                          |
| Resize                 | bilinear; nearest                                                                                             |
| Reverse                |                                                                                                               |
| ROIPooling             | same as MaxRoiPool according to https://github.com/twmht/caffe-pva-faster-rcnn                                |
| Scale                  | same as onnx Mul                                                                                              |
| Sigmoid                | same as onnx Sigmoid                                                                                          |
| Slice                  | same as onnx Split                                                                                            |
| Softmax                | same as onnx Softmax                                                                                          |
| Split                  | same as onnx Slice                                                                                            |
| TanH                   | same as onnx TanH                                                                                             |
| Tile                   | same as onnx Tile                                                                                             |
| Transpose              | same as onnx Transpose                                                                                        |
| Upsample               | according to https://github.com/SeanQ88/caffe_upsample and https://github.com/TimoSaemann/caffe-segnet-cudnn5 |


## Pytorch OPs supported by RKNN Toolkit2

The Pytorch version supported by RKNN Toolkit2 is >1.6.0, models generated by other versions may not support.  
The list of Pytorch OPs supported by RKNN Toolkit2 is as follows:

| **Operators**                 | **Remarks**                                                                        |
| ----------------------------- | ---------------------------------------------------------------------------------- |
| aten::_convolution            | same as onnx Conv                                                                  |
| aten::abs                     | Not supported                                                                      |
| aten::abs_                    | Not supported                                                                      |
| aten::adaptive_avg_pool1d     | Not supported                                                                      |
| aten::adaptive_avg_pool2d     | same as onnx AveragePool                                                           |
| aten::adaptive_max_pool1d     | Not supported                                                                      |
| aten::adaptive_max_pool2d     | same as onnx MaxPool                                                               |
| aten::add                     | same as onnx Add                                                                   |
| aten::add_                    |                                                                                    |
| aten::addmm                   | same as onnx Gemm                                                                  |
| aten::affine_grid_generator   | Not supported                                                                      |
| aten::alpha_dropout           |                                                                                    |
| aten::alpha_dropout_          | Not supported                                                                      |
| aten::arange                  | Not supported                                                                      |
| aten::avg_pool1d              | Not supported                                                                      |
| aten::avg_pool2d              | same as onnx AveragePool                                                           |
| aten::avg_pool3d              | Not supported                                                                      |
| aten::batch_norm              | same as onnx BatchNormalization                                                    |
| aten::bmm                     | same as onnx MatMul                                                                |
| aten::cat                     | same as onnx Concat                                                                |
| aten::celu                    | Not supported                                                                      |
| aten::celu_                   | Not supported                                                                      |
| aten::chunk                   |                                                                                    |
| aten::clamp                   |                                                                                    |
| aten::clamp_                  |                                                                                    |
| aten::clamp_max               | Not supported                                                                      |
| aten::clamp_max_              | Not supported                                                                      |
| aten::clamp_min               | Not supported                                                                      |
| aten::clamp_min_              | Not supported                                                                      |
| aten::clone                   |                                                                                    |
| aten::constant_pad_nd         | same as onnx Pad                                                                   |
| aten::contiguous              |                                                                                    |
| aten::copy                    |                                                                                    |
| aten::cos                     | Not supported                                                                      |
| aten::cos_                    | Not supported                                                                      |
| aten::cumsum                  | Not supported                                                                      |
| aten::detach                  |                                                                                    |
| aten::detach_                 | Not supported                                                                      |
| aten::div                     | same as onnx Div                                                                   |
| aten::div_                    |                                                                                    |
| aten::dropout                 |                                                                                    |
| aten::dropout_                |                                                                                    |
| aten::einsum                  | Not supported                                                                      |
| aten::elu                     | same as onnx Elu                                                                   |
| aten::elu_                    |                                                                                    |
| aten::embedding               | same as onnx Gather                                                                |
| aten::empty                   |                                                                                    |
| aten::eq                      | Not supported                                                                      |
| aten::eq_                     | Not supported                                                                      |
| aten::erf                     | Not supported                                                                      |
| aten::erf_                    | Not supported                                                                      |
| aten::erfc                    | Not supported                                                                      |
| aten::erfc_                   | Not supported                                                                      |
| aten::exp                     |                                                                                    |
| aten::exp_                    |                                                                                    |
| aten::expand                  | Not supported                                                                      |
| aten::expand_as               | Not supported                                                                      |
| aten::expm1                   | Not supported                                                                      |
| aten::expm1_                  | Not supported                                                                      |
| aten::feature_dropout         |                                                                                    |
| aten::feature_dropout_        | Not supported                                                                      |
| aten::flatten                 |                                                                                    |
| aten::floor                   | Not supported                                                                      |
| aten::floor_                  | Not supported                                                                      |
| aten::floor_divide            | Not supported                                                                      |
| aten::floor_divide_           | Not supported                                                                      |
| aten::gather                  | Not supported                                                                      |
| aten::ge                      | Not supported                                                                      |
| aten::ge_                     | Not supported                                                                      |
| aten::gelu                    |                                                                                    |
| aten::gelu_                   | Not supported                                                                      |
| aten::grid_sampler            | Not supported                                                                      |
| aten::gru                     |                                                                                    |
| aten::gt                      |                                                                                    |
| aten::gt_                     | Not supported                                                                      |
| aten::hardshrink              | Not supported                                                                      |
| aten::hardshrink_             | Not supported                                                                      |
| aten::hardswish               | same as onnx HardSwish                                                             |
| aten::hardswish_              |                                                                                    |
| aten::hardtanh                |                                                                                    |
| aten::hardtanh_               |                                                                                    |
| aten::index                   | Not supported                                                                      |
| aten::index_put               | Not supported                                                                      |
| aten::index_put_              | Not supported                                                                      |
| aten::instance_norm           | same as onnx InstanceNormalization                                                 |
| aten::Int                     |                                                                                    |
| aten::layer_norm              | **NPU Limit**<br />channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8192] |
| aten::le                      | Not supported                                                                      |
| aten::le_                     | Not supported                                                                      |
| aten::leaky_relu              | same as onnx LeakyRelu                                                             |
| aten::leaky_relu_             |                                                                                    |
| aten::lerp                    | Not supported                                                                      |
| aten::lerp_                   | Not supported                                                                      |
| aten::log                     | Not supported                                                                      |
| aten::log_                    | Not supported                                                                      |
| aten::log10                   | Not supported                                                                      |
| aten::log10_                  | Not supported                                                                      |
| aten::log1p                   | Not supported                                                                      |
| aten::log1p_                  | Not supported                                                                      |
| aten::log2                    | Not supported                                                                      |
| aten::log2_                   | Not supported                                                                      |
| aten::log_sigmoid             | Not supported                                                                      |
| aten::log_softmax             | Not supported                                                                      |
| aten::linear                  | same as onnx Gemm                                                                  |
| aten::lstm                    | same as onnx LSTM                                                                  |
| aten::lt                      |                                                                                    |
| aten::lt_                     | Not supported                                                                      |
| aten::matmul                  | same as onnx MatMul                                                                |
| aten::max                     |                                                                                    |
| aten::max_                    | Not supported                                                                      |
| aten::max_pool1d              | same as onnx MaxPool                                                               |
| aten::max_pool1d_with_indices |                                                                                    |
| aten::max_pool2d              | same as onnx MaxPool                                                               |
| aten::max_pool2d_with_indices |                                                                                    |
| aten::mean                    | same as onnx ReduceMean                                                            |
| aten::meshgrid                | Not supported                                                                      |
| aten::min                     |                                                                                    |
| aten::min_                    | Not supported                                                                      |
| aten::mm                      | same as onnx MatMul                                                                |
| aten::mul                     | same as onnx Mul                                                                   |
| aten::mul_                    |                                                                                    |
| aten::narrow                  | same as onnx Slice                                                                 |
| aten::ne                      | Not supported                                                                      |
| aten::ne_                     | Not supported                                                                      |
| aten::neg                     | Not supported                                                                      |
| aten::neg_                    | Not supported                                                                      |
| aten::new_full                | Not supported                                                                      |
| aten::new_zeros               | Not supported                                                                      |
| aten::nonzero                 | Not supported                                                                      |
| aten::norm                    | Not supported                                                                      |
| aten::ones                    |                                                                                    |
| aten::ones_like               |                                                                                    |
| aten::pad                     | Not supported                                                                      |
| aten::permute                 | same as onnx Transpose                                                             |
| aten::pow                     |                                                                                    |
| aten::pow_                    | Not supported                                                                      |
| aten::prelu                   | same as onnx PRelu                                                                 |
| aten::prelu_                  | Not supported                                                                      |
| aten::reciprocal              |                                                                                    |
| aten::reciprocal_             | Not supported                                                                      |
| aten::reflection_pad1d        |                                                                                    |
| aten::reflection_pad2d        |                                                                                    |
| aten::relu                    | same as onnx Relu                                                                  |
| aten::relu_                   |                                                                                    |
| aten::repeat                  |                                                                                    |
| aten::reshape                 |                                                                                    |
| aten::reshape_                | Not supported                                                                      |
| torchvision::roi_align        | Not supported                                                                      |
| aten::rsqrt                   | Not supported                                                                      |
| aten::rsqrt_                  | Not supported                                                                      |
| aten::ScalarImplicit          |                                                                                    |
| aten::select                  |                                                                                    |
| aten::selu                    | Not supported                                                                      |
| aten::selu_                   | Not supported                                                                      |
| aten::sigmoid                 | same as onnx Sigmoid                                                               |
| aten::sigmoid_                |                                                                                    |
| aten::silu                    |                                                                                    |
| aten::silu_                   |                                                                                    |
| aten::sin                     | Not supported                                                                      |
| aten::sin_                    | Not supported                                                                      |
| aten::size                    |                                                                                    |
| aten::slice                   | same as onnx Slice                                                                 |
| aten::softmax                 | same as onnx Softmax                                                               |
| aten::softplus                |                                                                                    |
| aten::softshrink              | Not supported                                                                      |
| aten::sort                    | Not supported                                                                      |
| aten::split                   | same as onnx Split                                                                 |
| aten::split_with_sizes        |                                                                                    |
| aten::sqrt                    | Not supported                                                                      |
| aten::sqrt_                   | Not supported                                                                      |
| aten::squeeze                 |                                                                                    |
| aten::squeeze_                | Not supported                                                                      |
| aten::stack                   |                                                                                    |
| aten::sub                     | same as onnx Sub                                                                   |
| aten::sub_                    |                                                                                    |
| aten::sum                     | same as onnx ReduceSum                                                             |
| aten::t                       |                                                                                    |
| aten::t_                      | Not supported                                                                      |
| aten::tanh                    |                                                                                    |
| aten::tanh_                   |                                                                                    |
| aten::threshold               |                                                                                    |
| aten::threshold_              |                                                                                    |
| aten::to                      |                                                                                    |
| aten::topk                    | Not supported                                                                      |
| aten::transpose               |                                                                                    |
| aten::transpose_              |                                                                                    |
| aten::true_divide             | same as onnx Div                                                                   |
| aten::true_divide_            | Not supported                                                                      |
| aten::type_as                 |                                                                                    |
| aten::unfold                  | Not supported                                                                      |
| aten::unsqueeze               |                                                                                    |
| aten::upsample_bilinear2d     |                                                                                    |
| aten::upsample_nearest2d      |                                                                                    |
| aten::view                    |                                                                                    |
| aten::view_                   | Not supported                                                                      |
| aten::view_as                 | Not supported                                                                      |
| aten::view_as_                | Not supported                                                                      |
| aten::zero_                   | Not supported                                                                      |
| aten::zeros                   |                                                                                    |
| aten::zeros_like              |                                                                                    |

## TensorFlow OPs supported by RKNN Toolkit2

The pb files (contain OPs belows) generated by TensorFlow version 1.12 - 1.15 for 1.x and 2.3 - 2.5 for 2.x are supported by RKNN Toolkit2. For more information on TensorFlow version compatibility, please refer to [tensorflow official instructions on OP version](https://www.tensorflow.org/guide/versions 'Tensorflow official instructions on OP version') . 
The list of TensorFlow OPs supported by RKNN Toolkit2 is as follows:

| **Operators**         | **Remarks**                                                     |
| --------------------- | --------------------------------------------------------------- |
| Add                   | same as onnx Add                                                |
| AvgPool               | same as onnx AveragePool                                        |
| Concat                | same as onnx Concat                                             |
| Conv2D                | same as onnx Conv                                               |
| DepthToSpace          |                                                                 |
| DepthwiseConv2d       | kernel height/width: [1, 8]<br />others same as onnx Conv       |
| Div                   | same as onnx Div                                                |
| Dropout               |                                                                 |
| Flatten               |                                                                 |
| LeakyRelu             | same as onnx LeakyRelu                                          |
| Less                  | same as onnx Less                                               |
| LRN                   |                                                                 |
| MatMul                |                                                                 |
| MaxPool               | same as onnx MaxPool                                            |
| Mean                  | output dims <= 4                                                |
| Pad                   | same as onnx Pad                                                |
| Relu                  | same as onnx Relu                                               |
| Reshape               |                                                                 |
| ResizeBilinear        |                                                                 |
| ResizeNearestNeighbor |                                                                 |
| Sigmoid               |                                                                 |
| Slice                 |                                                                 |
| Softmax               |                                                                 |
| Softplus              | channel: [1, 8192]<br />height: [1, 8192]<br />width: [1, 8176] |
| SpaceToDepth          |                                                                 |
| Split                 |                                                                 |
| Squeeze               |                                                                 |
| StridedSlice          |                                                                 |
| Tanh                  | same as onnx TanH                                               |
| Transpose             |                                                                 |

## Darknet OPs supported by RKNN Toolkit2
The list of Darknet OPs supported by RKNN Toolkit2 is as follows:

| **Operators**           | **Remarks**                                                                                                                                                                   |
| ----------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| add                     | same as onnx Add                                                                                                                                                              |
| batchnormalize          | same as onnx BatchNormalization                                                                                                                                               |
| concat                  | same as onnx Concat                                                                                                                                                           |
| convolutional           | same as onnx Conv                                                                                                                                                             |
| depthwise_convolutional | kernel height/width: [1, 8]<br />others same as onnx Conv                                                                                                                     |
| fullconnect             |                                                                                                                                                                               |
| leakyrelu               | same as onnx LeakyRelu                                                                                                                                                        |
| mish                    |                                                                                                                                                                               |
| pooling                 | **AveragePool**: same as onnx AveragePool   <br /> **GlobalAveragePool**: same as onnx GlobalAveragePool <br /> **MaxPool/GlobalMaxPool**: same as onnx MaxPool/GlobalMaxPool |
| route                   |                                                                                                                                                                               |
| shortcut                |                                                                                                                                                                               |
| softmax                 |                                                                                                                                                                               |
| upsampling              |                                                                                                                                                                               |