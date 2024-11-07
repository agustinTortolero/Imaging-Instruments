#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QImage>
#include <QDebug>
#include <opencv2/opencv.hpp>

class ImagingInstrumentsModel : public QObject
{
    Q_OBJECT

public:
    explicit ImagingInstrumentsModel(QObject *parent = nullptr);
    ~ImagingInstrumentsModel();

    // Example of member variables
    cv::Mat inputImage;
    cv::Mat outputImage;
    cv::Mat outputImageRGB;
    cv::Mat noisyImage;
    cv::Mat filteredImage;
    cv::Mat originalInputImage;
    cv::Mat drawingImage; // New member to hold the current drawing

    const cv::Mat& getOriginalInputImage() const {
        return originalInputImage;
    }

    void setOriginalInputImage(const cv::Mat& image) {
        originalInputImage = image.clone(); // Store a clone to preserve original data
    }

    void applySobelEdgeDetection();
    void applyBlur();
    void applyDeBlur();


    void applyBinarization(int threshold = 128);
    void applyErosion(int erosionSize = 3);
    void applyDilation(int dilationSize = 3);
    void applyOpening(int openingSize = 3);
    void applyClosing(int closingSize = 3);

    void updateDrawingImage(const QImage &drawing); // New method

private:
         // Any private member functions or variables
};

#endif // MODEL_H

