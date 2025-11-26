#include "cv_util.h"
#include "logger.h"

#include <algorithm>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

int main(int argc, char** argv)
{
    logger::Logger log{logger::Level::DEBUG, "[cv-demo] {}"};

    log.info("start");

    if (argc < 2)
    {
        log.error("Usage: {} <image_path>", argv[0]);
        return 2;
    }

    const std::string path = argv[1];
    log.debug("loading image: {}", path);
    cv::Mat img;
    try
    {
        img = cv_util::load(path);
    }
    catch (const std::exception& e)
    {
        log.error("{}", e.what());
        return 1;
    }

    log.info("loaded {}x{} ({} channels)", img.cols, img.rows, img.channels());

    // Simple annotate to prove processing
    cv::putText(img, "OpenCV loaded", {10, 30}, cv::FONT_ITALIC, 1.0, cv::Scalar(0, 255, 0), 2,
                cv::LINE_AA);

    if (!cv_util::quickDisplay(img, "Image", 0, true, 1024, 768))
    {
        const std::string out = "output.png";
        if (!cv::imwrite(out, img))
        {
            log.error("failed to write {}", out);
            return 1;
        }
        log.warn("no GUI detected; wrote {}", out);
    }

    log.info("done");
    return 0;
}
