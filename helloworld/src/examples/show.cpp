#include "cv_util.h"
#include "examples/registry.h"
#include "logger.h"

#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>

using examples::ExampleFn;

static int show_example(int argc, char** argv)
{
    logger::Logger log{logger::Level::INFO, "[show] {}"};
    std::string path;

    // Parse very simple args: first non-flag is path
    for (int i = 1; i < argc; ++i)
    {
        std::string a = argv[i];
        if (!a.empty() && a[0] != '-')
        {
            path = a;
            break;
        }
    }
    if (path.empty())
        path = "assets/lena_img.png"; // default

    log.info("loading {}", path);
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

    // Draw a small overlay to prove processing
    cv::putText(img, "show", {10, 30}, cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 0, 0), 2,
                cv::LINE_AA);

    if (!cv_util::quickDisplay(img, "Show", 0, true, 1024, 768))
    {
        const std::string out = "output.png";
        if (!cv::imwrite(out, img))
        {
            log.error("failed to write {}", out);
            return 1;
        }
        log.warn("headless environment; wrote {}", out);
    }

    return 0;
}

REGISTER_EXAMPLE("show", show_example, "Display an image (default: assets/lena_img.png)");
