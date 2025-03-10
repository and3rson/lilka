#include "txtviewer.h"
TextViewerApp::TextViewerApp(String path) : App("TxtViewer") {
    this->path = path;
}

void TextViewerApp::run() {
    lilka::TextView txtView(path, true);
    txtView.setColors(lilka::colors::White, lilka::colors::Black);
    while (!txtView.isFinished()) {
        txtView.update();
        txtView.draw(canvas);
        queueDraw();
    }
}