#include "qgameboard.h"
#include "Wt/WPainter.h"
#include <random>


namespace
{
	bool IsNull(const Wt::WColor& color)
	{
		return color == Wt::StandardColor::White;
	};

	size_t myrand(size_t bound)
	{
		static std::random_device rd;
		std::uniform_int_distribution<size_t> dist(0, bound -  1);
		return dist(rd);
	}

	const std::array<Wt::WColor, 6>  color_map =
	{
		Wt::StandardColor::Red,
		Wt::StandardColor::Blue,
		Wt::StandardColor::Green,
		Wt::StandardColor::Yellow,
		Wt::StandardColor::Magenta,
		Wt::StandardColor::Cyan
	};

	constexpr double coeff = 5.0;
	constexpr double M_1_PI = 0.318309886183790671538;

	const std::array<FigureCoord, 6> FigureCoords =
	{
		FigureCoord
		{
			{
				{ false,true,false,false },
				{ false,true,false,false },
				{ false,true,false,false },
				{ false,true,false,false }
			}
		},

		FigureCoord
		{
			{
				{ false,false,true,false },
				{ false,false,true,false },
				{ false,true,true,false },
				{ false,false,false,false }
			}
		},

		FigureCoord
		{
			{
				{ false,true,false,false },
				{ false,true,true,false },
				{ false,false,true,false },
				{ false,false,false,false }
			}
		},

		FigureCoord
		{
			{
				{ false,false,true,false },
				{ false,true,true,false },
				{ false,true,false,false },
				{ false,false,false,false }
			},
		},

		FigureCoord
		{
			{
				{ false,false,false,false },
				{ false,true,true,false },
				{ false,true,true,false },
				{ false,false,false,false }
			},
		},

		FigureCoord
		{
			{
				{ false,false,false,false },
				{ true,true,true,false },
				{ false,true,false,false },
				{ false,false,false,false }
			}
		}
	};
}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameDrawer::draw(Wt::WPainter* p, bool is_draw_figure)
{
	draw_field(p);

	if (is_draw_figure)
	{
		draw_figure(p);
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------

Wt::WPoint GameDrawer::ScreenToField(const Wt::WPoint& pt) const
{
	const auto center = rect.center();
	const Wt::WPoint  delta(pt.x() - std::round(center.x()), pt.y() - std::round(center.y()));
	const double r =  std::hypot(static_cast<double>(delta.x()), delta.y());
	const double angle = atan2(static_cast<double>(delta.y()), -delta.x()) / (2 * std::_Pi) + 0.5;

	int x = (angle * kColCount + 0.5);
	x %= kColCount;

	if (r < MinRadius)
	{
		return { x , 0 };
	}
	else
		if (r > MaxRadius) return { x, kRowCount - 1 };

	for (int i = 0; i < kRowCount; ++i)
	{
		if (radius_list[i + 1] > r)
		{

			return { x , i };
		}

	};

	return { std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };
}

//----------------------------------------------------------------------------------------------------------------------------------------

GameDrawer::GameDrawer(const Wt::WRectF& r, const GameField& fld, const Figure& fg) :
	m_field(fld),
	m_figure(fg)
{
	set_rect(r);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void GameDrawer::calcul_radius()
{
	MaxRadius = std::min(rect.width(), rect.height()) * 0.5 - 5;
	MinRadius = MaxRadius / coeff;

	const double step = 1.0 * (MaxRadius - MinRadius) / kRowCount;

	for (int i = 0; i <= kRowCount; ++i)
	{
		radius_list[i] = MinRadius + step * i;
	};
}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameDrawer::fill_path_list()
{
	for (int x = 0; x < kColCount; ++x)
	{
		const double angle = 2 * std::_Pi * ((x - 0.5) / kColCount);
		const double sweep_angle = 2 * std::_Pi / kColCount;
		const double c1 = cos(angle);
		const double s1 = sin(-angle);
		const double c2 = cos(-angle - sweep_angle);
		const double s2 = sin(-angle - sweep_angle);

		const auto center = rect.center();

		lines[x] =
		{
			radius_list[0] * c1 + center.x(),
			radius_list[0] * s1 + center.y(),
			radius_list[kRowCount] * c1 + center.x(),
			radius_list[kRowCount] * s1 + +center.y()
		};

		for (int y = 0; y < kRowCount; ++y)
		{
			const auto r1 = radius_list[y];
			const auto r2 = radius_list[y + 1];
			const auto x1 = r1 * c1 + center.x();
			const auto y1 = r1 * s1 + center.y();
			const auto x2 = r2 * c1 + center.x();
			const auto y2 = r2 * s1 + center.y();
			const auto x3 = r1 * c2 + center.x();
			const auto y3 = r1 * s2 + center.y();
			Wt::WPainterPath path;

			path.moveTo(x1, y1);
			path.lineTo(x2, y2);
			path.arcTo(
				center.x(), 
				center.y(),  
				r2, 
				180 * angle * M_1_PI,
				180 * sweep_angle * M_1_PI);

			path.lineTo(x3, y3);

			path.arcTo(
				center.x(),
				center.y(),
				r1, 
				180 * (angle + sweep_angle) * M_1_PI,
				-180 * sweep_angle * M_1_PI);
			paths[y][x] = path;
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameDrawer::draw_cell(const Wt::WPoint& cell_coord, Wt::WPainter* p, const Wt::WPen& pen, const Wt::WBrush& brush)
{

	if (cell_coord.y() >= kRowCount || cell_coord.y() < 0) return;

	int x = cell_coord.x() % kColCount;

	const auto& path = paths[cell_coord.y()][x];

	p->setPen(pen);
	p->fillPath(path, brush);
	p->drawPath(path);
}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameDrawer::draw_field(Wt::WPainter* p)
{
	Wt::WPen pen(Wt::StandardColor::Gray);
	Wt::WBrush brush;
	p->setPen(pen);
	p->drawLines(lines.data(), kColCount);

	for (int y = 0; y <= kRowCount; ++y)
	{
		int r = std::round(radius_list[y]);
		p->drawEllipse(rect.center().x() - r, rect.center().y() -r , 2*r, 2*r);
	}

	for (int y = 0; y < kRowCount; ++y)
	{
		for (int x = 0; x < kColCount; ++x)
		{
			Wt::WPoint pt(x, y);
			if (!m_field.CheckCell(pt)) continue;

			const auto  color = *m_field.GetCell(pt);
			brush.setColor(color);
			brush.setStyle(!m_field.CheckCell(pt) ? Wt::BrushStyle::None : Wt::BrushStyle::Solid);
			draw_cell(pt, p, pen, brush);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------

void PreviewWidget::paintEvent(Wt::WPaintDevice* paintDevice) 
{
	Wt::WPainter p(paintDevice);
	GameDrawer::draw_preview(&p, m_figure_idx, m_color_idx);
}

//----------------------------------------------------------------------------------------------------------------------------------------

void PreviewWidget::setFigure(size_t figure_idx, size_t color_idx)
{
	m_figure_idx = figure_idx;
	m_color_idx = color_idx;
	update();
}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameDrawer::draw_preview(Wt::WPainter* p, size_t figure_idx, size_t color_idx)
{
	p->setBrush(Wt::WBrush(color_map[color_idx]));
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			if (FigureCoords[figure_idx][y][x])
			{
				p->drawRect(15 + 15 * x, 15 + 15 * y, 15, 15);
			}

		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameDrawer::draw_figure(Wt::WPainter* p)
{
	int min_x = 1000;
	int max_x{};


	Wt::WPen pen(Wt::StandardColor::Black);

	int min_ys[4];
	int width = 0;

	for (int x = 0; x < 4; ++x)
	{
		int min_y = std::numeric_limits<int>::max();

		for (int y = 0; y < 4; ++y)
		{

			if (m_figure.m_coord[y][x])
			{
				const int xx = m_figure.cur_pt.x() + x + kColCount;
				const int yy = m_figure.cur_pt.y() - y;
				draw_cell({ xx,yy }, p, pen, Wt::WBrush(m_figure.m_color));
				min_x = std::min(min_x, xx);
				max_x = std::max(max_x, xx);
				min_y = std::min(min_y, yy);
			}

			if (m_figure.m_dropped_coord[y][x])
			{
				const int xx = m_figure.dropped_pt.x() + x + kColCount;
				const int yy = m_figure.dropped_pt.y() - y;
				draw_cell({ xx,yy }, p, pen, Wt::WBrush(Wt::BrushStyle::None));
			}

		}

		if (min_y <= kRowCount)
		{
			min_ys[width++] = min_y;
		}

	}

	Wt::WBrush brush(Wt::WColor(128, 0, 0, 50));

	pen.setStyle(Wt::PenStyle::None);

	int idx = 0;
	if (!width) return;

	for (int x = min_x; x <= max_x; ++x, ++idx)
	{
		for (int y = 0; y < min_ys[idx]; ++y)
		{
			draw_cell({ x,y }, p, pen, brush);
		}
	}

}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameDrawer::set_rect(const Wt::WRectF& r)
{
	rect = r;
	calcul_radius();
	fill_path_list();
}


//----------------------------------------------------------------------------------------------------------------------------------------

void GameField::setLevel(int new_level)
{
	level = new_level;
	levelChangedSignal.emit(level);
}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameField::setScores(int new_scores)
{
	if (scores / 100 < new_scores / 100)      setLevel(level + 1);
	scores = new_scores;
	scoreChangedSignal.emit(scores);
}

//------------------------------------------------------------------------------------------------------------------------------------

void GameField::clear()
{

	for (int y = 0; y < kRowCount; ++y)
	{
		for (int x = 0; x < kColCount; ++x)
			field[y][x] = Wt::StandardColor::White;
	}

	setScores(0);
	setLevel(1);
}

//----------------------------------------------------------------------------------------------------------------------------------------

GameField::GameField()
{
	clear();
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool GameField::Rotate(bool cw, const Figure* fg)
{
	const auto old_field = field;

	for (int y = 0; y < kRowCount; ++y)
	{
		if (cw)
		{
			const auto col = field[y][0];

			for (int x = 1; x < kColCount; ++x)
			{
				field[y][x - 1] = field[y][x];
			}

			field[y][kColCount - 1] = col;
		}
		else
		{
			const auto col = field[y][kColCount - 1];

			for (int x = kColCount - 1; x > 0; --x)
			{
				field[y][x] = field[y][x - 1];
			}

			field[y][0] = col;

		}

	}


	if (fg->check_self())		return true;

	field = old_field;

	return {};
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool  GameField::CheckCell(const Wt::WPoint& cell_coord) const
{
	if (cell_coord.y() >= kRowCount) return {};

	if (cell_coord.y() < 0) return true;

	return !IsNull(field[cell_coord.y()][cell_coord.x() % kColCount]);
}

//----------------------------------------------------------------------------------------------------------------------------------------

const Wt::WColor* GameField::GetCell(const Wt::WPoint& cell_coord) const
{
	if (cell_coord.y() >= kRowCount) return nullptr;
	return &field[cell_coord.y()][cell_coord.x() % kColCount];
}
//----------------------------------------------------------------------------------------------------------------------------------------

void  GameField::SetCell(const Wt::WPoint& cell_coord, const Wt::WColor& color)
{
	if (cell_coord.y() >= kRowCount) return;

	field[cell_coord.y()][cell_coord.x() % kColCount] = color;
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool  GameField::CheckLine(int y) const
{
	for (int x = 0; x < kColCount; ++x)
		if (IsNull(field[y][x])) return {};
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameField::RemoveLine(int line)
{
	for (int x = 0; x < kColCount; ++x)
	{
		for (int y = line + 1; y < kRowCount; ++y)
		{
			field[y - 1][x] = field[y][x];
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameField::CheckAll()
{
	int s = 0;

	for (int y = 0; y < kRowCount; ++y)
	{
		while (CheckLine(y))
		{
			RemoveLine(y);
			++s;
		}
	}

	if (s)
		setScores(scores + (10 + 10 * s) * s / 2);
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool GameField::AddFigure(const Figure& fg)
{
	bool game_over{};

	for (int y = 0; y < 4; ++y)
		for (int x = 0; x < 4; ++x)
		{
			if (fg.m_coord[y][x])
			{
				auto pt = fg.cur_pt;
				pt += Wt::WPoint(x, -y);
				game_over = game_over || pt.y() >= kRowCount;
				SetCell(pt, fg.m_color);
			}
		}

	if (game_over) return {};

	setScores(scores + 1);

	CheckAll();
	return true;

}

//----------------------------------------------------------------------------------------------------------------------------------------

size_t FigureProducer::NextFigure() const
{
	return  m_next_figure_idx;
}

//----------------------------------------------------------------------------------------------------------------------------------------

size_t FigureProducer::NextFigureColor() const
{
	return  m_next_figure_color;
}

//----------------------------------------------------------------------------------------------------------------------------------------

FigureProducer::FigureProducer() :
	m_next_figure_idx(myrand(FigureCoords.size())),
	m_next_figure_color(myrand(color_map.size())),
	m_direction(myrand(4))
{

}

//----------------------------------------------------------------------------------------------------------------------------------------

void FigureProducer::generate_figure(Figure* fg)
{
	fg->new_figure(
		FigureCoords[m_next_figure_idx],
		myrand(kColCount),
		m_direction,
		color_map[m_next_figure_color]
	);

	m_next_figure_idx = myrand(FigureCoords.size());
	m_next_figure_color = myrand(color_map.size());
	m_direction = myrand(4);
	NewFigureSignal.emit();
}


/************************************************************************************************************************************************
/*
/* Figure
/*
/************************************************************************************************************************************************/

bool Figure::hit_test(const Wt::WPoint& test) const
{
	for (int y = 0; y < 4; ++y)
	{

		for (int x = 0; x < 4; ++x)
		{

			if (m_coord[y][x])
			{
				const Wt::WPoint pt(
					(cur_pt.x() + x) % kColCount,
					cur_pt.y() - y
				);

				if (pt == test) return true;
			}

		}

	}
	return {};
}

//----------------------------------------------------------------------------------------------------------------------------------------

Figure::Figure(const GameField& field, const	FigureProducer& prod) :
	m_field(field),
	m_prod(prod)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------

FigureCoord Figure::rotate(const FigureCoord& coord)
{
	FigureCoord ret;

	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{

			ret[x][3 - y] = coord[y][x];
		}
	}

	return ret;
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool Figure::check(const FigureCoord& coord, const Wt::WPoint& pt) const
{

	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			if (coord[y][x])
			{
				if (m_field.CheckCell({ pt.x() + x, pt.y() - y })) return {};
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------

void Figure::calcul_dropped()
{
	m_dropped_coord = m_coord;

	dropped_pt = cur_pt;

	while (check(m_dropped_coord, { dropped_pt.x(),  dropped_pt.y() - 1 }))
	{
		dropped_pt += Wt::WPoint(0, -1);
	}

}

//----------------------------------------------------------------------------------------------------------------------------------------

bool Figure::Rotate()
{
	const auto new_coord = rotate(m_coord);

	if (!check(new_coord, cur_pt)) return {};

	m_coord = new_coord;

	calcul_dropped();

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool Figure::MoveX(int x, bool)
{
	const auto xx = (x + kColCount) % kColCount;
	if (!check(m_coord, { xx, cur_pt.y() })) return {};
	cur_pt = { xx % kColCount,cur_pt.y() };
	calcul_dropped();
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool Figure::Left()
{
	if (!check(m_coord, { (cur_pt.x() + kColCount - 1) % kColCount, cur_pt.y() })) return {};
	cur_pt = { (cur_pt.x() + kColCount - 1) % kColCount, cur_pt.y() };
	calcul_dropped();
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool Figure::Right()
{
	if (!check(m_coord, { cur_pt.x() + 1, cur_pt.y() })) return {};
	cur_pt = { (cur_pt.x() + 1) % kColCount,cur_pt.y() };
	calcul_dropped();
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool Figure::Droped()
{
	while (Down());
	return {};
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool Figure::Down()
{
	const Wt::WPoint test(cur_pt.x(), cur_pt.y() - 1);
	if (!check(m_coord, test))
	{
		return {};
	}

	cur_pt = test;
	calcul_dropped();
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------

void Figure::new_figure(const FigureCoord& coord, size_t x, size_t rotate_count, const Wt::WColor& color)
{
	m_color = color;
	cur_pt = Wt::WPoint(x % kColCount, kRowCount + 1);
	m_coord = coord;

	for (int i = 1; i <= rotate_count % 4; ++i)
	{
		m_coord = rotate(m_coord);
	}

	calcul_dropped();
}

//----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::NewFigure()
{

	if (m_preview)
		m_preview->update();

	step(true);
}

//----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::AttachPreview(PreviewWidget *preview)
{
	m_preview = preview;
}

//----------------------------------------------------------------------------------------------------------------------------------------

QGameBoard::QGameBoard() :
	figure(field, prod),
	drawer({}, field, figure)
{
	setCanReceiveFocus(true);
	//setFocusPolicy(Qt::StrongFocus);
	mouseWentDown().connect(this, &QGameBoard::mousePressEvent);
	mouseWentUp().connect(this, &QGameBoard::mouseReleaseEvent);
	mouseMoved().connect(this, &QGameBoard::mouseMoveEvent);
	doubleClicked().connect(this, &QGameBoard::mouseDoubleClickEvent);
	mouseWheel().connect(this, &QGameBoard::wheelEvent);
	keyWentDown().connect(this, &QGameBoard::keyPressEvent);
	field.scoreChangedSignal.connect([this](int value) { scoreChangedSignal.emit(value);});
	field.levelChangedSignal.connect(this, &QGameBoard::ChangLevel);
	prod.NewFigureSignal.connect(this, &QGameBoard::NewFigure);

	timer.timeout().connect(this, &QGameBoard::timerEvent);
}

//-------------------------------------------------------------------------------------------------------------

void QGameBoard::NewGame()
{
	timer.stop();
	field.clear();
	state = GameState::active;
	prod.generate_figure(&figure);
	update();
	speed = 1000;
	timer.setInterval(std::chrono::milliseconds(speed));
	timer.start();
}

//-------------------------------------------------------------------------------------------------------------

void QGameBoard::GameOver()
{
	state = GameState::stoping;
	timer.stop();
	GameOverSignal.emit();
}

//-------------------------------------------------------------------------------------------------------------

void QGameBoard::Pause()
{
	if (IsStoped()) return;

	if (state == GameState::active)
	{
		state = GameState::pause;
		timer.stop();
	}
	else
	{
		state = GameState::active;
		timer.setInterval(std::chrono::milliseconds(speed));
		timer.start();
	}
	update();
}

//-------------------------------------------------------------------------------------------------------------

void QGameBoard::ChangLevel(int level)
{
	speed = 2000 / (1 + level);

	timer.stop();
	timer.setInterval(std::chrono::milliseconds(speed));
	timer.start();
	update();
	levelChangedSignal.emit(level);
}


//----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::paintEvent(Wt::WPaintDevice* paintDevice)
{
	drawer.set_rect(Wt::WRectF(0.0, 0.0, 500, 500));
	if(m_preview)
		m_preview->setFigure(prod.NextFigure(), prod.NextFigureColor());
	Wt::WPainter p(paintDevice);
	drawer.draw(&p, !IsStoped());

	Wt::WPen  pen(Wt::StandardColor::Black);
	p.setPen(pen);

	if (IsStoped())
	{
		p.drawText(drawer.rect, Wt::AlignmentFlag::Center | Wt::AlignmentFlag::Middle, Wt::WString("Game Over"));
	}
	else if (state == GameState::pause)
	{
		p.drawText(drawer.rect, Wt::AlignmentFlag::Center | Wt::AlignmentFlag::Middle, Wt::WString("Pause"));
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------

void  QGameBoard::timerEvent()
{
	step(true);
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool QGameBoard::IsStoped() const
{
	return 	state == GameState::stoping;
}

//----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::step(bool down)
{
	if (down)
	{

		auto b = figure.Down();
		if (!b)
		{
			b = field.AddFigure(figure);

			if (b)
				prod.generate_figure(&figure);
			else
				GameOver();
		}
	}

	figure.calcul_dropped();
	update();
}

//----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::wheelEvent(const Wt::WMouseEvent& e)
{
	if (IsStoped()) return;

	field.Rotate(e.wheelDelta() > 0, &figure);

	step();
}

//----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::mouseDoubleClickEvent(const Wt::WMouseEvent& e)
{
	if (IsStoped()) return;

	figure.Droped();

	step();
}

//----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::mousePressEvent(const Wt::WMouseEvent& e)
{
	if (IsStoped()) return;

	if (e.button() == Wt::MouseButton::Left)
	{
		const auto pt = e.widget();
		sel_coord = drawer.ScreenToField({ pt.x, pt.y });
		is_down = true;
		figure.MoveX(sel_coord.x() - 2, {});
		step();
	}
	else
	{
		figure.Rotate();
		step();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::mouseMoveEvent(const Wt::WMouseEvent& e)
{
	if (IsStoped()) return;

	if ((e.button() == Wt::MouseButton::Left) && is_down)
	{
		const auto pt = e.widget();
		sel_coord = drawer.ScreenToField({ pt.x, pt.y });
		figure.MoveX(sel_coord.x() - 2, {});
		step();
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::mouseReleaseEvent(const Wt::WMouseEvent& e)
{
	if (IsStoped()) return;

	if ((e.button() == Wt::MouseButton::Left) && is_down)
	{
		is_down = false;
	}
}


//----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::keyPressEvent(const Wt::WKeyEvent& e)
{
 	if (IsStoped() || (state == GameState::pause && e.key() != Wt::Key::P))
	{
		return;
	}

	switch (e.key())
	{
	
		case Wt::Key::Left:
		{
			figure.Right();
			step();

		}; break;

		case Wt::Key::Right:
		{
			figure.Left();
			step();

		}; break;

		case Wt::Key::Up:
		{
			figure.Rotate();
			step();

		}; break;

		case Wt::Key::Space:
		{
			figure.Droped();
			step();

		}; break;

		case Wt::Key::Down:
		{
			step(true);
		}; break;

		case Wt::Key::P:
		{
			Pause();
		}; break;

		case Wt::Key::D:
		{
			field.Rotate(true, &figure);
			step();

		}; break;

		case Wt::Key::A:
		{
			field.Rotate(false, &figure);
			step();

		}; break;

		case Wt::Key::N:
		{
			NewGame();

		}; break;
	};
}

