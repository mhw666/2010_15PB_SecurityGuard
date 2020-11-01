#include "pch.h"
#include "CMyView.h"


CMyView gView;

CMyView::CMyView()
{
	this->m_tRoot = new MyFunction{ 0 };
	this->m_tRoot->fEND.uiDeep = 0xFFFF;
	PMyTreeInfo tmp = (PMyTreeInfo)this->m_tRoot;
	for (BYTE i = 0; tmp->uiDeep == 0; ++tmp) {
		tmp->str = gszTreeFunctions[i++];
	}
}

CMyView::~CMyView()
{
	delete this->m_tRoot;
}

void CMyView::Init(CDialogEx* wMain)
{
	this->m_Main = wMain;
	this->m_PVList = (CListCtrl*)wMain->GetDlgItem(IDC_LIST1);
	this->m_PVList->SetExtendedStyle(/*LVS_EX_CHECKBOXES |*/ //���Ǹ�ѡ��
		LVS_EX_GRIDLINES |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	this->m_PVTree = (CTreeCtrl*)wMain->GetDlgItem(IDC_TREE1);

	PMyTreeInfo tmp = (PMyTreeInfo)this->m_tRoot;
	while (tmp->uiDeep == 0)
	{
		tmp->htTree = this->m_PVTree->InsertItem(tmp->str);
		this->Init(tmp->htTree);
		++tmp;
	}
	return;
}
#pragma region ���ؼ�����
/*
	��������ʼ������
	����һ������������
*/
void CMyView::Init(const HTREEITEM& hTree)
{
	MyTreeInfo tmp{			//��ʱ��Ҷ����
		0,					//�����
		hTree,				//�����
		1					//���
	};


	//	IF START���ļ���������ʼ��
	if (hTree == this->m_tRoot->fFile.htTree) {			//�ж�����Ϊ�ļ�
		size_t stLen = GetLogicalDriveStrings(0, NULL);	//��ȡ������
		PTCHAR buff = new TCHAR[stLen + 2];				//��ʼ������
		GetLogicalDriveStrings((DWORD)stLen, buff);		//���뻺����
		PTCHAR pDriveStrings = buff;					//��ʼ���ַ���ָ��


		while (stLen = wcslen(pDriveStrings))			//ѭ���ж�������
		{
			tmp.htTree = m_PVTree->InsertItem(			//������Ҷ
				pDriveStrings, hTree);					//��ֵ����ʱ����
			tmp.str = pDriveStrings;					//��ֵ��Ҷ�ַ���
			m_tLeafs[tmp.htTree] = tmp;					//��map��¼��Ҷ
			OutputDebugString(tmp.str);
			pDriveStrings += stLen + 1;					//�ַ���ָ��ƫ��
		}
		delete[] buff;									//�ͷŻ�����
		//IF END���ļ���������ʼ��
	}


	//	IF START��PE��������ʼ��
	else if (hTree == this->m_tRoot->fPE.htTree) {		//�ж��Ƿ�ΪPE������
		for (size_t i = 0; i < defMAX_PEindex; i++)
		{
			tmp.str = gszPEFunctions[i];
			tmp.htTree = this->m_PVTree->InsertItem(tmp.str, hTree);
			m_tLeafs[tmp.htTree] = tmp;
		}
		//IF END��PE��������ʼ��
	}
	return;
}

/*
	�����������ؼ���������
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-31_09-30
*/
void CMyView::DoSomeThingTree(HTREEITEM& hTree)
{
	MyTreeInfo& tInfo = m_tLeafs[hTree];	//map�ҵ���Ӧ��[��Ҷ��Ϣ]
	InitList(tInfo);						//���б��ؼ���ʼ��������

	size_t i = 0;			//��ʱѭ������
	MyTreeInfo tmp{			//��ʱ��Ҷ����
		0,					//�����
		tInfo.hrTree,		//�����
		tInfo.uiDeep + 1	//���
	};


	//IF START�����ļ�����������Ŀ¼
	if (tInfo.hrTree == this->m_tRoot->fFile.htTree	//�������Ϊ�ļ�
		&& tInfo.uiDeep > 0) {						//�����>0
		if (m_PVTree->ItemHasChildren(hTree))		//�����Ҷ�ӽڵ�������
			return;									//�򷵻�

		OutputDebugString(_T("\tDo�����ļ�\n"));
		CString strPath;	UINT deep = tInfo.uiDeep;	//��ʼ��·�������
		GetTreePath(tInfo.htTree, deep, strPath);		//ͨ�����ɻ�ȡ·��
		vector<FILEINFO> FLs;							//��ʼ������


		if (m_CFile.SearchPaths(FLs, strPath.GetBuffer()))	//��ȡĿ¼
		{
			this->InitList(FLs);		//���б��ؼ���������
			size_t max = FLs.size();	//��ʼ����С
			LPFILEINFO pInfo = &FLs[0];	//��ʼ��ѭ��ָ��
			for (i = 0; i < max; ++i, ++pInfo)
			{
				tmp.htTree = m_PVTree->InsertItem(
					pInfo->data.cFileName, hTree);
				m_tLeafs[tmp.htTree] = tmp;
			}
			m_PVTree->Expand(hTree, TVE_EXPAND);	//ѭ����Ϻ�չ����֦
		}
		//IF END�����ļ�����������Ŀ¼
	}
	else if (tInfo.hrTree == this->m_tRoot->fPE.htTree	//�������ΪPE
		&& tInfo.uiDeep == 1) {							//�����>0
		//����������PE�����������List����
		CPE* pcPE = nullptr;
		if (tInfo.str == gszPEFunctions[0]) {		//PEͷ
			pcPE = new CPE(DLLP32);
			if (pcPE->GetNTHeadInfo())
				this->InitList(pcPE->NTHead_Info);
			pcPE->~CPE();
			pcPE = new CPE(DLLP64);
			if (pcPE->GetNTHeadInfo())
				this->InitList(pcPE->NTHead_Info, false);
		}
		else if (tInfo.str == gszPEFunctions[1]) {	//������Ϣ
			pcPE = new CPE(DLLP32);
			pcPE->RvaToFoa(0, true);
			pcPE->~CPE();
			pcPE = new CPE(DLLP64);
			pcPE->RvaToFoa(0, true);
		}

		if (pcPE)	pcPE->~CPE();
	}
	return;
}

/*
	�������������ؼ�
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-31_10-40
*/
BOOL CMyView::GetTreePath(const HTREEITEM& htTree,
	UINT& uiCOunt, CString& str)
{
	if (!uiCOunt)
		return FALSE;
	str = m_PVTree->GetItemText(htTree) + _T("\\") + str;
	GetTreePath(m_PVTree->GetParentItem(htTree), --uiCOunt, str);
	return TRUE;
}
#pragma endregion


#pragma region �б��ؼ�����
/*
	��������ʼ���б��ؼ���Ϣ
	���ߣ�CO0kieؼ
	ʱ�䣺2020-10-31_15-45
*/
void CMyView::InitList(const MyTreeInfo& tInfo)
{
	if (tInfo.hrTree == this->m_Statu.tKind)	return;		//�����ͬ�򷵻�
	this->m_Statu.tKind = tInfo.hrTree;
	m_PVList->DeleteAllItems();
	while (m_PVList->DeleteColumn(0));

	if (tInfo.hrTree == this->m_tRoot->fFile.htTree) {		//�������Ϊ�ļ�
		m_PVList->InsertColumn(0, _T("�޸�ʱ��"), LVCFMT_CENTER, 222);
		m_PVList->InsertColumn(0, _T("����ʱ��"), LVCFMT_CENTER, 222);
		m_PVList->InsertColumn(0, _T("�ļ���"), LVCFMT_CENTER, 222);
		m_PVList->InsertColumn(0, _T("��С"), LVCFMT_CENTER, 88);
	}
	else if (tInfo.hrTree == this->m_tRoot->fPE.htTree		//�������ΪPE
		&& tInfo.uiDeep == 1) {
		if (tInfo.str == gszPEFunctions[0]) {	//NTͷ
			m_PVList->InsertColumn(0, _T("��ע"), LVCFMT_CENTER, 123);
			m_PVList->InsertColumn(0, _T("����ֵ"), LVCFMT_CENTER, 123);
			m_PVList->InsertColumn(0, _T("������"), LVCFMT_CENTER, 150);
		}
	}
	m_PVList->InsertColumn(0, _T("��"), LVCFMT_CENTER, 50);
}

void CMyView::InitList(vector<FILEINFO>& FLs)
{
	size_t max = FLs.size(), i = max;
	if (max == 0)	return;
	this->m_PVList->DeleteAllItems();
	CString str;	FILEINFO* fl = nullptr;
	TCHAR buff[MAX_PATH];	int count = (int)max;
	for (i = max, fl = &FLs[max - 1]; i--; --fl) {
		if (fl->Size) {
			wsprintf(buff, _T("%d"), count--);
			this->m_PVList->InsertItem(0, buff);
			this->m_PVList->SetItemText(0, 2, fl->data.cFileName);
			StrFormatByteSize64(fl->Size, buff, MAX_PATH);
			this->m_PVList->SetItemText(0, 1, buff);
		}
	}
	for (i = max, fl = &FLs[max - 1]; i--; --fl) {
		if (!fl->Size) {
			wsprintf(buff, _T("%d"), count--);
			this->m_PVList->InsertItem(0, buff);
			this->m_PVList->SetItemText(0, 2, fl->data.cFileName);

		}
	}
	//END
	return;
}

void CMyView::InitList(_NTHead_INFO& NTHead, bool bClean /*= true*/)
{
	if (!NTHead.dwPEHead[0])	return;				//����ڵ�
	if (bClean)	this->m_PVList->DeleteAllItems();	//

	BYTE i = 16;
	
	while (i--)
	{
		this->m_str.Format(_T("%d"), i + 1);
		this->m_PVList->InsertItem(0, m_str);
		this->m_PVList->SetItemText(0, 1, gszNTHeadInfos[i]);
		m_str.Format(_T("0x%08lX"), NTHead.dwPEHead[i]);
		if (Idx01�����ַ == i && NTHead.x64ImageBase)
			m_str.Format(_T("0x%8llX"), NTHead.x64ImageBase);
		this->m_PVList->SetItemText(0, 2, m_str);
	}
	this->m_PVList->InsertItem(0, _T("0"));
	m_str.Format(_T("%d"), NTHead.x64ImageBase ? 64 : 32);
	this->m_PVList->SetItemText(0, 1, _T("�ļ�λ����"));
	this->m_PVList->SetItemText(0, 2, m_str);
}
#pragma endregion