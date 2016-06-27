/*
    Smokin::MasterVolumeControl - Win32 Master Volume Control.
    Copyright (C) 2007  Rui Varela - rui.filipe.varela@gmail.com

	This file is part of Smokin::MasterVolumeControl.

    Smokin::MasterVolumeControl is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <windows.h>
#include <mmsystem.h>
#include <string>

class MasterVolumeControl
{
public:
	MasterVolumeControl();
	~MasterVolumeControl();

	void setVolumeStep(DWORD increment) { volume_step = increment; }

	void operator++(int);			 //postfix
	void operator++() { (*this)++; } //prefix
	void operator--(int);
	void operator--() { (*this)--; }

	BOOL amdUninitialize();
	BOOL amdInitialize();
	BOOL amdGetMasterVolumeControl();
	BOOL amdGetMasterVolumeValue(DWORD &dwVal) const;
	BOOL amdSetMasterVolumeValue(DWORD dwVal) const;
private:

	UINT m_nNumMixers;
	HMIXER m_hMixer;
	MIXERCAPS m_mxcaps;

	std::string m_strDstLineName, m_strVolumeControlName;
	DWORD m_dwMinimum, m_dwMaximum;
	DWORD m_dwVolumeControlID;

	DWORD volume_step;

};
