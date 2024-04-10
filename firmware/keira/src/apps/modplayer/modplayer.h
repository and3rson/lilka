#pragma once

#include <lilka.h>

#include "app.h"
#include "analyzer.h"

typedef enum {
    CMD_SET_PAUSED,
    CMD_SET_GAIN,
    CMD_STOP,
} PlayerCommandType;

typedef struct {
    PlayerCommandType type;
    union {
        bool isPaused;
        float gain;
    };
} PlayerCommand;

typedef struct {
    AudioOutputAnalyzer* analyzer;
    bool isPaused;
    bool isFinished;
    float gain;
} PlayerTaskData;

class ModPlayerApp : public App {
public:
    explicit ModPlayerApp(String path);
    void run() override;

private:
    void mainWindow();
    void playTask();
    QueueHandle_t playerCommandQueue;
    String fileName;

    SemaphoreHandle_t playerMutex;
    // playerTaskData is accessed by both the player task and the app task.
    // It's important to always lock the mutex before accessing it.
    PlayerTaskData playerTaskData = {
        .analyzer = NULL,
        .isPaused = false,
        .isFinished = false,
        .gain = 1.0f,
    };
};
