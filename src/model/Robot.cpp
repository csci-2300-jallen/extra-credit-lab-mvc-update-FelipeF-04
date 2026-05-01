#include "model/Robot.h"

#include <fstream>

Robot::Robot(const std::string& name)
    : name_(name) {}

std::string Robot::getName() const {
    return name_;
}

int Robot::getX() const {
    return x;
}

int Robot::getY() const {
    return y;
}

int Robot::getCurrentHistoryIndex() const {
    return currentHistoryIndex;
}

const std::vector<Point2D>& Robot::getHistory() const {
    return history;
}

void Robot::moveUp() {
    int movesMade = static_cast<int>(history.size()) - 1;
    int step = (movesMade >= 10) ? 2 : 1;
    moveTo(x, y - step);
}

void Robot::moveDown() {
    int movesMade = static_cast<int>(history.size()) - 1;
    int step = (movesMade >= 10) ? 2 : 1;
    moveTo(x, y + step);
}

void Robot::moveLeft() {
    int movesMade = static_cast<int>(history.size()) - 1;
    int step = (movesMade >= 10) ? 2 : 1;
    moveTo(x - step, y);
}

void Robot::moveRight() {
    int movesMade = static_cast<int>(history.size()) - 1;
    int step = (movesMade >= 10) ? 2 : 1;
    moveTo(x + step, y);
}

void Robot::undo() {
    if (!canUndo()) {
        return;
    }

    currentHistoryIndex--;
    x = history[currentHistoryIndex].x;
    y = history[currentHistoryIndex].y;
}

void Robot::redo() {
    if (!canRedo()) {
        return;
    }

    currentHistoryIndex++;
    x = history[currentHistoryIndex].x;
    y = history[currentHistoryIndex].y;
}

bool Robot::canUndo() const {
    return currentHistoryIndex > 0;
}

bool Robot::canRedo() const {
    return currentHistoryIndex < static_cast<int>(history.size()) - 1;
}

void Robot::saveToFile(const std::string& filePath) const {
    std::ofstream file(filePath);

    file << name_ << '\n';
    file << currentHistoryIndex << '\n';

    for (const Point2D& point : history) {
        file << point.x << ' ' << point.y << '\n';
    }
}

void Robot::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);

    std::string loadedName;
    int loadedIndex = 0;
    std::getline(file, loadedName);
    file >> loadedIndex;

    int pointX = 0;
    int pointY = 0;
    std::vector<Point2D> loadedHistory;
    for (int index = 0; index <= loadedIndex; index++) {
        file >> pointX >> pointY;
        loadedHistory.push_back({pointX, pointY});
        x = pointX;
        y = pointY;
    }

    name_ = loadedName;
    history = loadedHistory;
    currentHistoryIndex = loadedIndex;
}

void Robot::moveTo(int newX, int newY) {
    if (newX < 0 || newX >= gridSize || newY < 0 || newY >= gridSize) {
        return;
    }

    if (newX == x && newY == y) {
        return;
    }

    x = newX;
    y = newY;
    recordPosition();
}

void Robot::recordPosition() {
    if (canRedo()) {
        history.erase(history.begin() + currentHistoryIndex + 1, history.end());
    }

    history.push_back({x, y});
    currentHistoryIndex = static_cast<int>(history.size()) - 1;
}
