/**
 * \file
 * Header-only CV utilities used by examples.
 */
#pragma once

#include <algorithm>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <stdexcept>
#include <string>

namespace cv_util
{

// Load an image or throw on failure.
inline cv::Mat load(const std::string& path, int flags = cv::IMREAD_COLOR)
{
    cv::Mat img = cv::imread(path, flags);
    if (img.empty())
    {
        throw std::runtime_error("cv_util::load: failed to load image: " + path);
    }
    return img;
}

// Show image in a resizable window with optional max size. Returns true if shown.
inline bool quickDisplay(const cv::Mat& img, const std::string& title = "Image", int wait_ms = 0,
                         bool resizable = true, int max_width = 1024, int max_height = 768)
{
    if (img.empty())
        return false;

    // Detect GUI availability (X11/Wayland)
    const bool has_gui = std::getenv("DISPLAY") || std::getenv("WAYLAND_DISPLAY");
    if (!has_gui)
        return false;

    int flags = resizable ? cv::WINDOW_NORMAL : cv::WINDOW_AUTOSIZE;
    cv::namedWindow(title, flags);

    // Set an initial reasonable size while keeping aspect ratio
    int w = img.cols;
    int h = img.rows;
    if (w > max_width || h > max_height)
    {
        const double rw = static_cast<double>(max_width) / std::max(1, w);
        const double rh = static_cast<double>(max_height) / std::max(1, h);
        const double r = std::min(rw, rh);
        w = std::max(1, static_cast<int>(w * r));
        h = std::max(1, static_cast<int>(h * r));
    }
    if (resizable)
        cv::resizeWindow(title, w, h);

    cv::imshow(title, img);
    cv::waitKey(wait_ms); // 0 waits indefinitely
    return true;
}

} // namespace cv_util
