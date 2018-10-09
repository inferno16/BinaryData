#pragma once
#include <vector>
#include <iostream>
#include <algorithm> // std::reverse(), std::swap()

typedef unsigned char byte_t;
typedef std::vector<byte_t> binary_t;

class Binary
{
public:
	// Constructors and Destructor
	Binary();
	Binary(const size_t &size);
	Binary(const binary_t &vec);
	Binary(const binary_t::iterator &begin, const binary_t::iterator &end);
	Binary(const Binary &bObj);
	~Binary();

	// Accessors and mutators
	void SetData(const binary_t &data);
	void SetData(const byte_t *data, const size_t &size);
	void SetData(const binary_t::iterator &begin, const binary_t::iterator &end);
	binary_t GetData() const;
	const byte_t* GetRawData() const;
	size_t GetSize() const;
	void SetUsedBits(const uint8_t &count);
	uint8_t GetUsedBits() const;
	void SetUnusedBits(const uint8_t &count);
	uint8_t GetUnusedBits() const;

	// Static functions
	static uint32_t ByteSwap(const uint32_t &num);
	static uint16_t ByteSwap(const uint16_t &num);

	// Other public methods
	void ReadFromStream(std::istream &stream, const size_t &size);
	void WriteToStream(std::ostream &stream) const;
	void AppendData(const binary_t &data);
	void AppendData(const Binary &bObj);
	void AppendData(const byte_t* data, const size_t &size);
	void AppendData(const byte_t byte);
	void AppendData(const binary_t::iterator &begin, const binary_t::iterator &end);
	void AppendUnalignedData(Binary &bObj);
	void PrependData(const binary_t &data);
	void PrependData(const Binary &bObj);
	void PrependData(const byte_t* data, const size_t &size);
	void PrependData(const byte_t byte);
	void PrependData(const binary_t::iterator &begin, const binary_t::iterator &end);
	uint32_t GetBits(const size_t &count);
	void FlushBits();
	void ReadData(byte_t* buffer, const size_t &size, bool autoFlush = true);
	Binary GetUnalignedData(const size_t &atIndex, const uint8_t &bitOffset, const size_t &bitCount);
	void ShiftLeft(const size_t &amount, const bool &resize = false);
	void ShiftRight(const size_t &amount, const bool &resize = false);
	void MoveToLeft(const size_t &amount);
	void MoveToRight(const size_t &amount);
	void ReverseOrder();
	void SwapBitPointers();
	void PopFront();
	void PopBack();
	void ShrinkToFit();
	void FreeData();

private: // Methods
	bool BufferSufficient(const size_t &bitCount);
	uint8_t GetAvailableBitsFromCurrentByte(size_t &bitCount);
	void PopByte(const binary_t::iterator &position);

private: // Variables
	binary_t m_vData;
	size_t m_uDataOffset;
	uint8_t m_uUsedBits; // number 0-7 representing the used bits from the current byte
	uint8_t m_uUnusedBits; // Number 0-7 representing the unused bits from the last byte
};
