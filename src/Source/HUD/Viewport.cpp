#pragma once
#include <metahook.h>
#include <vector>
#include <vgui/VGUI.h>
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/IEngineVGui.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/AnimationController.h>
#include "BaseUI.h"
#include "mymathlib.h"
#include "local.h"
#include "vguilocal.h"
#include <string>
#include "steam_api.h"
#include "player_info.h"

#include "motd.h"
#include "popnum.h"
#include "playerboard.h"
#include "scoreboard.h"
#include "vote.h"
#include "sidetext.h"
#include "textmenu.h"
#include "flashlight.h"
#include "notice.h"

#include "Viewport.h"
#include "exportfuncs.h"
#include "keydefs.h"

using namespace vgui;

CViewport *g_pViewPort = nullptr;

CViewport::CViewport(void) : Panel(nullptr, "ABCEnchanceViewport"){
	int swide, stall;
	surface()->GetScreenSize(swide, stall);

	MakePopup(false, true);

	SetScheme("ABCEnchanceScheme");
	SetBounds(0, 0, swide, stall);
	SetPaintBorderEnabled(false);
	SetPaintBackgroundEnabled(false);
	SetMouseInputEnabled(false);
	SetKeyBoardInputEnabled(false);
	SetProportional(true);

	//for popnumber
	vgui::scheme()->LoadSchemeFromFile(VGUI2_ROOT_DIR "PopNumerScheme.res", "PopNumerScheme");

	m_pPlayerTitle = CREATE_CVAR("cl_playertitle", "1", FCVAR_VALUE, nullptr);
	m_pPlayerTitleDanger = CREATE_CVAR("cl_playertitle_danger", "30", FCVAR_VALUE, nullptr);
	m_pPopNumber = CREATE_CVAR("cl_popnumber", "1", FCVAR_VALUE, nullptr);
}

CViewport::~CViewport(void){
	for (auto panel : m_Panels) {
		panel->~IViewportPanel();
	}
}

void CViewport::Start(void){
	AddNewPanel(m_pScorePanel = new CScorePanel());
	AddNewPanel(m_pVotePanel = new CVotePanel());
	AddNewPanel(m_pMOTDPanel = new CMotdPanel());
	AddNewPanel(m_pSidePanel = new CSidePanel());
	AddNewPanel(m_pTextMenu = new CTextMenu()); 
	AddNewPanel(m_pFlashLight = new CFlashLightPanel());
	AddNewPanel(m_pNotice = new CNoticePanel("NoticePanel"));
	AddNewPanel(m_pNoticeCenter = new CNoticePanel("NoticeCenterPanel"));
	for (size_t i = 0; i < 32; i++) {
		AddNewPanel(m_pPlayerInfoPanels[i] = new CPlayerInfoPanel());
		m_pPlayerInfoPanels[i]->SetId(i);
	}
	SetVisible(false);
}

void CViewport::SetParent(VPANEL vPanel){
	BaseClass::SetParent(vPanel);
	m_pScorePanel->SetParent(GetVPanel());
	m_pVotePanel->SetParent(GetVPanel());
	m_pMOTDPanel->SetParent(GetVPanel());
	m_pSidePanel->SetParent(GetVPanel());
	m_pTextMenu->SetParent(GetVPanel());
	m_pFlashLight->SetParent(GetVPanel());
	m_pNotice->SetParent(GetVPanel());
	m_pNoticeCenter->SetParent(GetVPanel());
	for (size_t i = 0; i < 32; i++) {
		m_pPlayerInfoPanels[i]->SetParent(GetVPanel());
	}
}

void CViewport::AddNewPanel(IViewportPanel* panel){
	m_Panels.push_back(panel);
	panel->SetParent(GetVPanel());
	dynamic_cast<vgui::Panel*>(panel)->MakeReadyForUse();
}

void CViewport::Think(void){
	vgui::GetAnimationController()->UpdateAnimations(gEngfuncs.GetClientTime());
	for (size_t i = 0; i < 32; i++) {
		m_pPlayerInfoPanels[i]->Think();
	}
}

void CViewport::VidInit(void){
	Reset();
}

void CViewport::Reset() {
	for (IViewportPanel* pPanel : m_Panels)
		pPanel->Reset();
	GetThisPlayerInfo()->ResetAll();
	m_iInterMission = 0;
}

void CViewport::Init(void){
	GetThisPlayerInfo()->InitPlayerInfos();
	GetTeamInfo(0)->InitTeamInfos();
}

void CViewport::ActivateClientUI(void){
	SetVisible(true);	
}

void CViewport::HideClientUI(void){
	SetVisible(false);
}

bool CViewport::KeyInput(int down, int keynum, const char* pszCurrentBinding){
	if (down){
		if (IsScoreBoardVisible()){
			if (!m_pScorePanel->IsMouseInputEnabled()){
				if (keynum == K_MOUSE2){
					m_pScorePanel->EnableMousePointer(true);
					return false;
				}
			}
		}
		return m_pVotePanel->KeyCodeTyped(keynum);
	}
	return true;
}

void CViewport::SetInterMission(int intermission) {
	if(intermission != m_iInterMission)
		m_pScorePanel->ShowPanel(intermission > 0);
	m_iInterMission = intermission;
}
int CViewport::GetInterMission() {
	return m_iInterMission;
}

bool CViewport::IsScoreBoardVisible(){
	return m_pScorePanel->IsVisible();
}
void CViewport::ShowScoreBoard(){
	m_pScorePanel->ShowPanel(true);
}
void CViewport::HideScoreBoard(){
	m_pScorePanel->ShowPanel(false);
}
long CViewport::GetTimeEnd() {
	return m_iTimeEnd;
}
char* CViewport::GetServerName(){
	return m_szServerName;
}
char* CViewport::GetNextMap() {
	return m_szNextMapName;
}
bool CViewport::IsPlayerTileEnable() {
	return m_pPlayerTitle->value > 0;
}
bool CViewport::IsVoteEnable(){
	return m_pVotePanel->IsVoteEnable();
}
void CViewport::StartVote(char* szContent, char* szYes, char* szNo, int iVoteType){
	m_pVotePanel->StartVote(szContent, szYes, szNo, iVoteType);
}
void CViewport::EndVote(){
	m_pVotePanel->EndVote();
}
void CViewport::AddPopNumber(vec3_t vecOrigin, Color& pColor, int value){
	CPopNumberPanel* p = new CPopNumberPanel(vecOrigin, pColor, value);
	p->SetParent(GetVPanel());
	dynamic_cast<vgui::Panel*>(p)->MakeReadyForUse();
	p->ShowPanel(true);
}
void CViewport::AppendMOTD(char* szMessage) {
	m_pMOTDPanel->AppendMotd(szMessage);
}
void CViewport::ShowMOTD(){
	m_pMOTDPanel->ShowMotd();
}
void CViewport::CloseMOTD(){
	m_pMOTDPanel->ShowPanel(false);
}
void CViewport::ForeceBuildPage() {
	m_pMOTDPanel->ForceAddPage();
}
void CViewport::ShowSideText(bool state){
	m_pSidePanel->ShowPanel(state);
}

bool CViewport::MsgShowMenu(const char* pszName, int iSize, void* pbuf){
	return m_pTextMenu->MsgShowMenu(pszName, iSize, pbuf);
}

void CViewport::ShowTextMenu(bool state){
	m_pTextMenu->ShowPanel(state);
}
void CViewport::SelectMenuItem(int slot){
	m_pTextMenu->SelectMenuItem(slot);
}
bool CViewport::IsTextMenuOpen(){
	return m_pTextMenu->IsVisible();
}
void CViewport::SetFlashLight(bool on, int battery){
	m_pFlashLight->SetFlashLight(on, battery);
}
void CViewport::SetFlashBattery(int battery){
	m_pFlashLight->SetFlashBattery(battery);
}
void CViewport::ShowNotice(HUDNOTICE type, const char* message){
	switch (type)
	{
	case CViewport::HUDNOTICE::PRINTNOTIFY:
		m_pNotice->ShowMessage(message); break;
	case CViewport::HUDNOTICE::PRINTCENTER:
		m_pNoticeCenter->ShowMessage(message); break;
	default:
		break;
	}
}
void CViewport::Paint(void){
	BaseClass::Paint();
}

CScorePanel* CViewport::GetScoreBoard() {
	return m_pScorePanel;
}

CVotePanel* CViewport::GetVotePanel(){
	return m_pVotePanel;
}

CMotdPanel* CViewport::GetMotdPanel(){
	return m_pMOTDPanel;
}

Color CViewport::GetPlayerColor(int index){
	vec3_t color;
	mathlib::VectorCopy(gHookFuncs.GetClientColor(index), color);
	return Color(color[0] * 255, color[1] * 255, color[2] * 255, 255);
}
