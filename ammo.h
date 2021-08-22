#define SLECTEDRIN_KEEP_TIME 5

class CHudCustomAmmo : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void Reset(void);
	int DrawWList(float flTime);
	void SlotInput(int iSlot, int fAdvance);
	void ChosePlayerWeapon(void);

	float StartX = 48;
	float IconSize = 0.5;
	float ElementGap = 0.2;
	float BackGroundY = 0.95;
	float BackGroundLength = 3;
	float BackGroundAlpha = 128;

	int iSelectCyclerSpr = 0;
	int iSelectCyclerRinSpr = 0;

	Color Ammo1IconColor;
	Color Ammo1BigTextColor;
	Color Ammo1TextColor;
	Color Ammo2IconColor;
	Color Ammo2BigTextColor;
	Color Ammo2TextColor;
	Color BackGroundColor;

	Color SelectCyclerColor;
	Color SelectCyclerRinColor;
	Color SelectCyclerIconColor;
	Color SelectCyclerTextColor;
	Color SelectCyclerEmptyColor;

	float SelectCyclerOffset;
	float SelectCyclerSize;
	float SelectCyclerRotate;

	vgui::HFont HUDFont;
	vgui::HFont HUDSmallFont;
	
	float m_fFade;
	WEAPON* m_pWeapon;
	int	m_HUD_bucket0;
	int m_HUD_selection;

};
extern CHudCustomAmmo m_HudCustomAmmo;