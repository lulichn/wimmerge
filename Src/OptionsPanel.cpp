/** 
 * @file  OptionsPanel.cpp
 *
 * @brief Implementation of OptionsPanel class.
 */
// ID line follows -- this is updated by SVN
// $Id: OptionsPanel.cpp 6725 2009-05-09 14:31:24Z kimmov $

#include "stdafx.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

/**
 * @brief Constructor.
 */
OptionsPanel::OptionsPanel(COptionsMgr *optionsMgr, UINT nIDTemplate)
 : CPropertyPage(nIDTemplate)
 , m_pOptionsMgr(optionsMgr)
{
}
