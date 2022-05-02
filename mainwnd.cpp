#include "mainwnd.h"
#include "qgameboard.h"
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WPushButton.h>
#include <Wt/WGridLayout.h>
#include <Wt/WLabel.h>
#include <Wt/WTimer.h>

MainWnd::MainWnd():
    WContainerWidget()
{
	setupUi();
}

void MainWnd::setupUi()
{
    resize(544, 474);
    auto board = std::make_unique<QGameBoard>();
    board->setMinimumSize(500, 500);
    auto* horizontalLayout = setLayout(std::make_unique<Wt::WHBoxLayout>());  
    
    auto prev_widget = std::make_unique<PreviewWidget>();
    prev_widget->setMinimumSize(90, 90);
    
    board->AttachPreview(prev_widget.get());
    board->setFocus();

    auto btnStart = std::make_unique <Wt::WPushButton>(Wt::WString("Start"));
    btnStart->clicked().connect([board = board.get()]{ board->NewGame();  board->setFocus(); });
    auto btnPause = std::make_unique <Wt::WPushButton>(Wt::WString("Pause"));
    btnPause->clicked().connect([board = board.get()]{ board->Pause(); });
    
    auto gridLayout = std::make_unique<Wt::WGridLayout>();
    
    gridLayout->addWidget(std::move(prev_widget), 0, 0, 1, 2);
    gridLayout->addWidget(std::make_unique<Wt::WLabel>("Level"), 1, 0);
    auto lcdLevel = std::make_unique<Wt::WLabel>("0");
    board->levelChangedSignal.connect(
        [lcdLevel = lcdLevel.get()](int level)
        {
            lcdLevel->setText(Wt::WString("{1}").arg(level));
        });

    gridLayout->addWidget(std::move(lcdLevel), 1, 1);

    gridLayout->addWidget(std::make_unique<Wt::WLabel>("Scores"), 2, 0);
    auto lcdScores = std::make_unique<Wt::WLabel>("0");
    board->levelChangedSignal.connect(
        [lcdScores = lcdScores.get()](int scores)
    {
        lcdScores->setText(Wt::WString("{1}").arg(scores));
    });
    gridLayout->addWidget(std::move(lcdScores), 2, 1);


    gridLayout->addWidget(std::move(btnStart), 3, 0, 1, 2);
    gridLayout->addWidget(std::move(btnPause), 4, 0, 1, 2);
    gridLayout->addWidget(std::make_unique<Wt::WBreak>(), 5, 0, 1, 2);
    gridLayout->setRowStretch(5, 1);

    horizontalLayout->addWidget(std::move(board),1);
    horizontalLayout->addLayout(std::move(gridLayout));

}

