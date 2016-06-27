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

#include "MasterVolumeControl.h"

#include <cassert>
#include <iostream>

MasterVolumeControl::MasterVolumeControl()
{
	if (this->amdInitialize())
	{
	//	std::cout << "VolumeControl Number Of Mixers : " << m_nNumMixers << std::endl;

		if (!this->amdGetMasterVolumeControl())
		{
			std::cout << "VolumeControl Error : Unable to get master volume control" << std::endl;
		}
	}

	volume_step = DWORD(double(m_dwMaximum - m_dwMinimum) * 0.01);
}

MasterVolumeControl::~MasterVolumeControl()
{

}

void MasterVolumeControl::operator++(int)
{
	DWORD volume;
	amdGetMasterVolumeValue(volume);

	if ( (m_dwMaximum - volume) < volume_step)
	{
		volume = m_dwMaximum;
	}
	else
	{
		volume += volume_step;
	}

	amdSetMasterVolumeValue(volume);
}

void MasterVolumeControl::operator--(int)
{
	DWORD volume;
	amdGetMasterVolumeValue(volume);

	if (volume < volume_step)
	{
		volume = m_dwMinimum;
	}
	else
	{
		volume -= volume_step;
	}

	amdSetMasterVolumeValue(volume);
}

BOOL MasterVolumeControl::amdInitialize()
{
	assert(m_hMixer == NULL);

	// get the number of mixer devices present in the system
	m_nNumMixers = ::mixerGetNumDevs();

	m_hMixer = NULL;
	::ZeroMemory(&m_mxcaps, sizeof(MIXERCAPS));

	m_strDstLineName = "";
	m_strVolumeControlName = "";
	m_dwMinimum = 0;
	m_dwMaximum = 0;
	m_dwVolumeControlID = 0;

	// open the first mixer
	// A "mapper" for audio mixer devices does not currently exist.
	if (m_nNumMixers != 0)
	{
		if (::mixerOpen(&m_hMixer,
						0,
						//reinterpret_cast<DWORD>(this->GetSafeHwnd()),
						NULL,
						NULL,
						MIXER_OBJECTF_MIXER | CALLBACK_WINDOW)
			!= MMSYSERR_NOERROR)
		{
			return FALSE;
		}

		if (::mixerGetDevCaps(reinterpret_cast<UINT>(m_hMixer),
							  &m_mxcaps, sizeof(MIXERCAPS))
			!= MMSYSERR_NOERROR)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL MasterVolumeControl::amdUninitialize()
{
	BOOL bSucc = TRUE;

	if (m_hMixer != NULL)
	{
		bSucc = (::mixerClose(m_hMixer) == MMSYSERR_NOERROR);
		m_hMixer = NULL;
	}

	return bSucc;
}

BOOL MasterVolumeControl::amdGetMasterVolumeControl()
{
	if (m_hMixer == NULL)
	{
		return FALSE;
	}

	// get dwLineID
	MIXERLINE mxl;
	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	if (::mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(m_hMixer),
						   &mxl,
						   MIXER_OBJECTF_HMIXER |
						   MIXER_GETLINEINFOF_COMPONENTTYPE)
		!= MMSYSERR_NOERROR)
	{
		return FALSE;
	}

	// get dwControlID
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;
	if (::mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(m_hMixer),
							   &mxlc,
							   MIXER_OBJECTF_HMIXER |
							   MIXER_GETLINECONTROLSF_ONEBYTYPE)
		!= MMSYSERR_NOERROR)
	{
		return FALSE;
	}

	// store dwControlID
	m_strDstLineName = mxl.szName;
	m_strVolumeControlName = mxc.szName;
	m_dwMinimum = mxc.Bounds.dwMinimum;
	m_dwMaximum = mxc.Bounds.dwMaximum;
	m_dwVolumeControlID = mxc.dwControlID;

	return TRUE;
}

BOOL MasterVolumeControl::amdGetMasterVolumeValue(DWORD &dwVal) const
{
	if (m_hMixer == NULL)
	{
		return FALSE;
	}

	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = m_dwVolumeControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;
	if (::mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer),
								 &mxcd,
								 MIXER_OBJECTF_HMIXER |
								 MIXER_GETCONTROLDETAILSF_VALUE)
		!= MMSYSERR_NOERROR)
	{
		return FALSE;
	}
	
	dwVal = mxcdVolume.dwValue;

	return TRUE;
}

BOOL MasterVolumeControl::amdSetMasterVolumeValue(DWORD dwVal) const
{
	if (m_hMixer == NULL)
	{
		return FALSE;
	}

	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume = { dwVal };
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = m_dwVolumeControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;
	if (::mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(m_hMixer),
								 &mxcd,
								 MIXER_OBJECTF_HMIXER |
								 MIXER_SETCONTROLDETAILSF_VALUE)
		!= MMSYSERR_NOERROR)
	{
		return FALSE;
	}
	
	return TRUE;
}
