/*
 * Hydrogen
 * Copyright(c) 2002-2008 by Alex >Comix< Cominu [comix@users.sourceforge.net]
 * Copyright(c) 2008-2021 The hydrogen development team [hydrogen-devel@lists.sourceforge.net]
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
 * along with this program. If not, see https://www.gnu.org/licenses
 *
 */

#include "core/MidiMap.h"
#include "MidiSenseWidget.h"
#include <core/Hydrogen.h>
#include "../HydrogenApp.h"
#include "../CommonStrings.h"

MidiSenseWidget::MidiSenseWidget(QWidget* pParent, bool bDirectWrite, std::shared_ptr<Action> pAction): QDialog( pParent )
{
	auto pCommonStrings = HydrogenApp::get_instance()->getCommonStrings();
	m_bDirectWrite = bDirectWrite;
	m_pAction = pAction;

	setWindowTitle( pCommonStrings->getMidiSenseWindowTitle() );
	setFixedSize( 280, 100 );

	bool midiOperable = false;
	
	m_pURLLabel = new QLabel( this );
	m_pURLLabel->setAlignment( Qt::AlignCenter );

	if(m_pAction != nullptr){
		m_pURLLabel->setText( pCommonStrings->getMidiSenseInput() );
		midiOperable = true;
	} else {

		/*
		 *   Check if this widget got called from the midiTable in the preferences
		 *   window(directWrite=false) or by clicking on a midiLearn-capable gui item(directWrite=true)
		 */

		if(m_bDirectWrite){
			m_pURLLabel->setText( pCommonStrings->getMidiSenseUnavailable() );
			midiOperable = false;
		} else {
			m_pURLLabel->setText( pCommonStrings->getMidiSenseInput() );
			midiOperable = true;
		}
	}
	
	QVBoxLayout* pVBox = new QVBoxLayout( this );
	pVBox->addWidget( m_pURLLabel );
	setLayout( pVBox );
	
	H2Core::Hydrogen *pHydrogen = H2Core::Hydrogen::get_instance();
	pHydrogen->m_LastMidiEvent = "";
	pHydrogen->m_nLastMidiEventParameter = 0;

	m_LastMidiEventParameter = 0;
	
	m_pUpdateTimer = new QTimer( this );

	if(midiOperable)
	{
		/*
		 * If the widget is not midi operable, we can omit
		 * starting the timer which listens to midi input..
		 */

		connect( m_pUpdateTimer, SIGNAL( timeout() ), this, SLOT( updateMidi() ) );
		m_pUpdateTimer->start( 100 );
	}
};

MidiSenseWidget::~MidiSenseWidget(){
	INFOLOG("DESTROY");
	m_pUpdateTimer->stop();
}

void MidiSenseWidget::updateMidi(){
	H2Core::Hydrogen *pHydrogen = H2Core::Hydrogen::get_instance();
	if(	!pHydrogen->m_LastMidiEvent.isEmpty() ){
		m_sLastMidiEvent = pHydrogen->m_LastMidiEvent;
		m_LastMidiEventParameter = pHydrogen->m_nLastMidiEventParameter;


		if( m_bDirectWrite ){
			//write the action / parameter combination to the midiMap
			MidiMap *pMidiMap = MidiMap::get_instance();

			assert(m_pAction);

			std::shared_ptr<Action> pAction = std::make_shared<Action>( m_pAction->getType() );

			pAction->setParameter1( m_pAction->getParameter1() );
			pAction->setParameter2( m_pAction->getParameter2() );
			pAction->setParameter3( m_pAction->getParameter3() );

			if( m_sLastMidiEvent.left(2) == "CC" ){
				pMidiMap->registerCCEvent( m_LastMidiEventParameter , pAction );
			} else if( m_sLastMidiEvent.left(3) == "MMC" ){
				pMidiMap->registerMMCEvent( m_sLastMidiEvent , pAction );
			} else if( m_sLastMidiEvent.left(4) == "NOTE" ){
				pMidiMap->registerNoteEvent( m_LastMidiEventParameter , pAction );
			} else if (m_sLastMidiEvent.left(14) == "PROGRAM_CHANGE" ){
				pMidiMap->registerPCEvent( pAction );
			} else {
				/* In all other cases, the midiMap cares for deleting the pointer */
			}
		}

		close();
	}

}

