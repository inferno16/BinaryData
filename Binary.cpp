#include "Binary.h"

Binary::Binary()
	:m_uUsedBits(0), m_uUnusedBits(0), m_uDataOffset(0)
{}

Binary::Binary(const size_t &size)
	: m_uUsedBits(0), m_uUnusedBits(0), m_uDataOffset(0)
{
	m_vData.reserve(size);
}

Binary::Binary(const binary_t & vec)
	: m_vData(vec), m_uUsedBits(0), m_uUnusedBits(0), m_uDataOffset(0)
{}

Binary::Binary(const binary_t::iterator & begin, const binary_t::iterator & end)
	: m_vData(begin, end), m_uUsedBits(0), m_uUnusedBits(0), m_uDataOffset(0)
{}

Binary::Binary(const Binary & bObj)
	: m_vData(bObj.m_vData), m_uUsedBits(bObj.m_uUsedBits), m_uUnusedBits(bObj.m_uUnusedBits), m_uDataOffset(bObj.m_uDataOffset)
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

void Binary::SetUsedBits(const uint8_t & count)
{
	if (count < 8)
		m_uUsedBits = count;
}

uint8_t Binary::GetUsedBits() const
{
	return m_uUsedBits;
}

void Binary::SetUnusedBits(const uint8_t & count)
{
	if (count < 8)
		m_uUnusedBits = count;
}

uint8_t Binary::GetUnusedBits() const
{
	return m_uUnusedBits;
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
	//AppendData(data.data(), data.size());
	m_vData.insert(m_vData.end(), data.begin(), data.end());
}

void Binary::AppendData(const Binary & bObj)
{
	AppendData(bObj.GetRawData(), bObj.GetSize());
}

void Binary::AppendData(const byte_t * data, const size_t &size)
{
	m_vData.insert(m_vData.end(), data, data + size);
}

void Binary::AppendData(const byte_t byte)
{
	//AppendData((byte_t*)&byte, 1);
	//m_vData.insert(m_vData.end(), byte, 1);
	m_vData.push_back(byte);
}

void Binary::AppendData(const binary_t::iterator & begin, const binary_t::iterator & end)
{
	AppendData(&*begin, begin - end);
}

void Binary::AppendUnalignedData(Binary & bObj)
{
	size_t unavBits = 8 - m_uUnusedBits;
	if (unavBits > bObj.GetUsedBits()) {
		bObj.MoveToRight(unavBits - bObj.GetUsedBits());
	}
	else if (unavBits < bObj.GetUsedBits()) {
		bObj.MoveToLeft(bObj.GetUsedBits() - unavBits);
	}
	m_vData.back() |= (bObj.GetData().front() & ~((1 << m_uUnusedBits) - 1));
	if (bObj.GetSize() > 1) {
		AppendData(bObj.GetData().begin() + 1, bObj.GetData().end());
	}
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

void Binary::PrependData(const byte_t byte)
{
	PrependData((byte_t*)&byte, 1);
}

void Binary::PrependData(const binary_t::iterator & begin, const binary_t::iterator & end)
{
	PrependData(&*begin, end - begin);
}

uint32_t Binary::GetBits(const size_t & count)
{
	if (count > 32 || !BufferSufficient(count)) {
		throw "GetBits can't retrieve more than 32 bits (4 bytes) at once!";
	}
	if (count == 0) { // This will always return zero, no need to calculate it
		return 0;
	}
	else if (count == 1) {
		uint32_t bits = (uint32_t)(m_vData.at(0 + m_uDataOffset) >> m_uUsedBits);
		if ((m_uUsedBits = ++m_uUsedBits % 8) == 0)
			PopFront();
		return bits & 1;
	}
	size_t bitCount;
	uint32_t bits = GetAvailableBitsFromCurrentByte(bitCount);
	while (bitCount < count) {
		PopFront();
		uint32_t currByte = (uint32_t)m_vData.at(0 + m_uDataOffset);
		bits |= (currByte << bitCount);
		bitCount += 8;
	}
	if (bitCount == count)
		PopFront();
	bits &= ((1 << count) - 1); // e.g. if count is 3 the mask has to be 0b00000111 or decimal '7' (1 << 3 = 8, 8 - 1 = 7)
	m_uUsedBits = (m_uUsedBits + count + (8 - bitCount % 8)) % 8;

	return bits;
}

void Binary::FlushBits()
{
	if (m_uUsedBits > 0) {
		PopFront();
		m_uUsedBits = 0;
	}
}

void Binary::ReadData(byte_t* buffer, const size_t & size, bool autoFlush)
{
	if (size > m_vData.size())
		throw "Internal data buffer size is insufficient!";
	if (autoFlush)
		FlushBits();

	memcpy_s((void*)buffer, size, (void*)(m_vData.data() + m_uDataOffset), size);
	m_uDataOffset += size;
	//m_vData.erase(m_vData.begin(), m_vData.begin() + m_uDataOffset + size);
}

Binary Binary::GetUnalignedData(const size_t & atIndex, const uint8_t & bitOffset, const size_t & bitCount)
{
	if (bitOffset >= 8)
		throw "Bit offset cannot be higher than 7 in Binary::GetUnalignedData!";

	// Calculating the byte count from the provided bit count and bit offset:
	// if the bit count is multiple of 8 we have 0 extra bytes otherwise we have 1
	// if the bit offset > 0 and the bit count is multiple of 8 we also have 1 extra byte
	// if the bit count is not multiple of 8 and the bit offset + remainder of the division is > 8 we have 1 more extra bit
	size_t byteCount = (bitCount % 8 == 0) ? 0 + (size_t)(bitOffset > 0) : 1 + (size_t)(bitCount % 8 + bitOffset > 8);
	byteCount += bitCount / 8;

	if (atIndex > m_vData.size() - 1 || atIndex + byteCount > m_vData.size() - 1)
		throw "The reqested data goes out of the boundaries of the data container!";

	// Construct the new object with the unaligned data
	Binary b(m_vData.begin() + atIndex, m_vData.begin() + atIndex + byteCount);
	uint8_t usedBits = bitCount % 8 + bitOffset;
	usedBits -= (usedBits > 8) ? 8 : 0;
	b.SetUsedBits(bitOffset);
	b.SetUnusedBits((8 - usedBits) % 8);

	return b;
}

void Binary::ShiftRight(const size_t & amount, const bool &resize)
{
	if (amount > m_vData.size() * 8) {
		throw "Amount of bits to shift is greater than the actual bit count!";
	}

	if (amount >= 8) {
		for (size_t i = 0; i < m_vData.size(); i++)
		{
			if (i < m_vData.size() - 1)
				m_vData.at(i) = m_vData.at(i + 1);
			else {
				if (resize)
					PopByte(m_vData.begin() + i);
				else
					m_vData.at(i) = 0;
			}
		}
		ShiftRight(amount - 8);
	}
	else {
		for (size_t i = 0; i < m_vData.size(); i++)
		{
			m_vData.at(i) = m_vData.at(i) >> amount;
			m_vData.at(i) |= (i < m_vData.size() - 1) ? (m_vData.at(i + 1) << (8 - amount)) : 0;
		}
		if (resize) {
			if (amount + m_uUnusedBits >= 8)
				PopBack();
			m_uUnusedBits = (amount + m_uUnusedBits) % 8;
		}
	}
}

void Binary::ShiftLeft(const size_t & amount, const bool &resize)
{
	if (amount > m_vData.size() * 8) {
		throw "Amount of bits to shift is greater than the actual bit count!";
	}

	if (amount >= 8) {
		for (int i = m_vData.size() - 1; i >= 0; i--)
		{
			if (i > 0)
				m_vData.at(i) = m_vData.at(i - 1);
			else {
				if (resize)
					PopByte(m_vData.begin() + i);
				else
					m_vData.at(i) = 0;
			}
		}
		ShiftLeft(amount - 8);
	}
	else {
		for (int i = m_vData.size() - 1; i >= 0; i--)
		{
			m_vData.at(i) = m_vData.at(i) << amount;
			m_vData.at(i) |= (i > 0) ? (m_vData.at(i - 1) >> (8 - amount)) : 0;
		}
		if (resize) {
			if (amount + m_uUsedBits >= 8)
				PopFront();
			m_uUsedBits = (amount + m_uUsedBits) % 8;
		}
	}
}

void Binary::MoveToLeft(const size_t & amount)
{
	if (amount > m_uUsedBits) {
		m_vData.insert(m_vData.begin(), 0);
		m_uUsedBits = 8 - (amount - m_uUsedBits);
	}
	else {
		m_uUsedBits -= amount;
	}
	ShiftRight(amount, true);
}

void Binary::MoveToRight(const size_t & amount)
{
	if (amount > m_uUnusedBits) {
		m_vData.push_back(0);
		m_uUnusedBits = 8 - (amount - m_uUnusedBits);
	}
	else {
		m_uUnusedBits -= amount;
	}
	ShiftLeft(amount, true);
}

void Binary::ReverseOrder()
{
	std::reverse(m_vData.begin(), m_vData.end());
}

void Binary::SwapBitPointers()
{
	std::swap(m_uUsedBits, m_uUnusedBits);
}

void Binary::PopFront()
{
	m_uDataOffset++;
	//PopByte(m_vData.begin());
}

void Binary::PopBack()
{
	PopByte(m_vData.end() - 1);
}

void Binary::ShrinkToFit()
{
	m_vData.erase(m_vData.begin(), m_vData.begin() + m_uDataOffset);
	m_vData.shrink_to_fit();
	m_uDataOffset = 0;
}

void Binary::FreeData()
{
	binary_t().swap(m_vData);
}

bool Binary::BufferSufficient(const size_t &bitCount)
{
	return ((m_vData.size() * 8 - m_uUsedBits) >= bitCount);
}

uint8_t Binary::GetAvailableBitsFromCurrentByte(size_t &bitCount)
{
	uint8_t bits = m_vData.at(0 + m_uDataOffset);
	bits = (bits >> m_uUsedBits);
	bitCount = 8 - m_uUsedBits;
	if (m_vData.size() == 1) {
		if (m_uUsedBits + m_uUnusedBits > 8)
			throw "The number of unavalable bits in the byte is greater than 8!";
		else
			bitCount -= m_uUnusedBits;
	}
	m_uUsedBits = 0;
	return bits;
}

void Binary::PopByte(const binary_t::iterator &position)
{
	if (m_vData.size() == 0)
		throw "Trying to erase byte from an empty buffer!";
	if (position < m_vData.begin() || position >= m_vData.end())
		throw "Trying to erase byte that is out of the data boundaries!";
	m_vData.erase(position);
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
