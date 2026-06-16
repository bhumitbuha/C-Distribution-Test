#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widgets/TextArea.hpp>

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

#include "UniformTest.h"
#include "NormalTest.h"

namespace {

using Clock = std::chrono::high_resolution_clock;

template <typename Test>
std::string formatStats(const Test& test) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Min:    " << test.getMin() << "\n";
    ss << "Max:    " << test.getMax() << "\n";
    ss << "Median: " << test.getMedian() << "\n";
    ss << "Range:  " << test.getNumberRange() << "\n";
    ss << "Histogram (bucket: count):\n";
    for (const auto& [bucket, count] : test.getHistogram())
        ss << "  " << std::setw(3) << bucket << ": " << count << "\n";
    return ss.str();
}

template <typename DistTest, typename... Args>
std::string runWithTiming(const std::string& label, Args&&... args) {
    const auto start = Clock::now();
    DistTest test(std::forward<Args>(args)...);
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        Clock::now() - start).count();

    std::ostringstream out;
    out << label << "  (" << elapsed << " ms)\n";
    out << formatStats(test) << "\n";
    return out.str();
}

float parseFloat(const tgui::EditBox::Ptr& box) {
    try { return std::stof(box->getText().toStdString()); }
    catch (...) { return 0.0f; }
}

int parseInt(const tgui::EditBox::Ptr& box) {
    try { return std::stoi(box->getText().toStdString()); }
    catch (...) { return 0; }
}

}  // namespace

int main() {
    sf::RenderWindow window(sf::VideoMode({700u, 500u}), "Distribution Tester");
    tgui::Gui gui(window);

    auto numSamplesBox = tgui::EditBox::create();
    numSamplesBox->setPosition(60, 30);
    numSamplesBox->setSize(120, 30);
    numSamplesBox->setDefaultText("Num Samples");
    gui.add(numSamplesBox, "numSamplesBox");

    auto param1Box = tgui::EditBox::create();
    param1Box->setPosition(200, 30);
    param1Box->setSize(120, 30);
    param1Box->setDefaultText("Min or Mean");
    gui.add(param1Box, "param1Box");

    auto param2Box = tgui::EditBox::create();
    param2Box->setPosition(340, 30);
    param2Box->setSize(120, 30);
    param2Box->setDefaultText("Max or Stddev");
    gui.add(param2Box, "param2Box");

    auto uniformButton = tgui::Button::create("Run Uniform Test");
    uniformButton->setPosition(60, 80);
    uniformButton->setSize(180, 40);
    gui.add(uniformButton);

    auto normalButton = tgui::Button::create("Run Normal Test");
    normalButton->setPosition(260, 80);
    normalButton->setSize(180, 40);
    gui.add(normalButton);

    auto outputBox = tgui::TextArea::create();
    outputBox->setPosition(60, 140);
    outputBox->setSize(580, 330);
    outputBox->setReadOnly(true);
    outputBox->setText("Results will appear here");
    gui.add(outputBox);

    uniformButton->onPress([&]() {
        const int n = parseInt(numSamplesBox);
        const float min = parseFloat(param1Box);
        const float max = parseFloat(param2Box);

        if (n <= 0 || max <= min) {
            outputBox->setText("Invalid inputs: need n > 0 and max > min.");
            return;
        }

        std::ostringstream out;
        out << runWithTiming<UniformTest<float>>(
                   "Uniform (Sequential)", n, min, max, false);
        out << runWithTiming<UniformTest<float>>(
                   "Uniform (Parallel OpenMP)", n, min, max, true);
        outputBox->setText(out.str());
    });

    normalButton->onPress([&]() {
        const int n = parseInt(numSamplesBox);
        const float mean = parseFloat(param1Box);
        const float stddev = parseFloat(param2Box);

        if (n <= 0 || stddev <= 0.0f) {
            outputBox->setText("Invalid inputs: need n > 0 and stddev > 0.");
            return;
        }

        std::ostringstream out;
        out << runWithTiming<NormalTest<float>>(
                   "Normal (Sequential)", n, mean, stddev, false);
        out << runWithTiming<NormalTest<float>>(
                   "Normal (Parallel OpenMP)", n, mean, stddev, true);
        outputBox->setText(out.str());
    });

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            gui.handleEvent(*event);
            if (event->is<sf::Event::Closed>()) window.close();
        }
        window.clear(sf::Color::White);
        gui.draw();
        window.display();
    }
    return 0;
}
