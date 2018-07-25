#include "Binary.h"

Binary::Binary()
	:m_nUsedBits(0)
{}

Binary::Binary(const binary_t & vec)
	: m_vData(vec), m_nUsedBits(0)
{}

Binary::Binary(const Binary & bObj)
	: m_vData(bObj.m_vData), m_nUsedBits(bObj.m_nUsedBits)
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
	int a = stream.rdstate();
	if (stream.good() || stream.eof()) {
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

uint32_t Binary::GetBits(const size_t & count)
{
	if (count > 32 || !BufferSufficient(count)) {
		throw "GetBits can't retrieve more than 32 bits (4 bytes) at once!\n";
	}
	size_t bitCount;
	uint32_t bits = GetAvailableBitsFromCurrentByte(bitCount);
	while (bitCount < count) {
		PopByte();
		bits = (bits << 8) | (uint8_t)m_vData.at(0);
		bitCount += 8;
	}
	if (bitCount == count)
		PopByte();
	bits = (bits >> m_nUsedBits) & ((1 << count) - 1);
	m_nUsedBits += (count + 8 - bitCount) % 8;

	return bits; // If count is 3 and the mask is 0b00000111 or decimal '7' (1 << 3 = 8, 8 - 1 = 7)
}

void Binary::FlushBits()
{
	if (m_nUsedBits > 0)
		PopByte();
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

uint8_t Binary::PopByte()
{
	uint8_t byte = m_vData.at(0);
	m_vData.erase(m_vData.begin());
	return byte;
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
