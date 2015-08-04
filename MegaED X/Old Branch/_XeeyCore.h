struct XFILE
{
	BYTE index, type, element;
	WORD size;
	DWORD position;
	WORD getID()
	{
		return (index<<8) | (type<<4) | element;
	}
	void setID(WORD id)
	{
		element = id & 0xF;
		type = (id>>4) & 0xF;
		index = id >> 8;
	}
};

class NDSCore : public Core
{
};

class XeeyCore : public NDSCore
{
public:
	XFILE *xFile;
	WORD fsElements;

	void EnumFS();
	void AddFile();
	void RemoveFile();
	LPWORD GenerateFS();


};