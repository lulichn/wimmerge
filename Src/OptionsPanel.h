/** 
 * @file  OptionsPanel.h
 *
 * @brief Declaration file for OptionsPanel class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: OptionsPanel.h 6725 2009-05-09 14:31:24Z kimmov $

#ifndef _OPTIONS_PANEL_H_
#define _OPTIONS_PANEL_H_

#include "IOptionsPanel.h"

class COptionsMgr;

/**
 * @brief A base class for WinMerge options dialogs.
 */
class OptionsPanel : public CPropertyPage, public IOptionsPanel
{
public:
	OptionsPanel(COptionsMgr *optionsMgr, UINT nIDTemplate);

protected:
	COptionsMgr * GetOptionsMgr() const { return m_pOptionsMgr; }

private:
	COptionsMgr * m_pOptionsMgr; /**< Options-manager for storage */
};

#endif // _OPTIONS_PANEL_H_
