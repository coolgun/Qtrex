#ifndef QGAMEBOARD_H
#define QGAMEBOARD_H

#include <QtWidgets/QFrame>
#include <array>
#include <math.h>

using FigureCoord = std::array<std::array<bool, 4>, 4>;


enum
{
	kPartCount = 5,
	kRowCount = 15,
	kColCount = 1 << kPartCount
};

const qreal	coeff=5.0;

class GameField;
class Figure ; 

class GameDrawer	
{

		qreal MaxRadius{};
        qreal MinRadius{};
		std::array<QPoint, kColCount * 2>   lines;
		std::array<std::array<QPainterPath, kColCount>, kRowCount> paths;
        QRect rect;
		std::array<qreal, kRowCount + 1>  radius_list;
        void calcul_radius();
        const Figure &m_figure;
        const GameField &m_field;
        void draw_cell(const QPoint &cell_coord,QPainter *p,const QPen &pen,const QBrush &brush);
        void draw_field( QPainter *p);
        void draw_figure(QPainter *p);
        void fill_path_list();
public:
		GameDrawer(const QRect &r,const GameField &fld,const Figure &fg);
		QPoint ScreenToField(const QPoint &pt) const;
		void set_rect (const QRect &r);
		void draw     (QPainter *p, bool is_draw_figure);
		void draw_preview(QPainter *p, size_t figure_idx, size_t color_idx);

};

class GameField: public QObject
{
	Q_OBJECT

		std::array<std::array<QColor, kColCount>, kRowCount> field;
        
		int scores{};
		int level{};

		void  SetCell(const QPoint &cell_coord,const QColor &color);

		void CheckAll();
		
		bool  CheckLine(int y) const;
		
		void RemoveLine(int line);

        void setScores(int new_scores);

        void setLevel(int new_level);
	
		
    public:
		void clear();
		GameField();
		bool  CheckCell(const QPoint &cell_coord)		 const;
		const QColor * GetCell(const QPoint &cell_coord) const;
        bool AddFigure(const Figure & fg );
		bool Rotate(bool cw,const Figure *fg);

      signals:
		void scoreChangedSignal(int score);
		void levelChangedSignal (int level);

};


class FigureProducer : public QObject
{
   Q_OBJECT
	
	public:
		FigureProducer();

		size_t NextFigure()	const;
		size_t NextFigureColor() const;
		size_t NextDirection() const
		{
			return m_direction;
		}
		
		void generate_figure(Figure * ) ;
        
	signals:
         void NewFigureSignal();
		
	private:

		mutable size_t m_next_figure_idx;
		mutable size_t m_next_figure_color;
		mutable size_t m_direction;



};


class Figure
{

		friend class GameDrawer;
		friend class GameField;
	
		FigureCoord m_coord;
		FigureCoord m_dropped_coord;

		QPoint		cur_pt;
		QPoint		dropped_pt;

		QColor		m_color;

		const		GameField			&m_field;
		const		FigureProducer		&m_prod;	

		FigureCoord rotate(const FigureCoord &coord);

		bool check(const FigureCoord &coord,const QPoint &pt) const;
		
		
			

	public:

		void calcul_dropped();

		bool check_self() const 
		{
			return check(m_coord,cur_pt);
		}
        
		Figure(const GameField &field,const	FigureProducer	&prod);
        
		void new_figure(const FigureCoord &coord,size_t x, size_t rotate_count,const QColor &color );
		bool hit_test(const QPoint &pt) const;
		bool MoveX(int x,bool cw);
		bool Droped();
        bool Rotate();
		bool Left();
		bool Right();
		bool Down();
};



class QGameBoard : public QWidget
{
	Q_OBJECT

	enum GameState
	{
		stoping,
		pause,
		active
	};

	GameState  state{};

public:
	QGameBoard(QWidget *parent);
	void AttachPreview(QWidget *preview);

public	slots:
	void NewGame();
	void Pause();
	void GameOver();

signals:
	void scoreChangedSignal(int score);
	void levelChangedSignal(int level);
	void GameOverSignal();

protected:


	void resizeEvent(QResizeEvent *e) override;
	void paintEvent(QPaintEvent *e) override;
	void keyPressEvent(QKeyEvent *e) override;
	void timerEvent(QTimerEvent *event) override;

	void wheelEvent(QWheelEvent *)override;
	void mousePressEvent(QMouseEvent*)override;
	void mouseDoubleClickEvent(QMouseEvent *event)override;
	void mouseMoveEvent(QMouseEvent *event)override;
	void mouseReleaseEvent(QMouseEvent *event)override;
	bool eventFilter(QObject *obj, QEvent *event)override;

protected slots:
	void ChangLevel(int level);
	void NewFigure();


private:

	void step(bool down = {});
	
	GameField		field;
	FigureProducer  prod;
	Figure			figure;
	GameDrawer      drawer;
	
	
	bool            use{};
	bool            is_down{};
	QPoint			sel_coord;
	int				timer_id{};
	int				speed{};

	QWidget *m_preview{};

};

#endif // QGAMEBOARD_H
