#pragma once
#include <iostream>

template <typename T>
size_t strlength(const T* str)
{
	size_t length = 0;
	while (*str++)
	{
		++length;
	}
	return length;
}

template<>
size_t strlength<char>(const char* str)
{
	return strlen(str);
}

template<>
size_t strlength<wchar_t>(const wchar_t* str)
{
	return wcslen(str);
}

template <typename T>
void strcopy(T* dest, const size_t size, const T* source)
{
	for (size_t i = 0; i < size; ++i)
	{
		dest[i] = source[i];
	}
}

template<>
void strcopy<char>(char* dest, const size_t size, const char* source)
{
	strcpy_s(dest, size, source);
}

template<>
void strcopy<wchar_t>(wchar_t* dest, const size_t size, const wchar_t* source)
{
	wcscpy_s(dest, size, source);
}

//precondition - T must have constuctor with one parameter of type char
template<typename T>
T getTerminator()
{
	return T{ '\0' };
}

template<>
char getTerminator<char>()
{
	return '\0';
}

template<>
wchar_t getTerminator<wchar_t>()
{
	return  L'\0';
}

template <typename T>
class String
{
private:

	//closed from outside
	class CharProxy;

	std::shared_ptr<T> _allocator;
	size_t _length;
	bool _shareable;

	void check(const size_t index) const;

public:

	class BadString;

	explicit String();
	explicit String(const T& ch);
	explicit String(const T* str);
	String(const String<T>& str);
	String(String<T>&& str);

	~String();

	String& operator=(const String<T>& str);
	String& operator=(String<T>&& str);
	String& operator=(const T& ch);
	String& operator=(const T* str);

	String<T>::CharProxy operator[](const size_t index);
	const T& operator[](const size_t index) const;

	inline size_t length() const { return _length; }
	inline bool isEmpty() const { return length() == 0; }

	void clear();

	String& operator+=(const String<T>& other);
	String& operator+=(const T& other);
	String& operator+=(const T* other);

	//testing function
	//used friend to be able to compare pointers 
	//definition in "Tests.h"
	friend void executeDeferredTests();
};

template<typename T>
class String<T>::CharProxy
{
private:

	//to be able to use constructor
	friend class String;

	String<T>& _proxyship;
	size_t _index;

	T* const getRawPointer() const;

	CharProxy(String<T>& ds, size_t index);

public:

	CharProxy(const CharProxy& other);
	CharProxy(CharProxy&& other);

	~CharProxy();

	CharProxy& operator=(const CharProxy& other);
	CharProxy& operator=(CharProxy&& other);
	CharProxy& operator=(const T& ch);

	const T* operator&() const;
	T* operator&();
	operator T() const;
	operator T& ();
	
	//Workaround because of nested template class String<T>::CharProxy
	friend std::ostream& operator<<(std::ostream& os, const CharProxy& proxy)
	{
		os << static_cast<T>(proxy);
		return os;
	}
};

template <typename T>
String<T>::String(): _length(0), _shareable(true)
{
	T* tempAlloc = new T[1];
	tempAlloc[0] = getTerminator<T>();
	_allocator = { tempAlloc, std::default_delete<T[]>() };
	//_allocator = { tempAlloc, [](T* ptr) {delete[] ptr; }}; - possible variant
}

template <typename T>
String<T>::String(const T& ch) : _length(1), _shareable(true)
{
	T* tempAlloc = new T[2];
	tempAlloc[0] = ch;
	tempAlloc[1] = getTerminator<T>();
	_allocator = { tempAlloc, std::default_delete<T[]>()}; 
}

template <typename T>
String<T>::String(const T* str)
{
	if (str == nullptr)
	{
		throw BadString("Can't create string from nullptr!", 0);
	}

	_length = strlength(str);
	_shareable = true;

	T* tempAlloc = new T[_length + 1];
	strcopy(tempAlloc, _length + 1, str); //copying with last symbol(terminator) included
	_allocator = { tempAlloc, std::default_delete<T[]>() };
}

template <typename T>
String<T>::String(const String<T>& str)
{
	if (str._shareable)
	{
		_allocator = str._allocator;
		_length = str.length();
		_shareable = true;
	}
	else
	{
		_length = str.length();
		_shareable = true;

		T* tempAlloc = new T[_length + 1];
		strcopy(tempAlloc, _length + 1, str._allocator.get());
		_allocator = { tempAlloc, std::default_delete<T[]>() };
	}
}

template <typename T>
String<T>::String(String<T>&& str)
{
	if (str._shareable)
	{
		_allocator = str._allocator;
		_length = str.length();
		_shareable = true;
	}
	else
	{
		_length = str.length();
		_shareable = true;

		T* tempAlloc = new T[_length + 1];
		strcopy(tempAlloc, _length + 1, str._allocator.get());
		_allocator = { tempAlloc, std::default_delete<T[]>() };
	}

	//making moved string empty
	T* emptyStr = new T[1];
	emptyStr[0] = getTerminator<T>();
	str._length = 0;
	str._allocator = { emptyStr, std::default_delete<T[]>() };
}

//because of using std::shared_ptr
//no need to manage the resources directly
template <typename T>
String<T>::~String() {}

template <typename T>
String<T>& String<T>::operator=(const String<T>& str)
{
	if ((this != &str) && (this->_allocator.get() != str._allocator.get()))
	{
		if (str._shareable)
		{
			_allocator = str._allocator;
			_length = str.length();
			_shareable = true;
		}
		else
		{
			_length = str.length();
			_shareable = true;

			T* tempAlloc = new T[_length + 1];
			strcopy(tempAlloc, _length + 1, str._allocator.get());
			_allocator = { tempAlloc, std::default_delete<T[]>() };
		}
	}

	return *this;
}

template <typename T>
String<T>& String<T>::operator=(String<T>&& str)
{
	if ((this != &str) && (this->_allocator.get() != str._allocator.get()))
	{
		if (str._shareable)
		{
			_allocator = str._allocator;
			_length = str.length();
			_shareable = true;
		}
		else
		{
			_length = str.length();
			_shareable = true;
			
			T* tempAlloc = new T[_length + 1];
			strcopy(tempAlloc, _length + 1, str._allocator.get());
			_allocator = { tempAlloc, std::default_delete<T[]>() };
		}

		T* emptyStr = new T[1];
		emptyStr[0] = getTerminator<T>();
		str._length = 0;
		str._allocator = { emptyStr, std::default_delete<T[]>() };
	}

	return *this;
}

template <typename T>
String<T>& String<T>::operator=(const T& ch)
{
	_length = 1;
	T* tempAlloc = new T[2];
	tempAlloc[0] = ch;
	tempAlloc[1] = getTerminator<T>();
	_allocator = { tempAlloc, std::default_delete<T[]>() };

	return *this;
}

template <typename T>
String<T>& String<T>::operator=(const T* str)
{
	if (str != nullptr && _allocator.get() != str)
	{
		_length = strlength(str);
		_shareable = true;

		T* tempAlloc = new T[_length + 1];
		strcopy(tempAlloc, _length + 1, str);
		_allocator = { tempAlloc, std::default_delete<T[]>() };
	}

	return *this;
}

template <typename T>
void String<T>::check(const size_t index) const
{
	if (this->isEmpty())
	{
		throw BadString("Can't read a character from empty string!", index);
	}
	if (index >= length())
	{
		throw BadString("Can't read character from position bigger or equal to length", index);
	}

	return;
}

template <typename T>
const T& String<T>::operator[](const size_t index) const
{
	check(index);

	//taking the raw pointer to use the indexing
	return (_allocator.get())[index];
}

template <typename T>
typename String<T>::CharProxy String<T>::operator[](const size_t index)
{
	check(index);
	return typename String<T>::CharProxy(*this, index);
}

template <typename T>
void String<T>::clear()
{
	if (!this->isEmpty())
	{
		_length = 0;
		_shareable = true;
		T* tempAlloc = new T[1];
		tempAlloc[0] = getTerminator<T>();
		_allocator = { tempAlloc, std::default_delete<T[]>() };
	}

	return;
}

template <typename T>
String<T>& String<T>::operator+=(const String<T>& other)
{
	if (!other.isEmpty())
	{
		size_t newLength = length() + other.length();
		T* concatenated = new T[newLength + 1];
		T* currentPos = concatenated;

		strcopy(currentPos, length() + 1, _allocator.get());
		currentPos += length();
		strcopy(currentPos, other.length() + 1, other._allocator.get());

		_allocator = { concatenated, std::default_delete<T[]>() };
		_length = newLength;
	}
	return *this;
}

template <typename T>
String<T>& String<T>::operator+=(const T& other)
{
	size_t newLength = length() + 1;
	T* concatenated = new T[newLength + 1];
	strcopy(concatenated, length() + 1, _allocator.get());
	concatenated[length()] = other;
	concatenated[newLength] = (_allocator.get())[length()]; //adding terminator

	_allocator = { concatenated, std::default_delete<T[]>() };
	_length = newLength;

	return *this;
}

template <typename T>
String<T>& String<T>::operator+=(const T* other)
{
	if(other != nullptr)
	{
		size_t otherLength = strlength(other);
		if(otherLength > 0)
		{
			size_t newLength = length() + otherLength;
			T* concatenated = new T[newLength + 1];
			T* currentPos = concatenated;

			strcopy(currentPos, length() + 1, _allocator.get());
			currentPos += length();
			strcopy(currentPos, otherLength + 1, other);

			_allocator = { concatenated, std::default_delete<T[]>() };
			_length = newLength;
		}
	}
	return *this;
}

//operators for comparing work correctly if type T has suitable defined operators to compare single elements  
template <typename T>
bool operator==(const String<T>& left, const String<T>& right)
{
	if (left.length() != right.length())
	{
		return false;
	}
	else
	{
		size_t length = left.length();
		for (size_t i = 0; i < length; ++i)
		{
			if (left[i] != right[i])
			{
				return false;
			}
		}

		return true;
	}
}

template <typename T>
bool operator!=(const String<T>& left, const String<T>& right)
{
	return !(left == right);
}

template <typename T>
bool operator==(const String<T>& left, const T& right)
{
	return (left.length() == 1) && (left[0] == right);
}

template <typename T>
bool operator==(const T& left, const String<T>& right)
{
	return (right == left);
}

template <typename T>
bool operator!=(const String<T>& left, const T& right)
{
	return !(left == right);
}

template <typename T>
bool operator!=(const T& left, const String<T>& right)
{
	return !(right == left);
}

template <typename T>
bool operator<(const String<T>& left, const String<T>& right)
{
	bool res = true;
	size_t i = 0;
	while (res && i < left.length() && i < right.length())
	{
		res = res && (left[i] == right[i]);
		++i;
	}

	if (res)
	{
		return left.length() < right.length();
	}

	--i;
	return left[i] < right[i];
}

template <typename T>
bool operator<=(const String<T>& left, const String<T>& right)
{
	if (left == right)
	{
		return true;
	}

	return left < right;
}

template <typename T>
bool operator>(const String<T>& left, const String<T>& right)
{
	bool res = true;
	size_t i = 0;
	while (res && i < left.length() && i < right.length())
	{
		res = res && (left[i] == right[i]);
		++i;
	}

	if (res)
	{
		return left.length() > right.length();
	}

	--i;
	return left[i] > right[i];
}

template <typename T>
bool operator>=(const String<T>& left, const String<T>& right)
{
	if (left == right)
	{
		return true;
	}

	return left > right;
}

template <typename T>
String<T> operator+(const String<T>& left, const String<T>& right)
{
	if (left.isEmpty())
	{
		return right;
	}
	else if (right.isEmpty())
	{
		return left;
	}
	else
	{
		String<T> res(left);
		return res += right;
	}
}

template <typename T>
String<T> operator+(const String<T>& left, const T& right)
{
	String<T> res(left);
	return res += right;
}

template <typename T>
String<T> operator+(const T& left, const String<T>& right)
{
	String<T> res(left);
	return res += right;
}

template <typename T>
String<T> operator+(const String<T>& left, const T* right)
{
	if (right != nullptr && strlength(right) > 0)
	{
		String<T> res(left);
		return res += right;
	}

	return left;
}

template <typename T>
String<T> operator+(const T* left, const String<T>& right)
{
	if (left != nullptr && strlength(left) > 0)
	{
		String<T> res(left);
		return res += right;
	}

	return right;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const String<T>& str)
{
	size_t len = str.length();
	for (size_t i = 0; i < len; ++i)
	{
		os << str[i];
	}
	return os;
}

//to print String<wchar_t> into wcout
template<typename T>
std::wostream& operator<<(std::wostream& os, const String<T>& str)
{
	size_t len = str.length();
	for (size_t i = 0; i < len; ++i)
	{
		os << str[i];
	}
	return os;
}


template <typename T>
String<T>::CharProxy::CharProxy(String<T>& ds, size_t index) : _proxyship(ds), _index(index) {}

template <typename T>
String<T>::CharProxy::CharProxy(const String<T>::CharProxy& other): _proxyship(other._proxyship), _index(other._index) {}

template <typename T>
String<T>::CharProxy::CharProxy(String<T>::CharProxy&& other) : _proxyship(other._proxyship), _index(other._index) {}

template <typename T>
String<T>::CharProxy::~CharProxy() {}

template <typename T>
T* const String<T>::CharProxy::getRawPointer() const
{
	return _proxyship._allocator.get();
}

template <typename T>
const T* String<T>::CharProxy::operator&() const
{
	return &(getRawPointer()[_index]);
}

template <typename T>
T* String<T>::CharProxy::operator&()
{
	if (_proxyship._shareable && _proxyship._allocator.use_count() > 1)
	{
		T* newStr = new T[_proxyship.length() + 1];
		strcopy(newStr, _proxyship.length() + 1, getRawPointer());
		_proxyship._allocator = { newStr, std::default_delete<T[]>() };
	}
	_proxyship._shareable = false;
	return &(getRawPointer()[_index]);
}

template <typename T>
String<T>::CharProxy::operator T() const
{
	return (getRawPointer()[_index]);
}

template <typename T>
String<T>::CharProxy::operator T&()
{
	if (_proxyship._shareable && _proxyship._allocator.use_count() > 1)
	{
		T* newStr = new T[_proxyship.length() + 1];
		strcopy(newStr, _proxyship.length() + 1, getRawPointer());
		_proxyship._allocator = { newStr, std::default_delete<T[]>() };
	}
	_proxyship._shareable = false;
	return (getRawPointer()[_index]);
}

template <typename T>
typename String<T>::CharProxy& String<T>::CharProxy::operator=(const T& ch)
{
	if (_proxyship._shareable && _proxyship._allocator.use_count() > 1)
	{
		T* newStr = new T[_proxyship.length() + 1];
		strcopy(newStr, _proxyship.length() + 1, getRawPointer());
		_proxyship._allocator = { newStr, std::default_delete<T[]>() };
	}
	(getRawPointer()[_index]) = ch;
	return *this;
}

//mimics the assignment of single char
template <typename T>
typename String<T>::CharProxy& String<T>::CharProxy::operator=(const typename String<T>::CharProxy& other)
{
	bool sameStr = this->getRawPointer() != other.getRawPointer();
	bool sameInd = this->_index == other._index;
	if (!sameStr || (sameStr && !sameInd))
	{
		if (_proxyship._shareable && _proxyship._allocator.use_count() > 1)
		{
			T* newStr = new T[_proxyship.length() + 1];
			strcopy(newStr, _proxyship.length() + 1, getRawPointer());
			_proxyship._allocator = { newStr, std::default_delete<T[]>() };
		}
		(getRawPointer()[_index]) = (other.getRawPointer()[other._index]);
	}
	return *this;
}

//mimics the assignment of single char
template <typename T>
typename String<T>::CharProxy& String<T>::CharProxy::operator=(typename String<T>::CharProxy&& other)
{
	bool sameStr = this->getRawPointer() != other.getRawPointer();
	bool sameInd = this->_index == other._index;
	if (!sameStr || (sameStr && !sameInd))
	{
		if (_proxyship._shareable && _proxyship._allocator.use_count() > 1)
		{
			T* newStr = new T[_proxyship.length() + 1];
			strcopy(newStr, _proxyship.length() + 1, getRawPointer());
			_proxyship._allocator = { newStr, std::default_delete<T[]>() };
		}
		(getRawPointer()[_index]) = (other.getRawPointer()[other._index]);
	}
	return *this;
}


template <typename T>
class String<T>::BadString {

private:
	const std::string _reason;
	const size_t _index;

public:

	BadString(const std::string& reason = "", const size_t index = 0) : _reason(reason), _index(index) {}

	~BadString() {}

	void diagnose() const
	{
		std::cout << _reason << " At index: " << _index << std::endl;
		return;
	}
};