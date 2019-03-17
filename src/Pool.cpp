#include "Pool.h"

void Pool::addData(UINT8 *data, int size)
{
	

	m_pool[m_writeIndex].frame = new UINT8[size];
	m_pool[m_writeIndex].size = size;

	memcpy(m_pool[m_writeIndex].frame, data, size);

	if (++m_writeIndex >= POOL_SIZE) m_writeIndex = 0;
}

StreamData* Pool::takeData()
{
	StreamData *data = NULL;

	if (m_readIndex == m_writeIndex) return data;

	data = &m_pool[m_readIndex];
	
	if (++m_readIndex >= POOL_SIZE) m_readIndex = 0;

	return data;
}

Pool::~Pool()
{
	for (int i = 0; i < POOL_SIZE; i++)
		delete[] m_pool[i].frame;
}