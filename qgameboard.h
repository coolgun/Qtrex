#ifndef QGAMEBOARD_H
#define QGAMEBOARD_H

#include <Wt/WPaintDevice.h>
#include <Wt/WPaintedWidget.h>
#include <Wt/WPen.h>
#include <Wt/WRectF.h>
#include <Wt/WPainterPath.h>
#include <Wt/WWidget.h>
#include <Wt/WPoint.h>
#include <Wt/WTimer.h>
#include <array>

using FigureCoord = std::array<std::array<bool, 4>, 4>;

enum
{
	kPartCount = 5,
	kRowCount = 15,
	kColCount = 1 << kPartCount
};

class GameField;
class Figure ; 

class GameDrawer	
{
		double MaxRadius{};
        double MinRadius{};
		std::array<Wt::WLineF, kColCount> lines;
		std::array<std::array<Wt::WPainterPath, kColCount>, kRowCount> paths;
		std::array<double, kRowCount + 1>  radius_list;
        void calcul_radius();
        const Figure &m_figure;
        const GameField &m_field;
        void draw_cell(const Wt::WPoint &cell_coord,Wt::WPainter *p,const Wt::WPen &pen,const Wt::WBrush &brush);
        void draw_field( Wt::WPainter *p);
        void draw_figure(Wt::WPainter *p);
        void fill_path_list();
	
public:
		GameDrawer(const Wt::WRectF &r,const GameField &fld,const Figure &fg);
		Wt::WPoint ScreenToField(const Wt::WPoint &pt) const;
		void set_rect(const Wt::WRectF &r);
		void draw(Wt::WPainter *p, bool is_draw_figure);
		static void draw_preview(Wt::WPainter *p, size_t figure_idx, size_t color_idx);
		Wt::WRectF rect;
};

class GameField
{
		std::array<std::array<Wt::WColor, kColCount>, kRowCount> field;
        
		int scores{};
		int level{};

		void SetCell(const Wt::WPoint &cell_coord,const Wt::WColor &color);
		void CheckAll();
		bool CheckLine(int y) const;
		void RemoveLine(int line);
        void setScores(int new_scores);
        void setLevel(int new_level);
    
public:
		GameField();
		void clear();
		bool  CheckCell(const Wt::WPoint &cell_coord)		 const;
		const Wt::WColor *GetCell(const Wt::WPoint &cell_coord) const;
        bool AddFigure(const Figure & fg );
		bool Rotate(bool cw,const Figure *fg);
		Wt::Signal<int> scoreChangedSignal;
		Wt::Signal<int> levelChangedSignal;

};


class FigureProducer : public Wt::WObject
{
	public:
		FigureProducer();

		size_t NextFigure()	const;
		size_t NextFigureColor() const;
		size_t NextDirection() const
		{
			return m_direction;
		}
		
		void generate_figure(Figure * ) ;
		Wt::Signal<> NewFigureSignal;

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

		Wt::WPoint cur_pt;
		Wt::WPoint dropped_pt;

		Wt::WColor m_color;

		const GameField &m_field;
		const FigureProducer &m_prod;	

		FigureCoord rotate(const FigureCoord &coord);

		bool check(const FigureCoord &coord,const Wt::WPoint &pt) const;
			

	public:

		void calcul_dropped();

		bool check_self() const 
		{
			return check(m_coord, cur_pt);
		}
        
		Figure(const GameField &field,const	FigureProducer	&prod);
        
		void new_figure(const FigureCoord &coord,size_t x, size_t rotate_count,const Wt::WColor &color );
		bool hit_test(const Wt::WPoint &pt) const;
		bool MoveX(int x,bool cw);
		bool Droped();
        bool Rotate();
		bool Left();
		bool Right();
		bool Down();
};

class PreviewWidget : public Wt::WPaintedWidget
{
		size_t m_figure_idx = 0;
		size_t m_color_idx = 0;
		void paintEvent(Wt::WPaintDevice* paintDevice) override;
	public:
		void setFigure(size_t figure_idx, size_t color_idx);

};

class QGameBoard : public Wt::WPaintedWidget
{
	enum class GameState
	{
		stoping,
		pause,
		active
	};

	GameState  state = GameState::stoping;

public:
	QGameBoard();
	void AttachPreview(PreviewWidget *preview);
	void NewGame();
	void Pause();
	void GameOver();

	Wt::Signal<int> scoreChangedSignal;
	Wt::Signal<int> levelChangedSignal;
	Wt::Signal<> GameOverSignal;

protected:

	void paintEvent(Wt::WPaintDevice* paintDevice) override;
	void keyPressEvent(const Wt::WKeyEvent &e);
	void timerEvent() ;
	void wheelEvent(const Wt::WMouseEvent&);
	void mousePressEvent(const Wt::WMouseEvent&);
	void mouseDoubleClickEvent(const Wt::WMouseEvent&);
	void mouseMoveEvent(const Wt::WMouseEvent&);
	void mouseReleaseEvent(const Wt::WMouseEvent&);
	void ChangLevel(int level);
	void NewFigure();

	void resize(const Wt::WLength& width, const Wt::WLength& height) override
	{
		Wt::WPaintedWidget::resize(width, height);
		drawer.set_rect(Wt::WRectF(0.0, 0.0, width.value() ,height.value()));
	}
private:

	bool IsStoped() const;
	void step(bool down = {});
	
	GameField		field;
	FigureProducer  prod;
	Figure			figure;
	GameDrawer      drawer;
	
	bool            use{};
	bool            is_down{};
	Wt::WPoint		sel_coord;
	int				timer_id{};
	int				speed{};

	PreviewWidget *m_preview{};
	Wt::WTimer  timer;

};

#endif // QGAMEBOARD_H
