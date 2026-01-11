#pragma once
#include <string>
#include <sstream>
#include <cstdio>
#include <Color.h>
#include <cmath>
#include <chrono>

namespace TemplatePlugin {
    struct colors {
        float r, g, b, a;

        colors(float red, float green, float blue, float alpha = 1.0f)
            : r(red), g(green), b(blue), a(alpha) {}

        static colors Red() { return colors(1.f, 0.f, 0.f, 1.f); }
        static colors Green() { return colors(0.f, 1.f, 0.f, 1.f); }
        static colors Blue() { return colors(0.f, 0.f, 1.f, 1.f); }
        static colors Yellow() { return colors(1.f, 1.f, 0.f, 1.f); }
        static colors Purple() { return colors(0.5f, 0.f, 0.5f, 1.f); }
        static colors Orange() { return colors(1.f, 0.5f, 0.f, 1.f); }
        static colors Rainbow() { return colors(0.247f, 0.655f, 0.839f, 1.0f); }

        static colors FromHex(const std::string& hex) {
            unsigned int rgb = 0;
            std::string clean = hex;
            if (clean.starts_with("#"))
                clean = clean.substr(1);

            std::stringstream ss;
            ss << std::hex << clean;
            ss >> rgb;

            float rf = ((rgb >> 16) & 0xFF) / 255.0f;
            float gf = ((rgb >> 8) & 0xFF) / 255.0f;
            float bf = (rgb & 0xFF) / 255.0f;

            return colors(rf, gf, bf);
        }

        static colors FromHSV(double h, double s, double v, double a = 1.0) {
            h = fmod(h, 360.0);
            double c = v * s;
            double x = c * (1 - std::fabs(fmod(h / 60.0, 2) - 1));
            double m = v - c;

            double rf = 0, gf = 0, bf = 0;

            if (h < 60) {
                rf = c; gf = x; bf = 0;
            } else if (h < 120) {
                rf = x; gf = c; bf = 0;
            } else if (h < 180) {
                rf = 0; gf = c; bf = x;
            } else if (h < 240) {
                rf = 0; gf = x; bf = c;
            } else if (h < 300) {
                rf = x; gf = 0; bf = c;
            } else {
                rf = c; gf = 0; bf = x;
            }

            return colors(
                static_cast<float>(rf + m),
                static_cast<float>(gf + m),
                static_cast<float>(bf + m),
                static_cast<float>(a)
            );
        }

        bool IsEmpty() const {
            return r == 0.0f && g == 0.0f && b == 0.0f && a == 0.0f;
        }

        bool IsRainbow() const {
            return ToHexString() == "#3FA7D6";
        }

        std::string ToHexString() const {
            int ir = static_cast<int>(r * 255);
            int ig = static_cast<int>(g * 255);
            int ib = static_cast<int>(b * 255);

            char buf[8];
            std::snprintf(buf, sizeof(buf), "#%02X%02X%02X", ir, ig, ib);
            return std::string(buf);
        }

        uint32_t ToARGB() const {
            return (static_cast<uint32_t>(a * 255) << 24) |
                   (static_cast<uint32_t>(b * 255) << 16) |
                   (static_cast<uint32_t>(g * 255) << 8) |
                   (static_cast<uint32_t>(r * 255));
        }

        static colors FromARGB(uint32_t packed) {
            float rf = (packed & 0xFF) / 255.0f;
            float gf = ((packed >> 8) & 0xFF) / 255.0f;
            float bf = ((packed >> 16) & 0xFF) / 255.0f;
            float af = ((packed >> 24) & 0xFF) / 255.0f;
            return colors(rf, gf, bf, af);
        }

        Color ToValveColor() const {
            return Color(
                static_cast<int>(r * 255.0f),
                static_cast<int>(g * 255.0f),
                static_cast<int>(b * 255.0f),
                static_cast<int>(a * 255.0f)
            );
        }

        static inline const char* MapColorToCode(const colors& c) {
            if (c.IsEmpty())
                return "\x01";

            const std::string hex = c.ToHexString();
            if (hex == "#FF0000") return "\x07"; // red
            if (hex == "#008000") return "\x04"; // green
            if (hex == "#0000FF") return "\x0B"; // blue
            if (hex == "#FFFF00") return "\x09"; // yellow
            if (hex == "#800080") return "\x0E"; // purple
            if (hex == "#FFA500") return "\x10"; // orange
            if (hex == "#FFFFFF") return "\x01"; // white
            return "\x01";
        }

        static colors GetRainbowColor() {
            using namespace std::chrono;
            auto now = system_clock::now();
            auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count();

            double hue = fmod((ms % 1000) / 1000.0 * 360.0, 360.0);
            return FromHSV(hue, 1.0, 1.0);
        }
    };
}