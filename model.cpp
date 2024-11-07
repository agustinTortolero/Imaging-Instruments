#include "model.h"



ImagingInstrumentsModel::ImagingInstrumentsModel(QObject *parent)
    : QObject(parent)
{
}

ImagingInstrumentsModel::~ImagingInstrumentsModel()
{

}

void ImagingInstrumentsModel::applyBlur()
{
    if (inputImage.empty()) return;

    cv::Mat rgbImage;
    cv::cvtColor(inputImage, rgbImage, cv::COLOR_BGR2RGB);

    cv::GaussianBlur(rgbImage, outputImage, cv::Size(15, 15), 0); // Adjust kernel size as needed
}



void ImagingInstrumentsModel::applyDeBlur() {
    if (inputImage.empty()) return;

    int iterations = 50;
    cv::Mat rgbImage;
    cv::cvtColor(inputImage, rgbImage, cv::COLOR_BGR2RGB);
    cv::Mat inputFloat;
    rgbImage.convertTo(inputFloat, CV_32F, 1.0 / 255.0);

    outputImage = inputFloat.clone();
    cv::Mat kernel = cv::getGaussianKernel(9, 2, CV_32F);
    kernel = kernel * kernel.t();

    for (int i = 0; i < iterations; ++i) {
        cv::Mat estimate = outputImage.clone();
        cv::Mat convolved;
        cv::filter2D(estimate, convolved, CV_32F, kernel);

        cv::Mat ratio;
        cv::divide(inputFloat, convolved + 1e-6, ratio);

        cv::threshold(ratio, ratio, 5.0, 5.0, cv::THRESH_TRUNC);

        cv::Mat update;
        cv::filter2D(ratio, update, CV_32F, kernel, cv::Point(-1, -1), 0, cv::BORDER_REPLICATE);

        outputImage = estimate.mul(update);
        outputImage = cv::max(outputImage, 0.0f);
        outputImage = cv::min(outputImage, 1.0f);

        double minVal, maxVal;
        cv::minMaxLoc(outputImage, &minVal, &maxVal);
        qDebug() << "Iteration:" << i << ", Min:" << minVal << ", Max:" << maxVal;
    }

    outputImage.convertTo(outputImage, CV_8U, 255.0);
}


void ImagingInstrumentsModel::applySobelEdgeDetection()
{
    if (inputImage.empty()) return;

    cv::Mat grayImage;
    cv::cvtColor(inputImage, grayImage, cv::COLOR_BGR2GRAY);

    cv::Mat grad_x, grad_y;
    cv::Mat abs_grad_x, abs_grad_y;

    cv::Sobel(grayImage, grad_x, CV_16S, 1, 0, 3);
    cv::convertScaleAbs(grad_x, abs_grad_x);

    cv::Sobel(grayImage, grad_y, CV_16S, 0, 1, 3);
    cv::convertScaleAbs(grad_y, abs_grad_y);

    cv::Mat sobelImage;
    cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, sobelImage);

    outputImage = sobelImage;
}

void ImagingInstrumentsModel::applyBinarization(int threshold)
{
    if (inputImage.empty()) {
        qDebug() << "Error: inputImage is empty before binarization.";
        return;
    }

    cv::Mat grayImage;

    if (inputImage.channels() == 3) {
        cv::cvtColor(inputImage, grayImage, cv::COLOR_BGR2GRAY);
        qDebug() << "Converted to grayscale.";
    } else {
        grayImage = inputImage;
    }

    cv::threshold(grayImage, outputImage, threshold, 255, cv::THRESH_BINARY);

    qDebug() << "Binarization applied with threshold:" << threshold;
}


void ImagingInstrumentsModel::applyErosion(int erosionSize)
{
    if (inputImage.empty()) {
        qDebug() << "Error: inputImage is empty before applying erosion.";
        return;
    }

    cv::Mat grayImage;

    if (inputImage.channels() == 3) {
        cv::cvtColor(inputImage, grayImage, cv::COLOR_BGR2GRAY);
        qDebug() << "Converted to grayscale.";
    } else {
        grayImage = inputImage;
    }

    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
                                                cv::Size(2 * erosionSize + 1, 2 * erosionSize + 1),
                                                cv::Point(erosionSize, erosionSize));

    cv::erode(grayImage, outputImage, element);

    qDebug() << "Erosion applied with kernel size:" << erosionSize;
}


void ImagingInstrumentsModel::applyDilation(int dilationSize)
{
    if (inputImage.empty()) {
        qDebug() << "Error: inputImage is empty before applying dilation.";
        return;
    }

    cv::Mat grayImage;

    if (inputImage.channels() == 3) {
        cv::cvtColor(inputImage, grayImage, cv::COLOR_BGR2GRAY);
        qDebug() << "Converted to grayscale.";
    } else {
        grayImage = inputImage;
    }

    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
                                                cv::Size(2 * dilationSize + 1, 2 * dilationSize + 1),
                                                cv::Point(dilationSize, dilationSize));

    cv::dilate(grayImage, outputImage, element);

    qDebug() << "Dilation applied with kernel size:" << dilationSize;
}


void ImagingInstrumentsModel::applyOpening(int openingSize)
{
    if (inputImage.empty()) {
        qDebug() << "Error: inputImage is empty before applying opening.";
        return;
    }

    cv::Mat grayImage;

    if (inputImage.channels() == 3) {
        cv::cvtColor(inputImage, grayImage, cv::COLOR_BGR2GRAY);
        qDebug() << "Converted to grayscale.";
    } else {
        grayImage = inputImage;
    }

    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
                                                cv::Size(2 * openingSize + 1, 2 * openingSize + 1),
                                                cv::Point(openingSize, openingSize));

    cv::morphologyEx(grayImage, outputImage, cv::MORPH_OPEN, element);

    qDebug() << "Opening applied with kernel size:" << openingSize;
}


void ImagingInstrumentsModel::applyClosing(int closingSize)
{
    if (inputImage.empty()) {
        qDebug() << "Error: inputImage is empty before applying closing.";
        return;
    }

    cv::Mat grayImage;

    if (inputImage.channels() == 3) {
        cv::cvtColor(inputImage, grayImage, cv::COLOR_BGR2GRAY);
        qDebug() << "Converted to grayscale.";
    } else {
        grayImage = inputImage;
    }

    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,
                                                cv::Size(2 * closingSize + 1, 2 * closingSize + 1),
                                                cv::Point(closingSize, closingSize));

    cv::morphologyEx(grayImage, outputImage, cv::MORPH_CLOSE, element);

    qDebug() << "Closing applied with kernel size:" << closingSize;
}

void ImagingInstrumentsModel::updateDrawingImage(const QImage &drawing) {
    if (!drawing.isNull()) {
        drawingImage = cv::Mat(drawing.height(), drawing.width(), CV_8UC4, (uchar*)drawing.bits(), drawing.bytesPerLine()).clone();
    }
}
