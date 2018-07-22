#include "Binary.h"

Binary::Binary()
{}

Binary::Binary(const binary_t & vec)
	: m_vData(vec)
{}

Binary::Binary(const Binary & bObj)
	: m_vData(bObj.m_vData)
{}

Binary::~Binary()
{}

void Binary::SetData(const binary_t & data)
{
	m_vData = data;
}

void Binary::SetData(const byte_t * data, const size_t &size)
{
	m_vData.assign(data, data + size);
}

binary_t Binary::GetData() const
{
	return m_vData;
}

const byte_t* Binary::GetRawData() const
{
	return m_vData.data();
}

size_t Binary::GetSize() const
{
	return m_vData.size();
}

void Binary::ReadFromStream(std::istream &stream, const size_t &size)
{
	binary_t vec(size);
	stream.read(vec.data(), size);
	if (stream) {
		m_vData = vec;
	}
}

void Binary::WriteToStream(std::ostream & stream) const
{
	stream.write(m_vData.data(), m_vData.size());
}

void Binary::AppendData(const binary_t & data)
{
	AppendData(data.data(), data.size());
}

void Binary::AppendData(const Binary & bObj)
{
	AppendData(bObj.GetRawData(), bObj.GetSize());
}

void Binary::AppendData(const byte_t * data, const size_t &size)
{
	m_vData.insert(m_vData.end(), data, data + size);
}

void Binary::PrependData(const binary_t & data)
{
	PrependData(data.data(), data.size());
}

void Binary::PrependData(const Binary & bObj)
{
	PrependData(bObj.GetRawData(), bObj.GetSize());
}

void Binary::PrependData(const byte_t * data, const size_t &size)
{
	m_vData.insert(m_vData.begin(), data, data + size);
}
