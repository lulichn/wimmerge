/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  ConflictFileParser.cpp
 *
 * @brief Implementation for conflict file parser.
 */
// ID line follows -- this is updated by SVN
// $Id: ConflictFileParser.cpp 6638 2009-04-03 23:17:42Z sdottaka $

// Conflict file parsing method modified from original code got from:
// TortoiseCVS - a Windows shell extension for easy version control
// Copyright (C) 2000 - Francis Irving
// <francis@flourish.org> - January 2001

#include "StdAfx.h"
#include "UnicodeString.h"
#include "UniFile.h"
#include "ConflictFileParser.h"


// Note: keep these strings in "wrong" order so we can resolve this file :)
/** @brief String separating Mine and Theirs blocks. */
static const TCHAR Separator[] = _T("=======");
/** @brief String ending Theirs block (and conflict). */
static const TCHAR TheirsEnd[] = _T(">>>>>>> ");
/** @brief String starting Mine block (and conflict). */
static const TCHAR MineBegin[] = _T("<<<<<<< ");

/**
 * @brief Check if the file is a conflict file.
 * This function checks if the conflict file marker is found from given file.
 * This is faster than trying to parse a file that is not conflict file.
 * @param [in] conflictFileName Full path to file to check.
 * @return true if given file is a conflict file, false otherwise.
 */
bool IsConflictFile(LPCTSTR conflictFileName)
{
	UniMemFile conflictFile;
	bool startFound = false;

	// open input file
	bool success = conflictFile.OpenReadOnly(conflictFileName);

	// Search for a conflict marker
	bool linesToRead = true;
	while (linesToRead && !startFound)
	{
		String line;
		bool lossy;
		String eol;
		linesToRead = conflictFile.ReadString(line, eol, &lossy);

		std::string::size_type pos;
		pos = line.find(MineBegin);
		if (pos == 0)
			startFound = true;
	}
	conflictFile.Close();

	return startFound;
}

/**
 * @brief Parse a conflict file to separate files.
 * This function parses a conflict file to two different files which can be
 * opened into WinMerge's file compare.
 * @param [in] conflictFileName Full path to conflict file.
 * @param [in] workingCopyFileName Full path for user's modified file in
 *  working copy/working folder.
 * @param [in] newRevisionFileName Full path for revision control file.
 * @param [out] bNestedConflicts returned as true if nested conflicts found.
 * @return true if conflict file was successfully parsed, false otherwise.
 */
bool ParseConflictFile(LPCTSTR conflictFileName,
		LPCTSTR workingCopyFileName, LPCTSTR newRevisionFileName,
		bool &bNestedConflicts)
{
	UniMemFile conflictFile;
	UniStdioFile workingCopy;
	UniStdioFile newRevision;
	String line;
	std::string::size_type pos;
	int state;
	int iNestingLevel = 0;
	bool bResult = false;
	String revision = _T("none");
	bNestedConflicts = false;

	// open input file
	bool success = conflictFile.OpenReadOnly(conflictFileName);

	// Create output files
	bool success2 = workingCopy.Open(workingCopyFileName, _T("wb"));
	bool success3 = newRevision.Open(newRevisionFileName, _T("wb"));

	state = 0;
	bool linesToRead = true;
	do
	{
		bool lossy;
		String eol;
		linesToRead = conflictFile.ReadString(line, eol, &lossy);
		switch (state)
		{
			// in common section
		case 0:
			// search beginning of conflict section
			pos = line.find(MineBegin);
			if (pos == 0)
			{
				// working copy section starts
				state = 1;
				bResult = true;
			}
			else
			{
				// we're in the common section, so write to both files
				newRevision.WriteString(line);
				newRevision.WriteString(eol);

				workingCopy.WriteString(line);
				workingCopy.WriteString(eol);
			}
			break;

			// in working copy section
		case 1:
			// search beginning of conflict section
			pos = line.find(MineBegin);
			if (pos == 0)
			{
				// nested conflict section starts
				state = 3;
				workingCopy.WriteString(line);
				workingCopy.WriteString(eol);
			}
			else
			{
				pos = line.find(Separator);
				if ((pos != std::string::npos) && (pos == (line.length() - 7)))
				{
					line = line.substr(0, pos);
					if (!line.empty())
					{
						workingCopy.WriteString(line);
						workingCopy.WriteString(eol);
					}

					//  new revision section
					state = 2;
				}
				else
				{
					workingCopy.WriteString(line);
					workingCopy.WriteString(eol);
				}
			}
			break;

			// in new revision section
		case 2:
			// search beginning of nested conflict section
			pos = line.find(MineBegin);
			if (pos == 0)
			{
				// nested conflict section starts
				state = 4;
				newRevision.WriteString(line);
				newRevision.WriteString(eol);
			}
			else
			{
				pos = line.find(TheirsEnd);
				if (pos != std::string::npos)
				{
					revision = line.substr(pos + 8);
					line = line.substr(0, pos);
					if (!line.empty())
					{
						newRevision.WriteString(line);
						newRevision.WriteString(eol);
					}

					//  common section
					state = 0;
				}
				else
				{
					newRevision.WriteString(line);
					newRevision.WriteString(eol);
				}
			}
			break;


			// in nested section in working copy section
		case 3:
			// search beginning of nested conflict section
			bNestedConflicts = true;
			pos = line.find(MineBegin);
			if (pos == 0)
			{
				iNestingLevel++;
			}
			else
			{
				pos = line.find(TheirsEnd);
				if (pos != std::string::npos)
				{
					if (iNestingLevel == 0)
					{
						state = 1;
					}
					else
					{
						iNestingLevel--;
					}
				}
			}
			workingCopy.WriteString(line);
			workingCopy.WriteString(eol);
			break;

			// in nested section in new revision section
		case 4:
			// search beginning of nested conflict section
			pos = line.find(MineBegin);
			if (pos == 0)
			{
				iNestingLevel++;
			}
			else
			{
				pos = line.find(TheirsEnd);
				if (pos != std::string::npos)
				{
					if (iNestingLevel == 0)
					{
						state = 2;
					}
					else
					{
						iNestingLevel--;
					}
				}
			}
			newRevision.WriteString(line);
			newRevision.WriteString(eol);
			break;
		}
	} while (linesToRead);

	// Close
	newRevision.Close();
	workingCopy.Close();
	conflictFile.Close();
	return bResult;
}
