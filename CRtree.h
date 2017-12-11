#ifndef CRTREE_H_
#define CRTREE_H_

#include <stdio.h>
#include <stdlib.h>

#define MID_NODE 0
#define LEAF_NODE 1

class CNode;

class CMBB
{
	private:
		float* m_maxFeatures;
		float* m_minFeatures;

		bool InitMaxMinFeatures();

	public:
		int m_numFeatures;
		CNode* m_ptrNode;

		CMBB(int numFeatures, CNode* ptrNode);
		bool SetMax(int feature, float maxValue);
		bool SetMin(int feature, float minValue);
		float GetMax(int feature);
		float GetMin(int feature);
};

class CData
{
	private:
		bool InitFeatures(float* features);

	public:
		float* m_features;
		int m_numFeatures;

		CData(int numFeatures, float* features);
		float GetFeature(int* feature);
};

class CNode
{
	private:
		bool InitEntriesMBB();
		bool InitEntriesData();

	public:
		int m_availEntry;
		bool m_typeNode;
		CMBB** m_entriesMBB;
		CData** m_entriesData;
		int m_numEntries;
		int m_numFeatures;
		CNode* m_parent;

		CNode(int numEntries, int numFeatures, CNode* parent, bool typeNode);
		bool PushEntryMBB(CNode* ptrNode);
		bool PushEntryData(float* features);
};

class CRtree
{
	public:
		CNode* m_root;
		int m_numEntries;
		int m_numFeatures;

		CRtree(int numEntries, int numFeatures);
		CNode** SearchData(float* features);
		bool InsertData(float* features);
		bool SplitNode(CNode** ptrNode);
};

#endif
