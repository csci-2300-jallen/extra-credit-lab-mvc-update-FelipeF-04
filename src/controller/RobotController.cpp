#include "controller/RobotController.h"

#include "model/Robot.h"
#include "view/RobotWindow.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeyEvent>
#include <QObject>
#include <QPushButton>
#include <QString>

namespace {
const QString savedRobotsFolder = "saved_robots";

QString savedRobotsPath(const QString& fileName = "") {
    QDir().mkpath(savedRobotsFolder);
    return fileName.isEmpty() ? savedRobotsFolder : savedRobotsFolder + "/" + fileName;
}
}

RobotController::RobotController(Robot* model, RobotWindow* view)
    : model(model), view(view) {}

void RobotController::initialize() {
    view->setController(this);
    connectButtons();
    refreshView();
}

void RobotController::moveUp() {
    model->moveUp();
    refreshView();
}

void RobotController::moveDown() {
    model->moveDown();
    refreshView();
}

void RobotController::moveLeft() {
    model->moveLeft();
    refreshView();
}

void RobotController::moveRight() {
    model->moveRight();
    refreshView();
}

void RobotController::undo() {
    model->undo();
    refreshView();
}

void RobotController::redo() {
    model->redo();
    refreshView();
}

void RobotController::saveHistory() {
    QString defaultFileName = QString::fromStdString(model->getName()) + ".robot";

    QString filePath = QFileDialog::getSaveFileName(
        view,
        "Save Robot History",
        savedRobotsPath(defaultFileName),
        "Robot Files (*.robot)"
    );

    if (filePath.isEmpty()) {
        return;
    }

    model->saveToFile(filePath.toStdString());
    view->setStatusText("Saved " + QFileInfo(filePath).fileName());
}

void RobotController::loadHistory() {
    QString filePath = QFileDialog::getOpenFileName(
        view,
        "Load Robot History",
        savedRobotsPath(),
        "Robot Files (*.robot)"
    );

    if (filePath.isEmpty()) {
        return;
    }

    model->loadFromFile(filePath.toStdString());
    refreshView();
    view->setStatusText("Loaded " + QFileInfo(filePath).fileName());
}

bool RobotController::handleKeyPress(QKeyEvent* event) {
    if (event->key() == Qt::Key_W) {
        moveUp();
        return true;
    }

    if (event->key() == Qt::Key_S) {
        moveDown();
        return true;
    }

    if (event->key() == Qt::Key_A) {
        moveLeft();
        return true;
    }

    if (event->key() == Qt::Key_D) {
        moveRight();
        return true;
    }

    if (event->key() == Qt::Key_Z && event->modifiers() == Qt::ControlModifier) {
        undo();
        return true;
    }

    if (event->key() == Qt::Key_Y && event->modifiers() == Qt::ControlModifier) {
        redo();
        return true;
    }

    return false;
}

void RobotController::refreshView() {
    view->setRobotPosition(model->getX(), model->getY());
    view->setUndoEnabled(model->canUndo());
    view->setRedoEnabled(model->canRedo());
    view->setStatusText(
        QString("%1: (%2, %3)    History: %4 of %5")
            .arg(QString::fromStdString(model->getName()))
            .arg(model->getX())
            .arg(model->getY())
            .arg(model->getCurrentHistoryIndex() + 1)
            .arg(static_cast<int>(model->getHistory().size()))
    );
}

void RobotController::connectButtons() {
    QObject::connect(view->getUndoButton(), &QPushButton::clicked, [this]() { undo(); });
    QObject::connect(view->getRedoButton(), &QPushButton::clicked, [this]() { redo(); });
    QObject::connect(view->getSaveButton(), &QPushButton::clicked, [this]() { saveHistory(); });
    QObject::connect(view->getLoadButton(), &QPushButton::clicked, [this]() { loadHistory(); });
}
