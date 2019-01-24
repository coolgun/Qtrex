#include "mainwnd.h"
#include "ui_mainwnd.h"
#include "qgameboard.h"
#include <QtWidgets/QHBoxLayout>
#include <QTime>



MainWnd::MainWnd(QWidget *parent) :
    QWidget(parent),
	ui(new Ui::MainWnd)
{
	ui->setupUi(this);

	board =new QGameBoard(this);
	auto *l=new QHBoxLayout(ui->frmGameBoard);
	l->addWidget(board,1);
	board->setFocus();
    board->AttachPreview(ui->frameNext);
    connect(ui->btnStart,&QPushButton::clicked,board, &QGameBoard::NewGame);
    connect(ui->btnPause,&QPushButton::clicked,board, &QGameBoard::Pause);

    connect(board, &QGameBoard::levelChangedSignal,ui->lcdLevel,static_cast<void (QLCDNumber::*)(int)>(&QLCDNumber::display));
    connect(board, &QGameBoard::scoreChangedSignal,ui->lcdScores, static_cast<void (QLCDNumber::*)(int)>(&QLCDNumber::display));

}


//----------------------------------------------------------------------------------------------------------------------------------------


MainWnd::~MainWnd()
{
    delete ui;
}

//----------------------------------------------------------------------------------------------------------------------------------------


