#include <src/CRtree.h>

// CMBB ==================================================
CMBB::CMBB(int numFeatures, CNode* ptrNode)
{
	m_numFeatures = numFeatures;
	m_ptrNode = ptrNode;
	InitMaxMinFeatures();
}

bool CMBB::InitMaxMinFeatures()
{
	m_maxFeatures = (float*) malloc(m_numFeatures * sizeof(float));
	m_minFeatures = (float*) malloc(m_numFeatures * sizeof(float));

	for(int i = 0; i < m_numFeatures; i++)
	{
		m_maxFeatures[i] = 0.0;
		m_minFeatures[i] = 0.0;
	}

	if(m_ptrNode->m_typeNode == LEAF_NODE)
	{
		for(int i = 0; i < m_numFeatures; i++)
		{
			m_maxFeatures[i] = m_ptrNode->m_entriesData[0]->m_features[i];
			m_minFeatures[i] = m_ptrNode->m_entriesData[0]->m_features[i];
		}

		for(int i = 1; i < m_ptrNode->m_availEntry ; i++)
		{
			for(int j = 0; j < m_numFeatures; j++)
			{
				if(m_ptrNode->m_entriesData[i]->m_features[j] > m_maxFeatures[j])
					m_maxFeatures[j] = m_ptrNode->m_entriesData[i]->m_features[j];

				if(m_ptrNode->m_entriesData[i]->m_features[j] < m_minFeatures[j])
					m_minFeatures[j] = m_ptrNode->m_entriesData[i]->m_features[j];
			}
		}
	}
	else
	{
		for(int i = 0; i < m_numFeatures; i++)
		{
			m_maxFeatures[i] = m_ptrNode->m_entriesMBB[0]->GetMax(i);
			m_minFeatures[i] = m_ptrNode->m_entriesMBB[0]->GetMin(i);
		}

		for(int i = 1; i < m_ptrNode->m_availEntry ; i++)
		{
			for(int j = 0; j < m_numFeatures; j++)
			{
				if(m_ptrNode->m_entriesMBB[i]->GetMax(j) > m_maxFeatures[j])
					m_maxFeatures[j] = m_ptrNode->m_entriesMBB[i]->GetMax(j);

				if(m_ptrNode->m_entriesMBB[i]->GetMin(j) < m_minFeatures[j])
					m_minFeatures[j] = m_ptrNode->m_entriesMBB[i]->GetMin(j);
			}
		}
	}

	return true;
}

bool CMBB::SetMax(int feature, float maxValue)
{
	m_maxFeatures[feature] = maxValue;

	return true;
}

bool CMBB::SetMin(int feature, float minValue)
{
	m_minFeatures[feature] = minValue;

	return true;
}

float CMBB::GetMax(int feature)
{
	return m_maxFeatures[feature];
}

float CMBB::GetMin(int feature)
{
	return m_minFeatures[feature];
}

// CDATA ==================================================
CData::CData(int numFeatures, float* features)
{
	m_numFeatures = numFeatures;
	InitFeatures(features);
}

bool CData::InitFeatures(float* features)
{
	m_features = (float*) malloc(m_numFeatures * sizeof(float));

	for(int i = 0; i < m_numFeatures; i++)
		m_features[i] = features[i];

	return true;
}

float CData::GetFeature(int* feature)
{
	return m_features[*feature];
}

// CNODE ==================================================
CNode::CNode(int numEntries, int numFeatures, CNode* parent, bool typeNode)
{
	m_typeNode = typeNode;
	m_numEntries = numEntries;
	m_numFeatures = numFeatures;
	m_availEntry = 0;
	m_parent = parent;

	if(m_typeNode == MID_NODE)
		InitEntriesMBB();
	else
		InitEntriesData();
}

bool CNode::InitEntriesMBB()
{
	m_entriesMBB = (CMBB**) malloc(m_numEntries * sizeof(CMBB*));

	for(int i = 0; i < m_numEntries; i++)
		m_entriesMBB[i] = NULL;

	return true;
}

bool CNode::InitEntriesData()
{
	m_entriesData = (CData**) malloc(m_numEntries * sizeof(CData*));

	for(int i = 0; i < m_numEntries; i++)
		m_entriesData[i] = NULL;

	return true;
}

bool CNode::PushEntryMBB(CNode* ptrNode)
{
	if(ptrNode &&
	   m_availEntry < m_numEntries &&
	   m_typeNode == MID_NODE)
	{
		m_entriesMBB[m_availEntry] = new CMBB(m_numFeatures, ptrNode);
		m_availEntry++;

		return true;
	}
	else
		return false;
}

bool CNode::PushEntryData(float* features)
{
	if(features &&
	   m_availEntry < m_numEntries &&
	   m_typeNode == LEAF_NODE)
	{
		m_entriesData[m_availEntry] = new CData(m_numFeatures, features);
		m_availEntry++;

		return true;
	}
	else
		return false;
}

// RTREE ==================================================
CRtree::CRtree(int numEntries, int numFeatures)
{
	m_numEntries = numEntries;
	m_numFeatures = numFeatures;

	m_root = new CNode(m_numEntries, m_numFeatures, NULL, LEAF_NODE);
}

CNode** CRtree::SearchData(float* features)
{
	CNode** ptrNode = &m_root;

	while( (*ptrNode)->m_typeNode != LEAF_NODE )
	{
		bool isIn = true;

		for(int i = 0; i < m_numEntries; i++)
		{
			isIn = true;

			for(int j = 0; j < m_numFeatures; j++)
			{
				if( features[j] < (*ptrNode)->m_entriesMBB[i]->GetMin(j) ||
				    features[j] > (*ptrNode)->m_entriesMBB[i]->GetMax(j) )
				{
					isIn = false;
					break;
				}
			}

			if(isIn)
				*ptrNode = (*ptrNode)->m_entriesMBB[i]->m_ptrNode;
		}

		if(isIn == false)
			break;
	}

	return ptrNode;
}

bool CRtree::InsertData(float* features)
{
	CNode** ptrNode;
	//ptrNode = &m_root;
	ptrNode = SearchData(features);

	if(ptrNode)
	{
		if((*ptrNode)->PushEntryData(features))
		{
			printf("[out] InsertData(): OK\n");
			return true;
		}
		else // DESBORDAMIENTO
		{
			printf("[!] InsertData(): Desbordamiento\n");

			SplitNode(ptrNode);

			printf("    [out] InsertData(): OK\n");
			return true;
		}
	}
	else
	{
		printf("[out] InsertData(): ERROR\n");
		return false;
	}
}

bool CRtree::SplitNode(CNode** ptrNode)
{
	if( ptrNode == NULL ||
		(*ptrNode) == NULL )
		return false;

	if( (*ptrNode)->m_typeNode == LEAF_NODE ) // DIVIDIENDO UN NODO HOJA
	{
		// CREANDO UN NUEVO NODO HOJA
		CNode* ptrNewNodeLEAF = new CNode(m_numEntries, m_numFeatures, NULL, LEAF_NODE);

		// DIVIDIENDO EN DOS EL NODO CON DESBORDAMIENTO
		int midIndex = m_numEntries / 2;

		for(int i = midIndex; i < m_numEntries; i++)
		{
			ptrNewNodeLEAF->PushEntryData( (*ptrNode)->m_entriesData[i]->m_features );
			(*ptrNode)->m_entriesData[i] = NULL;
		}

		(*ptrNode)->m_availEntry = midIndex;

		if( (*ptrNode)->m_parent == NULL ) // SE TRATA DE LA RAIZ
		{
			// CREANDO UN NUEVO NODO MEDIO
			CNode* ptrNewNodeMID = new CNode(m_numEntries, m_numFeatures, NULL, MID_NODE);

			// UNIENDO LOS DOS NODOS HOJA EN EL NODO MEDIO
			ptrNewNodeMID->PushEntryMBB(*ptrNode);
			ptrNewNodeMID->PushEntryMBB(ptrNewNodeLEAF);

			// ESTABLECIENDO EL NODO MEDIO COMO RAIZ LOCAL
			CNode* ptrTemp = *ptrNode;

			*ptrNode = ptrNewNodeMID;

			ptrTemp->m_parent = *ptrNode;
			ptrNewNodeLEAF->m_parent = *ptrNode;
		}
		else
		{
			if( (*ptrNode)->m_parent &&
				(*ptrNode)->m_parent->m_availEntry < m_numEntries )
			{
				// UNIENDO EL HOJA EN EL NODO MEDIO
				(*ptrNode)->m_parent->PushEntryMBB(ptrNewNodeLEAF);

				// AGREGANDO EL NUEVO NODO AL NODO MEDIO EXISTENTE
				ptrNewNodeLEAF->m_parent = (*ptrNode)->m_parent;
			}
			else if( (*ptrNode)->m_parent )
			{
				// HACIENDO ESPACIO EN EL PADRE
				SplitNode( &( (*ptrNode)->m_parent ) );

				// UNIENDO EL HOJA EN EL NODO MEDIO
				(*ptrNode)->m_parent->PushEntryMBB(ptrNewNodeLEAF);

				// AGREGANDO EL NUEVO NODO AL NODO MEDIO EXISTENTE
				ptrNewNodeLEAF->m_parent = (*ptrNode)->m_parent;
			}
		}

		printf("    [out] SpritNode(): OK, LEAF\n");
		return true;
	}
	else // DIVIDIENDO UN NODO MEDIO
	{
		// CREANDO UN NUEVO NODO MEDIO
		CNode* ptrNewNodeMID = new CNode(m_numEntries, m_numFeatures, NULL, MID_NODE);

		// DIVIDIENDO EN DOS EL NODO CON DESBORDAMIENTO
		int midIndex = m_numEntries / 2;

		for(int i = midIndex; i < m_numEntries; i++)
		{
			ptrNewNodeMID->PushEntryMBB( (*ptrNode)->m_entriesMBB[i]->m_ptrNode );
			(*ptrNode)->m_entriesMBB[i] = NULL;
		}

		(*ptrNode)->m_availEntry = midIndex;

		if( (*ptrNode)->m_parent == NULL ) // SE TRATA DE LA RAIZ
		{
			// CREANDO UN NUEVO NODO MEDIO
			CNode* ptrNewNodeMID_root = new CNode(m_numEntries, m_numFeatures, NULL, MID_NODE);

			// UNIENDO LOS DOS NODOS HOJA EN EL NODO MEDIO
			ptrNewNodeMID_root->PushEntryMBB(*ptrNode);
			ptrNewNodeMID_root->PushEntryMBB(ptrNewNodeMID);

			// ESTABLECIENDO EL NODO MEDIO COMO RAIZ LOCAL
			CNode* ptrTemp = *ptrNode;

			*ptrNode = ptrNewNodeMID_root;

			ptrTemp->m_parent = *ptrNode;
			ptrNewNodeMID->m_parent = *ptrNode;
		}
		else
		{
			// NO HAY NINGUN PROBLEMA AGREGANDO EL NUEVO NODO MEDIO EN EL PADRE
			if( (*ptrNode)->m_parent &&
				(*ptrNode)->m_parent->m_availEntry < m_numEntries)
			{
				// AGREGANDO EL NUEVO NODO MEDIO AL PADRE
				(*ptrNode)->m_parent->PushEntryMBB(ptrNewNodeMID);

				// REGISTRANDO EL PADRE DEL NUEVO NODO MEDIO
				ptrNewNodeMID->m_parent = (*ptrNode)->m_parent;
			}
			else if( (*ptrNode)->m_parent )
			{
				// HACIENDO ESPACIO EN EL PADRE
				SplitNode( &( (*ptrNode)->m_parent ) );

				// AGREGANDO EL NUEVO NODO MEDIO AL PADRE
				(*ptrNode)->m_parent->PushEntryMBB(ptrNewNodeMID);

				// REGISTRANDO EL PADRE DEL NUEVO NODO MEDIO
				ptrNewNodeMID->m_parent = (*ptrNode)->m_parent;
			}
		}
	}

	return true;
}

