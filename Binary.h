#pragma once
#include <vector>
#include <iostream>

typedef char byte_t;
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

	// Other public methods
	void ReadFromStream(std::istream &stream, const size_t &size);
	void WriteToStream(std::ostream &stream) const;
	void AppendData(const binary_t &data);
	void AppendData(const Binary &bObj);
	void AppendData(const byte_t* data, const size_t &size);
	void PrependData(const binary_t &data);
	void PrependData(const Binary &bObj);
	void PrependData(const byte_t* data, const size_t &size);

private:
	binary_t m_vData;
};
