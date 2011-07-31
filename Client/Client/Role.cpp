#include "Role.h"
#include "MainRoot.h"
#include "World.h"
#include "Audio.h"
#include "PlayerMe.h"
#include "RPGSkyUIGraph.h"
#include "CsvFile.h"
#include "SkinModel.h"
#include "SkeletonData.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const int DX[DIR_COUNT] = { 0, 1, 1, 1, 0,-1,-1,-1};
const int DY[DIR_COUNT] = {-1,-1, 0, 1, 1, 1, 0,-1};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const Vec3D DirNormalize[DIR_COUNT] = 
{
	DIR_NORMALIZE(0),
	DIR_NORMALIZE(1),
	DIR_NORMALIZE(2),
	DIR_NORMALIZE(3),
	DIR_NORMALIZE(4),
	DIR_NORMALIZE(5),
	DIR_NORMALIZE(6),
	DIR_NORMALIZE(7),
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const float DirLength[DIR_COUNT] = 
{
	DIR_LENGTH(0),
	DIR_LENGTH(1),
	DIR_LENGTH(2),
	DIR_LENGTH(3),
	DIR_LENGTH(4),
	DIR_LENGTH(5),
	DIR_LENGTH(6),
	DIR_LENGTH(7),
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CRole::CRole()
{
	m_fHeadRotate 			= 0.0f;
	m_fCurrentHeadRotate 	= 0.0f;
	m_fOneWalkLength 		= 2.35f;
	// ----
	m_uActionState 			= STAND;
	// ----
	m_uDir 					= 0;
	m_uTargetDir 			= 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CRole::~CRole()
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CRole::walk(unsigned char uDir)
{
	m_vPos.x	+=	DX[uDir];
	m_vPos.z 	+=	DY[uDir];
	// ----
	float fRate = ((float)m_AnimMgr.uFrame / (float)m_AnimMgr.uTotalFrames);
	// ----
	float fX 	= m_vPos.x+fRate;
	float fZ	= m_vPos.z+fRate;
	// ----
	m_vPos.y	= CWorld::getInstance().getHeight(fX, fZ);
	// ----
	setDir(uDir);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CRole::damage(int nDamage, unsigned char uDamageType, int nShieldDamage)
{
	std::wstring wcsDamageInfo;
	// ----
	if(nDamage==0)
	{
		wcsDamageInfo = L"Miss";
	}
	else
	{
		if (CPlayerMe::getInstance().getID() == getID())
		{
			wcsDamageInfo = L"[color=255,0,0]" + i2ws(nDamage) + L"[/color]";
		}
		else
		{
			wcsDamageInfo = L"[color=255,96,0]" + i2ws(nDamage) + L"[/color]";
		}
	}
	// ----
	CWorld::getInstance().addDamageInfo(getPos(),wcsDamageInfo);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CRole::setSkeleton()
{
	//BBox box;
	// ----
	// # Player.bmd
	// ----
	load(PLAYER_BMD_PATCH);
	// ----
	//box.vMin = Vec3D(-0.5f, 0.0f, -0.5f);
	//box.vMax = Vec3D(0.5f, 2.0f, 0.5f);
	// ----
	//CModelObject::setBBox(box);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CRole::setSet(const CHARSET & charSet)
{
	setClass(charSet.CharSet[CS_CLASS]);
	// ----
	setSkeleton();
	// ----
	setHelm	(charSet.getHelm());
	setArmor(charSet.getArmor());
	setGlove(charSet.getGlove());
	setPant	(charSet.getPant());
	setBoot	(charSet.getBoot());
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CRole::setEquip(const char* szPart, int nType, int nEquipID)
{
	bool bShowClass = true;
	// ----
	CCsvFile csv;
	// ----
	std::string strClassPart = "class";
	// ----
	strClassPart.append(szPart);
	// ----
	if(nEquipID < 0x0F)
	{
		// ----
		// # Delete old equip!
		// ----
		this->delChild(this->getChild(szPart));
		// ----
		// # Attach new equip!
		// ----
		csv.open(ITEM_CVS_PATCH);
		// ----
		csv.seek(0, nType);
		csv.seek(1, nEquipID);
		// ----
		const char* szModelFilename	= csv.getStr("Model","000000");
		// ----
		CSkinModel* pSkinMesh = new CSkinModel;
		pSkinMesh->load(szModelFilename);
		pSkinMesh->setName(szPart);
		this->addChild(pSkinMesh);
		// ----
		// # Get Need Show Class!
		// ----
		csv.open(EQUIP_CVS_PATCH);
		// ----
		csv.seek(0 ,nType);
		csv.seek(1 ,nEquipID);
		// ----
		bShowClass = csv.getBool("ShowClass");
	}
	// ----
	if(bShowClass == true)
	{
		sprintf_s(m_szTemp, "Data\\Player\\%sClass%02d.bmd",szPart, (m_nClass >> 5) + 1);
		// ----
		CSkinModel* pSkinMesh = new CSkinModel;
		pSkinMesh->load(m_szTemp);
		pSkinMesh->setName(strClassPart.c_str());
		this->addChild(pSkinMesh);
	}
	else
	{
		// ----
		// # Delete old class part!
		// ----
		this->delChild(this->getChild(strClassPart.c_str()));
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CRole::setCellPos(int x, int y)
{
	m_posCell.x 	= x;
	m_posCell.y 	= y;
	// ----
	m_vPos.x 		= (x + 0.5f);
	m_vPos.z 		= (y + 0.5f);
	// ----
	m_vPos.y		= CWorld::getInstance().getHeight(m_vPos.x, m_vPos.z);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CRole::setTargetCellPos(int x, int y)
{
	int nSrcX 	= m_posCell.x;
	int nSrcY 	= m_posCell.y;
	// ----
	if(m_uActionState == WALK)
	{
		nSrcX 	+= DX[m_uDir];
		nSrcY 	+= DY[m_uDir];
	}
	// ----
	m_uTargetDir = CWorld::getInstance().getPath(nSrcX, nSrcY, x, y,getPath());
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CRole::drawName()const
{
	if(m_wstrName.empty() == false)
	{
		Pos2D pos(0,0);
		Vec3D vPos;
		BBox box 	= C3DMapObj::getWorldBBox();
		// ----
		vPos 		= (box.vMin+box.vMax)*0.5f;
		vPos.y 		= box.vMax.y;
		// ----
		GetRenderSystem().world2Screen(vPos, pos);
		// ----
		RECT rc 	={pos.x - 100, pos.y - 30 , pos.x + 100, pos.y};
		// ----
		std::wstring wstrText = L"[color=255,255,0]Level" + i2ws(m_nLevel) +
			L"[/color] [color=128,255,255]" + m_wstrName + L"[/color]";
		// ----
		RPGSkyUIGraph::getInstance().drawText(m_wstrName.c_str(), m_wstrName.length(), rc, ALIGN_TYPE_CENTER);
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CRole::render(const Matrix& mWorld, E_MATERIAL_RENDER_TYPE eRenderType)const
{
	//Matrix mWorld 	= C3DMapObj::getWorldMatrix();
	// ----
	//GetRenderSystem().setWorldMatrix(mWorld);
	// ----
	C3DMapObj::render(mWorld,eRenderType);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CRole::setActionState(unsigned char uActionState)
{
	m_uActionState=uActionState;
	// ----
	switch(m_uActionState)
	{
		case STAND:
		{
			if((m_nClass >> 5) == 2)
			{
				setAnim("2");
			}
			else
			{
				setAnim("1");
			}
		}
		break;
		
		case WALK:
		{
			if((m_nClass >> 5) == 2)
			{
				setAnim("16");
			}
			else
			{
				setAnim("15");
			}
		}
		break;
		
		case HIT1:
		{
			setAnim("38");
		}
		break;
		
		case DIE:
		{
			setAnim("221");
		}
		break;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CRole::playWalkSound()
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CRole::frameMove(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	m_vRotate.x 			= 0.0f;
	m_vRotate.y 			= fmod(m_vRotate.y, PI_X2);
	// ----
	float fTargetRotateY 	= (m_uDir * PI * 0.25f);
	float fAngle 			= (fTargetRotateY - m_vRotate.y);
	// ----
	fAngle 					+= (fAngle > PI ? - PI_X2 : (fAngle < - PI ? PI_X2 : 0));
	// ----
	m_vRotate.y 			+= (fAngle * fElapsedTime * 10.0f);
	m_vRotate.z 			= 0.0f;
	// ----
	//if(CModelComplex::isCreated() == false)
	//{
	//	CModelComplex::create();
	//}
	// ----
	if(m_uActionState == DEATH)
	{
		return;
	}
	// ----
	m_AnimMgr.Tick(int(fElapsedTime * 1000));
	// ----
	if(m_uActionState == WALK)
	{
		float fRate 		= ((float)m_AnimMgr.uFrame / (float)m_AnimMgr.uTotalFrames);
		float fWalkLength	= ((float)m_AnimMgr.CurLoop + fRate) * m_fOneWalkLength;
		// ----
		if(m_fWalkLength+DirLength[m_uDir] <= fWalkLength)
		{
			m_fWalkLength	+=DirLength[m_uDir];
			// ----
			m_posCell.x		+=DX[m_uDir];
			m_posCell.y		+=DY[m_uDir];
			// ----
			if(m_setPath.size() == 0)
			{
				setActionState(STAND);
				// ----
				setDir(m_uTargetDir);
			}
			else
			{
				playWalkSound();
				// ----
				setDir(m_setPath[0]);
				// ----
				m_setPath.erase(m_setPath.begin());
			}
		}
		// ----
		m_vPos.x	= (m_posCell.x + 0.5f);
		m_vPos.z 	= (m_posCell.y + 0.5f);
		// ----
		m_vPos 		+= DirNormalize[m_uDir] * (fWalkLength-m_fWalkLength);
		m_vPos.y	= CWorld::getInstance().getHeight(m_vPos.x, m_vPos.z);
	}
	else if(m_uActionState == STAND)
	{
		if(m_setPath.size() > 0)
		{
			playWalkSound();
			// ----
			setActionState(WALK);
			// ----
			m_fWalkLength = 0;
			// ----
			setDir(m_setPath[0]);
			// ----
			m_setPath.erase(m_setPath.begin());
		}
	}
	else if(m_uActionState == HIT1)
	{
		if(m_AnimMgr.CurLoop > 0)
		{
			// ----
			sprintf_s(m_szTemp, "Data\\Sound\\eMeleeHit%d.wav", (rand() % 4) + 1);
			// ----
			// # Play Hit Sound.
			// ----
			GetAudio().playSound(m_szTemp);
			// # Set stand state.
			// ----
			setActionState(STAND);
		}
	}
	else if(m_uActionState==DIE)
	{
		if(m_AnimMgr.CurLoop > 0)
		{
			/*if (getID()==CPlayerMe::getInstance().getID())
			{
				setActionState(STAND);
			}*/
			// ----
			setActionState(DEATH);
			return;
		}
	}
	// ----
	animate(m_strAnimName.c_str());
	// ----
	if(m_pSkeletonData > 0)
	{
		size_t uBoneCount = m_pSkeletonData->m_Bones.size();
		// ----
		for(size_t i = 0 ; i < uBoneCount; i++)
		{
			if(m_pSkeletonData->m_Bones[i].m_strName == "Bip01 Head")
			{
				if(rand() % 53 == 0)
				{
					m_fHeadRotate = (rand() % 100) * 0.02f - 1.0f;
				}
				// ----
				m_fCurrentHeadRotate += (m_fHeadRotate-m_fCurrentHeadRotate) * fElapsedTime;
				// ----
				Matrix mRotate;
				// ----
				mRotate.rotate(Vec3D(0.0f, m_fCurrentHeadRotate, 0.0f));
				// ----
				m_setBonesMatrix[i] *= mRotate;
			}
		}
	}
	// ----
	Matrix mNewWorld = mWorld*m_mWorldMatrix;
	C3DMapObj::frameMove(mNewWorld, fTime, fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned char GetDir(const Vec3D & vSrcDir, const Vec3D & vDestDir)
{
	Vec3D vDir	= vDestDir-vSrcDir;
	vDir.y		= 0.0f;
	// ----
	vDir.normalize();
	// ----
	float fDot	= vDir.dot(Vec3D(0.0f ,0.0f, 1.0f));
	// ----
	fDot		+=1.0f;
	// ----
	if(vDir.x < 0)
	{
		fDot =4.0f - fDot;
	}
	// ----
	return (unsigned char)(fDot * 2.0f + 0.5f);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------