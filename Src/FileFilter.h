/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  FileFilter.h
 *
 * @brief Declaration file for FileFilter
 */
// ID line follows -- this is updated by SVN
// $Id: FileFilter.h 6861 2009-06-25 12:11:07Z kimmov $

#include <vector>
#include "pcre.h"
#include "UnicodeString.h"

/**
 * @brief FileFilter rule.
 *
 * Contains one filtering element definition (rule). In addition to
 * regular expression there is boolean value for defining if rule
 * is inclusive or exclusive. File filters have global inclusive/exclusive
 * selection but this per-rule setting overwrites it.
 *
 * We are using PCRE for regular expressions and pRegExp points to compiled
 * regular expression. pRegExpExtra contains additional information about
 * the expression used to optimize matching.
 */
struct FileFilterElement
{
	pcre *pRegExp; /**< Compiled regular expression */
	pcre_extra *pRegExpExtra; /**< Additional information got from regex study */
	FileFilterElement() : pRegExp(NULL), pRegExpExtra(NULL) { };
};

/**
 * @brief One actual filter.
 *
 * For example, this might be a GNU C filter, excluding *.o files and CVS
 * directories. That is to say, a filter is a set of file masks and
 * directory masks. Usually FileFilter contains rules from one filter
 * definition file. So it can be thought as filter file contents.
 * @sa FileFilterList
 */
struct FileFilter
{
	bool default_include;	/**< If true, filter rules are inclusive by default */
	String name;			/**< Filter name (shown in UI) */
	String description;	/**< Filter description text */
	String fullpath;		/**< Full path to filter file */
	std::vector<FileFilterElement*> filefilters; /**< List of rules for files */
	std::vector<FileFilterElement*> dirfilters;  /**< List of rules for directories */
	FileFilter() : default_include(true) { }
	~FileFilter();
	
	static void EmptyFilterList(std::vector<FileFilterElement*> *filterList);
};
