#pragma once

#include "app.h"

class ComboApp : public App {
public:
    ComboApp();

private:
    void run() override;

    void mainUI(lilka::Canvas& buffer, lilka::State& st);
    void scene_0(lilka::Canvas& buffer, lilka::State& st);
    void scene_1(lilka::Canvas& buffer, lilka::State& st);
    void scene_2(lilka::Canvas& buffer, lilka::State& st);
    void scene_3(lilka::Canvas& buffer, lilka::State& st);
    void scene_4(lilka::Canvas& buffer, lilka::State& st, int timeout);
    void scene_final(lilka::Canvas& buffer, lilka::State& st);
};
