#include "view/RobotGridWidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QPainter>
#include <QLinearGradient>

RobotGridWidget::RobotGridWidget(QWidget* parent)
    : QWidget(parent), robotIcon("assets/robot-icon.png") {
    if (robotIcon.isNull()) {
        robotIcon.load("../assets/robot-icon.png");
    }

    setFixedSize(250, 250);

    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    for (int row = 0; row < gridSize; row++) {
        gridLayout->setRowStretch(row, 1);

        for (int column = 0; column < gridSize; column++) {
            gridLayout->setColumnStretch(column, 1);

            QLabel* cell = new QLabel(this);
            cell->setMinimumSize(50, 50);
            cell->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
            cell->setAlignment(Qt::AlignCenter);
            cell->setScaledContents(false);
            cell->setStyleSheet("border: 1px solid gray;");

            cells[row][column] = cell;
            gridLayout->addWidget(cell, row, column);
        }
    }

    updateRobotIcon();
}

static QPixmap makeFlashyIcon(const QPixmap& src, const QSize& targetSize) {
    if (src.isNull() || targetSize.isEmpty()) {
        return QPixmap();
    }

    QPixmap scaled = src.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap out(targetSize);
    out.fill(Qt::transparent);

    QPainter p(&out);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Draw a soft halo behind the robot
    QPoint center(out.width() / 2, out.height() / 2);
    int radius = qMax(out.width(), out.height()) / 2;

    QRadialGradient halo(center, radius);
    halo.setColorAt(0.0, QColor(255, 220, 60, 200));
    halo.setColorAt(0.6, QColor(255, 120, 30, 120));
    halo.setColorAt(1.0, QColor(255, 120, 30, 0));

    p.setBrush(halo);
    p.setPen(Qt::NoPen);
    p.drawEllipse(center, radius, radius);

    // Optionally draw small sparkles
    p.setPen(QPen(QColor(255, 255, 255, 200), 2));
    int w = out.width();
    int h = out.height();
    p.drawLine(w/6, h/6, w/6 + 6, h/6 - 6);
    p.drawLine(w*5/6, h/3, w*5/6 + 6, h/3 + 6);

    // Draw the robot on top centered
    QPoint topLeft((out.width() - scaled.width()) / 2, (out.height() - scaled.height()) / 2);
    p.drawPixmap(topLeft, scaled);

    p.end();
    return out;
}

void RobotGridWidget::setRobotPosition(int newX, int newY) {
    x = newX;
    y = newY;
    updateRobotIcon();
}

void RobotGridWidget::updateRobotIcon() {
    for (int row = 0; row < gridSize; row++) {
        for (int column = 0; column < gridSize; column++) {
            cells[row][column]->clear();
        }
    }

    QLabel* robotCell = cells[y][x];
    QSize target = robotCell->contentsRect().size();
    QPixmap flashy = makeFlashyIcon(robotIcon, target);
    if (!flashy.isNull()) {
        robotCell->setPixmap(flashy);
    } else {
        robotCell->setPixmap(robotIcon.scaled(
            target,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
    }
}

void RobotGridWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateRobotIcon();
}
