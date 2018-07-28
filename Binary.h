#pragma once
#include <vector>
#include <iostream>
#include <sstream>

typedef unsigned char byte_t;
typedef std::vector<byte_t> binary_t;

class Binary
{
public:
	// Constructors and Destructor
	Binary();
	Binary(const binary_t &vec);
	Binary(const Binary &bObj);
	~Binary();

	// Accessors and mutators
	void SetData(const binary_t &data);
	void SetData(const byte_t *data, const size_t &size);
	binary_t GetData() const;
	const byte_t* GetRawData() const;
	size_t GetSize() const;

	// Static functions
	static uint32_t ByteSwap(const uint32_t &num);
	static uint16_t ByteSwap(const uint16_t &num);

	// Other public methods
	void ReadFromStream(std::istream &stream, const size_t &size);
	void WriteToStream(std::ostream &stream) const;
	void AppendData(const binary_t &data);
	void AppendData(const Binary &bObj);
	void AppendData(const byte_t* data, const size_t &size);
	void PrependData(const binary_t &data);
	void PrependData(const Binary &bObj);
	void PrependData(const byte_t* data, const size_t &size);
	uint32_t GetBits(const size_t &count);
	void FlushBits();
	void ReadData(byte_t* buffer, const size_t &size, bool autoFlush = true);

private: // Methods
	bool BufferSufficient(const size_t &bitCount);
	uint8_t GetAvailableBitsFromCurrentByte(size_t &bitCount);
	void PopByte();

private: // Variables
	binary_t m_vData;
	uint8_t m_nUsedBits; // number 0-7 representing the used bits from the current byte
};
