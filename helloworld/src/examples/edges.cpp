#include "cv_util.h"
#include "examples/registry.h"
#include "logger.h"

#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>

using examples::ExampleFn;

static int edges_example(int argc, char** argv)
{
    logger::Logger log{logger::Level::INFO, "[edges] {}"};

    // Defaults
    std::string path;
    int t1 = 100;
    int t2 = 200;
    int blur = 3; // odd > 0 to smooth noise

    // Parse flags: --t1 N --t2 N --blur K, first non-flag is path
    for (int i = 1; i < argc; ++i)
    {
        std::string a = argv[i];
        if (a == "--t1" && i + 1 < argc)
        {
            t1 = std::max(0, std::stoi(argv[++i]));
        }
        else if (a == "--t2" && i + 1 < argc)
        {
            t2 = std::max(0, std::stoi(argv[++i]));
        }
        else if (a == "--blur" && i + 1 < argc)
        {
            blur = std::max(0, std::stoi(argv[++i]));
            if (blur % 2 == 0 && blur > 0)
                ++blur; // make odd
        }
        else if (!a.empty() && a[0] != '-')
        {
            path = a;
        }
    }
    if (path.empty())
        path = "assets/lena_img.png";

    log.info("loading {} (t1={}, t2={}, blur={})", path, t1, t2, blur);

    cv::Mat src;
    try
    {
        src = cv_util::load(path);
    }
    catch (const std::exception& e)
    {
        log.error("{}", e.what());
        return 1;
    }

    cv::Mat work = src;
    if (blur > 0)
    {
        cv::GaussianBlur(src, work, cv::Size(blur, blur), 0);
    }

    cv::Mat gray;
    cv::cvtColor(work, gray, cv::COLOR_BGR2GRAY);
    cv::Mat edges;
    cv::Canny(gray, edges, t1, t2);

    // Visualize: paint edges in red over original
    cv::Mat vis = src.clone();
    vis.setTo(cv::Scalar(0, 0, 255), edges);

    if (!cv_util::quickDisplay(vis, "Edges", 0, true, 1024, 768))
    {
        const std::string out = "output_edges.png";
        if (!cv::imwrite(out, vis))
        {
            log.error("failed to write {}", out);
            return 1;
        }
        log.warn("headless environment; wrote {}", out);
    }

    return 0;
}

REGISTER_EXAMPLE("edges", edges_example, "Canny edge detection with --t1/--t2/--blur");
