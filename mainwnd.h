#ifndef MAINWND_H
#define MAINWND_H

#include <QtWidgets/QWidget>

class QGameBoard;

namespace Ui {
    class MainWnd;
}

class MainWnd : public QWidget {
    Q_OBJECT
public:
    MainWnd(QWidget *parent = {});
    ~MainWnd();

private:
	
	Ui::MainWnd *ui;
	QGameBoard	* board;



};

#endif // MAINWND_H
