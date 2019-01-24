#include "qgameboard.h"
#include <QPainter>
#include <QResizeEvent>
#include <QtMath>
#include <QRandomGenerator>
#include <QTime>


namespace
{
	bool IsNull(const QColor &color)
	{
		return color == Qt::white;
	};
	
	//----------------------------------------------------------------------------------------------------------------------------------------

	size_t myrand(size_t bound)
	{
		return QRandomGenerator::global()->bounded(static_cast<quint32>(bound));
	}

	
	const std::array<QColor,6>  color_map =
	{
		Qt::red,
		Qt::blue,
		Qt::green,
		Qt::yellow,
		Qt::magenta,
		Qt::cyan

	};



	const std::array<FigureCoord,6> FigureCoords =
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

void GameDrawer::draw(QPainter *p, bool is_draw_figure)
{
	draw_field(p);

     if(is_draw_figure)
            draw_figure(p);
}

//----------------------------------------------------------------------------------------------------------------------------------------

QPoint GameDrawer::ScreenToField(const QPoint &pt) const
{
	const auto  delta =pt-rect.center();
    const qreal  r=  sqrt(static_cast<qreal>(delta.x()*delta.x()+delta.y()*delta.y()));
	const qreal angle=atan2(static_cast<qreal>(delta.y()),-delta.x())/(2*M_PI)+0.5;
	
	int x = (angle*kColCount +0.5);
	x%=kColCount;

	if( r<MinRadius)
	{
		return { x ,0 };
	}
	else
		if (r > MaxRadius) return { x,kRowCount - 1 };

	for(int i=0;i<kRowCount;++i)
    {
		if(radius_list[i+1]>r)
		{
		
			return { x ,i };
		}

    };

	return { 100000,100000 };

}
				
//----------------------------------------------------------------------------------------------------------------------------------------

GameDrawer::GameDrawer(const QRect &r,const GameField &fld,const Figure &fg):
	m_field(fld),
	m_figure(fg)


{
	set_rect(r);
}

//----------------------------------------------------------------------------------------------------------------------------------------
void GameDrawer::calcul_radius()
{
        MaxRadius=qMin(rect.width(),rect.height())*0.5-5;
        MinRadius=MaxRadius/coeff;

        const qreal step= 1.0*(MaxRadius-MinRadius)/kRowCount;

        for(int i=0;i<=kRowCount;++i)
        {
               
                radius_list[i]=MinRadius+step*i;

        };

}

//----------------------------------------------------------------------------------------------------------------------------------------

 void GameDrawer::fill_path_list()
{


        for(int x=0;x<kColCount;++x)
        {
            const qreal angle =2*M_PI*((x-0.5)/kColCount);
            const qreal sweep_angle =2* M_PI /kColCount;
			const qreal c1=   cos(angle);
			const qreal s1=   sin(-angle);
			const qreal c2=   cos(-angle-sweep_angle);
			const qreal s2=   sin(-angle-sweep_angle);

            lines[2*x]      = QPoint(radius_list[0]*c1,radius_list[0]*s1)+rect.center();
            lines[2*x+1]   = QPoint(radius_list[kRowCount]*c1,radius_list[kRowCount]*s1)+rect.center();

            for(int y=0;y<kRowCount;++y)
            {
                    const auto r1=radius_list[y];
					const auto r2=radius_list[y+1];
					const auto x1= r1* c1+rect.center().x();
					const auto y1= r1* s1+rect.center().y();
					const auto x2= r2*c1+rect.center().x();
					const auto y2= r2*s1+rect.center().y();
					const auto x3= r1* c2+rect.center().x();
					const auto y3= r1* s2+rect.center().y();
                    QPainterPath path;

                    path.moveTo(x1,y1);
                    path.lineTo(x2,y2);
                    path.arcTo(rect.center().x()-r2,rect.center().y()-r2,2*r2,2*r2,180*angle*M_1_PI,180*sweep_angle*M_1_PI);

                    path.lineTo(x3,y3);

                    path.arcTo(rect.center().x()-r1,rect.center().y()-r1,2*r1,2*r1,180*(angle+sweep_angle)*M_1_PI,-180*sweep_angle*M_1_PI);
                    paths[y][x]=path.simplified();
                }

        }


}

	
//----------------------------------------------------------------------------------------------------------------------------------------

void GameDrawer::draw_cell(const QPoint &cell_coord,QPainter *p,const QPen &pen,const QBrush &brush)
{
	
		if(cell_coord.y()>=kRowCount||cell_coord.y()<0) return ;

		int x=cell_coord.x()%kColCount;
		
		const auto &path=paths[cell_coord.y()][x];

		p->setPen(pen);
		p->fillPath(path,brush);
		p->drawPath(path);
	
}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameDrawer::draw_field(QPainter *p)
{

		QPen pen(Qt::gray) ;
	//pen.setStyle(Qt::DashLine);
		QBrush brush;
        p->setPen(pen);

        p->drawLines(lines.data(),kColCount);

        for(int y=0;y<=kRowCount;++y)
         {
                int r= qRound(radius_list[y]);
                p->drawEllipse(rect.center(), r,r);
         }



		for(int y=0;y<kRowCount;++y)
			for(int x=0;x<kColCount;++x)			
			{
				QPoint pt(x,y);
				if(!m_field.CheckCell(pt)) continue;

				const auto  color=*m_field.GetCell(pt);
				brush.setColor(color);
				brush.setStyle(!m_field.CheckCell(pt)?Qt::NoBrush:Qt::SolidPattern);
				draw_cell(pt,p,pen, brush);
			}

	//draw_cell(QPoint(10,10),p,Qt::white);
}

//----------------------------------------------------------------------------------------------------------------------------------------
#include <QDebug>

void GameDrawer::draw_preview(QPainter *p, size_t figure_idx, size_t color_idx)
{

    p->setBrush(QBrush(color_map[color_idx]));

    for(int y=0;y<4;++y)
            for(int x=0;x<4;++x)
            {
                    if( FigureCoords[figure_idx][y][x])
                    {
                        p->drawRect( 15+15*x,15+15*y,15,15);
                    }

            }
}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameDrawer::draw_figure(QPainter *p)
{
		int min_x=1000;
		int max_x{};
		

		QPen pen(Qt::black) ;

		int min_ys[4];
		int width=0;

		for(int x=0;x<4;++x)			
		{
			int min_y=1000;

			for(int y=0;y<4;++y)			
			{
				
				if(m_figure.m_coord[y][x])
				{
						int xx=m_figure.cur_pt.x()+x+kColCount;
						int yy=m_figure.cur_pt.y()-y;
						draw_cell({ xx,yy }, p, pen, QBrush(m_figure.m_color));
						min_x=qMin(min_x,xx);
						max_x=qMax(max_x,xx);
						min_y=qMin(min_y,yy);
				}

				if(m_figure.m_dropped_coord[y][x])
				{
					int xx=m_figure.dropped_pt.x()+x+kColCount;
					int yy=m_figure.dropped_pt.y()-y;	
					draw_cell({ xx,yy }, p, pen, QBrush(Qt::NoBrush));
						
				}

			}

			if(min_y<=kRowCount)
			{
			min_ys[width++]=min_y;
			}

		}

		QBrush brush(QColor(128,0,0,50));

		pen.setStyle(Qt::NoPen);

		int idx=0;
		if(!width) return;

		for(int x=min_x;x<=max_x;++x,++idx)
		{
				for(int y=0;y<min_ys[idx];++y) 
				{
						draw_cell({x,y},p,pen,brush);
				}
		}

}

//----------------------------------------------------------------------------------------------------------------------------------------
			
void GameDrawer::set_rect(const QRect &r)
{
		rect			=	r;
		calcul_radius();
        fill_path_list();
}

			
//----------------------------------------------------------------------------------------------------------------------------------------

void GameField::setLevel(int new_level)
{
           level = new_level;
           emit levelChangedSignal(level);

}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameField::setScores(int new_scores)
{
        if(scores/100<new_scores/100)      setLevel(level+1);
		scores    =   new_scores;
        emit scoreChangedSignal(scores);
}

//------------------------------------------------------------------------------------------------------------------------------------

void GameField::clear()
{

		for(int y=0;y<kRowCount;++y)
		{
			for(int x=0;x<kColCount;++x)			
				field[y][x]=Qt::white;
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


bool GameField::Rotate(bool cw,const Figure *fg)
{
	const auto old_field = field;
	
    for(int y=0;y<kRowCount;++y)
    {
        if(cw)
        {
            const auto col= field[y][0];

            for(int x=1;x<kColCount;++x)
            {
                field[y][x-1]=field[y][x];
            }

            field[y][kColCount-1]=col;
        }
        else
        {
			const auto col= field[y][kColCount-1];

            for(int x=kColCount-1;x>0;--x)
            {
                field[y][x]=field[y][x-1];
            }

            field[y][0]=col;

        }

    }



	if(fg->check_self())		return true;
	
	field = old_field;

	return {};


}

//----------------------------------------------------------------------------------------------------------------------------------------
		
bool  GameField::CheckCell(const QPoint &cell_coord) const
{
		if (cell_coord.y() >= kRowCount) return {};

		if(cell_coord.y()<0) return true;
		
		return !IsNull(field[cell_coord.y()][cell_coord.x()%kColCount]);
}

//----------------------------------------------------------------------------------------------------------------------------------------

const QColor * GameField::GetCell(const QPoint &cell_coord) const
{
	if(cell_coord.y()>=kRowCount) return 0;
		return &field[cell_coord.y()][cell_coord.x()%kColCount];
}
//----------------------------------------------------------------------------------------------------------------------------------------

void  GameField::SetCell(const QPoint &cell_coord,const QColor &color) 
{
		if(cell_coord.y()>=kRowCount) return ;
				
		field[cell_coord.y()][cell_coord.x()%kColCount]=color;
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool  GameField::CheckLine(int y) const
{
		for(int x=0;x<kColCount;++x)			
			if (IsNull(field[y][x])) return {};
		return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameField::RemoveLine(int line)
{
		for(int x=0;x<kColCount;++x)			
		{
			for(int y=line+1;y<kRowCount;++y)			
			{
				field[y-1][x]=field[y][x];
			}
		}
}

//----------------------------------------------------------------------------------------------------------------------------------------

void GameField::CheckAll()
{
    int s=0;

	for(int y=0;y<kRowCount;++y)
	{
		while(CheckLine(y))
		{
			RemoveLine(y);
			++s;
		}
	}

	if(s)
         setScores(scores+(10+10*s)*s/2);

}

//----------------------------------------------------------------------------------------------------------------------------------------

bool GameField::AddFigure(const Figure & fg )
{
	bool game_over{};

	for(int y=0;y<4;++y)			
		for(int x=0;x<4;++x)			
		{
			if(fg.m_coord[y][x])
			{
				const auto pt= fg.cur_pt+QPoint(x,-y);
				game_over=game_over||pt.y()>=kRowCount;
				SetCell(pt,fg.m_color) ;
			}
		}

	if(game_over) return {};

	setScores(scores+1);

	CheckAll();
	return true;

}

/*
class FigureProducer
{
	public:
		void generate_figure(Figure * ) const;


};*/


//----------------------------------------------------------------------------------------------------------------------------------------

size_t FigureProducer::NextFigure()		const
{
	return  m_next_figure_idx;
}

//----------------------------------------------------------------------------------------------------------------------------------------

size_t FigureProducer::NextFigureColor()	const
{
	return  m_next_figure_color;
}

//----------------------------------------------------------------------------------------------------------------------------------------

FigureProducer::FigureProducer():
	m_next_figure_idx(myrand(FigureCoords.size())),
	m_next_figure_color(myrand(color_map.size())),
	m_direction(myrand(4))
{
  
}

	//----------------------------------------------------------------------------------------------------------------------------------------

void FigureProducer::generate_figure(Figure * fg)
{
	
	fg->new_figure(
						FigureCoords[m_next_figure_idx],
						myrand(kColCount),
						m_direction,
						color_map[m_next_figure_color]
					);

	m_next_figure_idx	=	myrand(FigureCoords.size());
	m_next_figure_color	=	myrand(color_map.size());
	m_direction			=	myrand(4);

    emit NewFigureSignal();

}


//----------------------------------------------------------------------------------------------------------------------------------------
/************************************************************************************************************************************************

Figure


***********************************************************************************************************************************************/

bool Figure::hit_test(const QPoint &test) const
{	
	
	for(int y=0;y<4;++y)			
	{
		
		for(int x=0;x<4;++x)			
		{
			
			if(m_coord[y][x])
			{
				const QPoint pt( 
							(cur_pt.x()+x)%kColCount,
							cur_pt.y()-y 
					     );

				if (pt==test) return true;
			}

		}

	}

	return {};
	
}

//----------------------------------------------------------------------------------------------------------------------------------------

Figure::Figure(const GameField &field,const	FigureProducer	&prod):
		m_field(field),
		m_prod(prod)
{
	
}


//----------------------------------------------------------------------------------------------------------------------------------------

FigureCoord Figure::rotate(const FigureCoord &coord)
{
	FigureCoord ret;	

	for(int y=0;y<4;++y)			
	{
		for(int x=0;x<4;++x)			
		{
		
			ret[x][3-y]= coord[y][x];
		}
	}


	return ret;
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool Figure::check(const FigureCoord &coord,const QPoint &pt) const
{

	for(int y=0;y<4;++y)
	{
			for(int x=0;x<4;++x)			
			{
				if(coord[y][x])
				{
					if (m_field.CheckCell(pt + QPoint(x, -y))) return {};
				}
			}
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------
void Figure::calcul_dropped()
{
	m_dropped_coord=m_coord;

	dropped_pt=cur_pt;

	while(check(m_dropped_coord,dropped_pt+QPoint(0,-1)))
	{
		dropped_pt=dropped_pt+QPoint(0,-1);
	}
	
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool Figure::Rotate()
{	
	const auto new_coord=rotate(m_coord);
	
	if (!check(new_coord, cur_pt)) return {};

	m_coord=new_coord;

	calcul_dropped();

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool Figure::MoveX(int x,bool)
{
	const auto xx=(x+kColCount)%kColCount;
	
	if (!check(m_coord, { xx, cur_pt.y() })) return {};

	cur_pt = { xx%kColCount,cur_pt.y() };

	calcul_dropped();

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool Figure::Left()
{
	
	if (!check(m_coord, cur_pt + QPoint(-1, 0))) return {};
	
	cur_pt = { (cur_pt.x() + kColCount - 1) % kColCount,cur_pt.y() };

	calcul_dropped();
	
	return true;

}

//----------------------------------------------------------------------------------------------------------------------------------------

bool Figure::Right()
{

	if (!check(m_coord, cur_pt + QPoint(+1, 0))) return {};
	cur_pt = { (cur_pt.x() + 1) % kColCount,cur_pt.y() };
	calcul_dropped();

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------

bool Figure::Droped()
{
	while(Down());
	return {};
	
}

//----------------------------------------------------------------------------------------------------------------------------------------
        
bool Figure::Down()
{

	if(!check(m_coord,cur_pt+QPoint(0,-1))) 
	{
		return {};
	}

	cur_pt=cur_pt+QPoint(0,-1);
	calcul_dropped();

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------

void Figure::new_figure(const FigureCoord &coord, size_t x, size_t rotate_count,const QColor &color )
{
		m_color	=	color;
		cur_pt	=	QPoint(x%kColCount,kRowCount+1);
		m_coord	=	coord;

		for(int i=1;i<=rotate_count%4;++i)
		{
			m_coord=rotate(m_coord);
		}

		calcul_dropped();

}
			
//----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::NewFigure()
{

        if(m_preview)
             m_preview->repaint();

        step(true);

}

//----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::AttachPreview(QWidget *preview)
{
   m_preview=preview;
   m_preview->installEventFilter(this);
}
//----------------------------------------------------------------------------------------------------------------------------------------

bool QGameBoard::eventFilter(QObject *obj, QEvent *event)
{

        if (event->type() == QEvent::Paint)
        {

            QPainter p(m_preview);
            drawer.draw_preview(&p,prod.NextFigure(),prod.NextFigureColor());

             return false;
         }
        else
        {
             // standard event processing
             return QObject::eventFilter(obj, event);
         }
}

//----------------------------------------------------------------------------------------------------------------------------------------
QGameBoard::QGameBoard(QWidget *parent)
        :QWidget(parent),
		state(stoping),
		figure(field,prod),
		drawer(rect(), field, figure)
{
	
	setFocusPolicy(Qt::StrongFocus);
    connect(&field,&GameField::scoreChangedSignal,this, &QGameBoard::scoreChangedSignal);
    connect(&field, &GameField::levelChangedSignal,this, &QGameBoard::ChangLevel);
    connect(&prod,&FigureProducer ::NewFigureSignal,this,&QGameBoard::NewFigure);

}

//-------------------------------------------------------------------------------------------------------------

void QGameBoard::NewGame()
{
	killTimer(timer_id);
	field.clear();
	state	= active;
	prod.generate_figure(&figure);
	update();
	speed=1000;
	timer_id	=	startTimer(speed);
}

//-------------------------------------------------------------------------------------------------------------
void QGameBoard::GameOver()
{
        state=stoping    ;
        killTimer(timer_id);
        emit GameOverSignal();
}

//-------------------------------------------------------------------------------------------------------------
void QGameBoard::Pause()
{
        if(state==stoping)            return;
        

        if (state==active)
        {
               state=pause;
               killTimer(timer_id);

        }
        else
        {
                state=active;
                timer_id=startTimer(speed);

        }
        update();

        
}
//-------------------------------------------------------------------------------------------------------------

void QGameBoard::ChangLevel(int level)
{
        speed=2000 / (1 + level);

         killTimer(timer_id);
         timer_id=startTimer(speed);
         update();
         emit levelChangedSignal(level);
}


//-------------------------------------------------------------------------------------------------------------



void QGameBoard::resizeEvent(QResizeEvent *)
{
	drawer.set_rect(rect());
}

//----------------------------------------------------------------------------------------------------------------------------------------


void QGameBoard::paintEvent (QPaintEvent *)
{
		QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        drawer.draw(&p,state!=stoping);

        QPen  pen (Qt::black);
        p.setPen( pen);

        if(state==stoping)
        {
                p.drawText(rect(), Qt::AlignCenter,QString("Game Over"));
        }
        else      if(state==pause)
        {
                p.drawText(rect(), Qt::AlignCenter,QString("Pause"));
        }


}


//----------------------------------------------------------------------------------------------------------------------------------------


void  QGameBoard::timerEvent(QTimerEvent * e)
{
    if(e->timerId()==timer_id)
    {
           step(true);
    }
    else
        QWidget::timerEvent(e);
	
}

//----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::step(bool down)
{
        if(down)
        {
	
				auto b = figure.Down();
                if(!b)
                {
                      b=field.AddFigure(figure);

                      if(b)
                           prod.generate_figure(&figure);
                      else
                          GameOver();

                }

        }

		figure.calcul_dropped();
        update();
}


//----------------------------------------------------------------------------------------------------------------------------------------


void QGameBoard::wheelEvent(QWheelEvent * e)
{
    if(state==stoping) return;
	
    field.Rotate(e->delta()>0,&figure);

    step();
}

//----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::mouseDoubleClickEvent(QMouseEvent*)
{
	if(state==stoping) return;

	figure.Droped();
	step();
}

//----------------------------------------------------------------------------------------------------------------------------------------


 void QGameBoard::mousePressEvent(QMouseEvent *event)
 {
     if(state==stoping) return;
	 
	 if (event->button() == Qt::LeftButton) 
	 {
         
		 grabMouse();
		 sel_coord	=	drawer.ScreenToField(event->pos());
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

 void QGameBoard::mouseMoveEvent(QMouseEvent *event)
 {
	 if(state==stoping) return;
     
	 if ((event->buttons() & Qt::LeftButton) && is_down)
	 {
		 sel_coord	=	drawer.ScreenToField(event->pos());
		 figure.MoveX(sel_coord.x() - 2, {});
		 step();
	 }

 }

 //----------------------------------------------------------------------------------------------------------------------------------------

 void QGameBoard::mouseReleaseEvent(QMouseEvent *event)
 {
	 if(state==stoping) return;

	 if (event->button() == Qt::LeftButton && is_down) 
	 {
		 releaseMouse();
             
		 is_down = {};

     }
 }


 //----------------------------------------------------------------------------------------------------------------------------------------

void QGameBoard::keyPressEvent(QKeyEvent *e)
{
    if (state==stoping||(state==pause&&e->key()!=Qt::Key_P))
    {
        QWidget::keyPressEvent(e);
        return;
    }
	
	switch(e->key())
	{
			case Qt::Key_Left:	
			{
                 figure.Right();
				 step();
					
			};break;
			
			case Qt::Key_Right:	
			{
                  figure.Left();
				  step();
					
			};break;

			case Qt::Key_Up:	
			{
				figure.Rotate();
				step();
					
			};break;

			case Qt::Key_Space:	
			{
				figure.Droped();
				step();
					
			};break;

			case Qt::Key_Down:	
			{
				step(true);
            };break;

			case Qt::Key_P:
			{
					Pause();
			};break;

			case Qt::Key_D:
			{
					field.Rotate(true,&figure);
					step();

			};break;

			case Qt::Key_A:
			{
					field.Rotate(false,&figure);
					step();

			};break;

			case Qt::Key_N:
			{
					NewGame();

			};break;


			default :
                        QWidget::keyPressEvent(e);


	};
	
	
	
}

