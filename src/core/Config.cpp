/*
 ===========================================================================
 ARX FATALIS GPL Source Code
 Copyright (C) 1999-2010 Arkane Studios SA, a ZeniMax Media company.

 This file is part of the Arx Fatalis GPL Source Code ('Arx Fatalis Source Code').

 Arx Fatalis Source Code is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

 Arx Fatalis Source Code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with Arx Fatalis Source Code.  If not, see
 <http://www.gnu.org/licenses/>.

 In addition, the Arx Fatalis Source Code is also subject to certain additional terms. You should have received a copy of these
 additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Arx
 Fatalis Source Code. If not, please request a copy in writing from Arkane Studios at the address below.

 If you have questions concerning this license or the applicable additional terms, you may contact in writing Arkane Studios, c/o
 ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.
 ===========================================================================
 */

#include "core/Config.h"

#include <cstdio>

#include <fstream>
#include <sstream>

#include "core/Localization.h"
#include "graphics/data/Texture.h"
#include "gui/MenuPublic.h"
#include "gui/MenuWidgets.h"
#include "io/Logger.h"
#include "io/String.h"
#include "scene/GameSound.h"
#include "window/Input.h"

// To avoid conflicts with potential other classes/namespaces
namespace
{

	/* Default values for config */
	namespace Default
	{
		std::string
			language = "english",
			resolution = "640x480",
			modpak = "mod.pak",
			modsfxpak = "modsfx.pak",
			speechmodpak = "modspeech.pak",
			empty_string = "";

		int
			bpp = 16,
			texture = 2,
			mesh_reduction = 0,
			others_details = 2,
			fog = 5,
			gamma = 5,
			luminosity = 4,
			contrast = 5,
			master_volume = 10,
			effects_volume = 10,
			speech_volume = 10,
			ambiance_volume = 10,
			mouse_sensitivity = 4;

		bool
			first_run = true,
			full_screen = true,
			bump = false,
			show_crosshair = true,
			antialiasing = false,
			EAX = false,
			invert_mouse = false,
			auto_ready_weapon = false,
			mouse_look_toggle = false,
			mouse_smoothing = false,
			auto_description = true,
			link_mouse_look_to_use = false,
			softfog = false,
			forcenoeax = false,
			forcezbias = false,
			forcetoggle = false,
			fg = true,
			new_control = false;
	};

	namespace Section
	{
		std::string
			Language = "LANGUAGE",
			FirstRun = "FIRSTRUN",
			Video = "VIDEO",
			Audio = "AUDIO",
			Input = "INPUT",
			Key = "KEY",
			Misc = "MISC";
	};

	namespace Key
	{
		std::string
			// Language options
			language_string = "string",
			// First run options
			first_run_int = "int",
			// Video options
			resolution = "resolution",
			bpp = "bpp",
			full_screen = "full_screen",
			bump = "bump",
			texture = "texture",
			mesh_reduction = "mesh_reduction",
			others_details = "others_details",
			fog = "fog",
			gamma = "gamma",
			luminosity = "luminosity",
			contrast = "contrast",
			show_crosshair = "show_crosshair",
			antialiasing = "antialiasing",
			// Audio options
			master_volume = "master_volume",
			effects_volume = "effects_volume",
			speech_volume = "speech_volume",
			ambiance_volume = "ambiance_volume",
			EAX = "EAX",
			// Input options
			invert_mouse = "invert_mouse",
			auto_ready_weapon = "auto_ready_weapon",
			mouse_look_toggle = "mouse_look_toggle",
			mouse_sensitivity = "mouse_sensitivity",
			mouse_smoothing = "mouse_smoothing",
			auto_description = "auto_description",
			link_mouse_look_to_use = "link_mouse_look_to_use",
			// Input key options
			jump = "jump",
			magic_mode = "magic_mode",

			// Misc options
			softfog = "softfog",
			forcenoeax = "forcenoeax",
			forcezbias = "forcezbias",
			forcetoggle = "forcetoggle",
			fg = "fg",
			modpak = "mod",
			modsfxpak = "modsfx",
			modspeechpak = "modspeech",
			newcontrol = "newcontrol"
			;
	};

	ActionKey sakActionDefaultKey[MAX_ACTION_KEY] =
	{
	
	};
};

void ARX_SetAntiAliasing();

	/* Externs */
extern long INTERNATIONAL_MODE;
extern CDirectInput * pGetInfoDirectInput;
extern long GORE_MODE;
extern long GERMAN_VERSION;
extern long FRENCH_VERSION;
extern bool bForceNoEAX;
extern CMenuState *pMenu;
extern bool bALLOW_BUMP;
extern long WILL_RELOAD_ALL_TEXTURES;

/* Global variables */
std::string pStringMod;
std::string pStringModSfx;
std::string pStringModSpeech;
Config *pMenuConfig = 0;

Config::Config()
{
	First();
}

Config::Config( const std::string& _filename)
{
	// if _pName equals exactly "cfg"
	if ( _filename == "cfg" )
		pcName = "cfg.ini";
	else
		pcName = _filename;

	// Load the config file
	std::ifstream ifs( pcName.c_str() );
	config_map = ConfigHashMap( 100, ifs );

	First();
}

Config::~Config()
{

}

void Config::First()
{
	sakActionDefaultKey[CONTROLS_CUST_JUMP].iKey[0]=DIK_SPACE;
	sakActionDefaultKey[CONTROLS_CUST_JUMP].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_MAGICMODE].iKey[0]=DIK_LCONTROL;
	sakActionDefaultKey[CONTROLS_CUST_MAGICMODE].iKey[1]=DIK_RCONTROL;
	sakActionDefaultKey[CONTROLS_CUST_STEALTHMODE].iKey[0]=DIK_LSHIFT;
	sakActionDefaultKey[CONTROLS_CUST_STEALTHMODE].iKey[1]=DIK_RSHIFT;
	sakActionDefaultKey[CONTROLS_CUST_WALKFORWARD].iKey[0] = DIK_W;
	sakActionDefaultKey[CONTROLS_CUST_WALKFORWARD].iKey[1]=DIK_UP;
	sakActionDefaultKey[CONTROLS_CUST_WALKBACKWARD].iKey[0] = DIK_S;
	sakActionDefaultKey[CONTROLS_CUST_WALKBACKWARD].iKey[1]=DIK_DOWN;
	sakActionDefaultKey[CONTROLS_CUST_STRAFELEFT].iKey[0] = DIK_A;
	sakActionDefaultKey[CONTROLS_CUST_STRAFELEFT].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_STRAFERIGHT].iKey[0] = DIK_D;
	sakActionDefaultKey[CONTROLS_CUST_STRAFERIGHT].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_LEANLEFT].iKey[0] = DIK_Q;
	sakActionDefaultKey[CONTROLS_CUST_LEANLEFT].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_LEANRIGHT].iKey[0] = DIK_E;
	sakActionDefaultKey[CONTROLS_CUST_LEANRIGHT].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_CROUCH].iKey[0] = DIK_X;
	sakActionDefaultKey[CONTROLS_CUST_CROUCH].iKey[1]=-1;

	sakActionDefaultKey[CONTROLS_CUST_MOUSELOOK].iKey[0]=DIK_F;
	sakActionDefaultKey[CONTROLS_CUST_MOUSELOOK].iKey[1]=DIK_RETURN;

	sakActionDefaultKey[CONTROLS_CUST_ACTION].iKey[0]=DIK_BUTTON1;
	sakActionDefaultKey[CONTROLS_CUST_ACTION].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_INVENTORY].iKey[0] = DIK_I;
	sakActionDefaultKey[CONTROLS_CUST_INVENTORY].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_INVENTORY].iPage=1;
	sakActionDefaultKey[CONTROLS_CUST_BOOK].iKey[0]=DIK_BACKSPACE;
	sakActionDefaultKey[CONTROLS_CUST_BOOK].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_BOOK].iPage=1;
	sakActionDefaultKey[CONTROLS_CUST_BOOKCHARSHEET].iKey[0] = DIK_F1;
	sakActionDefaultKey[CONTROLS_CUST_BOOKCHARSHEET].iPage=1;
	sakActionDefaultKey[CONTROLS_CUST_BOOKSPELL].iKey[0] = DIK_F2;
	sakActionDefaultKey[CONTROLS_CUST_BOOKSPELL].iPage=1;
	sakActionDefaultKey[CONTROLS_CUST_BOOKMAP].iKey[0] = DIK_F3;
	sakActionDefaultKey[CONTROLS_CUST_BOOKMAP].iPage=1;
	sakActionDefaultKey[CONTROLS_CUST_BOOKQUEST].iKey[0] = DIK_F4;
	sakActionDefaultKey[CONTROLS_CUST_BOOKQUEST].iPage=1;
	sakActionDefaultKey[CONTROLS_CUST_DRINKPOTIONLIFE].iKey[0] = DIK_H;
	sakActionDefaultKey[CONTROLS_CUST_DRINKPOTIONLIFE].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_DRINKPOTIONLIFE].iPage=1;
	sakActionDefaultKey[CONTROLS_CUST_DRINKPOTIONMANA].iKey[0] = DIK_G;
	sakActionDefaultKey[CONTROLS_CUST_DRINKPOTIONMANA].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_DRINKPOTIONMANA].iPage=1;
	sakActionDefaultKey[CONTROLS_CUST_TORCH].iKey[0]=DIK_T;
	sakActionDefaultKey[CONTROLS_CUST_TORCH].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_TORCH].iPage=1;

	sakActionDefaultKey[CONTROLS_CUST_CANCELCURSPELL].iKey[0] = DIK_4;
	sakActionDefaultKey[CONTROLS_CUST_CANCELCURSPELL].iKey[1] = -1;
	sakActionDefaultKey[CONTROLS_CUST_CANCELCURSPELL].iPage=1;

	sakActionDefaultKey[CONTROLS_CUST_PRECAST1].iKey[0] = DIK_1;
	sakActionDefaultKey[CONTROLS_CUST_PRECAST1].iPage=1;
	sakActionDefaultKey[CONTROLS_CUST_PRECAST2].iKey[0] = DIK_2;
	sakActionDefaultKey[CONTROLS_CUST_PRECAST2].iPage=1;
	sakActionDefaultKey[CONTROLS_CUST_PRECAST3].iKey[0] = DIK_3;
	sakActionDefaultKey[CONTROLS_CUST_PRECAST3].iPage=1;
	sakActionDefaultKey[CONTROLS_CUST_WEAPON].iKey[0] = DIK_TAB;
	sakActionDefaultKey[CONTROLS_CUST_WEAPON].iKey[1] = DIK_NUMPAD0;
	sakActionDefaultKey[CONTROLS_CUST_WEAPON].iPage=1;
	sakActionDefaultKey[CONTROLS_CUST_QUICKLOAD].iKey[0] = DIK_F9;
	sakActionDefaultKey[CONTROLS_CUST_QUICKLOAD].iPage=1;
	sakActionDefaultKey[CONTROLS_CUST_QUICKSAVE].iKey[0] = DIK_F5;
	sakActionDefaultKey[CONTROLS_CUST_QUICKSAVE].iPage=1;

	sakActionDefaultKey[CONTROLS_CUST_TURNLEFT].iKey[0]=DIK_LEFT;
	sakActionDefaultKey[CONTROLS_CUST_TURNLEFT].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_TURNRIGHT].iKey[0]=DIK_RIGHT;
	sakActionDefaultKey[CONTROLS_CUST_TURNRIGHT].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_LOOKUP].iKey[0]=DIK_PGUP;
	sakActionDefaultKey[CONTROLS_CUST_LOOKUP].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_LOOKDOWN].iKey[0]=DIK_PGDN;
	sakActionDefaultKey[CONTROLS_CUST_LOOKDOWN].iKey[1]=-1;

	sakActionDefaultKey[CONTROLS_CUST_STRAFE].iKey[0]=DIK_LALT;
	sakActionDefaultKey[CONTROLS_CUST_STRAFE].iKey[1]=-1;
	sakActionDefaultKey[CONTROLS_CUST_CENTERVIEW].iKey[0]=DIK_END;
	sakActionDefaultKey[CONTROLS_CUST_CENTERVIEW].iKey[1]=-1;

	sakActionDefaultKey[CONTROLS_CUST_FREELOOK].iKey[0]=DIK_L;
	sakActionDefaultKey[CONTROLS_CUST_FREELOOK].iKey[1]=DIK_BUTTON2;

	sakActionDefaultKey[CONTROLS_CUST_PREVIOUS].iKey[0] = GetDIKWithASCII(")");
	sakActionDefaultKey[CONTROLS_CUST_PREVIOUS].iPage=1;
	sakActionDefaultKey[CONTROLS_CUST_NEXT].iKey[0] = GetDIKWithASCII("=");
	sakActionDefaultKey[CONTROLS_CUST_NEXT].iPage=1;

	sakActionDefaultKey[CONTROLS_CUST_CROUCHTOGGLE].iKey[0] = GetDIKWithASCII("C");

	sakActionDefaultKey[CONTROLS_CUST_UNEQUIPWEAPON].iKey[0] = DIK_B;
	sakActionDefaultKey[CONTROLS_CUST_UNEQUIPWEAPON].iKey[1] = -1;
	sakActionDefaultKey[CONTROLS_CUST_UNEQUIPWEAPON].iPage=1;

	sakActionDefaultKey[CONTROLS_CUST_MINIMAP].iKey[0] = DIK_R;
	sakActionDefaultKey[CONTROLS_CUST_MINIMAP].iKey[1] = DIK_M;
	sakActionDefaultKey[CONTROLS_CUST_MINIMAP].iPage=1;

	bChangeResolution = false;
	bChangeTextures = false;
	bNoReturnToWindows=false;
	bLinkMouseLookToUse=false;
	bForceZBias=false;

	SetDefaultKey();
}

void Config::SetDefaultKey()
{
	int iI=MAX_ACTION_KEY;

	while(iI--)
	{
		sakActionKey[iI].iKey[0]=sakActionDefaultKey[iI].iKey[0];
		sakActionKey[iI].iKey[1]=sakActionDefaultKey[iI].iKey[1];
		sakActionKey[iI].iPage=sakActionDefaultKey[iI].iPage;
	}

	if (!INTERNATIONAL_MODE)
		bLinkMouseLookToUse=true;
	else
		bLinkMouseLookToUse=false;
}

int Config::GetDIKWithASCII( const std::string& _pcTouch)
{
	std::string pcT = _pcTouch;
	

	if( strcasecmp(pcT.c_str(), "---"  ) == 0 )
	{
		return -1;
	}

	for(int iI=0;iI<256;iI++)
	{
		std::string pcT1 = pGetInfoDirectInput->GetFullNameTouch(iI);

		if( !pcT.compare( pcT1 ) )
			return iI;

		pcT1 = pGetInfoDirectInput->GetFullNameTouch(iI | (DIK_LSHIFT << 16));

		if( !pcT.compare( pcT1 ) )
			return iI|(DIK_LSHIFT<<16);

		pcT1 = pGetInfoDirectInput->GetFullNameTouch(iI|(DIK_RSHIFT<<16));

		if( !pcT.compare( pcT1 ) )
			return iI|(DIK_RSHIFT<<16);

		pcT1 = pGetInfoDirectInput->GetFullNameTouch(iI|(DIK_LCONTROL<<16));

		if( !pcT.compare( pcT1 ) )
			return iI|(DIK_LCONTROL<<16);

		pcT1 = pGetInfoDirectInput->GetFullNameTouch(iI|(DIK_RCONTROL<<16));

		if( !pcT.compare( pcT1 ) )
			return iI|(DIK_RCONTROL<<16);

		pcT1 = pGetInfoDirectInput->GetFullNameTouch(iI|(DIK_LALT<<16));

		if( ! pcT.compare( pcT1 ) )
			return iI|(DIK_LALT<<16);

		pcT1 = pGetInfoDirectInput->GetFullNameTouch(iI|(DIK_RALT<<16));

		if( !pcT.compare( pcT1 ) )
			return iI|(DIK_RALT<<16);
	}

	for(int iI=DIK_BUTTON1;iI<=(int)DIK_BUTTON32;iI++)
	{
		std::string pcT1 = pGetInfoDirectInput->GetFullNameTouch(iI);

		if( !pcT.compare( pcT1 ) )
			return iI;
	}

	for(int iI=DIK_WHEELUP;iI<=DIK_WHEELDOWN;iI++)
	{
		std::string pcT1 = pGetInfoDirectInput->GetFullNameTouch(iI);

		if( !pcT.compare( pcT1 ) )
			return iI;
	}

	return -1;
}

//-----------------------------------------------------------------------------
std::string Config::ReadConfig( const std::string& _section, const std::string& _key) const
{
	
	LogDebug << "Read section: " << _section << " key: " << _key << " from " << pcName << " as:" << config_map.getConfigValue( _section, "", _key );
	return config_map.getConfigValue( _section, "", _key );
}

//-----------------------------------------------------------------------------

int Config::ReadConfigInt( const std::string& _pcSection, const std::string& _pcKey, bool &_bOk )
{
	std::string pcText=ReadConfig(_pcSection,_pcKey);

	if ( pcText.empty() )
	{
		_bOk = false;
		return 0;
	}

	std::stringstream ss( pcText );

	int iI;
	ss >> iI;
	_bOk=true;
	return iI;
}

/**
 * Reads a string from the Config and returns it, returning the default value if an empty string was found
 * @param section The section to read from
 * @param key The key in the section to return
 * @param default_value The default value to be returned in the case of an empty string
 */
std::string Config::ReadConfig( const std::string& section, const std::string& key, const std::string& default_value ) const
{
	std::string temp( ReadConfig( section, key ) );

	if ( temp.empty() )
		return default_value;
	else
		return temp;
}

/**
 * Reads a string from the Config and returns its converted int value,
 * return the default value if an empty string is found.
 * @param section The section to read from
 * @param key The key in the section to return
 * @param default_value The default value to return in the case of an empty string
 */
int Config::ReadConfig( const std::string& section, const std::string& key, int default_value ) const
{
	std::string temp( ReadConfig( section, key ) );

	if ( temp.empty() )
		return default_value;
	else
		return atoi( temp );
}

//-----------------------------------------------------------------------------

void Config::WriteConfig( const std::string& section, const std::string& key, const std::string& value )
{
	config_map.updateConfigValue( section, key, value );
}

void Config::WriteConfig( const std::string& section, const std::string& key, int value )
{
	WriteConfig( section, key, itoa( value ) );
}

//-----------------------------------------------------------------------------

bool Config::WriteConfigInt( const std::string& _pcSection, const std::string& _pcKey, int data )
{
	WriteConfig(_pcSection,_pcKey, itoa( data ) );
	return false;
}

//-----------------------------------------------------------------------------

bool Config::WriteConfigString( const std::string& _pcSection, const std::string& _pcKey, const std::string& _pcDatas)
{
	WriteConfig(_pcSection,_pcKey,_pcDatas);
	return false;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

bool Config::SetActionKey(int _iAction,int _iActionNum,int _iVirtualKey)
{
	if(    (_iAction>=MAX_ACTION_KEY)||
		(_iActionNum>1) ) return false;

	bool bChange=false;
	bool bSecondChoice=false;
	int iOldVirtualKey=sakActionKey[_iAction].iKey[_iActionNum];
	sakActionKey[_iAction].iKey[_iActionNum]=_iVirtualKey;

	if(_iActionNum)
	{
		if(sakActionKey[_iAction].iKey[0]==-1)
		{
			sakActionKey[_iAction].iKey[0]=iOldVirtualKey;
			bSecondChoice=true;
		}

		if(sakActionKey[_iAction].iKey[0]==_iVirtualKey)
		{
			sakActionKey[_iAction].iKey[0]=-1;
		}

		bChange=true;
	}
	else
	{
		if(sakActionKey[_iAction].iKey[1]==-1)
		{
			sakActionKey[_iAction].iKey[1]=iOldVirtualKey;
			bSecondChoice=true;
		}

		if(sakActionKey[_iAction].iKey[1]==_iVirtualKey)
		{
			sakActionKey[_iAction].iKey[1]=-1;
		}
		bChange=true;
	}

	if(bSecondChoice)
	{
		bChange=true;
		iOldVirtualKey=-1;
	}

	//on remove les doublons de keys
	int iI=MAX_ACTION_KEY;

	while(iI--)
	{
		if(iI==_iAction) continue;

		if(sakActionKey[iI].iPage!=sakActionKey[_iAction].iPage) continue;

		if(sakActionKey[iI].iKey[0]==_iVirtualKey)
		{
			sakActionKey[iI].iKey[0]=iOldVirtualKey;
			bChange=true;
			break;
		}
		else
		{
			if(sakActionKey[iI].iKey[1]==_iVirtualKey)
			{
				sakActionKey[iI].iKey[1]=iOldVirtualKey;
				bChange=true;
				break;
			}
		}
	}

	return bChange;
}

//-----------------------------------------------------------------------------

bool Config::WriteConfigKey( const std::string& _pcKey,int _iAction)
{
	char tcTxt[256];
	char tcTxt2[256];
	std::string pcText;
	bool bOk=true;
	std::string pcText1;

	strcpy(tcTxt,_pcKey.c_str());

	int iL;
	pcText1 = pGetInfoDirectInput->GetFullNameTouch(sakActionKey[_iAction].iKey[0]);
	iL = pcText1.length() + 1;

	pcText = pcText1;

	if( !pcText.empty() )
	{
		strcpy(tcTxt2,tcTxt);
		strcat(tcTxt2,"_k0");
		bOk&=WriteConfigString("KEY",tcTxt2,pcText);
	}

	pcText1 = pGetInfoDirectInput->GetFullNameTouch(sakActionKey[_iAction].iKey[1]);
	iL = pcText1.length() + 1;
		
	pcText = pcText1;

	if( !pcText.empty() )
	{
		strcpy(tcTxt2,tcTxt);
		strcat(tcTxt2,"_k1");
		bOk&=WriteConfigString("KEY",tcTxt2,pcText);
	}

	return bOk;
}

bool Config::ReadConfigKey( const std::string& _pcKey, int _iAction )
{
	char tcTxt[256];
	char tcTxt2[256];
	std::string pcText;
	bool bOk=true;
	strcpy(tcTxt, _pcKey.c_str());


	int iDIK;
	strcpy(tcTxt2,tcTxt);
	strcat(tcTxt2,"_k0");
	pcText = ReadConfig( "KEY", tcTxt2 );

	if( pcText.empty() )
		bOk=false;
	else
	{
		iDIK = GetDIKWithASCII( pcText );

		if( iDIK == -1 )
			sakActionKey[_iAction].iKey[0]=-1;
		else
			SetActionKey( _iAction, 0, iDIK) ;
	}

	strcpy( tcTxt2, tcTxt );
	strcat( tcTxt2, "_k1" );
	pcText = ReadConfig( "KEY", tcTxt2 );

	if( pcText.empty() )
		bOk = false;
	else
	{
		iDIK = GetDIKWithASCII( pcText );

		if( iDIK == -1 )
		{
			sakActionKey[_iAction].iKey[1]=-1;
		}
		else
		{
			SetActionKey( _iAction, 1, iDIK );
		}
	}

	return bOk;
}

//-----------------------------------------------------------------------------

void Config::SaveAll()
{
	std::ofstream out( pcName.c_str() );
	config_map.save_all( out );
	char tcTxt[256];

	//language
	strcpy(tcTxt,"\"");
	strcat(tcTxt,Project.localisationpath.c_str());
	strcat(tcTxt,"\"");
	WriteConfig( Section::Language, Key::language_string, Project.localisationpath );
	WriteConfig( Section::FirstRun, Key::first_run_int, first_launch?1:0);
	//video
	sprintf(tcTxt,"%dx%d",iWidth,iHeight);
	WriteConfig( Section::Video, Key::resolution, std::string(tcTxt) );
	WriteConfig( Section::Video, Key::bpp, bpp);
	WriteConfig( Section::Video, Key::full_screen, (bFullScreen)?1:0);
	WriteConfig( Section::Video, Key::bump, (bBumpMapping)?1:0);
	WriteConfig( Section::Video, Key::texture, iTextureResol);
	WriteConfig( Section::Video, Key::mesh_reduction, iMeshReduction);
	WriteConfig( Section::Video, Key::others_details, iLevelOfDetails);
	WriteConfig( Section::Video, Key::fog, iFogDistance);
	WriteConfig( Section::Video, Key::gamma, iGamma);
	WriteConfig( Section::Video, Key::luminosity, iLuminosite);
	WriteConfig( Section::Video, Key::contrast, iContrast);
	WriteConfig( Section::Video, Key::show_crosshair, bShowCrossHair?1:0);
	WriteConfig( Section::Video, Key::antialiasing, bAntiAliasing?1:0);
	//audio
	WriteConfig( Section::Audio, Key::master_volume, iMasterVolume );
	WriteConfig( Section::Audio, Key::effects_volume, iSFXVolume);
	WriteConfig( Section::Audio, Key::speech_volume, iSpeechVolume);
	WriteConfig( Section::Audio, Key::ambiance_volume, iAmbianceVolume);
	WriteConfig( Section::Audio, Key::EAX, (bEAX)?1:0);
	//input
	WriteConfig( Section::Input, Key::invert_mouse, (bInvertMouse)?1:0);
	WriteConfig( Section::Input, Key::auto_ready_weapon, (bAutoReadyWeapon)?1:0);
	WriteConfig( Section::Input, Key::mouse_look_toggle, (bMouseLookToggle)?1:0);
	WriteConfig( Section::Input, Key::mouse_sensitivity, iMouseSensitivity);
	WriteConfig( Section::Input, Key::mouse_smoothing, (bMouseSmoothing)?1:0);
	WriteConfig( Section::Input, Key::auto_description, (bAutoDescription)?1:0);
	//key
	WriteConfigKey("jump",CONTROLS_CUST_JUMP);
	WriteConfigKey("magic_mode",CONTROLS_CUST_MAGICMODE);
	WriteConfigKey("stealth_mode",CONTROLS_CUST_STEALTHMODE);
	WriteConfigKey("walk_forward",CONTROLS_CUST_WALKFORWARD);
	WriteConfigKey("walk_backward",CONTROLS_CUST_WALKBACKWARD);
	WriteConfigKey("strafe_left",CONTROLS_CUST_STRAFELEFT);
	WriteConfigKey("strafe_right",CONTROLS_CUST_STRAFERIGHT);
	WriteConfigKey("lean_left",CONTROLS_CUST_LEANLEFT);
	WriteConfigKey("lean_right",CONTROLS_CUST_LEANRIGHT);
	WriteConfigKey("crouch",CONTROLS_CUST_CROUCH);
	WriteConfigKey("mouselook",CONTROLS_CUST_MOUSELOOK);
	WriteConfigInt("INPUT","link_mouse_look_to_use",(bLinkMouseLookToUse)?1:0);
	WriteConfigKey("action_combine",CONTROLS_CUST_ACTION);
	WriteConfigKey("inventory",CONTROLS_CUST_INVENTORY);
	WriteConfigKey("book",CONTROLS_CUST_BOOK);
	WriteConfigKey("char_sheet",CONTROLS_CUST_BOOKCHARSHEET);
	WriteConfigKey("magic_book",CONTROLS_CUST_BOOKSPELL);
	WriteConfigKey("map",CONTROLS_CUST_BOOKMAP);
	WriteConfigKey("quest_book",CONTROLS_CUST_BOOKQUEST);
	WriteConfigKey("drink_potion_life",CONTROLS_CUST_DRINKPOTIONLIFE);
	WriteConfigKey("drink_potion_mana",CONTROLS_CUST_DRINKPOTIONMANA);
	WriteConfigKey("torch",CONTROLS_CUST_TORCH);

	WriteConfigKey("cancel_current_spell",CONTROLS_CUST_CANCELCURSPELL);
	WriteConfigKey("precast_1",CONTROLS_CUST_PRECAST1);
	WriteConfigKey("precast_2",CONTROLS_CUST_PRECAST2);
	WriteConfigKey("precast_3",CONTROLS_CUST_PRECAST3);
	WriteConfigKey("draw_weapon",CONTROLS_CUST_WEAPON);
	WriteConfigKey("quicksave",CONTROLS_CUST_QUICKSAVE);
	WriteConfigKey("quickload",CONTROLS_CUST_QUICKLOAD);

	WriteConfigKey("turn_left",CONTROLS_CUST_TURNLEFT);
	WriteConfigKey("turn_right",CONTROLS_CUST_TURNRIGHT);
	WriteConfigKey("look_up",CONTROLS_CUST_LOOKUP);
	WriteConfigKey("look_down",CONTROLS_CUST_LOOKDOWN);

	WriteConfigKey("strafe",CONTROLS_CUST_STRAFE);
	WriteConfigKey("center_view",CONTROLS_CUST_CENTERVIEW);

	WriteConfigKey("freelook",CONTROLS_CUST_FREELOOK);

	WriteConfigKey("previous",CONTROLS_CUST_PREVIOUS);
	WriteConfigKey("next",CONTROLS_CUST_NEXT);

	WriteConfigKey("crouch_toggle",CONTROLS_CUST_CROUCHTOGGLE);

	WriteConfigKey("unequip_weapon",CONTROLS_CUST_UNEQUIPWEAPON);

	WriteConfigKey("minimap",CONTROLS_CUST_MINIMAP);

	//misc
	WriteConfig( Section::Misc, Key::softfog,(bATI)?1:0);
	WriteConfig( Section::Misc, Key::forcenoeax,(bForceNoEAX)?1:0);
	WriteConfig( Section::Misc, Key::forcezbias,(bForceZBias)?1:0);
	WriteConfig( Section::Misc, Key::newcontrol,(INTERNATIONAL_MODE)?1:0);
	WriteConfig( Section::Misc, Key::forcetoggle,(bOneHanded)?1:0);
	WriteConfig( Section::Misc, Key::fg,(uiGoreMode)?1:0);
}

extern bool IsNoGore( void );

void Config::ReadAll()
{
	bool bWarningGore=false;

	// Check if this is the first run of the game
	first_launch = ReadConfig( Section::FirstRun, Key::first_run_int, 1 );

	// Get the locale language
	Project.localisationpath = ReadConfig( Section::Language, Key::language_string, Default::language );

	// Get the video settings
	std::string resolution = ReadConfig( Section::Video, Key::resolution, Default::resolution );
	iWidth = atoi( resolution.substr( 0, resolution.find( 'x' ) ) );
	iHeight = atoi( resolution.substr( resolution.find('x') + 1 ) );
	bpp = ReadConfig( Section::Video, Key::bpp, Default::bpp );
	bFullScreen = ReadConfig( Section::Video, Key::full_screen, 1 );
	bBumpMapping = ReadConfig( Section::Video, Key::bump, 0 );
	iTextureResol = ReadConfig( Section::Video, Key::texture, Default::texture );
	iMeshReduction = ReadConfig( Section::Video, Key::mesh_reduction, Default::mesh_reduction );
	iLevelOfDetails = ReadConfig( Section::Video, Key::others_details, Default::others_details );
	iFogDistance = ReadConfig( Section::Video, Key::fog, Default::fog );
	iGamma = ReadConfig( Section::Video, Key::gamma, Default::gamma );
	iLuminosite = ReadConfig( Section::Video, Key::luminosity, Default::luminosity );
	iContrast = ReadConfig( Section::Video, Key::contrast, Default::contrast );
	bShowCrossHair = ReadConfig( Section::Video, Key::show_crosshair, 1 );
	bAntiAliasing = ReadConfig( Section::Video, Key::antialiasing, 0 );

	// Get audio settings
	iMasterVolume = ReadConfig( Section::Audio, Key::master_volume, Default::master_volume );
	iSFXVolume = ReadConfig( Section::Audio, Key::effects_volume, Default::effects_volume );
	iSpeechVolume = ReadConfig( Section::Audio, Key::speech_volume, Default::speech_volume );
	iAmbianceVolume = ReadConfig( Section::Audio, Key::ambiance_volume, Default::ambiance_volume );
	bEAX = ReadConfig( Section::Audio, Key::EAX, 0 );

	// Get input settings
	bInvertMouse = ReadConfig( Section::Input, Key::invert_mouse, 0 );
	bAutoReadyWeapon = ReadConfig( Section::Input, Key::auto_ready_weapon, 0 );
	bMouseLookToggle = ReadConfig( Section::Input, Key::mouse_look_toggle, 0 );
	iMouseSensitivity = ReadConfig( Section::Input, Key::mouse_sensitivity, Default::mouse_sensitivity );
	bMouseSmoothing = ReadConfig( Section::Input, Key::mouse_smoothing, 0 );
	bAutoDescription = ReadConfig( Section::Input, Key::auto_description, 1 );
	bLinkMouseLookToUse = ReadConfig( Section::Input, Key::link_mouse_look_to_use, 0 );

	//key
	ReadConfigKey("jump",CONTROLS_CUST_JUMP);
	ReadConfigKey("magic_mode",CONTROLS_CUST_MAGICMODE);
	ReadConfigKey("stealth_mode",CONTROLS_CUST_STEALTHMODE);
	ReadConfigKey("walk_forward",CONTROLS_CUST_WALKFORWARD);
	ReadConfigKey("walk_backward",CONTROLS_CUST_WALKBACKWARD);
	ReadConfigKey("strafe_left",CONTROLS_CUST_STRAFELEFT);
	ReadConfigKey("strafe_right",CONTROLS_CUST_STRAFERIGHT);
	ReadConfigKey("lean_left",CONTROLS_CUST_LEANLEFT);
	ReadConfigKey("lean_right",CONTROLS_CUST_LEANRIGHT);
	ReadConfigKey("crouch",CONTROLS_CUST_CROUCH);
	ReadConfigKey("mouselook",CONTROLS_CUST_MOUSELOOK);


	ReadConfigKey("action_combine",CONTROLS_CUST_ACTION);
	ReadConfigKey("inventory",CONTROLS_CUST_INVENTORY);
	ReadConfigKey("book",CONTROLS_CUST_BOOK);
	ReadConfigKey("char_sheet",CONTROLS_CUST_BOOKCHARSHEET);
	ReadConfigKey("magic_book",CONTROLS_CUST_BOOKSPELL);
	ReadConfigKey("map",CONTROLS_CUST_BOOKMAP);
	ReadConfigKey("quest_book",CONTROLS_CUST_BOOKQUEST);
	ReadConfigKey("drink_potion_life",CONTROLS_CUST_DRINKPOTIONLIFE);
	ReadConfigKey("drink_potion_mana",CONTROLS_CUST_DRINKPOTIONMANA);
	ReadConfigKey("torch",CONTROLS_CUST_TORCH);

	ReadConfigKey("cancel_current_spell",CONTROLS_CUST_CANCELCURSPELL);
	ReadConfigKey("precast_1",CONTROLS_CUST_PRECAST1);
	ReadConfigKey("precast_2",CONTROLS_CUST_PRECAST2);
	ReadConfigKey("precast_3",CONTROLS_CUST_PRECAST3);
	ReadConfigKey("draw_weapon",CONTROLS_CUST_WEAPON);
	ReadConfigKey("quicksave",CONTROLS_CUST_QUICKSAVE);
	ReadConfigKey("quickload",CONTROLS_CUST_QUICKLOAD);
	ReadConfigKey("turn_left",CONTROLS_CUST_TURNLEFT);
	ReadConfigKey("turn_right",CONTROLS_CUST_TURNRIGHT);
	ReadConfigKey("look_up",CONTROLS_CUST_LOOKUP);
	ReadConfigKey("look_down",CONTROLS_CUST_LOOKDOWN);
	ReadConfigKey("strafe",CONTROLS_CUST_STRAFE);
	ReadConfigKey("center_view",CONTROLS_CUST_CENTERVIEW);
	ReadConfigKey("freelook",CONTROLS_CUST_FREELOOK);

	ReadConfigKey("previous",CONTROLS_CUST_PREVIOUS);
	ReadConfigKey("next",CONTROLS_CUST_NEXT);
	ReadConfigKey("crouch_toggle",CONTROLS_CUST_CROUCHTOGGLE);

	ReadConfigKey("unequip_weapon",CONTROLS_CUST_UNEQUIPWEAPON);

	ReadConfigKey("minimap",CONTROLS_CUST_MINIMAP);

// TODO Replace with defaulted functions above
/*	if(!bOk2)
	{
		int iI=MAX_ACTION_KEY;

		while(iI--)
		{
			sakActionKey[iI].iKey[0]=sakActionDefaultKey[iI].iKey[0];
			sakActionKey[iI].iKey[1]=sakActionDefaultKey[iI].iKey[1];
		}
	}
*/
	// Get miscellaneous settings
	bATI = ReadConfig( Section::Misc, Key::softfog, 0 );
	bForceNoEAX = ReadConfig( Section::Misc, Key::forcenoeax, 0 );
	bForceZBias = ReadConfig( Section::Misc, Key::forcezbias, 0 );
	bOneHanded = ReadConfig( Section::Misc, Key::forcetoggle, 0 );
	uiGoreMode = ReadConfig(Section::Misc, Key::fg, 1 );
	pStringMod = ReadConfig( Section::Misc, Key::modpak, Default::modpak );
	pStringModSfx = ReadConfig(Section::Misc, Key::modsfxpak, Default::modsfxpak );
	pStringModSpeech = ReadConfig(Section::Misc, Key::modspeechpak, Default::speechmodpak );

	INTERNATIONAL_MODE = ReadConfig(Section::Misc, Key::newcontrol, 0);

	switch(uiGoreMode)
	{
	case 0:
		{
			if(bWarningGore)
			{
				uiGoreMode=0;
				GORE_MODE=0;
			}
			else
			{
				uiGoreMode=1;
				GORE_MODE=1;
			}
		}
		break;

	case 1:
		GORE_MODE=1;
		break;

	case 2:
		GORE_MODE=0;
		break;

	default:
		uiGoreMode=0;
		GORE_MODE=0;
		break;
	}

	//on set les options

	ARXMenu_Options_Video_SetFogDistance(iFogDistance);
	ARXMenu_Options_Video_SetTextureQuality(iTextureResol);
	ARXMenu_Options_Video_SetBump(bBumpMapping);
	ARXMenu_Options_Video_SetLODQuality(iMeshReduction);
	ARXMenu_Options_Video_SetDetailsQuality(iLevelOfDetails);
	ARXMenu_Options_Video_SetGamma(iGamma);
	ARX_SetAntiAliasing();
	ARXMenu_Options_Audio_SetMasterVolume(iMasterVolume);
	ARXMenu_Options_Audio_SetSfxVolume(iSFXVolume);
	ARXMenu_Options_Audio_SetSpeechVolume(iSpeechVolume);
	ARXMenu_Options_Audio_SetAmbianceVolume(iAmbianceVolume);

	ARXMenu_Options_Control_SetInvertMouse(bInvertMouse);
	ARXMenu_Options_Control_SetAutoReadyWeapon(bAutoReadyWeapon);
	ARXMenu_Options_Control_SetMouseLookToggleMode(bMouseLookToggle);
	ARXMenu_Options_Control_SetMouseSensitivity(iMouseSensitivity);
	ARXMenu_Options_Control_SetAutoDescription(bAutoDescription);

	if(pMenu)
	{
		pMenu->bReInitAll=true;
	}

	//mixer Game
	ARX_SOUND_MixerSetVolume(ARX_SOUND_MixerGame, ARX_SOUND_MixerGetVolume(ARX_SOUND_MixerMenu));
	ARX_SOUND_MixerSetVolume(ARX_SOUND_MixerGameSample, ARX_SOUND_MixerGetVolume(ARX_SOUND_MixerMenuSample));
	ARX_SOUND_MixerSetVolume(ARX_SOUND_MixerGameSpeech, ARX_SOUND_MixerGetVolume(ARX_SOUND_MixerMenuSpeech));
	ARX_SOUND_MixerSetVolume(ARX_SOUND_MixerGameAmbiance, ARX_SOUND_MixerGetVolume(ARX_SOUND_MixerMenuAmbiance));

	Localisation_Close();

	bALLOW_BUMP=bBumpMapping;
	WILL_RELOAD_ALL_TEXTURES=1;
	GORE_MODE = IsNoGore()? 0 : 1;
	Localisation_Init();

	if(bBumpMapping)
		EERIE_ActivateBump();
	else
		EERIE_DesactivateBump();

	if( iTextureResol==2 ) Project.TextureSize=0;

	if( iTextureResol==1 ) Project.TextureSize=2;

	if( iTextureResol==0 ) Project.TextureSize=64;
}

