#include "PlayerMe.h"
#include "MainRoot.h"
#include "protocol.h"
#include "World.h"
#include "Audio.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CPlayerMe::CPlayerMe()
{
	m_uAttackTarget = 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CPlayerMe::~CPlayerMe()
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CPlayerMe::walk(int x, int y)
{
	// ----
	// Clear attack target.
	// ----
	m_uAttackTarget		= 0;
	// ----
	CRole::setTargetCellPos(x, y);
	// ----
	int nSrcX			= m_posCell.x;
	int nSrcY			= m_posCell.y;
	// ----
	if(m_uActionState == WALK)
	{
		nSrcX += DX[m_uDir];
		nSrcY += DY[m_uDir];
	}
	// ----
	CSMove(nSrcX, nSrcY, getPath(), m_uTargetDir);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CPlayerMe::playWalkSound()
{
	if(this->getID() == CPlayerMe::getInstance().getID())
	{
		if (CWorld::getInstance().getTerrain() && CWorld::getInstance().getTerrain()->getTerrainData())
		{
			unsigned char uTileID = CWorld::getInstance().getTerrain()->getTerrainData()->getCellTileID(m_posCell.x, m_posCell.y ,0);
			// ----
			if (uTileID==0)
			{
				GetAudio().playSound("Data\\Sound\\pWalk(Grass).wav");
			}
			else
			{
				GetAudio().playSound("Data\\Sound\\pWalk(Soil).wav");
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CPlayerMe::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	// ----
	// # If has the attack target, player should walk around the target, then attack the target.
	// ----
	if(m_uAttackTarget > 0)
	{
		CRole * pRole = CWorld::getInstance().getRole(m_uAttackTarget);
		// ----
		if(pRole == NULL)
		{
			m_uAttackTarget = 0;
		}
		else
		{
			std::vector<unsigned char>	setPath;
			Pos2D						posTarget;
			// ----
			float fDistance = (pRole->getPos() - getPos()).length();
			// ----
			if(fDistance >= 2.0f)
			{
				int nSrcX = m_posCell.x;
				int nSrcY = m_posCell.y;
				// ----
				if(m_uActionState == WALK)
				{
					nSrcX += DX[m_uDir];
					nSrcY += DY[m_uDir];
				}
				// ----
				pRole->GetCellPos(posTarget);
				// ----
				CWorld::getInstance().getPath(nSrcX, nSrcY, posTarget.x, posTarget.y,setPath);
				// ----
				if(setPath.size() > 1)
				{
					unsigned uTargetDir = setPath.back();
					// ----
					setPath.erase(setPath.end() - 1);
					// ----
					CSMove(nSrcX,nSrcY,setPath,uTargetDir);
				}
			}
			else if(m_uActionState == STAND)
			{
				unsigned uTargetDir = GetDir(getPos(),pRole->getPos());
				// ----
				CSAttack(0x78, m_uAttackTarget, uTargetDir);
				// ----
				setDir(uTargetDir);
				// ----
				setActionState(HIT1);
				// ----
				m_uAttackTarget = 0;
			}
		}
	}
	// ----
	CRole::frameMove(mWorld, fTime, fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CPlayerMe::renderFocus()const
{
	C3DMapObj::renderFocus(0xFF00FFFF);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------