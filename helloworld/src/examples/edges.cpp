/**
 * \file
 * \ingroup examples
 * Canny edge detection with optional Gaussian blur.
 */
#include "cli/argparse.h"
#include "cv_util.h"
#include "examples/registry.h"
#include "logger.h"

#include <algorithm>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>

using examples::ExampleFn;

static int edges_example(int argc, char** argv)
{
    logger::Logger log{"edges", logger::Level::INFO};

    cli::ArgParser ap{"edges"};
    ap.add_option("t1", 'l', "Canny lower threshold", "100");
    ap.add_option("t2", 'u', "Canny upper threshold", "200");
    ap.add_option("blur", 'b', "Gaussian blur kernel size (odd)", "3");
    ap.add_positional("path", "Image path (default: assets/lena_img.png)");
    if (!ap.parse(argc, argv) || ap.help())
    {
        log.info("\n{}", ap.usage());
        return ap.help() ? 0 : 2;
    }
    std::string path =
        ap.positionals().empty() ? std::string{"assets/lena_img.png"} : ap.positionals().front();
    int t1 = std::max(0, ap.get_int("t1", 100));
    int t2 = std::max(0, ap.get_int("t2", 200));
    int blur = std::max(0, ap.get_int("blur", 3));
    if (blur % 2 == 0 && blur > 0)
        ++blur;

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
