#include "Binary.h"

Binary::Binary()
	:m_nUsedBits(0), m_nUnusedBits(0)
{}

Binary::Binary(const binary_t & vec)
	: m_vData(vec), m_nUsedBits(0), m_nUnusedBits(0)
{}

Binary::Binary(const binary_t::iterator & begin, const binary_t::iterator & end)
	: m_vData(begin, end), m_nUsedBits(0), m_nUnusedBits(0)
{}

Binary::Binary(const Binary & bObj)
	: m_vData(bObj.m_vData), m_nUsedBits(bObj.m_nUsedBits), m_nUnusedBits(bObj.m_nUnusedBits)
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

void Binary::SetData(const binary_t::iterator & begin, const binary_t::iterator & end)
{
	m_vData.assign(begin, end);
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
	stream.read((char*)vec.data(), size);
	int a = stream.rdstate();
	if (stream.good() || stream.eof()) {
		m_vData = vec;
	}
}

void Binary::WriteToStream(std::ostream & stream) const
{
	stream.write((char*)m_vData.data(), m_vData.size());
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

uint32_t Binary::GetBits(const size_t & count)
{
	if (count > 32 || !BufferSufficient(count)) {
		throw "GetBits can't retrieve more than 32 bits (4 bytes) at once!";
	}
	if (count == 0) { // This is always zero, no need to calculate it
		return 0;
	}
	size_t bitCount;
	uint32_t bits = GetAvailableBitsFromCurrentByte(bitCount);
	while (bitCount < count) {
		PopByte();
		uint32_t currByte = (uint32_t)m_vData.at(0);
		bits |= (currByte << bitCount);
		bitCount += 8;
	}
	if (bitCount == count)
		PopByte();
	bits &= ((1 << count) - 1);
	m_nUsedBits = (m_nUsedBits + count + (8 - bitCount % 8)) % 8;

	return bits; // If count is 3 and the mask is 0b00000111 or decimal '7' (1 << 3 = 8, 8 - 1 = 7)
}

void Binary::FlushBits()
{
	if (m_nUsedBits > 0) {
		PopByte();
		m_nUsedBits = 0;
	}
}

void Binary::ReadData(byte_t* buffer, const size_t & size, bool autoFlush)
{
	if (size > m_vData.size())
		throw "Internal data buffer size is insufficient!";
	if (autoFlush)
		FlushBits();

	memcpy_s((void*)buffer, size, (void*)m_vData.data(), size);
	m_vData.erase(m_vData.begin(), m_vData.begin() + size);
}

void Binary::ShiftRight(const size_t & amount)
{
	if (amount > m_vData.size() * sizeof(byte_t) * 8) {
		throw "Amount of bits to shift is greater than the actual bit count!";
	}

	if (amount >= sizeof(byte_t) * 8) {
		for (size_t i = 0; i < m_vData.size(); i++)
		{
			m_vData.at(i) = (i < m_vData.size() - 1) ? m_vData.at(i + 1) : 0;
		}
		ShiftRight(amount - sizeof(byte_t) * 8);
	}
	else {
		for (size_t i = 0; i < m_vData.size(); i++)
		{
			m_vData.at(i) = m_vData.at(i) >> amount;
			m_vData.at(i) |= (i < m_vData.size() - 1) ? (m_vData.at(i + 1) << (sizeof(byte_t) * 8 - amount)) : 0;
		}
	}
}

void Binary::ShiftLeft(const size_t & amount)
{
	if (amount > m_vData.size() * sizeof(byte_t) * 8) {
		throw "Amount of bits to shift is greater than the actual bit count!";
	}

	if (amount >= sizeof(byte_t) * 8) {
		for (int i = m_vData.size() - 1; i >= 0; i--)
		{
			m_vData.at(i) = (i > 0) ? m_vData.at(i - 1) : 0;
		}
		ShiftLeft(amount - sizeof(byte_t) * 8);
	}
	else {
		for (int i = m_vData.size() - 1; i >= 0; i--)
		{
			m_vData.at(i) = m_vData.at(i) << amount;
			m_vData.at(i) |= (i > 0) ? (m_vData.at(i - 1) >> (sizeof(byte_t) * 8 - amount)) : 0;
		}
	}
}

bool Binary::BufferSufficient(const size_t &bitCount)
{
	return ((m_vData.size() * 8 - m_nUsedBits) >= bitCount);
}

uint8_t Binary::GetAvailableBitsFromCurrentByte(size_t &bitCount)
{
	uint8_t bits = m_vData.at(0);
	bits = (bits >> m_nUsedBits);
	bitCount = 8 - m_nUsedBits;
	m_nUsedBits = 0;
	return bits;
}

void Binary::PopByte()
{
	m_vData.erase(m_vData.begin());
}

uint32_t Binary::ByteSwap(const uint32_t &num)
{
	return (
		(num << 24) |
		((num & 0xFF00) << 8) |
		((num >> 8) & 0xFF00) |
		(num >> 24)
		);
}

uint16_t Binary::ByteSwap(const uint16_t &num)
{
	return (num << 8 | num >> 8);
}
