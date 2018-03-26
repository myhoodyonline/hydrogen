/*
 * Hydrogen
 * Copyright(c) 2002-2008 by Alex >Comix< Cominu [comix@users.sourceforge.net]
 *
 * http://www.hydrogen-music.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "LCDCombo.h"

#include "../Skin.h"
#include "LCD.h"
#include "Button.h"

#include <hydrogen/globals.h>

const char* LCDCombo::__class_name = "LCDCombo";

LCDCombo::LCDCombo(QWidget *pParent, int digits)
 : QWidget(pParent)
 , Object( __class_name )
{
	INFOLOG( "INIT" );

	display = new LCDDisplay( this, LCDDigit::SMALL_BLUE, digits, false);
	button = new Button( this,
			"/patternEditor/btn_dropdown_on.png",
			"/patternEditor/btn_dropdown_off.png",
			"/patternEditor/btn_dropdown_over.png",
			QSize(13, 13)
	);
	pop = new QMenu( this );
	size = digits;
	active = 0;

	button->move( ( digits * 8 ) + 5 , 1 );
	setFixedSize( ( digits * 8 ) + 17, display->height() );

	connect( button, SIGNAL( clicked( Button* ) ), this, SLOT( onClick( Button* ) ) );

	connect( pop, SIGNAL( triggered(QAction*) ), this, SLOT( changeText(QAction*) ) );
}




LCDCombo::~LCDCombo()
{
}


QString LCDCombo::getText()
{
	return display->getText();
};


void LCDCombo::changeText(QAction* pAction)
{
	//_WARNINGLOG("triggered");
// 	display->setText(pAction->text());
// 	emit valueChanged( pAction->text() );
	set_text( pAction->text() );
}



void LCDCombo::onClick(Button*)
{
	pop->popup( display->mapToGlobal( QPoint( 1, display->height() + 2 ) ) );
}

bool LCDCombo::addItem( const QString &text )
{
	//INFOLOG( "add item" );

	if ( text.size() <= size ){
		items.append( pop->addAction(text) );
		return true;
	}else{
		return false;
	}
}



void LCDCombo::addSeparator()
{
	items.append( pop->addSeparator() );
}

void LCDCombo::mousePressEvent(QMouseEvent *ev)
{
	UNUSED( ev );
	pop->popup( display->mapToGlobal( QPoint( 1, display->height() + 2 ) ) );
}

void LCDCombo::wheelEvent( QWheelEvent * ev )
{
	ev->ignore();
	const int n = items.size();
	const int d = ( ev->delta() > 0 ) ? -1: 1;
	active = ( n + active + d ) % n;
	if ( items.at( active )->isSeparator() )
		active = ( n + active + d ) % n;
	set_text( items.at( active )->text() );
}


void LCDCombo::set_text( const QString &text)
{
	set_text(text, true);
}

void LCDCombo::set_text( const QString &text, bool emit_on_change)
{
	if (display->getText() == text) {
		return;
	}
	//INFOLOG( text );
	display->setText( text );
	for ( int i = 0; i < items.size(); i++ ) {
		if ( items.at(i)->text() == text )
			active = i;
	}
	
	if(emit_on_change)
		emit valueChanged( text );
}


