#include <NumberDetector.h>
#include <MNN/ImageProcess.hpp>
typedef std::chrono::duration<double, std::ratio<1, 1000>> ms;
NumberDetector::NumberDetector() {
    status = 0;
    TOTAL_INFER_TIME = 0;
}

void NumberDetector::Initialize() {
    interpreter = Interpreter::createFromFile(model_name);
    ScheduleConfig config;
    config.numThread = 4;
    MNN::BackendConfig backendConfig;
    backendConfig.memory = MNN::BackendConfig::Memory_High;
    backendConfig.power = MNN::BackendConfig::Power_High;
    backendConfig.precision = MNN::BackendConfig::Precision_Low;
    config.backendConfig = &backendConfig;
    config.backupType = MNN_FORWARD_VULKAN;
    session = interpreter->createSession(config);
    input_tensor = interpreter->getSessionInput(session,NULL);
    nchwTensor = new Tensor(input_tensor,Tensor::CAFFE);
    status = 1;
    LOG(INFO) << "net init done!";
}

pair<int, float> NumberDetector::detect(const Mat &img_raw){
    if(status == -1) return pair<int,double>(-1,0);
    auto t0 = std::chrono::high_resolution_clock::now();
    Mat img;
    cv::cvtColor(img_raw,img,COLOR_BGR2RGB);
    img.convertTo(img,CV_32FC3);
    img = (img - cv::Scalar(0.485, 0.456, 0.406) * 255) / (cv::Scalar(0.229, 0.224, 0.225) * 255);
    cv::Mat inputBlob = cv::dnn::blobFromImage(img, 1.0,cv::Size(224,224));

    auto data = nchwTensor->host<float>();
    ::memcpy(data,inputBlob.data,nchwTensor->size());
    input_tensor->copyFromHostTensor(nchwTensor);
    interpreter->runSession(session);
    Tensor* out = interpreter->getSessionOutput(session,NULL);

    out->copyFromHostTensor(out);

    auto shape = out->shape();
    float outs[16] = {0};
    ::memcpy(outs,out->host<float>(),out->size());
    //softmax
    int max_id = std::max_element(outs, outs + 16) - outs;
    float maxv = outs[max_id];
    float tot = 0;
    for(int i = 0;i < 16;++i){
        outs[i] = std::exp(outs[i]- maxv);
        tot += outs[i];
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double nowms = std::chrono::duration_cast<ms>(t1 - t0).count();
    TOTAL_INFER_TIME += nowms;

    return pair<int,float>(max_id,outs[max_id] / tot);
}