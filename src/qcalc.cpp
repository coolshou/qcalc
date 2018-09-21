/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtWidgets>

#include <cmath>

//#include "button.h"
#include "qcalc.h"
#include "ui_qcalc.h"

Calculator::Calculator(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setting = new QSettings();
    loadSetting();

    sumInMemory = 0.0;
    sumSoFar = 0.0;
    factorSoFar = 0.0;
    waitingForOperand = true;

    display = ui->display;
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    //TODO: limit to 15 ? why?
    display->setMaxLength(15);

    QFont font = display->font();
    font.setPointSize(font.pointSize() + 8);
    display->setFont(font);

    connect(ui->pb_0, SIGNAL(clicked()), this, SLOT(digitClicked()));
    connect(ui->pb_1, SIGNAL(clicked()), this, SLOT(digitClicked()));
    connect(ui->pb_2, SIGNAL(clicked()), this, SLOT(digitClicked()));
    connect(ui->pb_3, SIGNAL(clicked()), this, SLOT(digitClicked()));
    connect(ui->pb_4, SIGNAL(clicked()), this, SLOT(digitClicked()));
    connect(ui->pb_5, SIGNAL(clicked()), this, SLOT(digitClicked()));
    connect(ui->pb_6, SIGNAL(clicked()), this, SLOT(digitClicked()));
    connect(ui->pb_7, SIGNAL(clicked()), this, SLOT(digitClicked()));
    connect(ui->pb_8, SIGNAL(clicked()), this, SLOT(digitClicked()));
    connect(ui->pb_9, SIGNAL(clicked()), this, SLOT(digitClicked()));

    connect(ui->pointButton, SIGNAL(clicked()), this, SLOT(pointClicked()));
    ui->changeSignButton->setText(tr("\302\261"));
    connect(ui->changeSignButton, SIGNAL(clicked()), this, SLOT(changeSignClicked()));

    connect(ui->backspaceButton, SIGNAL(clicked()), this, SLOT(backspaceClicked()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(ui->clearAllButton, SIGNAL(clicked()), this, SLOT(clearAll()));

    connect(ui->clearMemoryButton, SIGNAL(clicked()), this, SLOT(clearMemory()));
    connect(ui->readMemoryButton, SIGNAL(clicked()), this, SLOT(readMemory()));
    connect(ui->setMemoryButton, SIGNAL(clicked()), this, SLOT(setMemory()));
    connect(ui->addToMemoryButton, SIGNAL(clicked()), this, SLOT(addToMemory()));

    ui->divisionButton->setText(tr("\303\267"));
    connect(ui->divisionButton, SIGNAL(clicked()), this, SLOT(multiplicativeOperatorClicked()));
    ui->timesButton->setText(tr("\303\227"));
    connect(ui->timesButton, SIGNAL(clicked()), this, SLOT(multiplicativeOperatorClicked()));
    connect(ui->minusButton, SIGNAL(clicked()), this, SLOT(additiveOperatorClicked()));
    connect(ui->plusButton, SIGNAL(clicked()), this, SLOT(additiveOperatorClicked()));

    connect(ui->squareRootButton, SIGNAL(clicked()), this, SLOT(unaryOperatorClicked()));
    ui->powerButton->setText(tr("x\302\262"));
    connect(ui->powerButton, SIGNAL(clicked()), this, SLOT(unaryOperatorClicked()));
    connect(ui->reciprocalButton, SIGNAL(clicked()), this, SLOT(unaryOperatorClicked()));
    connect(ui->equalButton, SIGNAL(clicked()), this, SLOT(equalClicked()));

    setWindowTitle(qApp->applicationName());
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::digitClicked()
{
    //Button *clickedButton = qobject_cast<Button *>(sender());
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    int digitValue = clickedButton->text().toInt();
    if (display->text() == "0" && digitValue == 0.0)
        return;

    if (waitingForOperand) {
        display->clear();
        waitingForOperand = false;
    }
    display->setText(display->text() + QString::number(digitValue));
}
void Calculator::unaryOperatorClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();
    double result = 0.0;

    if (clickedOperator == tr("Sqrt")) {
        if (operand < 0.0) {
            abortOperation();
            return;
        }
        result = std::sqrt(operand);
    } else if (clickedOperator == tr("x\302\262")) {
        result = std::pow(operand, 2.0);
    } else if (clickedOperator == tr("1/x")) {
        if (operand == 0.0) {
            abortOperation();
            return;
        }
        result = 1.0 / operand;
    }
    display->setText(QString::number(result));
    waitingForOperand = true;
}
void Calculator::additiveOperatorClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(factorSoFar));
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }

    if (!pendingAdditiveOperator.isEmpty()) {
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(sumSoFar));
    } else {
        sumSoFar = operand;
    }

    pendingAdditiveOperator = clickedOperator;
    waitingForOperand = true;
}
void Calculator::multiplicativeOperatorClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        display->setText(QString::number(factorSoFar));
    } else {
        factorSoFar = operand;
    }

    pendingMultiplicativeOperator = clickedOperator;
    waitingForOperand = true;
}
void Calculator::equalClicked()
{
    double operand = display->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }
    if (!pendingAdditiveOperator.isEmpty()) {
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
            return;
        }
        pendingAdditiveOperator.clear();
    } else {
        sumSoFar = operand;
    }

    display->setText(QString::number(sumSoFar));
    sumSoFar = 0.0;
    waitingForOperand = true;
}
void Calculator::pointClicked()
{
    if (waitingForOperand)
        display->setText("0");
    if (!display->text().contains("."))
        display->setText(display->text() + tr("."));
    waitingForOperand = false;
}
void Calculator::changeSignClicked()
{
    QString text = display->text();
    double value = text.toDouble();

    if (value > 0.0) {
        text.prepend(tr("-"));
    } else if (value < 0.0) {
        text.remove(0, 1);
    }
    display->setText(text);
}
void Calculator::backspaceClicked()
{
    if (waitingForOperand)
        return;

    QString text = display->text();
    text.chop(1);
    if (text.isEmpty()) {
        text = "0";
        waitingForOperand = true;
    }
    display->setText(text);
}
void Calculator::clear()
{
    if (waitingForOperand)
        return;

    display->setText("0");
    waitingForOperand = true;
}
void Calculator::clearAll()
{
    sumSoFar = 0.0;
    factorSoFar = 0.0;
    pendingAdditiveOperator.clear();
    pendingMultiplicativeOperator.clear();
    display->setText("0");
    waitingForOperand = true;
}
void Calculator::clearMemory()
{
    sumInMemory = 0.0;
}

void Calculator::readMemory()
{
    display->setText(QString::number(sumInMemory));
    waitingForOperand = true;
}

void Calculator::setMemory()
{
    equalClicked();
    sumInMemory = display->text().toDouble();
}

void Calculator::addToMemory()
{
    equalClicked();
    sumInMemory += display->text().toDouble();
}
/*
Button *Calculator::createButton(const QString &text, const char *member)
{
    Button *button = new Button(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}*/
void Calculator::abortOperation()
{
    clearAll();
    display->setText(tr("####"));
}
bool Calculator::calculate(double rightOperand, const QString &pendingOperator)
{
    if (pendingOperator == tr("+")) {
        sumSoFar += rightOperand;
    } else if (pendingOperator == tr("-")) {
        sumSoFar -= rightOperand;
    } else if (pendingOperator == tr("\303\227")) {
        factorSoFar *= rightOperand;
    } else if (pendingOperator == tr("\303\267")) {
        if (rightOperand == 0.0)
            return false;
        factorSoFar /= rightOperand;
    }
    return true;
}
void Calculator::closeEvent(QCloseEvent *event)
{
    //if (maybeSave()) {
        saveSetting();
        event->accept();
    //} else {
    //    event->ignore();
    //}
}

void Calculator::loadSetting()
{
    setting->beginGroup("Window");
    QRect geo = setting->value("Geometry", QRect(20,20,280,280)).value<QRect>();
    setGeometry(geo);
    setting->endGroup();
}
void Calculator::saveSetting()
{
    setting->beginGroup("Window");
    setting->setValue("Geometry", this->geometry());
    setting->endGroup();

}
void Calculator::keyPressEvent(QKeyEvent *event)
{
    //num pad: 0~9, /, x, +, -, .
    int key = event->key();
    switch (key) {
    case Qt::Key_0:
        ui->pb_0->animateClick();
        break;
    case Qt::Key_1:
        ui->pb_1->animateClick();
        break;
    case Qt::Key_2:
        ui->pb_2->animateClick();
        break;
    case Qt::Key_3:
        ui->pb_3->animateClick();
        break;
    case Qt::Key_4:
        ui->pb_4->animateClick();
        break;
    case Qt::Key_5:
        ui->pb_5->animateClick();
        break;
    case Qt::Key_6:
        ui->pb_6->animateClick();
        break;
    case Qt::Key_7:
        ui->pb_7->animateClick();
        break;
    case Qt::Key_8:
        ui->pb_8->animateClick();
        break;
    case Qt::Key_9:
        ui->pb_9->animateClick();
        break;
    case Qt::Key_Plus: //+
        ui->plusButton->animateClick();
        break;
    case Qt::Key_Minus: //-
        ui->minusButton->animateClick();
        break;
    case Qt::Key_Asterisk: //*
        ui->timesButton->animateClick();
        break;
    case Qt::Key_Slash:  // /
        ui->divisionButton->animateClick();
        break;
    case Qt::Key_Enter:  // =
        ui->equalButton->animateClick();
        break;
    case Qt::Key_Period: // .
        ui->pointButton->animateClick();
        break;
    default:
        break;
    }
    QWidget::keyPressEvent(event);
}
//TODO: Key_Backspace, Key_Delete
/*
bool Calculator::eventFilter(QObject *target, QEvent *event)
{
    if (target == display)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            int  key = keyEvent->key();
            switch (key){
            case Qt::Key_Backspace:
                ui->backspaceButton->animateClick();
                return true;
            case Qt::Key_Delete:
                ui->clearButton->animateClick();
                return true;
            default:
                break;
            }
        }
    }
    return QWidget::eventFilter(target, event);
}
*/
