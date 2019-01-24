/********************************************************************************
** Form generated from reading UI file 'mainwnd.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWND_H
#define UI_MAINWND_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWnd
{
public:
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    QPushButton *btnStart;
    QPushButton *btnPause;
    QPushButton *btnQuit;
    QLCDNumber *lcdLevel;
    QLCDNumber *lcdScores;
    QFrame *frmGameBoard;
    QFrame *frameNext;
    QFrame *line;
    QLabel *label;
    QLabel *label_2;

    void setupUi(QWidget *MainWnd)
    {
        if (MainWnd->objectName().isEmpty())
            MainWnd->setObjectName(QString::fromUtf8("MainWnd"));
        MainWnd->resize(544, 474);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWnd->sizePolicy().hasHeightForWidth());
        MainWnd->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/main/main.PNG"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/new/main/main.PNG"), QSize(), QIcon::Normal, QIcon::On);
        icon.addFile(QString::fromUtf8(":/new/main/main.PNG"), QSize(), QIcon::Active, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/new/main/main.PNG"), QSize(), QIcon::Active, QIcon::On);
        MainWnd->setWindowIcon(icon);
        horizontalLayout = new QHBoxLayout(MainWnd);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 11, 4, 1, 2);

        btnStart = new QPushButton(MainWnd);
        btnStart->setObjectName(QString::fromUtf8("btnStart"));
        btnStart->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnStart, 8, 2, 1, 4);

        btnPause = new QPushButton(MainWnd);
        btnPause->setObjectName(QString::fromUtf8("btnPause"));
        btnPause->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnPause, 9, 2, 1, 4);

        btnQuit = new QPushButton(MainWnd);
        btnQuit->setObjectName(QString::fromUtf8("btnQuit"));
        btnQuit->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnQuit, 10, 2, 1, 4);

        lcdLevel = new QLCDNumber(MainWnd);
        lcdLevel->setObjectName(QString::fromUtf8("lcdLevel"));

        gridLayout->addWidget(lcdLevel, 4, 5, 1, 1);

        lcdScores = new QLCDNumber(MainWnd);
        lcdScores->setObjectName(QString::fromUtf8("lcdScores"));

        gridLayout->addWidget(lcdScores, 6, 5, 1, 1);

        frmGameBoard = new QFrame(MainWnd);
        frmGameBoard->setObjectName(QString::fromUtf8("frmGameBoard"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frmGameBoard->sizePolicy().hasHeightForWidth());
        frmGameBoard->setSizePolicy(sizePolicy1);
        frmGameBoard->setFocusPolicy(Qt::StrongFocus);
        frmGameBoard->setFrameShape(QFrame::Box);
        frmGameBoard->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(frmGameBoard, 2, 1, 10, 1);

        frameNext = new QFrame(MainWnd);
        frameNext->setObjectName(QString::fromUtf8("frameNext"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(frameNext->sizePolicy().hasHeightForWidth());
        frameNext->setSizePolicy(sizePolicy2);
        frameNext->setMinimumSize(QSize(100, 100));
        frameNext->setFrameShape(QFrame::Box);
        frameNext->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(frameNext, 2, 2, 1, 4);

        line = new QFrame(MainWnd);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 7, 2, 1, 4);

        label = new QLabel(MainWnd);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 4, 2, 1, 1);

        label_2 = new QLabel(MainWnd);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 6, 2, 1, 1);


        horizontalLayout->addLayout(gridLayout);


        retranslateUi(MainWnd);
        QObject::connect(btnQuit, SIGNAL(clicked()), MainWnd, SLOT(close()));

        QMetaObject::connectSlotsByName(MainWnd);
    } // setupUi

    void retranslateUi(QWidget *MainWnd)
    {
        MainWnd->setWindowTitle(QApplication::translate("MainWnd", "Qtrex", nullptr));
        btnStart->setText(QApplication::translate("MainWnd", "New Game", nullptr));
        btnPause->setText(QApplication::translate("MainWnd", "Pause", nullptr));
        btnQuit->setText(QApplication::translate("MainWnd", "Quit", nullptr));
        label->setText(QApplication::translate("MainWnd", "Level", nullptr));
        label_2->setText(QApplication::translate("MainWnd", "Scores", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWnd: public Ui_MainWnd {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWND_H
