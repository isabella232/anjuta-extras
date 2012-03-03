/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
    aneditor-indent.cxx
    Copyright (C) 2004 Naba Kumar

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "aneditor-priv.h"

void AnEditor::IndentationIncrease(){
	CharacterRange crange = GetSelection();
	if (crange.cpMin != crange.cpMax)
	{
		SendEditor (SCI_TAB);
		return;
	}
	int line =SendEditor(SCI_LINEFROMPOSITION, SendEditor (SCI_GETCURRENTPOS));
	int indent =GetLineIndentation(line);
	indent +=SendEditor(SCI_GETINDENT);
	SetLineIndentation(line, indent);
}

void AnEditor::IndentationDecrease(){
	CharacterRange crange = GetSelection();
	if (crange.cpMin != crange.cpMax)
	{
		SendEditor (SCI_BACKTAB);
		return;
	}
	int line =SendEditor(SCI_LINEFROMPOSITION, SendEditor (SCI_GETCURRENTPOS));
	int indent = GetLineIndentation(line);
	indent -=SendEditor(SCI_GETINDENT);
	if (indent < 0) indent = 0;
	SetLineIndentation(line, indent);
}

void AnEditor::SetLineIndentation(int line, int indent) {
	if (indent < 0)
		return;
	CharacterRange crange = GetSelection();
	int posBefore = GetLineIndentPosition(line);
	SendEditor(SCI_SETLINEINDENTATION, line, indent);
	int posAfter = GetLineIndentPosition(line);
	int posDifference =  posAfter - posBefore;
	if (posAfter > posBefore) {
		// Move selection on
		if (crange.cpMin >= posBefore) {
			crange.cpMin += posDifference;
		}
		if (crange.cpMax >= posBefore) {
			crange.cpMax += posDifference;
		}
	} else if (posAfter < posBefore) {
		// Move selection back
		if (crange.cpMin >= posAfter) {
			if (crange.cpMin >= posBefore)
				crange.cpMin += posDifference;
			else
				crange.cpMin = posAfter;
		}
		if (crange.cpMax >= posAfter) {
			if (crange.cpMax >= posBefore)
				crange.cpMax += posDifference;
			else
				crange.cpMax = posAfter;
		}
	}
	SetSelection(crange.cpMin, crange.cpMax);
}

int AnEditor::GetLineIndentation(int line) {
	return SendEditor(SCI_GETLINEINDENTATION, line);
}

int AnEditor::GetLineIndentPosition(int line) {
	return SendEditor(SCI_GETLINEINDENTPOSITION, line);
}

bool AnEditor::RangeIsAllWhitespace(int start, int end) {
	char *buffer = new char [end - start + 1];
	bool all_white = true;
	
	GetRange(wEditor, start, end, buffer);
	
	for (int i = start;i < end;i++) {
		if ((buffer[i] != ' ') && (buffer[i] != '\t'))
			all_white = false;
			break;
	}

	return all_white;
}

void AnEditor::MaintainIndentation(char ch) {
	int eolMode = SendEditor(SCI_GETEOLMODE);
	int curLine = GetCurrentLineNumber();
	int lastLine = curLine - 1;
	int indentAmount = 0;

	if (((eolMode == SC_EOL_CRLF || eolMode == SC_EOL_LF) && ch == '\n') ||
		 (eolMode == SC_EOL_CR && ch == '\r')) {
		if (props->GetInt("indent.automatic")) {
			while (lastLine >= 0 && GetLineLength(lastLine) == 0)
				lastLine--;
		}
		if (lastLine >= 0) {
			indentAmount = GetLineIndentation(lastLine);
		}
		if (indentAmount > 0) {
			SetLineIndentation(curLine, indentAmount);
		}
	}
}

#if 0
void AnEditor::AutomaticIndentation(char ch) {
	CharacterRange crange = GetSelection();
	int selStart = crange.cpMin;
	int curLine = GetCurrentLineNumber();
	int thisLineStart = SendEditor(SCI_POSITIONFROMLINE, curLine);
	int indentSize = SendEditor(SCI_GETINDENT);

	if (blockEnd.IsSingleChar() && ch == blockEnd.words[0]) {
		// Dedent maybe
		if (!indentClosing) {
			if (RangeIsAllWhitespace(thisLineStart, selStart - 1)) {
				int indentBlock = IndentOfBlockProper(curLine - 1);
				SetLineIndentation(curLine, indentBlock - indentSize);
			}
		}
	} else if (!blockEnd.IsSingleChar() && (ch == ' ')) {
		// Dedent maybe
		if (!indentClosing && (GetIndentState(curLine) == isBlockEnd)) {}
	} else if (ch == blockStart.words[0]) {
		// Dedent maybe if first on line and previous line was starting keyword
		if (!indentOpening &&
			(GetIndentState(curLine - 1) == isKeyWordStart)) {
			if (RangeIsAllWhitespace(thisLineStart, selStart - 1)) {
				int indentBlock = IndentOfBlockProper(curLine - 1);
				SetLineIndentation(curLine, indentBlock - indentSize);
			}
		}
	} else if ((ch == '\r' || ch == '\n') && (selStart == thisLineStart)) {
		// printf("New line block\n");
		int indentBlock = IndentOfBlock(curLine - 1);
		if (!indentClosing && !blockEnd.IsSingleChar()) {
			// Dedent previous line maybe
			
			SString controlWords[1];
			// printf ("First if\n");
			
			if (GetLinePartsInStyle(curLine-1, blockEnd.styleNumber,
				-1, controlWords, ELEMENTS(controlWords))) {
				// printf ("Second if\n");
				if (includes(blockEnd, controlWords[0])) {
					// printf ("Third if\n");
					// Check if first keyword on line is an ender
					SetLineIndentation(curLine-1,
									   IndentOfBlockProper(curLine-2)
									   - indentSize);
					// Recalculate as may have changed previous line
					indentBlock = IndentOfBlock(curLine - 1);
				}
			}
		}
		SetLineIndentation(curLine, indentBlock);
		
		// Home cursor.
		if (SendEditor (SCI_GETCOLUMN,
						SendEditor(SCI_GETCURRENTPOS)) < indentBlock)
			SendEditor (SCI_VCHOME);
		
	} else if (lexLanguage == SCLEX_CPP) {
		if ((ch == '\t')) {
		
			int indentBlock = IndentOfBlock(curLine - 1);
			int indentState = GetIndentState (curLine);
			
			if (blockStart.IsSingleChar() && indentState == isBlockStart) {
				if (!indentOpening) {
					if (RangeIsAllWhitespace(thisLineStart, selStart - 1)) {
						// int indentBlock = IndentOfBlockProper(curLine - 1);
						SetLineIndentation(curLine, indentBlock - indentSize);
					}
				}
			} else if (blockEnd.IsSingleChar() && indentState == isBlockEnd) {
				if (!indentClosing) {
					if (RangeIsAllWhitespace(thisLineStart, selStart - 1)) {
						// int indentBlock = IndentOfBlockProper(curLine - 1);
						SetLineIndentation(curLine, indentBlock - indentSize);
					}
				}
			} else {
				SetLineIndentation(curLine, indentBlock);
			}
			
			// Home cursor.
			if (SendEditor (SCI_GETCOLUMN,
							SendEditor(SCI_GETCURRENTPOS)) < indentBlock)
				SendEditor (SCI_VCHOME);
		}
	}
}
#endif
